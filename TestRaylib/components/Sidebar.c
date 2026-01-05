#include "Sidebar.h"

#include "../ResourcesLoad.h"
#include "../3rdparty/clay.h"

static Clay_String gProfileText = CLAY_STRING_CONST("Profile Page one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen");

static void SidebarBlob(const Clay_ElementId clay_element_id) {
	CLAY(clay_element_id, {
		.layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) } },
		.backgroundColor = {110, 110, 255, 255},
		.cornerRadius = CLAY_CORNER_RADIUS(8)
	}) {}
}

void SideBarRender(void) {
	CLAY(CLAY_ID("SideBar"), {
		.layout = {
			.layoutDirection = CLAY_TOP_TO_BOTTOM,
			.sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) },
			.padding = {16, 16, 16, 16 },
			.childGap = 16
		},
		.backgroundColor = {150, 150, 255, 255}
	}) {
		CLAY(CLAY_ID("ProfilePictureOuter"), {
			.layout = {
				.sizing = { .width = CLAY_SIZING_GROW(0) },
				.padding = { 8, 8, 8, 8 },
				.childGap = 8,
				.childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
			},
			.backgroundColor = {130, 130, 255, 255},
			.cornerRadius = CLAY_CORNER_RADIUS(8)
		}) {
			CLAY(CLAY_ID("ProfilePicture"), {
				.layout = { .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) } },
				.image = { .imageData = getProfilePicture() }
			}) {}
			CLAY_TEXT(gProfileText, CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = {0, 0, 0, 255}, .textAlignment = CLAY_TEXT_ALIGN_RIGHT }));
		}
		SidebarBlob(CLAY_ID("SidebarBlob1"));
		SidebarBlob(CLAY_ID("SidebarBlob2"));
		SidebarBlob(CLAY_ID("SidebarBlob3"));
		SidebarBlob(CLAY_ID("SidebarBlob4"));
	}
}
