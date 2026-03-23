#include <GL/glut.h>
#include <cmath>
#include <string>
#include "utils.h"

// --- Global state ---
std::vector<Item>        worldItems;
std::vector<std::string> inventory;
std::string              hudMessage = "";
float                    hudMessageTimer = 0.0f;   // counts down, clears message

// --- Drawing ---
void drawQuad(
    float x1,float y1,float z1,
    float x2,float y2,float z2,
    float x3,float y3,float z3,
    float x4,float y4,float z4,
    float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex3f(x1,y1,z1);
        glVertex3f(x2,y2,z2);
        glVertex3f(x3,y3,z3);
        glVertex3f(x4,y4,z4);
    glEnd();
}

// --- Item helpers ---
int getNearbyItem(float camX, float camY, float camZ) {
    int   closest = -1;
    float closestDist = 9999.0f;

    for (int i = 0; i < (int)worldItems.size(); i++) {
        if (worldItems[i].pickedUp) continue;

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
}