#ifndef BR_TEST_RAYLIB_CLAY_RENDERER_RAYLIB_H
#define BR_TEST_RAYLIB_CLAY_RENDERER_RAYLIB_H

#include "clay.h"
#include "raylib.h"

typedef enum CustomLayoutElementType {
	CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL
} CustomLayoutElementType;

typedef struct CustomLayoutElement_3DModel {
	Model model;
	float scale;
	Vector3 position;
	Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct CustomLayoutElement {
	CustomLayoutElementType type;

	union {
		CustomLayoutElement_3DModel model;
	} customData;
} CustomLayoutElement;

#define RAY_VECTOR2_TO_CLAY(vector) (Clay_Vector2) { .x = (vector).x, .y = (vector).y }
#define CLAY_RECTANGLE_TO_RAY(rectangle) (Rectangle) { .x = rectangle.x, .y = rectangle.y, .width = rectangle.width, .height = rectangle.height }
#define CLAY_COLOR_TO_RAY(color) (Color) { .r = (unsigned char)roundf(color.r), .g = (unsigned char)roundf(color.g), .b = (unsigned char)roundf(color.b), .a = (unsigned char)roundf(color.a) }

void ClayRay_Render(Clay_RenderCommandArray renderCommands, const Font* fonts);
void ClayRay_Cleanup();

Clay_Dimensions ClayRay_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);

#endif //BR_TEST_RAYLIB_CLAY_RENDERER_RAYLIB_H