#pragma once

#include <cstdint>

void initRenderer(uint32_t maxSpriteCount,
                  uint32_t windowWidth,
                  uint32_t windowHeight);

void destroyRenderer();

void beginDrawing();
void endDrawing();

void drawQuad(float x, float y, float width, float height, const Color& color);
