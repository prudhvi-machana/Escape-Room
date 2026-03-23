#pragma once
#include <string>
#include <vector>

// --- Item ---
struct Item {
    std::string name;
    float x, y, z;
    float pickupRadius;
    bool  pickedUp;

    Item(std::string n, float x, float y, float z, float r = 1.0f)
        : name(n), x(x), y(y), z(z), pickupRadius(r), pickedUp(false) {}
};

// --- Globals ---
extern std::vector<Item>        worldItems;
extern std::vector<std::string> inventory;
extern std::string              hudMessage;
extern float                    hudMessageTimer;

// --- Drawing ---
void drawQuad(
    float x1,float y1,float z1,
    float x2,float y2,float z2,
    float x3,float y3,float z3,
    float x4,float y4,float z4,
    float r, float g, float b);

// --- Item helpers ---
int   getNearbyItem(float camX, float camY, float camZ);
Item* getItemByName(const std::string& name);
bool  hasInInventory(const std::string& name);

// --- HUD ---
void setHudMessage(const std::string& msg, float duration = 2.5f);
void updateHud(float deltaTime);
void drawHUD(int winW, int winH);