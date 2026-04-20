#include "utils.h"

std::vector<Item>        worldItems;
std::vector<std::string> inventory;
std::string              hudMessage = "";
float                    hudMessageTimer = 0.0f;
bool                     roomLightOn = false;
bool                     drawerOpen = false;
bool                     drawerBookOverlayActive = false;
bool                     codeBoxOverlayActive = false;
bool                     codeBoxUnlocked = false;
float                    codeBoxOpenProgress = 0.0f;
std::string              codeBoxInput = "";
