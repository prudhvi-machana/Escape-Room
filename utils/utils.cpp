#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include "utils.h"

// --- Global state ---
std::vector<Item>        worldItems;
std::vector<std::string> inventory;
std::string              hudMessage = "";
float                    hudMessageTimer = 0.0f;   // counts down, clears message
bool                     roomLightOn = false;
bool                     drawerOpen = false;

namespace {

GLuint gTextures[11] = {};

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
            int r = 170, g = 170, b = 170;

            switch (id) {
                case TEX_PLASTER: {
                    int base = 188 + (n0 % 28) - 14;
                    int stain = ((x + y) % 17 == 0) ? -18 : 0;
                    r = base + stain;
                    g = base + 4 + stain;
                    b = base + 18 + stain / 2;
                    break;
                }
                case TEX_FLOOR: {
                    int board = (x / 10) % 2;
                    int grain = (n1 % 36) - 18;
                    int seam = (x % 10 == 0) ? -30 : 0;
                    r = 95 + board * 10 + grain + seam;
                    g = 78 + board * 8 + grain / 2 + seam;
                    b = 58 + board * 4 + grain / 3 + seam;
                    break;
                }
                case TEX_WOOD: {
                    int grain = static_cast<int>(std::sin((fx * 18.0f + fy * 3.5f) * 3.14159f) * 16.0f) + (n0 % 22) - 11;
                    r = 132 + grain;
                    g = 88 + grain / 2;
                    b = 46 + grain / 3;
                    break;
                }
                case TEX_WOOD_DARK: {
                    int grain = static_cast<int>(std::sin((fx * 22.0f + fy * 4.0f) * 3.14159f) * 12.0f) + (n0 % 20) - 10;
                    r = 84 + grain;
                    g = 54 + grain / 2;
                    b = 28 + grain / 3;
                    break;
                }
                case TEX_METAL: {
                    int brushed = ((x * 5 + n1) % 30) - 15;
                    int highlight = (y % 16 < 2) ? 16 : 0;
                    r = 128 + brushed + highlight;
                    g = 132 + brushed + highlight;
                    b = 138 + brushed + highlight;
                    break;
                }
                case TEX_FABRIC: {
                    int weave = ((x % 4 == 0) || (y % 4 == 0)) ? -20 : 0;
                    int fuzz = (n0 % 20) - 10;
                    r = 118 + weave + fuzz;
                    g = 82 + weave + fuzz / 2;
                    b = 52 + weave / 2;
                    break;
                }
                case TEX_PAPER: {
                    int speck = (n0 % 24) - 12;
                    int line = (y % 12 == 3) ? -16 : 0;
                    r = 222 + speck + line / 2;
                    g = 214 + speck + line / 2;
                    b = 188 + speck / 2;
                    break;
                }
                case TEX_PLASTIC: {
                    int speck = (n0 % 18) - 9;
                    int band = (x % 14 < 2) ? 10 : 0;
                    r = 90 + speck + band;
                    g = 98 + speck + band;
                    b = 108 + speck + band;
                    break;
                }
                case TEX_BOOK: {
                    int stripe = (x % 22 < 3) ? 28 : 0;
                    r = 154 + stripe + (n1 % 16) - 8;
                    g = 78 + (n0 % 18) - 9;
                    b = 48 + (n1 % 14) - 7;
                    break;
                }
                case TEX_GENERIC:
                default: {
                    int speck = (n0 % 30) - 15;
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
    glGenTextures(11, gTextures);
    const int size = 64;
    unsigned char data[size * size * 3];

    for (int texture = TEX_GENERIC; texture <= TEX_BOOK; ++texture) {
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
    int   closest = -1;
    float closestDist = 9999.0f;

    for (int i = 0; i < (int)worldItems.size(); i++) {
        if (worldItems[i].pickedUp) continue;
        if (worldItems[i].name == "Drawer Note" && !drawerOpen) continue;

        float dx = camX - worldItems[i].x;
        float dy = camY - worldItems[i].y;
        float dz = camZ - worldItems[i].z;
        float dist = sqrtf(dx*dx + dy*dy + dz*dz);

        // Return the CLOSEST item within range, not just the first
        if (dist <= worldItems[i].pickupRadius && dist < closestDist) {
            closest     = i;
            closestDist = dist;
        }
    }
    return closest;
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
        drawerOpen = !drawerOpen;
        setHudMessage(drawerOpen ? "The drawer slides open. A folded note rests inside."
                                 : "You push the drawer shut.",
                      2.0f);
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

// --- HUD ---
void setHudMessage(const std::string& msg, float duration) {
    hudMessage      = msg;
    hudMessageTimer = duration;   // seconds to display
}

void updateHud(float deltaTime) {
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

    // ── Crosshair ──────────────────────────────────────────
    int cx = winW / 2, cy = winH / 2;
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2i(cx - 10, cy);  glVertex2i(cx + 10, cy);
        glVertex2i(cx, cy - 10);  glVertex2i(cx, cy + 10);
    glEnd();

    // ── "Press E to …" prompt (centre bottom) ──────────────
    // Caller must set hudMessage before drawing
    if (!hudMessage.empty()) {
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
        for (char c : hudMessage)
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
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    if (lightingEnabled) glEnable(GL_LIGHTING);
}
