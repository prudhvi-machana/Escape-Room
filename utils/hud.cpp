#include <GL/glut.h>
#include <cctype>
#include <string>
#include "../camera/camera.h"
#include "utils.h"

namespace {

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
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        glutBitmapCharacter(font, *it);
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
        ? "Press any key, click, or Esc to return."
        : "Letters only. Backspace deletes. Esc closes.";
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

void restoreHudMatrices(bool lightingEnabled) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    if (lightingEnabled) glEnable(GL_LIGHTING);
}

} // namespace

void setHudMessage(const std::string& msg, float duration) {
    hudMessage = msg;
    hudMessageTimer = duration;
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
            hudMessage = "";
        }
    }
}

void drawHUD(int winW, int winH) {
    const bool lightingEnabled = glIsEnabled(GL_LIGHTING);
    if (lightingEnabled) glDisable(GL_LIGHTING);

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
        restoreHudMatrices(lightingEnabled);
        return;
    }

    if (codeBoxOverlayActive) {
        drawCodeBoxOverlay(winW, winH);
        restoreHudMatrices(lightingEnabled);
        return;
    }

    const int cx = winW / 2;
    const int cy = winH / 2;
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2i(cx - 10, cy); glVertex2i(cx + 10, cy);
        glVertex2i(cx, cy - 10); glVertex2i(cx, cy + 10);
    glEnd();

    const std::string prompt = hudMessageTimer <= 0.0f ? getInteractionPrompt(camX, camY, camZ) : "";
    const std::string hudLine = !hudMessage.empty() ? hudMessage : prompt;
    if (!hudLine.empty()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
            glVertex2i(winW / 2 - 180, 30);
            glVertex2i(winW / 2 + 180, 30);
            glVertex2i(winW / 2 + 180, 58);
            glVertex2i(winW / 2 - 180, 58);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2i(winW / 2 - 170, 40);
        for (std::string::const_iterator it = hudLine.begin(); it != hudLine.end(); ++it) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *it);
        }
    }

    if (!inventory.empty()) {
        glColor3f(1.0f, 1.0f, 0.0f);
        int yPos = winH - 30;
        for (std::vector<std::string>::const_iterator it = inventory.begin(); it != inventory.end(); ++it) {
            const std::string line = "[+] " + *it;
            glRasterPos2i(10, yPos);
            for (std::string::const_iterator ch = line.begin(); ch != line.end(); ++ch) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *ch);
            }
            yPos -= 20;
        }
    }

    restoreHudMatrices(lightingEnabled);
}
