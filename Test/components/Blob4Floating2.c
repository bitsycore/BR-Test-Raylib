#include "Blob4Floating2.h"

#include "../3rdparty/clay.h"

static Clay_LayoutConfig gDropdownTextItemLayout = {.padding = {8, 8, 4, 4}};
static Clay_TextElementConfig gDropdownTextElementConfig = {.fontSize = 24, .textColor = {255, 255, 255, 255}};

static void RenderDropdownTextItem(int index) {
	CLAY_AUTO_ID({ .layout = gDropdownTextItemLayout}) {
		CLAY_TEXT(CLAY_STRING("I'm a text field in a scroll container."), &gDropdownTextElementConfig);
	}
}

void Blob4Floating2Render(void) {
	CLAY(CLAY_ID("Blob4Floating2"), {
		.floating = { .attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID, .zIndex = 1, .parentId = Clay_GetElementId(CLAY_STRING("SidebarBlob4")).id },
		.cornerRadius = CLAY_CORNER_RADIUS(8),
		.clip = {true, true}
	}) {
		CLAY(CLAY_ID("ScrollContainer"), {
			.layout = { .sizing = { .height = CLAY_SIZING_FIXED(200) }, .childGap = 2 },
			.clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
		}) {
			CLAY(CLAY_ID("FloatingContainer2"), {
				.layout.sizing.height = CLAY_SIZING_GROW(),
				.floating = { .attachTo = CLAY_ATTACH_TO_PARENT, .zIndex = 1 },
			}) {
				CLAY(CLAY_ID("FloatingContainerInner"), {
					.layout = { .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW() }, .padding = {16, 16, 16, 16} },
					.backgroundColor = {140, 80, 200, 200},
				}) {
					CLAY_TEXT(CLAY_STRING("I'm an inline floating container."), CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = {255,255,255,255} }));
				}
			}
			CLAY(CLAY_ID("ScrollContainerInner"), {
				.layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM },
				.backgroundColor = {160, 160, 160, 255},
			}) {
				for (int i = 0; i < 100; i++) {
					RenderDropdownTextItem(i);
				}
			}
		}
	}
}