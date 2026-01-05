#ifndef BR_TEST_RAYLIB_SCROLLBAR_H
#define BR_TEST_RAYLIB_SCROLLBAR_H

#include "../3rdparty/clay.h"

typedef struct ScrollbarData {
	Clay_Vector2 clickOrigin;
	Clay_Vector2 positionOrigin;
	bool mouseDown;
} ScrollbarData;

extern ScrollbarData gScrollbarData;

void ScrollBarRender(void);

#endif //BR_TEST_RAYLIB_SCROLLBAR_H