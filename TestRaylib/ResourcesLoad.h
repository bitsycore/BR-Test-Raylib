#ifndef BR_TEST_RAYLIB_RESOURCESLOAD_H
#define BR_TEST_RAYLIB_RESOURCESLOAD_H

#include <stdint.h>
#include "raylib.h"

static const uint32_t FONT_ID_BODY_24 = 0;
static const uint32_t FONT_ID_BODY_16 = 1;

Font* getFonts();
Texture2D* getProfilePicture();
void unloadResources();

#endif //BR_TEST_RAYLIB_RESOURCESLOAD_H