#include "ResourcesLoad.h"

static Texture2D gProfilePicture;
static Font gFonts[2] = {0};

Font* getFonts() {
	if (gFonts[0].texture.id == 0) {
		gFonts[FONT_ID_BODY_24] = LoadFontEx("resources/Roboto-Regular.ttf", 48, NULL, 400);
		SetTextureFilter(gFonts[FONT_ID_BODY_24].texture, TEXTURE_FILTER_BILINEAR);
		gFonts[FONT_ID_BODY_16] = LoadFontEx("resources/Roboto-Regular.ttf", 32, NULL, 400);
		SetTextureFilter(gFonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
	}
	return gFonts;
}

Texture2D* getProfilePicture() {
	if (gProfilePicture.id == 0) {
		gProfilePicture = LoadTexture("resources/profile-picture.png");
	}
	return &gProfilePicture;
}

void unloadResources() {
	if (gProfilePicture.id != 0) {
		UnloadTexture(gProfilePicture);
		gProfilePicture.id = 0;
	}
	if (gFonts[FONT_ID_BODY_24].texture.id != 0) {
		UnloadFont(gFonts[FONT_ID_BODY_24]);
		gFonts[FONT_ID_BODY_24].texture.id = 0;
	}
	if (gFonts[FONT_ID_BODY_16].texture.id != 0) {
		UnloadFont(gFonts[FONT_ID_BODY_16]);
		gFonts[FONT_ID_BODY_16].texture.id = 0;
	}
}