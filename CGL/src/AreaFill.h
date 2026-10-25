#ifndef AREA_FILL_H
#define AREA_FILL_H

#include <vector>
#include <cstdint>
#include "Bresenham.h"
#include "Renderer.h"

// ============================================================================
// PIXEL COLOR STRUCT
// ============================================================================
struct PixelColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    PixelColor() : r(0), g(0), b(0), a(255) {}
    PixelColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    bool operator==(const PixelColor& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    bool operator!=(const PixelColor& other) const {
        return !(*this == other);
    }

    static PixelColor fromFloat(float rf, float gf, float bf, float af = 1.0f) {
        return PixelColor(
            static_cast<uint8_t>(rf * 255.0f),
            static_cast<uint8_t>(gf * 255.0f),
            static_cast<uint8_t>(bf * 255.0f),
            static_cast<uint8_t>(af * 255.0f)
        );
    }

    // Common predefined color constants
    static const PixelColor Black;
    static const PixelColor White;
    static const PixelColor Red;
    static const PixelColor Green;
    static const PixelColor Blue;
    static const PixelColor Cyan;
    static const PixelColor Magenta;
    static const PixelColor Yellow;
    static const PixelColor DarkGray;
    static const PixelColor ShieldBlue;
    static const PixelColor AsteroidBrown;
};

// ============================================================================
// PIXEL BUFFER (Controlled Off-screen Surface for Area Filling)
// ============================================================================
class PixelBuffer {
private:
    int width;
    int height;
    std::vector<PixelColor> pixels;

public:
    PixelBuffer(int w, int h, PixelColor clearColor = PixelColor::Black);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    size_t getTotalPixels() const { return static_cast<size_t>(width * height); }

    // Coordinate validation
    bool isInBounds(int x, int y) const {
        return (x >= 0 && x < width && y >= 0 && y < height);
    }

    // Pixel access
    PixelColor getPixel(int x, int y) const;
    void setPixel(int x, int y, PixelColor color);
    void clear(PixelColor color = PixelColor::Black);

    // Direct geometric drawing into buffer (reusing existing Bresenham algorithms)
    void drawLine(int x0, int y0, int x1, int y1, PixelColor color);
    void drawCircle(int xc, int yc, int radius, PixelColor color);
    void drawRect(int x, int y, int w, int h, PixelColor color);
    void fillRect(int x, int y, int w, int h, PixelColor color);
    void drawPolygon(const std::vector<Point2D>& vertices, PixelColor color);

    // Render buffer contents to OpenGL screen at (originX, originY) with pixel scaling
    void render(float originX, float originY, float pixelScale = 1.0f) const;

    const std::vector<PixelColor>& getRawData() const { return pixels; }
};

// ============================================================================
// [CGL CO-3] AREA-FILL ALGORITHMS: FLOOD-FILL & BOUNDARY-FILL
// Course Outcome 3: Develop 2D and 3D objects using various algorithms.
// - Iterative 4-connected seed traversal (using std::vector as explicit stack).
// - Eliminates call-stack recursion overflow on large raster areas.
// - Operates on controlled off-screen PixelBuffer.
// ============================================================================
class AreaFill {
public:
    // ========================================================================
    // [CGL CO-3] FLOOD-FILL ALGORITHM (Iterative 4-Connected)
    // ========================================================================
    // Replaces all 4-connected interior pixels matching targetColor with fillColor.
    // Expands: (x+1, y), (x-1, y), (x, y+1), (x, y-1).
    static int floodFill4(PixelBuffer& buffer, int seedX, int seedY,
                         PixelColor fillColor,
                         std::vector<IntPoint2D>* outFilledPixels = nullptr);

    // Overload allowing explicit specification of targetColor to replace
    static int floodFill4Explicit(PixelBuffer& buffer, int seedX, int seedY,
                                 PixelColor targetColor, PixelColor fillColor,
                                 std::vector<IntPoint2D>* outFilledPixels = nullptr);

    // ========================================================================
    // [CGL CO-3] BOUNDARY-FILL ALGORITHM (Iterative 4-Connected)
    // ========================================================================
    // Expands outward from interior seed (seedX, seedY) until hitting boundaryColor.
    // Stops when pixel == boundaryColor OR pixel == fillColor (prevents infinite loop).
    static int boundaryFill4(PixelBuffer& buffer, int seedX, int seedY,
                            PixelColor fillColor, PixelColor boundaryColor,
                            std::vector<IntPoint2D>* outFilledPixels = nullptr);
};

#endif // AREA_FILL_H
