#include <cmath>
#include <cctype>
#include "../camera/camera.h"
#include "utils.h"

namespace {

constexpr float kBrassKeyRevealProgress = 0.72f;

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
    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (dist > drawerBook->pickupRadius || dist < 1e-4f) return false;

    float viewX = 0.0f;
    float viewY = 0.0f;
    float viewZ = 0.0f;
    forward3D(viewX, viewY, viewZ);
    dx /= dist;
    dy /= dist;
    dz /= dist;

    return (viewX * dx + viewY * dy + viewZ * dz) > 0.82f;
}

} // namespace

int getNearbyItem(float camX, float camY, float camZ) {
    int bestFacing = -1;
    float bestFacingScore = -9999.0f;
    int closest = -1;
    float closestDist = 9999.0f;
    float viewX = 0.0f;
    float viewY = 0.0f;
    float viewZ = 0.0f;
    forward3D(viewX, viewY, viewZ);

    for (int i = 0; i < static_cast<int>(worldItems.size()); ++i) {
        if (worldItems[i].pickedUp) continue;
        if (worldItems[i].name == "Drawer Book" && !drawerOpen) continue;
        if (worldItems[i].name == "Brass Key" && (!codeBoxUnlocked || codeBoxOpenProgress < kBrassKeyRevealProgress)) continue;

        const float dx = camX - worldItems[i].x;
        const float dy = camY - worldItems[i].y;
        const float dz = camZ - worldItems[i].z;
        const float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (dist > worldItems[i].pickupRadius) continue;

        if (dist < closestDist) {
            closest = i;
            closestDist = dist;
        }

        float toItemX = worldItems[i].x - camX;
        float toItemY = worldItems[i].y - camY;
        float toItemZ = worldItems[i].z - camZ;
        const float toItemLen = std::sqrt(toItemX * toItemX + toItemY * toItemY + toItemZ * toItemZ);
        if (toItemLen < 1e-4f) return i;

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
    for (std::vector<Item>::iterator it = worldItems.begin(); it != worldItems.end(); ++it) {
        if (it->name == name) return &(*it);
    }
    return nullptr;
}

bool hasInInventory(const std::string& name) {
    for (std::vector<std::string>::const_iterator it = inventory.begin(); it != inventory.end(); ++it) {
        if (*it == name) return true;
    }
    return false;
}

bool interactWithNearbyItem(float camX, float camY, float camZ) {
    if (isAnyOverlayActive()) {
        return true;
    }

    const int itemIndex = getNearbyItem(camX, camY, camZ);
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
        if (codeBoxUnlocked) return true;
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
    if (!drawerBookOverlayActive) return false;
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
    if (!codeBoxOverlayActive) return false;

    if (key == 27) {
        hideCodeBoxOverlay();
        return true;
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
