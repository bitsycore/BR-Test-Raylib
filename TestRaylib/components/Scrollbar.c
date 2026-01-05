#include "Scrollbar.h"

ScrollbarData gScrollbarData = {0};

void ScrollBarRender(void) {
	const Clay_ScrollContainerData scrollData = Clay_GetScrollContainerData(Clay_GetElementId(CLAY_STRING("MainContent")));
	if (scrollData.found) {
		CLAY(CLAY_ID("ScrollBar"), {
			.floating = {
				.attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID,
				.offset = { .y = -(scrollData.scrollPosition->y / scrollData.contentDimensions.height) * scrollData.scrollContainerDimensions.height },
				.zIndex = 1,
				.parentId = Clay_GetElementId(CLAY_STRING("MainContent")).id,
				.attachPoints = { .element = CLAY_ATTACH_POINT_RIGHT_TOP, .parent = CLAY_ATTACH_POINT_RIGHT_TOP }
			}
		}) {
			CLAY(CLAY_ID("ScrollBarButton"), {
				.layout.sizing = {
					CLAY_SIZING_FIXED(12),
					CLAY_SIZING_FIXED((scrollData.scrollContainerDimensions.height / scrollData.contentDimensions.height) * scrollData.scrollContainerDimensions.height)
				},
				.backgroundColor = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("ScrollBar")))
					? (Clay_Color){100, 100, 140, 150}
					: (Clay_Color){120, 120, 160, 150},
				.cornerRadius = CLAY_CORNER_RADIUS(6)
			}) {}
		}
	}
}