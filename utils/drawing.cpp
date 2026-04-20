#include <GL/glut.h>
#include <cmath>
#include <cctype>
#include <fstream>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include "utils.h"

namespace {

GLuint gTextures[TEX_COUNT] = {};
std::map<std::string, GLuint> gExternalTextures;

unsigned char hashNoise(int x, int y, int seed) {
    unsigned int n = static_cast<unsigned int>(x * 1973 + y * 9277 + seed * 26699);
    n = (n << 13) ^ n;
    return static_cast<unsigned char>((n * (n * n * 60493u + 19990303u) + 1376312589u) >> 24);
}

void uploadTexture(TextureId id, unsigned char* data, int size) {
    glBindTexture(GL_TEXTURE_2D, gTextures[id]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, size, size, GL_RGB, GL_UNSIGNED_BYTE, data);
}

bool readPpmHeader(std::ifstream& in, int& width, int& height, int& maxValue) {
    auto nextToken = [&in]() -> std::string {
        std::string token;
        char ch = '\0';
        while (in.get(ch)) {
            if (ch == '#') {
                in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            if (!std::isspace(static_cast<unsigned char>(ch))) {
                token.push_back(ch);
                break;
            }
        }

        while (in.get(ch)) {
            if (std::isspace(static_cast<unsigned char>(ch))) break;
            token.push_back(ch);
        }
        return token;
    };

    const std::string magic = nextToken();
    if (magic != "P6") return false;

    const std::string widthToken = nextToken();
    const std::string heightToken = nextToken();
    const std::string maxValueToken = nextToken();
    if (widthToken.empty() || heightToken.empty() || maxValueToken.empty()) return false;

    width = std::stoi(widthToken);
    height = std::stoi(heightToken);
    maxValue = std::stoi(maxValueToken);
    return width > 0 && height > 0 && maxValue == 255;
}

void fillTexture(unsigned char* data, int size, TextureId id) {
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const int i = (y * size + x) * 3;
            const float fx = static_cast<float>(x) / static_cast<float>(size);
            const float fy = static_cast<float>(y) / static_cast<float>(size);
            const unsigned char n0 = hashNoise(x, y, static_cast<int>(id) * 11 + 3);
            const unsigned char n1 = hashNoise(x / 2, y / 2, static_cast<int>(id) * 17 + 7);
            const unsigned char n2 = hashNoise(x / 4, y / 4, static_cast<int>(id) * 23 + 11);
            const int soft0 = static_cast<int>(n0) - 128;
            const int soft1 = static_cast<int>(n1) - 128;
            const int soft2 = static_cast<int>(n2) - 128;
            int r = 170;
            int g = 170;
            int b = 170;

            switch (id) {
                case TEX_PLASTER: {
                    int cloud = static_cast<int>((0.5f + 0.5f * std::sin((fx * 3.4f + fy * 5.8f) * 3.14159f)) * 14.0f);
                    int base = 202 + soft0 / 24 + soft1 / 36 + cloud / 2;
                    int stain = ((x + y) % 31 == 0) ? -7 : 0;
                    int hairline = (std::abs(x - y) % 47 < 1) ? -5 : 0;
                    r = base + stain + hairline;
                    g = base + 2 + stain + hairline / 2;
                    b = base + 10 + stain / 2;
                    break;
                }
                case TEX_FLOOR: {
                    int tile = (((x / 12) + (y / 12)) % 2) * 6;
                    int wear = soft1 / 10;
                    int seam = (x % 12 == 0 || y % 12 == 0) ? -16 : 0;
                    int sheen = static_cast<int>((0.5f + 0.5f * std::sin((fx * 8.0f + fy * 6.0f) * 3.14159f)) * 8.0f);
                    r = 82 + tile + wear / 2 + seam + sheen;
                    g = 74 + tile + wear / 3 + seam + sheen;
                    b = 66 + tile + wear / 4 + seam / 2 + sheen;
                    break;
                }
                case TEX_WOOD: {
                    int grain = static_cast<int>(std::sin((fx * 16.0f + fy * 3.0f) * 3.14159f) * 14.0f) + soft0 / 18;
                    int ring = static_cast<int>(std::sin((fx * 3.0f + fy * 12.0f) * 3.14159f) * 6.0f) + soft2 / 40;
                    r = 138 + grain + ring / 2;
                    g = 92 + grain / 2 + ring / 3;
                    b = 48 + grain / 3;
                    break;
                }
                case TEX_WOOD_DARK: {
                    int grain = static_cast<int>(std::sin((fx * 18.0f + fy * 4.0f) * 3.14159f) * 11.0f) + soft0 / 20;
                    int ring = static_cast<int>(std::sin((fx * 2.5f + fy * 14.0f) * 3.14159f) * 5.0f) + soft2 / 44;
                    r = 78 + grain + ring / 2;
                    g = 50 + grain / 2 + ring / 3;
                    b = 28 + grain / 3;
                    break;
                }
                case TEX_METAL: {
                    int brushed = ((x * 4 + n1) % 26) - 13;
                    int highlight = (y % 22 < 2) ? 14 : 0;
                    int cool = static_cast<int>((0.5f + 0.5f * std::sin(fx * 16.0f * 3.14159f)) * 6.0f);
                    r = 120 + brushed + highlight + cool / 2;
                    g = 126 + brushed + highlight + cool / 2;
                    b = 134 + brushed + highlight + cool;
                    break;
                }
                case TEX_FABRIC: {
                    int weave = ((x % 5 == 0) || (y % 5 == 0)) ? -12 : 0;
                    int fuzz = soft0 / 18;
                    r = 118 + weave + fuzz;
                    g = 82 + weave + fuzz / 2;
                    b = 52 + weave / 2;
                    break;
                }
                case TEX_PAPER: {
                    int fiber = (n0 % 20) - 10;
                    int warm = (n1 % 12) - 6;
                    int crease = static_cast<int>(std::sin((fx * 7.0f + fy * 13.0f) * 3.14159f) * 18.0f);
                    int fold = (std::abs(x - y) % 19 < 2) ? -18 : 0;
                    int stain = ((x + 2 * y) % 29 == 0) ? -14 : 0;
                    r = 226 + fiber + warm / 2 + crease / 6 + fold + stain;
                    g = 218 + fiber + warm / 3 + crease / 7 + fold + stain;
                    b = 196 + fiber / 2 + crease / 10 + fold / 2 + stain / 2;
                    break;
                }
                case TEX_PLASTIC: {
                    int scuff = soft0 / 18;
                    int grime = (y > size * 3 / 4) ? -14 : 0;
                    int seam = (x % 20 < 2) ? 8 : 0;
                    int gloss = static_cast<int>((0.5f + 0.5f * std::sin(fx * 8.0f * 3.14159f)) * 10.0f);
                    int dents = ((x / 9 + y / 13) % 7 == 0) ? -5 : 0;
                    r = 74 + scuff / 2 + seam + gloss / 2 + grime + dents;
                    g = 84 + scuff / 2 + seam + gloss / 2 + grime + dents;
                    b = 92 + scuff + seam + gloss + grime + dents;
                    break;
                }
                case TEX_BOOK: {
                    int stripe = (x % 22 < 3) ? 28 : 0;
                    r = 154 + stripe + (n1 % 16) - 8;
                    g = 78 + (n0 % 18) - 9;
                    b = 48 + (n1 % 14) - 7;
                    break;
                }
                case TEX_WALLPAPER: {
                    int motif = (std::abs((x % 24) - 12) + std::abs((y % 24) - 12) < 8) ? 10 : -2;
                    int band = (y % 24 < 2) ? 8 : 0;
                    int fade = soft0 / 22;
                    r = 146 + motif + fade / 2 + band;
                    g = 132 + motif / 2 + fade / 3 + band / 2;
                    b = 108 + motif / 3 + fade / 4;
                    break;
                }
                case TEX_GRASS: {
                    int blade = static_cast<int>((0.5f + 0.5f * std::sin((fx * 18.0f + fy * 5.0f) * 3.14159f)) * 16.0f);
                    int patch = static_cast<int>((0.5f + 0.5f * std::sin((fx * 3.5f + fy * 2.0f) * 3.14159f)) * 16.0f) - 8;
                    int soil = (n2 % 31 == 0) ? -10 : 0;
                    r = 50 + blade / 4 + patch / 3 + soil + soft1 / 48;
                    g = 118 + blade + patch + soil / 2 + soft0 / 24;
                    b = 40 + blade / 5 + patch / 4 + soft2 / 56;
                    break;
                }
                case TEX_BRASS: {
                    int brushed = ((x * 3 + y * 2 + n0) % 22) - 11;
                    int gleam = (y % 18 < 2) ? 18 : 0;
                    int tarnish = (n1 % 29 == 0) ? -12 : 0;
                    r = 182 + brushed + gleam + tarnish;
                    g = 138 + brushed / 2 + gleam / 2 + tarnish / 2;
                    b = 56 + brushed / 3 + tarnish / 3;
                    break;
                }
                case TEX_GENERIC:
                default: {
                    int speck = soft0 / 12;
                    r = 148 + speck;
                    g = 138 + speck;
                    b = 126 + speck;
                    break;
                }
            }

            data[i + 0] = static_cast<unsigned char>(r < 0 ? 0 : (r > 255 ? 255 : r));
            data[i + 1] = static_cast<unsigned char>(g < 0 ? 0 : (g > 255 ? 255 : g));
            data[i + 2] = static_cast<unsigned char>(b < 0 ? 0 : (b > 255 ? 255 : b));
        }
    }
}

void bindTexture(TextureId texture) {
    if (texture == TEX_NONE) {
        glDisable(GL_TEXTURE_2D);
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gTextures[texture]);
}

void computeQuadFrame(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float& nx, float& ny, float& nz,
    float& lenU, float& lenV,
    float textureScale)
{
    const float ux = x2 - x1;
    const float uy = y2 - y1;
    const float uz = z2 - z1;
    const float vx = x3 - x1;
    const float vy = y3 - y1;
    const float vz = z3 - z1;

    nx = uy * vz - uz * vy;
    ny = uz * vx - ux * vz;
    nz = ux * vy - uy * vx;
    const float len = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 1e-6f) {
        nx /= len;
        ny /= len;
        nz /= len;
    }

    lenU = std::sqrt(ux * ux + uy * uy + uz * uz) * textureScale;
    lenV = std::sqrt(vx * vx + vy * vy + vz * vz) * textureScale;
    if (lenU < 0.001f) lenU = 1.0f;
    if (lenV < 0.001f) lenV = 1.0f;
}

} // namespace

void initTextures() {
    glGenTextures(TEX_COUNT, gTextures);
    const int size = 128;
    unsigned char data[size * size * 3];

    for (int texture = TEX_GENERIC; texture < TEX_COUNT; ++texture) {
        fillTexture(data, size, static_cast<TextureId>(texture));
        uploadTexture(static_cast<TextureId>(texture), data, size);
    }
}

void applyMaterial(float r, float g, float b, float ambientBoost, float shininess) {
    GLfloat ambient[] = { r * ambientBoost, g * ambientBoost, b * ambientBoost, 1.0f };
    GLfloat diffuse[] = { r, g, b, 1.0f };
    GLfloat specular[] = {
        0.18f + r * 0.10f,
        0.18f + g * 0.10f,
        0.18f + b * 0.10f,
        1.0f
    };

    glColor3f(r, g, b);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

unsigned int loadExternalTexture(const std::string& path) {
    std::map<std::string, GLuint>::const_iterator it = gExternalTextures.find(path);
    if (it != gExternalTextures.end()) return it->second;

    std::ifstream in(path.c_str(), std::ios::binary);
    if (!in) return 0;

    int width = 0;
    int height = 0;
    int maxValue = 0;
    if (!readPpmHeader(in, width, height, maxValue)) return 0;

    std::vector<unsigned char> pixels(static_cast<size_t>(width) * static_cast<size_t>(height) * 3u);
    in.read(reinterpret_cast<char*>(pixels.data()), static_cast<std::streamsize>(pixels.size()));
    if (in.gcount() != static_cast<std::streamsize>(pixels.size())) return 0;

    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    gExternalTextures[path] = textureId;
    return textureId;
}

void drawQuad(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    float r, float g, float b,
    TextureId texture,
    float textureScale)
{
    float nx = 0.0f;
    float ny = 0.0f;
    float nz = 0.0f;
    float lenU = 1.0f;
    float lenV = 1.0f;
    computeQuadFrame(x1, y1, z1, x2, y2, z2, x3, y3, z3, nx, ny, nz, lenU, lenV, textureScale);

    applyMaterial(r, g, b);
    bindTexture(texture);

    glBegin(GL_QUADS);
        glNormal3f(nx, ny, nz);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, z1);
        glTexCoord2f(lenU, 0.0f); glVertex3f(x2, y2, z2);
        glTexCoord2f(lenU, lenV); glVertex3f(x3, y3, z3);
        glTexCoord2f(0.0f, lenV); glVertex3f(x4, y4, z4);
    glEnd();

    if (texture != TEX_NONE) glDisable(GL_TEXTURE_2D);
}

void drawExternalTexturedQuad(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    float r, float g, float b,
    unsigned int textureId,
    float textureScale)
{
    if (textureId == 0) {
        drawQuad(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, r, g, b, TEX_GENERIC, textureScale);
        return;
    }

    float nx = 0.0f;
    float ny = 0.0f;
    float nz = 0.0f;
    float lenU = 1.0f;
    float lenV = 1.0f;
    computeQuadFrame(x1, y1, z1, x2, y2, z2, x3, y3, z3, nx, ny, nz, lenU, lenV, textureScale);

    applyMaterial(r, g, b);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glBegin(GL_QUADS);
        glNormal3f(nx, ny, nz);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, z1);
        glTexCoord2f(lenU, 0.0f); glVertex3f(x2, y2, z2);
        glTexCoord2f(lenU, lenV); glVertex3f(x3, y3, z3);
        glTexCoord2f(0.0f, lenV); glVertex3f(x4, y4, z4);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawExternalImageQuad(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    float r, float g, float b,
    unsigned int textureId)
{
    if (textureId == 0) {
        drawQuad(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, r, g, b, TEX_GENERIC, 1.0f);
        return;
    }

    float nx = 0.0f;
    float ny = 0.0f;
    float nz = 0.0f;
    float lenU = 1.0f;
    float lenV = 1.0f;
    computeQuadFrame(x1, y1, z1, x2, y2, z2, x3, y3, z3, nx, ny, nz, lenU, lenV, 1.0f);
    static_cast<void>(lenU);
    static_cast<void>(lenV);

    applyMaterial(r, g, b);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBegin(GL_QUADS);
        glNormal3f(nx, ny, nz);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x1, y1, z1);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(x2, y2, z2);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x3, y3, z3);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x4, y4, z4);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
