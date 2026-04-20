#pragma once
#include <string>
#include <vector>

enum TextureId {
    TEX_NONE = 0,
    TEX_GENERIC,
    TEX_PLASTER,
    TEX_FLOOR,
    TEX_WOOD,
    TEX_WOOD_DARK,
    TEX_METAL,
    TEX_FABRIC,
    TEX_PAPER,
    TEX_PLASTIC,
    TEX_BOOK,
    TEX_WALLPAPER,
    TEX_GRASS,
    TEX_BRASS,
    TEX_COUNT
};

// --- Item ---
struct Item {
    std::string name;
    float x, y, z;
    float pickupRadius;
    bool  pickedUp;
    std::string action;
    std::string interactText;
    bool consumeOnUse;

    Item(std::string n, float x, float y, float z, float r = 1.0f,
         std::string action = "inspect",
         std::string interactText = "",
         bool consumeOnUse = false)
        : name(n), x(x), y(y), z(z), pickupRadius(r), pickedUp(false),
          action(action), interactText(interactText), consumeOnUse(consumeOnUse) {}
};

// --- Globals ---
extern std::vector<Item>        worldItems;
extern std::vector<std::string> inventory;
extern std::string              hudMessage;
extern float                    hudMessageTimer;
extern bool                     roomLightOn;
extern bool                     drawerOpen;
extern bool                     drawerBookOverlayActive;
extern bool                     codeBoxOverlayActive;
extern bool                     codeBoxUnlocked;
extern float                    codeBoxOpenProgress;
extern std::string              codeBoxInput;

// --- Drawing ---
void initTextures();
void drawQuad(
    float x1,float y1,float z1,
    float x2,float y2,float z2,
    float x3,float y3,float z3,
    float x4,float y4,float z4,
    float r, float g, float b,
    TextureId texture = TEX_GENERIC,
    float textureScale = 1.0f);
void applyMaterial(float r, float g, float b, float ambientBoost = 0.22f, float shininess = 24.0f);
unsigned int loadExternalTexture(const std::string& path);
void drawExternalTexturedQuad(
    float x1,float y1,float z1,
    float x2,float y2,float z2,
    float x3,float y3,float z3,
    float x4,float y4,float z4,
    float r, float g, float b,
    unsigned int textureId,
    float textureScale = 1.0f);
void drawExternalImageQuad(
    float x1,float y1,float z1,
    float x2,float y2,float z2,
    float x3,float y3,float z3,
    float x4,float y4,float z4,
    float r, float g, float b,
    unsigned int textureId);

// --- Item helpers ---
int   getNearbyItem(float camX, float camY, float camZ);
Item* getItemByName(const std::string& name);
bool  hasInInventory(const std::string& name);
bool  interactWithNearbyItem(float camX, float camY, float camZ);
void  showDrawerBookOverlay();
void  hideDrawerBookOverlay();
bool  handleDrawerBookOverlayInput();
void  showCodeBoxOverlay();
void  hideCodeBoxOverlay();
bool  handleCodeBoxKeypress(unsigned char key);
bool  isAnyOverlayActive();

// --- HUD ---
void setHudMessage(const std::string& msg, float duration = 2.5f);
void updateHud(float deltaTime);
void drawHUD(int winW, int winH);
