#include "claysdl3.h"

#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

static int MAX_CIRCLE_SEGMENTS = 24;

static void SDL_Clay_RenderFillRoundedRect(Clay_SDL3RendererData *rendererData, const SDL_FRect rect, const float cornerRadius, const Clay_Color _color) {
    // 1. Setup Colors
    // Inner color is full opacity, Outer color is transparent (for AA fade)
    const float alpha = _color.a / 255.0f;
    const SDL_FColor colInner = { _color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, alpha };
    const SDL_FColor colOuter = { _color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, 0.0f };

    // 2. Geometry Setup
    // We expand/contract by 0.5px to create a 1px wide antialiasing strip.
    const float halfStroke = 0.5f;
    const float minDimension = SDL_min(rect.w, rect.h) / 2.0f;
    const float r = SDL_clamp(cornerRadius, 0.0f, minDimension);

    // Adaptive segments based on size (small corners don't need 32 segments)
    int numSegments = SDL_clamp((int)(r * 0.5f), 6, MAX_CIRCLE_SEGMENTS);

    // Radii for the inner solid core and the outer transparent edge
    // If r is very small, we clamp inner to 0 to avoid artifacts.
    const float rInner = SDL_max(r - halfStroke, 0.0f);
    const float rOuter = r + halfStroke;

    // 3. Memory Allocation
    // 4 Hubs + 4 Corners * (numSegments + 1) * 2 (Inner+Outer)
    int totalVertices = 4 + 4 * (numSegments + 1) * 2;

    // Indices:
    // Center Quad: 6
    // 4 Side Quads (Fill): 4 * 6
    // 4 Side Quads (AA): 4 * 6
    // 4 Corner Fans (Fill): 4 * numSegments * 3
    // 4 Corner Strips (AA): 4 * numSegments * 6
    int totalIndices = 6 + 24 + 24 + 4 * numSegments * 3 + 4 * numSegments * 6;

    // Use stack if small, or heap if large (SDL_small_alloc handles this logic automatically in internal SDL, but here we use VLA or standard heap)
    // For standard UI, this fits on the stack easily.
    SDL_Vertex vertices[totalVertices];
    int indices[totalIndices];

    int vIndex = 0;
    int iIndex = 0;

    // 4. Define "Hub" Vertices (The 4 corners of the inner rectangle)
    // These act as the center pivot for the corner fans.
    float hx = rect.x + r;
    float hy = rect.y + r;
    float hw = rect.w - 2.0f * r;
    float hh = rect.h - 2.0f * r;

    // Order: TL, TR, BR, BL
    SDL_FPoint hubs[4] = {
        { hx,      hy },      // 0: TL
        { hx + hw, hy },      // 1: TR
        { hx + hw, hy + hh }, // 2: BR
        { hx,      hy + hh }  // 3: BL
    };

    // Add Hubs to vertex buffer
    int hubStart = vIndex;
    for(int i=0; i<4; i++) {
        vertices[vIndex++] = (SDL_Vertex){ hubs[i], colInner, {0,0} };
    }

    // Fill Center Quad (Connects the 4 hubs)
    indices[iIndex++] = hubStart + 0; indices[iIndex++] = hubStart + 1; indices[iIndex++] = hubStart + 2;
    indices[iIndex++] = hubStart + 0; indices[iIndex++] = hubStart + 2; indices[iIndex++] = hubStart + 3;

    // 5. Generate Corners & Sides
    // We loop through 4 corners. For each corner, we generate the arc vertices.
    // We also bridge the gap to the *next* corner (Side connections).

    struct { float startAng; float endAng; int hubIdx; } corners[4] = {
        { SDL_PI_F,       1.5f * SDL_PI_F, 0 }, // TL
        { 1.5f * SDL_PI_F, 2.0f * SDL_PI_F, 1 }, // TR
        { 0.0f,           0.5f * SDL_PI_F, 2 }, // BR
        { 0.5f * SDL_PI_F, SDL_PI_F,       3 }  // BL
    };

    // Keep track of the indices of the "Start" and "End" of the previous corner's arc
    // to connect them with quads (Sides).
    int firstCornerStartInner = -1;
    int firstCornerStartOuter = -1;
    int prevCornerEndInner = -1;
    int prevCornerEndOuter = -1;

    for (int c = 0; c < 4; c++) {
        int currentHub = corners[c].hubIdx;
        float angleStep = (corners[c].endAng - corners[c].startAng) / (float)numSegments;

        int currentCornerStartInner = -1;
        int currentCornerStartOuter = -1;

        // Generate Arc Vertices
        for (int i = 0; i <= numSegments; i++) {
            float angle = corners[c].startAng + (float)i * angleStep;
            float cosA = SDL_cosf(angle);
            float sinA = SDL_sinf(angle);

            // Inner Vertex (Full Alpha)
            SDL_FPoint posInner = { hubs[c].x + cosA * rInner, hubs[c].y + sinA * rInner };
            vertices[vIndex] = (SDL_Vertex){ posInner, colInner, {0,0} };
            int idxInner = vIndex++;

            // Outer Vertex (Zero Alpha)
            SDL_FPoint posOuter = { hubs[c].x + cosA * rOuter, hubs[c].y + sinA * rOuter };
            vertices[vIndex] = (SDL_Vertex){ posOuter, colOuter, {0,0} };
            int idxOuter = vIndex++;

            // Capture start/end indices for side connections
            if (i == 0) {
                currentCornerStartInner = idxInner;
                currentCornerStartOuter = idxOuter;
                if (c == 0) {
                    firstCornerStartInner = idxInner;
                    firstCornerStartOuter = idxOuter;
                }
            }

            // Tesselate the Arc
            if (i > 0) {
                int prevInner = idxInner - 2;
                int prevOuter = idxOuter - 2;

                // 1. Triangle Fan (Core Fill): Hub -> PrevInner -> CurrInner
                indices[iIndex++] = currentHub;
                indices[iIndex++] = prevInner;
                indices[iIndex++] = idxInner;

                // 2. Triangle Strip (AA Fringe): PrevInner -> PrevOuter -> CurrOuter -> CurrInner
                indices[iIndex++] = prevInner;
                indices[iIndex++] = prevOuter;
                indices[iIndex++] = idxOuter;

                indices[iIndex++] = prevInner;
                indices[iIndex++] = idxOuter;
                indices[iIndex++] = idxInner;
            }
        }

        int currentCornerEndInner = vIndex - 2;
        int currentCornerEndOuter = vIndex - 1;

        // Connect Side (Rectangle between this corner's start and previous corner's end)
        // We do this for corners 1, 2, 3. For corner 0, we do it at the very end (loop close).
        if (c > 0) {
            int prevHub = corners[c-1].hubIdx;

            // Side Fill (Hub -> Hub -> Inner -> Inner)
            indices[iIndex++] = prevHub;
            indices[iIndex++] = currentHub;
            indices[iIndex++] = currentCornerStartInner;

            indices[iIndex++] = prevHub;
            indices[iIndex++] = currentCornerStartInner;
            indices[iIndex++] = prevCornerEndInner;

            // Side AA (Inner -> Outer -> Outer -> Inner)
            indices[iIndex++] = prevCornerEndInner;
            indices[iIndex++] = prevCornerEndOuter;
            indices[iIndex++] = currentCornerStartOuter;

            indices[iIndex++] = prevCornerEndInner;
            indices[iIndex++] = currentCornerStartOuter;
            indices[iIndex++] = currentCornerStartInner;
        }

        prevCornerEndInner = currentCornerEndInner;
        prevCornerEndOuter = currentCornerEndOuter;
    }

    // Connect the final side (Left Edge: Corner 3 End -> Corner 0 Start)
    int lastHub = 3;
    int firstHub = 0;

    // Side Fill
    indices[iIndex++] = lastHub;
    indices[iIndex++] = firstHub;
    indices[iIndex++] = firstCornerStartInner;

    indices[iIndex++] = lastHub;
    indices[iIndex++] = firstCornerStartInner;
    indices[iIndex++] = prevCornerEndInner;

    // Side AA
    indices[iIndex++] = prevCornerEndInner;
    indices[iIndex++] = prevCornerEndOuter;
    indices[iIndex++] = firstCornerStartOuter;

    indices[iIndex++] = prevCornerEndInner;
    indices[iIndex++] = firstCornerStartOuter;
    indices[iIndex++] = firstCornerStartInner;

    // 6. Draw
    SDL_RenderGeometry(rendererData->renderer, NULL, vertices, vIndex, indices, iIndex);
}

static void SDL_Clay_RenderBorder(Clay_SDL3RendererData *rendererData, const SDL_FRect rect, const Clay_BorderRenderData *config) {

    float alpha = config->color.a / 255.0f;
    SDL_FColor colSolid = { config->color.r / 255.0f, config->color.g / 255.0f, config->color.b / 255.0f, alpha };
    SDL_FColor colTrans = { config->color.r / 255.0f, config->color.g / 255.0f, config->color.b / 255.0f, 0.0f };

    // 2. Setup Geometry Constraints
    // Clamp radii to half the smallest dimension to prevent corners overlapping
    float minDim = SDL_min(rect.w, rect.h) / 2.0f;
    Clay_CornerRadius r = config->cornerRadius;
    r.topLeft = SDL_clamp(r.topLeft, 0.0f, minDim);
    r.topRight = SDL_clamp(r.topRight, 0.0f, minDim);
    r.bottomRight = SDL_clamp(r.bottomRight, 0.0f, minDim);
    r.bottomLeft = SDL_clamp(r.bottomLeft, 0.0f, minDim);

    // 3. Define the path cycle (Top-Left -> Top-Right -> Bottom-Right -> Bottom-Left)
    // We map the "Incoming" width and "Outgoing" width for each corner.
    typedef struct {
        float radius;
        SDL_FPoint center;
        float startAngle; // Radians
        float endAngle;   // Radians
        float widthStart; // Width at start of arc
        float widthEnd;   // Width at end of arc
    } CornerDef;

    CornerDef corners[4] = {
        // TL: Left Width -> Top Width. Angles: PI -> 1.5 PI (Bottom-up clockwise visual, or standard trig: 180->270)
        // Note: SDL Y is down.
        // TL is PI to 1.5 PI. Center is (x+r, y+r)
        { r.topLeft,     {rect.x + r.topLeft, rect.y + r.topLeft},         SDL_PI_F,       1.5f * SDL_PI_F, (float)config->width.left, (float)config->width.top },

        // TR: Top Width -> Right Width. Angles: 1.5 PI -> 2.0 PI
        { r.topRight,    {rect.x + rect.w - r.topRight, rect.y + r.topRight}, 1.5f * SDL_PI_F, 2.0f * SDL_PI_F, (float)config->width.top,  (float)config->width.right },

        // BR: Right Width -> Bottom Width. Angles: 0 -> 0.5 PI
        { r.bottomRight, {rect.x + rect.w - r.bottomRight, rect.y + rect.h - r.bottomRight}, 0.0f, 0.5f * SDL_PI_F, (float)config->width.right, (float)config->width.bottom },

        // BL: Bottom Width -> Left Width. Angles: 0.5 PI -> PI
        { r.bottomLeft,  {rect.x + r.bottomLeft, rect.y + rect.h - r.bottomLeft}, 0.5f * SDL_PI_F, SDL_PI_F, (float)config->width.bottom, (float)config->width.left }
    };

    // 4. Memory Estimation
    // Max segments per corner
    int maxSegments = 16;
    // Vertices: 4 corners * (maxSegments+1) * 4 points + 4 connection points
    // This is an over-estimation, but safe for stack allocation
    int totalVertices = 4 * (maxSegments + 2) * 4;
    // Indices: (Triangles per step * steps)
    int totalIndices = totalVertices * 6;

    SDL_Vertex vertices[totalVertices];
    int indices[totalIndices];
    int vCount = 0;
    int iCount = 0;

    // AA Offset (0.5 for half-pixel fade in, 0.5 for fade out)
    const float aaSize = 0.5f;

    // 5. Build Mesh
    for (int c = 0; c < 4; c++) {
        CornerDef *cd = &corners[c];

        // Adaptive segments: fewer for small radii, more for large
        int numSegments = SDL_max(2, (int)(cd->radius * 0.5f));
        if (numSegments > maxSegments) numSegments = maxSegments;

        for (int i = 0; i <= numSegments; i++) {
            // Lerp angle and width
            float t = (float)i / (float)numSegments; // 0.0 to 1.0
            float currentAngle = cd->startAngle + t * (cd->endAngle - cd->startAngle);
            float currentWidth = cd->widthStart + t * (cd->widthEnd - cd->widthStart);

            float cosA = SDL_cosf(currentAngle);
            float sinA = SDL_sinf(currentAngle);

            // Calculate Edge Radii
            // Outer is fixed by the corner radius.
            // Inner is determined by the width.
            float rOuter = cd->radius;
            float rInner = rOuter - currentWidth;

            // Geometry Points
            // We define 4 rings: InnerTransparent, InnerSolid, OuterSolid, OuterTransparent

            // 0: Inner AA (Transparent) - Only if we have a hole.
            // If rInner <= 0, the border collapses into a pie slice, but we clamp rInner.
            float r0 = SDL_max(0.0f, rInner - aaSize);
            float r1 = SDL_max(0.0f, rInner + aaSize);
            float r2 = rOuter - aaSize;
            float r3 = rOuter + aaSize;

            // Apply rotation and translation
            SDL_FPoint center = cd->center;

            // Generate 4 Vertices for this "slice"
            // If width is 0, we still generate geometry, but it will be invisible or sub-pixel.

            // V0: Inner AA
            vertices[vCount++] = (SDL_Vertex){ {center.x + cosA * r0, center.y + sinA * r0}, colTrans, {0,0} };
            // V1: Inner Solid
            vertices[vCount++] = (SDL_Vertex){ {center.x + cosA * r1, center.y + sinA * r1}, colSolid, {0,0} };
            // V2: Outer Solid
            vertices[vCount++] = (SDL_Vertex){ {center.x + cosA * r2, center.y + sinA * r2}, colSolid, {0,0} };
            // V3: Outer AA
            vertices[vCount++] = (SDL_Vertex){ {center.x + cosA * r3, center.y + sinA * r3}, colTrans, {0,0} };

            // Generate Indices (Connect to previous slice)
            // We need at least one previous slice in this loop, OR a connection to the previous corner
            if (vCount > 4) {
                int base = vCount - 4; // Current slice start
                int prev = base - 4;   // Previous slice start

                // Strip 1: Inner AA (0-1) -> connects prev 0-1 to curr 0-1
                indices[iCount++] = prev + 0; indices[iCount++] = base + 0; indices[iCount++] = base + 1;
                indices[iCount++] = prev + 0; indices[iCount++] = base + 1; indices[iCount++] = prev + 1;

                // Strip 2: Solid Body (1-2)
                indices[iCount++] = prev + 1; indices[iCount++] = base + 1; indices[iCount++] = base + 2;
                indices[iCount++] = prev + 1; indices[iCount++] = base + 2; indices[iCount++] = prev + 2;

                // Strip 3: Outer AA (2-3)
                indices[iCount++] = prev + 2; indices[iCount++] = base + 2; indices[iCount++] = base + 3;
                indices[iCount++] = prev + 2; indices[iCount++] = base + 3; indices[iCount++] = prev + 3;
            }
        }
    }

    // 6. Close the Loop (Connect Last Corner End to First Corner Start)
    // The loop above generates 4 discrete ribbons. We need to bridge Corner 0->1, 1->2, 2->3, 3->0.
    // However, because we filled the vertex buffer sequentially, the "Last" vertex of Corner 0
    // and the "First" vertex of Corner 1 are mathematically aligned (forming the straight edge),
    // but they are distinct vertices in memory.
    // The simplest way (visualized) is to actually bridge the indices between the corners.

    // We generated: [C0_Start ... C0_End] [C1_Start ... C1_End] ...
    // We need to stitch C0_End to C1_Start.

    int startOfCorner[4];
    int endOfCorner[4];
    int currentV = 0;

    // Recalculate where corners start/end in the buffer to stitch them
    for(int c=0; c<4; c++) {
        int numSegs = SDL_max(2, (int)(corners[c].radius * 0.5f));
        startOfCorner[c] = currentV;
        currentV += (numSegs + 1) * 4;
        endOfCorner[c] = currentV - 4; // Pointing to the start of the last slice of this corner
    }

    for (int c = 0; c < 4; c++) {
        int nextC = (c + 1) % 4;
        int prev = endOfCorner[c];
        int next = startOfCorner[nextC];

        // Stitch the gap (The Straight Sides)
        // Inner AA
        indices[iCount++] = prev + 0; indices[iCount++] = next + 0; indices[iCount++] = next + 1;
        indices[iCount++] = prev + 0; indices[iCount++] = next + 1; indices[iCount++] = prev + 1;
        // Solid Body
        indices[iCount++] = prev + 1; indices[iCount++] = next + 1; indices[iCount++] = next + 2;
        indices[iCount++] = prev + 1; indices[iCount++] = next + 2; indices[iCount++] = prev + 2;
        // Outer AA
        indices[iCount++] = prev + 2; indices[iCount++] = next + 2; indices[iCount++] = next + 3;
        indices[iCount++] = prev + 2; indices[iCount++] = next + 3; indices[iCount++] = prev + 3;
    }

    // 7. Render
    SDL_RenderGeometry(rendererData->renderer, NULL, vertices, vCount, indices, iCount);
}

void SDL_Clay_RenderText(const Clay_SDL3RendererData *rendererData, const SDL_FRect rect, const Clay_TextRenderData *config) {
    TTF_Font *font = rendererData->fonts[config->fontId];
    TTF_SetFontSize(font, config->fontSize);
    TTF_Text *text = TTF_CreateText(rendererData->textEngine, font, config->stringContents.chars, config->stringContents.length);
    TTF_SetTextColor(text, (Uint8)config->textColor.r, (Uint8)config->textColor.g, (Uint8)config->textColor.b, (Uint8)config->textColor.a);
    TTF_DrawRendererText(text, rect.x, rect.y);
    TTF_DestroyText(text);
}

void SDL_Clay_RenderClayCommands(Clay_SDL3RendererData *rendererData, Clay_RenderCommandArray *renderCommands)
{
    for (int32_t i = 0; i < renderCommands->length; i++) {

        const Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(renderCommands, i);
        const Clay_BoundingBox bounding_box = renderCommand->boundingBox;
        const SDL_FRect rect = { bounding_box.x, bounding_box.y, bounding_box.width, bounding_box.height };

        switch (renderCommand->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                const Clay_RectangleRenderData *config = &renderCommand->renderData.rectangle;
                SDL_SetRenderDrawBlendMode(rendererData->renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(rendererData->renderer, (Uint8)config->backgroundColor.r, (Uint8)config->backgroundColor.g, (Uint8)config->backgroundColor.b, (Uint8)config->backgroundColor.a);
                if (config->cornerRadius.topLeft > 0) {
                    SDL_Clay_RenderFillRoundedRect(rendererData, rect, config->cornerRadius.topLeft, config->backgroundColor);
                } else {
                    SDL_RenderFillRect(rendererData->renderer, &rect);
                }
            }
            break;
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                SDL_Clay_RenderText(rendererData, rect, &renderCommand->renderData.text);
            }
            break;
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                SDL_Clay_RenderBorder(rendererData, rect, &renderCommand->renderData.border);
            }
            break;
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
                const Clay_BoundingBox boundingBox = renderCommand->boundingBox;
                SDL_SetRenderClipRect(
                    rendererData->renderer,
                    &(SDL_Rect){
                        .x = (int) boundingBox.x,
                        .y = (int) boundingBox.y,
                        .w = (int) boundingBox.width,
                        .h = (int) boundingBox.height,
                    }
                );
            }
            break;
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
                SDL_SetRenderClipRect(rendererData->renderer, NULL);
            }
            break;
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                SDL_Texture *texture = renderCommand->renderData.image.imageData;
                const SDL_FRect dest = { rect.x, rect.y, rect.w, rect.h };
                SDL_RenderTexture(rendererData->renderer, texture, NULL, &dest);
            }
            break;
            default:
                SDL_Log("Unknown render command type: %d", renderCommand->commandType);
        }
    }
}
