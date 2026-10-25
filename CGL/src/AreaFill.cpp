#include "AreaFill.h"
#include <cmath>
#include <algorithm>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

// ============================================================================
// PREDEFINED COLOR CONSTANTS
// ============================================================================
const PixelColor PixelColor::Black(0, 0, 0, 255);
const PixelColor PixelColor::White(255, 255, 255, 255);
const PixelColor PixelColor::Red(255, 45, 45, 255);
const PixelColor PixelColor::Green(45, 255, 45, 255);
const PixelColor PixelColor::Blue(45, 120, 255, 255);
const PixelColor PixelColor::Cyan(0, 240, 255, 255);
const PixelColor PixelColor::Magenta(255, 50, 255, 255);
const PixelColor PixelColor::Yellow(255, 230, 40, 255);
const PixelColor PixelColor::DarkGray(40, 45, 55, 255);
const PixelColor PixelColor::ShieldBlue(0, 190, 255, 255);
const PixelColor PixelColor::AsteroidBrown(130, 90, 65, 255);

// ============================================================================
// PIXEL BUFFER IMPLEMENTATION
// ============================================================================
PixelBuffer::PixelBuffer(int w, int h, PixelColor clearColor)
    : width(w), height(h) {
    if (width <= 0) width = 1;
    if (height <= 0) height = 1;
    pixels.assign(width * height, clearColor);
}

PixelColor PixelBuffer::getPixel(int x, int y) const {
    if (!isInBounds(x, y)) {
        return PixelColor::Black;
    }
    return pixels[y * width + x];
}

void PixelBuffer::setPixel(int x, int y, PixelColor color) {
    if (isInBounds(x, y)) {
        pixels[y * width + x] = color;
    }
}

void PixelBuffer::clear(PixelColor color) {
    std::fill(pixels.begin(), pixels.end(), color);
}

// Draw line into buffer reusing Bresenham line algorithm
void PixelBuffer::drawLine(int x0, int y0, int x1, int y1, PixelColor color) {
    std::vector<IntPoint2D> pts = Bresenham::calculateLine(x0, y0, x1, y1);
    for (const auto& pt : pts) {
        setPixel(pt.x, pt.y, color);
    }
}

// Draw circle into buffer reusing Bresenham midpoint circle algorithm
void PixelBuffer::drawCircle(int xc, int yc, int radius, PixelColor color) {
    std::vector<IntPoint2D> pts = Bresenham::calculateCircle(xc, yc, radius);
    for (const auto& pt : pts) {
        setPixel(pt.x, pt.y, color);
    }
}

// Draw rectangle perimeter
void PixelBuffer::drawRect(int x, int y, int w, int h, PixelColor color) {
    if (w <= 0 || h <= 0) return;
    drawLine(x, y, x + w - 1, y, color);
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
    drawLine(x + w - 1, y + h - 1, x, y + h - 1, color);
    drawLine(x, y + h - 1, x, y, color);
}

// Fill solid rectangle
void PixelBuffer::fillRect(int x, int y, int w, int h, PixelColor color) {
    int xStart = std::max(0, x);
    int yStart = std::max(0, y);
    int xEnd = std::min(width, x + w);
    int yEnd = std::min(height, y + h);

    for (int py = yStart; py < yEnd; ++py) {
        for (int px = xStart; px < xEnd; ++px) {
            pixels[py * width + px] = color;
        }
    }
}

// Draw polygon boundary connecting consecutive vertices
void PixelBuffer::drawPolygon(const std::vector<Point2D>& vertices, PixelColor color) {
    int n = static_cast<int>(vertices.size());
    if (n < 2) return;
    for (int i = 0; i < n; ++i) {
        Point2D p1 = vertices[i];
        Point2D p2 = vertices[(i + 1) % n];
        drawLine(static_cast<int>(std::round(p1.x)), static_cast<int>(std::round(p1.y)),
                 static_cast<int>(std::round(p2.x)), static_cast<int>(std::round(p2.y)),
                 color);
    }
}

// Render buffer contents to OpenGL screen at (originX, originY)
void PixelBuffer::render(float originX, float originY, float pixelScale) const {
    glBegin(GL_QUADS);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const PixelColor& c = pixels[y * width + x];
            if (c.a == 0) continue; // Skip completely transparent pixels

            glColor4ub(c.r, c.g, c.b, c.a);
            float px = originX + static_cast<float>(x) * pixelScale;
            float py = originY + static_cast<float>(y) * pixelScale;

            glVertex2f(px, py);
            glVertex2f(px + pixelScale, py);
            glVertex2f(px + pixelScale, py + pixelScale);
            glVertex2f(px, py + pixelScale);
        }
    }
    glEnd();
}

// ============================================================================
// [CGL CO-3] AREA FILL ALGORITHMS IMPLEMENTATION
// ============================================================================

// [CGL CO-3] Iterative 4-connected Flood-Fill algorithm
int AreaFill::floodFill4(PixelBuffer& buffer, int seedX, int seedY,
                         PixelColor fillColor,
                         std::vector<IntPoint2D>* outFilledPixels) {
    if (!buffer.isInBounds(seedX, seedY)) {
        return 0; // Invalid seed
    }
    PixelColor targetColor = buffer.getPixel(seedX, seedY);
    return floodFill4Explicit(buffer, seedX, seedY, targetColor, fillColor, outFilledPixels);
}

// [CGL CO-3] Flood-Fill: Replaces connected pixels matching targetColor with fillColor
int AreaFill::floodFill4Explicit(PixelBuffer& buffer, int seedX, int seedY,
                                 PixelColor targetColor, PixelColor fillColor,
                                 std::vector<IntPoint2D>* outFilledPixels) {
    if (!buffer.isInBounds(seedX, seedY)) {
        return 0; // Coordinate boundary validation
    }
    if (targetColor == fillColor) {
        return 0; // Target is already fill color; prevents infinite cycle
    }
    if (buffer.getPixel(seedX, seedY) != targetColor) {
        return 0; // Seed does not match requested target color
    }

    // [CGL CO-3] Explicit heap stack avoids OS stack overflow on large surfaces
    std::vector<IntPoint2D> stack;
    stack.reserve(std::min(static_cast<size_t>(buffer.getWidth() * 4), buffer.getTotalPixels()));

    // Color seed immediately before pushing to guarantee at most one entry per pixel
    buffer.setPixel(seedX, seedY, fillColor);
    stack.push_back({ seedX, seedY });
    if (outFilledPixels) {
        outFilledPixels->push_back({ seedX, seedY });
    }

    int filledCount = 0;
    // 4-connected orthogonal neighbor offsets: Right, Left, Up, Down
    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    while (!stack.empty()) {
        IntPoint2D pt = stack.back();
        stack.pop_back();
        filledCount++;

        // [CGL CO-3] 4-connected neighbor expansion
        for (int i = 0; i < 4; ++i) {
            int nx = pt.x + dx[i];
            int ny = pt.y + dy[i];

            if (buffer.isInBounds(nx, ny) && buffer.getPixel(nx, ny) == targetColor) {
                buffer.setPixel(nx, ny, fillColor);
                stack.push_back({ nx, ny });
                if (outFilledPixels) {
                    outFilledPixels->push_back({ nx, ny });
                }
            }
        }
    }

    return filledCount;
}

// [CGL CO-3] Boundary-Fill: Expands until encountering boundaryColor or fillColor
int AreaFill::boundaryFill4(PixelBuffer& buffer, int seedX, int seedY,
                            PixelColor fillColor, PixelColor boundaryColor,
                            std::vector<IntPoint2D>* outFilledPixels) {
    if (!buffer.isInBounds(seedX, seedY)) {
        return 0; // Boundary validation
    }
    if (fillColor == boundaryColor) {
        return 0; // Fill color cannot equal boundary color
    }

    PixelColor seedColor = buffer.getPixel(seedX, seedY);
    if (seedColor == boundaryColor || seedColor == fillColor) {
        return 0; // Seed already on boundary or already filled
    }

    // [CGL CO-3] Explicit heap stack eliminates call-stack limits
    std::vector<IntPoint2D> stack;
    stack.reserve(std::min(static_cast<size_t>(buffer.getWidth() * 4), buffer.getTotalPixels()));

    // Color seed immediately before pushing to avoid redundant stack insertions
    buffer.setPixel(seedX, seedY, fillColor);
    stack.push_back({ seedX, seedY });
    if (outFilledPixels) {
        outFilledPixels->push_back({ seedX, seedY });
    }

    int filledCount = 0;
    // 4-connected orthogonal neighbor offsets
    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    while (!stack.empty()) {
        IntPoint2D pt = stack.back();
        stack.pop_back();
        filledCount++;

        // [CGL CO-3] Boundary check: pixel != boundaryColor && pixel != fillColor
        for (int i = 0; i < 4; ++i) {
            int nx = pt.x + dx[i];
            int ny = pt.y + dy[i];

            if (buffer.isInBounds(nx, ny)) {
                PixelColor c = buffer.getPixel(nx, ny);
                if (c != boundaryColor && c != fillColor) {
                    buffer.setPixel(nx, ny, fillColor);
                    stack.push_back({ nx, ny });
                    if (outFilledPixels) {
                        outFilledPixels->push_back({ nx, ny });
                    }
                }
            }
        }
    }

    return filledCount;
}
