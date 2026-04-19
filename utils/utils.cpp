#include <GL/glut.h>
#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <string>
#include "../camera/camera.h"
#include "utils.h"

// --- Global state ---
std::vector<Item>        worldItems;
std::vector<std::string> inventory;
std::string              hudMessage = "";
float                    hudMessageTimer = 0.0f;   // counts down, clears message
bool                     roomLightOn = false;
bool                     drawerOpen = false;
bool                     drawerBookOverlayActive = false;
bool                     codeBoxOverlayActive = false;
bool                     codeBoxUnlocked = false;
float                    codeBoxOpenProgress = 0.0f;
std::string              codeBoxInput = "";

namespace {

GLuint gTextures[TEX_COUNT] = {};

const char* kDrawerBookOverlayLines[] = {
    "K eep    searching...",
    "  E ven the    smallest detail...",
    "        feels... out   of place...",
    "",
    " Y ou're     closer...",
    "        than you think..."
};

struct LetterColor {
    float r;
    float g;
    float b;
};

void* kOverlayFont = GLUT_BITMAP_HELVETICA_18;

LetterColor getLetterColor(char c) {
    if (!std::isalpha(static_cast<unsigned char>(c))) {
        return {0.90f, 0.82f, 0.28f};
    }

    if (c == 'K' || c == 'E' || c == 'Y') {
        return {0.92f, 0.20f, 0.18f};
    }
    return {0.95f, 0.84f, 0.26f};
}

int overlayTextWidth(const char* text) {
    int width = 0;
    for (const char* p = text; *p != '\0'; ++p) {
        width += glutBitmapWidth(kOverlayFont, *p);
    }
    return width;
}

void drawOverlayGlyph(int x, int y, char c, const LetterColor& color) {
    glColor4f(0.02f, 0.02f, 0.03f, 0.85f);
    glRasterPos2i(x + 2, y - 2);
    glutBitmapCharacter(kOverlayFont, c);

    glColor4f(color.r * 0.55f, color.g * 0.55f, color.b * 0.55f, 1.0f);
    glRasterPos2i(x + 1, y);
    glutBitmapCharacter(kOverlayFont, c);

    glColor4f(color.r, color.g, color.b, 1.0f);
    glRasterPos2i(x, y);
    glutBitmapCharacter(kOverlayFont, c);
    glRasterPos2i(x + 1, y);
    glutBitmapCharacter(kOverlayFont, c);
}

void drawOverlayTextLine(int x, int y, const char* text) {
    int cursorX = x;

    for (const char* p = text; *p != '\0'; ++p) {
        const LetterColor color = getLetterColor(*p);
        drawOverlayGlyph(cursorX, y, *p, color);
        cursorX += glutBitmapWidth(kOverlayFont, *p);
    }
}

void drawDrawerBookOverlay(int winW, int winH) {
    const int lineHeight = 38;
    const int lineCount = sizeof(kDrawerBookOverlayLines) / sizeof(kDrawerBookOverlayLines[0]);
    const int yStart = winH / 2 + ((lineCount - 1) * lineHeight) / 2;
    const int xOffsets[lineCount] = { -128, -58, 56, -10, -84, 48 };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.05f, 0.06f, 0.94f);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(winW, 0);
        glVertex2i(winW, winH);
        glVertex2i(0, winH);
    glEnd();

    for (int i = 0; i < lineCount; ++i) {
        const char* line = kDrawerBookOverlayLines[i];
        const int lineWidth = overlayTextWidth(line);
        const int baseX = winW / 2 - lineWidth / 2;
        const int y = yStart - i * lineHeight;
        drawOverlayTextLine(baseX + xOffsets[i], y, line);
    }

    glDisable(GL_BLEND);
}

void drawBitmapString(int x, int y, void* font, const std::string& text,
                      float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2i(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void drawCodeBoxOverlay(int winW, int winH) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.04f, 0.04f, 0.05f, 0.95f);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(winW, 0);
        glVertex2i(winW, winH);
        glVertex2i(0, winH);
    glEnd();

    const int panelLeft = winW / 2 - 220;
    const int panelRight = winW / 2 + 220;
    const int panelBottom = winH / 2 - 120;
    const int panelTop = winH / 2 + 120;

    glColor4f(0.10f, 0.10f, 0.12f, 0.96f);
    glBegin(GL_QUADS);
        glVertex2i(panelLeft, panelBottom);
        glVertex2i(panelRight, panelBottom);
        glVertex2i(panelRight, panelTop);
        glVertex2i(panelLeft, panelTop);
    glEnd();

    glColor3f(0.75f, 0.76f, 0.80f);
    glBegin(GL_LINE_LOOP);
        glVertex2i(panelLeft, panelBottom);
        glVertex2i(panelRight, panelBottom);
        glVertex2i(panelRight, panelTop);
        glVertex2i(panelLeft, panelTop);
    glEnd();

    drawBitmapString(panelLeft + 28, panelTop - 36, GLUT_BITMAP_HELVETICA_18,
                     "Wooden Chest", 0.95f, 0.92f, 0.82f);

    const std::string statusText = codeBoxUnlocked
        ? "Unlocked. The chest is opening."
        : "Enter the 3-letter code and press Enter.";
    drawBitmapString(panelLeft + 28, panelTop - 72, GLUT_BITMAP_HELVETICA_18,
                     statusText, 0.84f, 0.84f, 0.88f);

    glColor4f(0.02f, 0.02f, 0.03f, 0.90f);
    glBegin(GL_QUADS);
        glVertex2i(panelLeft + 28, winH / 2 - 10);
        glVertex2i(panelRight - 28, winH / 2 - 10);
        glVertex2i(panelRight - 28, winH / 2 + 34);
        glVertex2i(panelLeft + 28, winH / 2 + 34);
    glEnd();

    glColor3f(0.65f, 0.66f, 0.72f);
    glBegin(GL_LINE_LOOP);
        glVertex2i(panelLeft + 28, winH / 2 - 10);
        glVertex2i(panelRight - 28, winH / 2 - 10);
        glVertex2i(panelRight - 28, winH / 2 + 34);
        glVertex2i(panelLeft + 28, winH / 2 + 34);
    glEnd();

    const std::string entered = codeBoxInput.empty() ? "_ _ _" : codeBoxInput;
    drawBitmapString(panelLeft + 42, winH / 2 + 6, GLUT_BITMAP_TIMES_ROMAN_24,
                     entered, 0.98f, 0.83f, 0.26f);

    const std::string footer = codeBoxUnlocked
        ? "Press any key or click to return."
        : "Letters only. Backspace deletes.";
    drawBitmapString(panelLeft + 28, panelBottom + 28, GLUT_BITMAP_HELVETICA_12,
                     footer, 0.76f, 0.77f, 0.82f);

    glDisable(GL_BLEND);
}

std::string getInteractionPrompt(float camX, float camY, float camZ) {
    const int itemIndex = getNearbyItem(camX, camY, camZ);
    if (itemIndex < 0) return "";

    const Item& item = worldItems[itemIndex];
    if (item.name == "Brass Key") return "Press E to collect the key";
    if (item.name == "Door") return "Press E to open the door";
    if (item.name == "Code Box" && !codeBoxUnlocked) return "Press E to inspect the chest";
    if (item.name == "Desk Drawer") return "Press E to use the drawer";
    if (item.name == "Drawer Book") return "Press E to read the book";
    return "";
}

float interactionPriorityBonus(const Item& item) {
    if (item.name == "Brass Key" && codeBoxUnlocked) return 0.60f;
    if (item.name == "Drawer Book") return 0.35f;
    if (item.name == "Desk Drawer" && drawerOpen) return -0.22f;
    if (item.name == "Code Box" && codeBoxUnlocked) return -0.28f;
    return 0.0f;
}

bool isDrawerBookCentered(float camX, float camY, float camZ) {
    if (!drawerOpen) return false;

    Item* drawerBook = getItemByName("Drawer Book");
    if (drawerBook == nullptr || drawerBook->pickedUp) return false;

    float dx = drawerBook->x - camX;
    float dy = drawerBook->y - camY;
    float dz = drawerBook->z - camZ;
    float dist = sqrtf(dx * dx + dy * dy + dz * dz);
    if (dist > drawerBook->pickupRadius || dist < 1e-4f) return false;

    float viewX, viewY, viewZ;
    forward3D(viewX, viewY, viewZ);
    dx /= dist;
    dy /= dist;
    dz /= dist;

    return (viewX * dx + viewY * dy + viewZ * dz) > 0.82f;
}

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
            int r = 170, g = 170, b = 170;

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
                    int crease = (static_cast<int>(std::sin((fx * 7.0f + fy * 13.0f) * 3.14159f) * 18.0f));
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

} // namespace

// --- Drawing ---
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
    GLfloat ambient[] = {
        r * ambientBoost,
        g * ambientBoost,
        b * ambientBoost,
        1.0f
    };
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

void drawQuad(
    float x1,float y1,float z1,
    float x2,float y2,float z2,
    float x3,float y3,float z3,
    float x4,float y4,float z4,
    float r, float g, float b,
    TextureId texture,
    float textureScale)
{
    float ux = x2 - x1, uy = y2 - y1, uz = z2 - z1;
    float vx = x3 - x1, vy = y3 - y1, vz = z3 - z1;
    float nx = uy * vz - uz * vy;
    float ny = uz * vx - ux * vz;
    float nz = ux * vy - uy * vx;
    float len = sqrtf(nx * nx + ny * ny + nz * nz);
    if (len > 1e-6f) {
        nx /= len;
        ny /= len;
        nz /= len;
    }

    applyMaterial(r, g, b);
    bindTexture(texture);

    float lenU = sqrtf(ux * ux + uy * uy + uz * uz) * textureScale;
    float lenV = sqrtf(vx * vx + vy * vy + vz * vz) * textureScale;
    if (lenU < 0.001f) lenU = 1.0f;
    if (lenV < 0.001f) lenV = 1.0f;

    glBegin(GL_QUADS);
        glNormal3f(nx, ny, nz);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x1,y1,z1);
        glTexCoord2f(lenU, 0.0f);
        glVertex3f(x2,y2,z2);
        glTexCoord2f(lenU, lenV);
        glVertex3f(x3,y3,z3);
        glTexCoord2f(0.0f, lenV);
        glVertex3f(x4,y4,z4);
    glEnd();

    if (texture != TEX_NONE) glDisable(GL_TEXTURE_2D);
}

// --- Item helpers ---
int getNearbyItem(float camX, float camY, float camZ) {
    int   bestFacing = -1;
    float bestFacingScore = -9999.0f;
    int   closest = -1;
    float closestDist = 9999.0f;
    float viewX, viewY, viewZ;
    forward3D(viewX, viewY, viewZ);

    for (int i = 0; i < (int)worldItems.size(); i++) {
        if (worldItems[i].pickedUp) continue;
        if (worldItems[i].name == "Drawer Book" && !drawerOpen) continue;
        if (worldItems[i].name == "Brass Key" && (!codeBoxUnlocked || codeBoxOpenProgress < 0.72f)) continue;

        float dx = camX - worldItems[i].x;
        float dy = camY - worldItems[i].y;
        float dz = camZ - worldItems[i].z;
        float dist = sqrtf(dx*dx + dy*dy + dz*dz);

        if (dist > worldItems[i].pickupRadius) continue;

        if (dist < closestDist) {
            closest = i;
            closestDist = dist;
        }

        float toItemX = worldItems[i].x - camX;
        float toItemY = worldItems[i].y - camY;
        float toItemZ = worldItems[i].z - camZ;
        float toItemLen = sqrtf(toItemX * toItemX + toItemY * toItemY + toItemZ * toItemZ);
        if (toItemLen < 1e-4f) {
            return i;
        }

        toItemX /= toItemLen;
        toItemY /= toItemLen;
        toItemZ /= toItemLen;

        const float facing = viewX * toItemX + viewY * toItemY + viewZ * toItemZ;
        if (facing < 0.68f) continue;

        const float score = facing * 1.35f - dist * 0.10f + interactionPriorityBonus(worldItems[i]);
        if (score > bestFacingScore) {
            bestFacingScore = score;
            bestFacing = i;
        }
    }

    return bestFacing >= 0 ? bestFacing : closest;
}

Item* getItemByName(const std::string& name) {
    for (auto& item : worldItems)
        if (item.name == name) return &item;
    return nullptr;
}

bool hasInInventory(const std::string& name) {
    for (auto& i : inventory)
        if (i == name) return true;
    return false;
}

bool interactWithNearbyItem(float camX, float camY, float camZ) {
    if (isAnyOverlayActive()) {
        return true;
    }

    int itemIndex = getNearbyItem(camX, camY, camZ);
    if (itemIndex < 0) {
        setHudMessage("Nothing to interact with.", 1.2f);
        return false;
    }

    Item& item = worldItems[itemIndex];
    if (item.name == "Light Switch") {
        roomLightOn = !roomLightOn;
        setHudMessage(roomLightOn ? "You flip the switch. The room light hums on."
                                  : "You flip the switch. The room falls dark.",
                      2.0f);
        return true;
    }
    if (item.name == "Desk Drawer") {
        if (isDrawerBookCentered(camX, camY, camZ)) {
            showDrawerBookOverlay();
            return true;
        }

        drawerOpen = !drawerOpen;
        return true;
    }
    if (item.name == "Drawer Book") {
        showDrawerBookOverlay();
        return true;
    }
    if (item.name == "Code Box") {
        if (codeBoxUnlocked) {
            return true;
        }
        showCodeBoxOverlay();
        return true;
    }
    if (item.name == "Door") {
        if (!hasInInventory("Brass Key")) {
            setHudMessage("The door is locked. A brass key should fit it.", 2.2f);
            return true;
        }

        item.pickedUp = true;
        setHudMessage("The brass key turns. The door swings open.", 2.2f);
        return true;
    }
    if (item.name == "Brass Key") {
        item.pickedUp = true;
        inventory.push_back(item.name);
        setHudMessage("You take the brass key from the chest.", 2.2f);
        return true;
    }
    if (item.action == "pickup") {
        item.pickedUp = true;
        inventory.push_back(item.name);
        setHudMessage(item.interactText.empty() ? ("You pick up the " + item.name + ".")
                                                : item.interactText,
                      2.5f);
        return true;
    }

    if (!item.interactText.empty()) {
        setHudMessage(item.interactText, 2.5f);
        if (item.action == "open") item.pickedUp = true;
        return true;
    }

    setHudMessage("Nothing happens.", 1.2f);
    return false;
}

void showDrawerBookOverlay() {
    drawerBookOverlayActive = true;
    hudMessage.clear();
    hudMessageTimer = 0.0f;
}

void hideDrawerBookOverlay() {
    drawerBookOverlayActive = false;
}

bool handleDrawerBookOverlayInput() {
    if (!drawerBookOverlayActive) {
        return false;
    }

    hideDrawerBookOverlay();
    return true;
}

void showCodeBoxOverlay() {
    codeBoxOverlayActive = true;
    codeBoxInput.clear();
    hudMessage.clear();
    hudMessageTimer = 0.0f;
}

void hideCodeBoxOverlay() {
    codeBoxOverlayActive = false;
    codeBoxInput.clear();
}

bool handleCodeBoxKeypress(unsigned char key) {
    if (!codeBoxOverlayActive) {
        return false;
    }

    if (key == 13 || key == '\r' || key == '\n') {
        if (codeBoxInput == "KEY") {
            codeBoxUnlocked = true;
            hideCodeBoxOverlay();
            setHudMessage("The wooden chest unlocks with a heavy click.", 2.0f);
        } else {
            codeBoxInput.clear();
            setHudMessage("The wooden chest stays locked.", 1.8f);
        }
        return true;
    }

    if (key == 8 || key == 127) {
        if (!codeBoxInput.empty()) codeBoxInput.pop_back();
        return true;
    }

    if (std::isalpha(static_cast<unsigned char>(key)) && codeBoxInput.size() < 3) {
        codeBoxInput.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(key))));
        return true;
    }

    return true;
}

bool isAnyOverlayActive() {
    return drawerBookOverlayActive || codeBoxOverlayActive;
}

// --- HUD ---
void setHudMessage(const std::string& msg, float duration) {
    hudMessage      = msg;
    hudMessageTimer = duration;   // seconds to display
}

void updateHud(float deltaTime) {
    const float targetOpen = codeBoxUnlocked ? 1.0f : 0.0f;
    const float openSpeed = 1.35f;
    if (codeBoxOpenProgress < targetOpen) {
        codeBoxOpenProgress += deltaTime * openSpeed;
        if (codeBoxOpenProgress > targetOpen) codeBoxOpenProgress = targetOpen;
    } else if (codeBoxOpenProgress > targetOpen) {
        codeBoxOpenProgress -= deltaTime * openSpeed;
        if (codeBoxOpenProgress < targetOpen) codeBoxOpenProgress = targetOpen;
    }

    if (hudMessageTimer > 0.0f) {
        hudMessageTimer -= deltaTime;
        if (hudMessageTimer < 0.0f) {
            hudMessageTimer = 0.0f;
            hudMessage      = "";
        }
    }
}

void drawHUD(int winW, int winH) {
    bool lightingEnabled = glIsEnabled(GL_LIGHTING);
    if (lightingEnabled) glDisable(GL_LIGHTING);

    // Switch to 2D screen-space
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    if (drawerBookOverlayActive) {
        drawDrawerBookOverlay(winW, winH);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_COLOR_MATERIAL);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        if (lightingEnabled) glEnable(GL_LIGHTING);
        return;
    }

    if (codeBoxOverlayActive) {
        drawCodeBoxOverlay(winW, winH);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_COLOR_MATERIAL);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        if (lightingEnabled) glEnable(GL_LIGHTING);
        return;
    }

    // ── Crosshair ──────────────────────────────────────────
    int cx = winW / 2, cy = winH / 2;
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2i(cx - 10, cy);  glVertex2i(cx + 10, cy);
        glVertex2i(cx, cy - 10);  glVertex2i(cx, cy + 10);
    glEnd();

    // ── "Press E to …" prompt (centre bottom) ──────────────
    // Caller must set hudMessage before drawing
    const std::string prompt = hudMessageTimer <= 0.0f ? getInteractionPrompt(camX, camY, camZ) : "";
    const std::string hudLine = !hudMessage.empty() ? hudMessage : prompt;
    if (!hudLine.empty()) {
        // Semi-transparent black backing bar
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
            glVertex2i(winW/2 - 180, 30);
            glVertex2i(winW/2 + 180, 30);
            glVertex2i(winW/2 + 180, 58);
            glVertex2i(winW/2 - 180, 58);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2i(winW/2 - 170, 40);
        for (char c : hudLine)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // ── Inventory list (top-left) ───────────────────────────
    if (!inventory.empty()) {
        glColor3f(1.0f, 1.0f, 0.0f);
        int yPos = winH - 30;
        for (auto& itemName : inventory) {
            std::string line = "[+] " + itemName;
            glRasterPos2i(10, yPos);
            for (char c : line)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
            yPos -= 20;
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    if (lightingEnabled) glEnable(GL_LIGHTING);
}
