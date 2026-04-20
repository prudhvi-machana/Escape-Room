#include "room.h"

#include "room_internal.h"

#include "../objects/object_utils.h"
#include "../utils/utils.h"

void initRoom() {
    room_internal::initSkyboxTextures();
    room_internal::gFloorTexture = loadExternalTexture("resources/Mantel.ppm");
    worldItems.clear();
    drawerOpen = false;
    codeBoxUnlocked = false;
    codeBoxOpenProgress = 0.0f;
    codeBoxInput.clear();

    worldItems.push_back(Item(
        "Door", 0.0f, 1.25f, 5.0f, 2.0f,
        "open",
        "The door is locked tight for now."
    ));
    worldItems.push_back(Item(
        "Highlighted Book", 4.15f, 1.85f, -3.14f, 1.1f,
        "read",
        "Book note: 'The room rewards careful observation.'"
    ));
    worldItems.push_back(Item(
        "Dustbin Paper", 4.50f, 0.38f, 4.52f, 1.0f,
        "inspect",
        "The paper says: 'Books belong on shelves... so why is one not?'"
    ));
    worldItems.push_back(Item(
        "Light Switch", -4.55f, 1.25f, 1.26f, 0.9f,
        "toggle",
        "Left click to flip the room light."
    ));
    worldItems.push_back(Item(
        "Desk Drawer", -2.90f, 0.84f, -2.30f, 1.15f,
        "open",
        "A shallow drawer is built into the table."
    ));
    worldItems.push_back(Item(
        "Drawer Book", -2.90f, 0.85f, -2.08f, 0.75f,
        "read",
        "The secret code to your destination is u4u03."
    ));
    worldItems.push_back(Item(
        "Code Box", 3.90f, 0.84f, 1.45f, 1.75f,
        "inspect",
        "A large wooden chest waits for a three-letter code."
    ));
    worldItems.push_back(Item(
        "Brass Key", 3.94f, 0.28f, 1.72f, 1.08f,
        "pickup",
        ""
    ));
}

void drawRoom() {
    room_internal::drawOutdoorScene();

    if (room_internal::gFloorTexture != 0) {
        drawExternalTexturedQuad(
            -5,0,-5,   5,0,-5,   5,0,5,   -5,0,5,
            1.0f, 1.0f, 1.0f, room_internal::gFloorTexture, 0.55f
        );
    } else {
        drawQuad(
            -5,0,-5,   5,0,-5,   5,0,5,   -5,0,5,
            0.44f, 0.41f, 0.38f, TEX_FLOOR, 0.48f
        );
    }

    drawQuad(
        -5,4,-5,   5,4,-5,   5,4,5,   -5,4,5,
        0.78f, 0.76f, 0.70f, TEX_PLASTER, 0.65f
    );

    drawQuad(
        -5,0,-5,   5,0,-5,   5,4,-5,   -5,4,-5,
        0.58f, 0.54f, 0.46f, TEX_WALLPAPER, 0.55f
    );

    drawQuad(
        -5,0,-5,   -5,0,5,   -5,4,5,   -5,4,-5,
        0.56f, 0.50f, 0.42f, TEX_WALLPAPER, 0.55f
    );

    drawQuad(
        5,0,-5,   5,0,5,   5,4,5,   5,4,-5,
        0.56f, 0.50f, 0.42f, TEX_WALLPAPER, 0.55f
    );

    drawBox(
        -5.0f, 0.0f, room_internal::kFrontWallInnerZ,  -1.0f, 4.0f, room_internal::kFrontWallOuterZ,
        0.60f, 0.55f, 0.47f, TEX_WALLPAPER, 0.55f
    );

    drawBox(
        1.0f, 0.0f, room_internal::kFrontWallInnerZ,   5.0f, 4.0f, room_internal::kFrontWallOuterZ,
        0.60f, 0.55f, 0.47f, TEX_WALLPAPER, 0.55f
    );

    drawBox(
        -1.0f, 2.5f, room_internal::kFrontWallInnerZ,   1.0f, 4.0f, room_internal::kFrontWallOuterZ,
        0.60f, 0.55f, 0.47f, TEX_WALLPAPER, 0.55f
    );

    room_internal::drawRoomTrim();
    room_internal::drawCeilingLightFixture();
    room_internal::drawLightSwitch();
    room_internal::drawCodeBox();
}

void drawDoor() {
    Item* door = getItemByName("Door");
    bool isOpen = door && door->pickedUp;
    room_internal::drawDoorShadow(isOpen);

    glPushMatrix();
        glTranslatef(-1.0f, 0.0f, 5.0f);
        glRotatef(isOpen ? -90.0f : 0.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);

        float brightness = isOpen ? 0.75f : 0.6f;
        drawQuad(
            -1,0,0,   1,0,0,   1,2.5,0,   -1,2.5,0,
            brightness, 0.36f, 0.12f, TEX_WOOD_DARK, 1.1f
        );

        float hr = isOpen ? 0.5f : 1.0f;
        float hg = isOpen ? 0.5f : 0.8f;
        float hb = isOpen ? 0.5f : 0.0f;
        drawQuad(
            0.6f, 1.1f, 0.01f,
            0.8f, 1.1f, 0.01f,
            0.8f, 1.3f, 0.01f,
            0.6f, 1.3f, 0.01f,
            hr, hg, hb, TEX_PLASTIC, 4.0f
        );

        drawQuad(
            0.67f, 0.85f, 0.01f,
            0.73f, 0.85f, 0.01f,
            0.73f, 0.95f, 0.01f,
            0.67f, 0.95f, 0.01f,
            0.2f, 0.2f, 0.2f, TEX_METAL, 4.0f
        );

        drawBox(0.72f, 1.06f, -0.02f, 0.84f, 1.18f, 0.05f,
                0.86f, 0.70f, 0.22f, TEX_BRASS, 5.0f);

    glPopMatrix();

    drawBox(
        -1.15f, 0.0f, room_internal::kDoorFrameInnerZ,  -1.0f, 2.65f, room_internal::kDoorFrameOuterZ,
        0.30f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
    drawBox(
        1.0f, 0.0f, room_internal::kDoorFrameInnerZ,   1.15f, 2.65f, room_internal::kDoorFrameOuterZ,
        0.30f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
    drawBox(
        -1.15f, 2.52f, room_internal::kDoorFrameInnerZ,   1.15f, 2.66f, room_internal::kDoorFrameOuterZ,
        0.30f, 0.18f, 0.05f, TEX_WOOD_DARK, 0.8f
    );
}
