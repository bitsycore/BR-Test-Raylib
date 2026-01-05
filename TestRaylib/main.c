#define BF_IMPLEMENT_MAIN
#include "BFramework/BF.h"

#define CLAY_IMPLEMENTATION
#include "3rdparty/clay.h"
#include "renderer/raylib/clayray.h"

#include "ResourcesLoad.h"

#include "BCore/Memory/BC_Memory.h"
#include "components/Blob4Floating2.h"
#include "components/RightPanel.h"
#include "components/Scrollbar.h"
#include "components/Sidebar.h"

static bool gDebugEnabled = true;
static bool gReinitializeClay = false;

void HandleClayErrors(const Clay_ErrorData errorData) {
	printf("%s", errorData.errorText.chars);
	if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
		gReinitializeClay = true;
		Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
	}
	else if (errorData.errorType == CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
		gReinitializeClay = true;
		Clay_SetMaxMeasureTextCacheWordCount(Clay_GetMaxMeasureTextCacheWordCount() * 2);
	}
}

Clay_RenderCommandArray CreateLayout(void) {
	Clay_BeginLayout();
	CLAY(CLAY_ID("OuterContainer"), {
		.layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, .padding = { 16, 16, 16, 16 }, .childGap = 16 },
		.backgroundColor = {200, 200, 200, 255}
	}) {
		SideBarRender();
		RightPanelRender();
		Blob4Floating2Render();
		ScrollBarRender();
	}
	return Clay_EndLayout();
}

void UpdateDrawFrame(const Font* fonts) {
	const Vector2 mouseWheelDelta = GetMouseWheelMoveV();
	const float mouseWheelX = mouseWheelDelta.x;
	const float mouseWheelY = mouseWheelDelta.y;

	if (IsKeyPressed(KEY_D)) {
		gDebugEnabled = !gDebugEnabled;
		Clay_SetDebugModeEnabled(gDebugEnabled);
	}
	//----------------------------------------------------------------------------------
	// Handle scroll containers
	const Clay_Vector2 mousePosition = RAY_VECTOR2_TO_CLAY(GetMousePosition());
	Clay_SetPointerState(mousePosition, IsMouseButtonDown(0) && !gScrollbarData.mouseDown);
	Clay_SetLayoutDimensions((Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});
	if (!IsMouseButtonDown(0)) {
		gScrollbarData.mouseDown = false;
	}

	if (IsMouseButtonDown(0) && !gScrollbarData.mouseDown && Clay_PointerOver(Clay_GetElementId(CLAY_STRING("ScrollBar")))) {
		const Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay_GetElementId(CLAY_STRING("MainContent")));
		gScrollbarData.clickOrigin = mousePosition;
		gScrollbarData.positionOrigin = *scrollContainerData.scrollPosition;
		gScrollbarData.mouseDown = true;
	}
	else if (gScrollbarData.mouseDown) {
		const Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay_GetElementId(CLAY_STRING("MainContent")));
		if (scrollContainerData.contentDimensions.height > 0) {
			const Clay_Vector2 ratio = (Clay_Vector2){
				scrollContainerData.contentDimensions.width / scrollContainerData.scrollContainerDimensions.width,
				scrollContainerData.contentDimensions.height / scrollContainerData.scrollContainerDimensions.height,
			};
			if (scrollContainerData.config.vertical) {
				scrollContainerData.scrollPosition->y = gScrollbarData.positionOrigin.y + (gScrollbarData.clickOrigin.y - mousePosition.y) * ratio.y;
			}
			if (scrollContainerData.config.horizontal) {
				scrollContainerData.scrollPosition->x = gScrollbarData.positionOrigin.x + (gScrollbarData.clickOrigin.x - mousePosition.x) * ratio.x;
			}
		}
	}

	Clay_UpdateScrollContainers(true, (Clay_Vector2){mouseWheelX, mouseWheelY}, GetFrameTime());

	const Clay_RenderCommandArray renderCommands = CreateLayout();
	BeginDrawing();
	ClearBackground(BLACK);
	ClayRay_Render(renderCommands, fonts);
	EndDrawing();
}

int BF_Main(void) {

	// ==================================
	// Clay Initialization
	uint64_t vClayMinSize = Clay_MinMemorySize();
	Clay_Arena vClayMemory = Clay_CreateArenaWithCapacityAndMemory(vClayMinSize, BC_Malloc(vClayMinSize));
	Clay_Initialize(vClayMemory, (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors, 0});

	// ==================================
	// Raylib Initialization
	SetTraceLogLevel(0);
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
	InitWindow(1024, 768, "Clay - Raylib Renderer Example");

	// ==================================
	// Load resources
	Clay_SetMeasureTextFunction(ClayRay_MeasureText, getFonts());

	// ==================================
	// Main loop
	while (!WindowShouldClose()) {
		if (gReinitializeClay) {
			Clay_SetMaxElementCount(8192);
			vClayMinSize = Clay_MinMemorySize();
			vClayMemory = Clay_CreateArenaWithCapacityAndMemory(vClayMinSize, BC_Malloc(vClayMinSize));
			Clay_Initialize(vClayMemory, (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors, 0});
			gReinitializeClay = false;
		}
		UpdateDrawFrame(getFonts());
	}

	// ==================================
	// Cleanup
	unloadResources();
	BC_Free(vClayMemory.memory);
	ClayRay_Cleanup();

	return 0;
}