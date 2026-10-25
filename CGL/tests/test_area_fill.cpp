#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include "../src/AreaFill.h"
#include "../src/Bresenham.h"

// ============================================================================
// TEST 1: SIMPLE RECTANGLE FILLING (BOUNDARY-FILL & FLOOD-FILL)
// ============================================================================
void testRectangleFill() {
    std::cout << "[TEST 1] Area Fill: Simple Enclosed Rectangle...\n";

    const int W = 50, H = 50;
    PixelBuffer buf(W, H, PixelColor::Black);

    // Draw enclosed rectangle: x in [10, 30], y in [10, 30] (21x21 perimeter)
    buf.drawRect(10, 10, 21, 21, PixelColor::White);

    // Interior is strictly x in [11, 29], y in [11, 29] -> 19 x 19 = 361 pixels
    int filled = AreaFill::boundaryFill4(buf, 20, 20, PixelColor::Red, PixelColor::White);
    assert(filled == 361);

    // Verify all interior pixels are Red
    for (int y = 11; y <= 29; ++y) {
        for (int x = 11; x <= 29; ++x) {
            assert(buf.getPixel(x, y) == PixelColor::Red);
        }
    }

    // Verify boundary pixels remain White
    for (int x = 10; x <= 30; ++x) {
        assert(buf.getPixel(x, 10) == PixelColor::White);
        assert(buf.getPixel(x, 30) == PixelColor::White);
    }
    for (int y = 10; y <= 30; ++y) {
        assert(buf.getPixel(10, y) == PixelColor::White);
        assert(buf.getPixel(30, y) == PixelColor::White);
    }

    // Verify outside pixels remain Black
    assert(buf.getPixel(5, 5) == PixelColor::Black);
    assert(buf.getPixel(35, 35) == PixelColor::Black);
    assert(buf.getPixel(20, 5) == PixelColor::Black);
    assert(buf.getPixel(20, 35) == PixelColor::Black);

    // Test Flood-Fill on another rectangle
    PixelBuffer floodBuf(W, H, PixelColor::Black);
    floodBuf.fillRect(10, 10, 21, 21, PixelColor::DarkGray);
    int floodFilled = AreaFill::floodFill4(floodBuf, 20, 20, PixelColor::Cyan);
    assert(floodFilled == 21 * 21); // Replaced DarkGray box
    assert(floodBuf.getPixel(20, 20) == PixelColor::Cyan);
    assert(floodBuf.getPixel(5, 5) == PixelColor::Black);

    std::cout << " -> Rectangle boundary-fill (361 interior pixels) and flood-fill successfully verified.\n";
}

// ============================================================================
// TEST 2: CIRCLE WITH CLOSED BOUNDARY (BRESENHAM CIRCLE REUSE)
// ============================================================================
void testCircleClosedBoundaryFill() {
    std::cout << "\n[TEST 2] Area Fill: Circle with Closed Bresenham Boundary...\n";

    const int W = 70, H = 70;
    PixelBuffer buf(W, H, PixelColor::Black);

    int cx = 35, cy = 35, radius = 15;
    // Draw boundary using existing Bresenham midpoint circle algorithm
    buf.drawCircle(cx, cy, radius, PixelColor::ShieldBlue);

    // Boundary-fill starting at center seed
    int filled = AreaFill::boundaryFill4(buf, cx, cy, PixelColor::Cyan, PixelColor::ShieldBlue);
    assert(filled > 500); // Interior disk area

    // Verify center is filled
    assert(buf.getPixel(cx, cy) == PixelColor::Cyan);

    // Verify all filled pixels are strictly inside the circle boundary
    int checkedFilled = 0;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if (buf.getPixel(x, y) == PixelColor::Cyan) {
                checkedFilled++;
                float distSq = static_cast<float>((x - cx) * (x - cx) + (y - cy) * (y - cy));
                assert(distSq < static_cast<float>(radius * radius));
            }
        }
    }
    assert(checkedFilled == filled);

    // Verify outside points remain strictly untouched
    assert(buf.getPixel(10, 10) == PixelColor::Black);
    assert(buf.getPixel(60, 60) == PixelColor::Black);
    assert(buf.getPixel(cx, cy - radius - 5) == PixelColor::Black);
    assert(buf.getPixel(cx, cy + radius + 5) == PixelColor::Black);

    std::cout << " -> Circular shield boundary contained fill strictly within radius "
              << radius << " (" << filled << " pixels filled).\n";
}

// ============================================================================
// TEST 3: CONCAVE POLYGON FILLING (AROUND CORNERS & NOTCHES)
// ============================================================================
void testConcavePolygonFill() {
    std::cout << "\n[TEST 3] Area Fill: Concave U-Shaped Polygon...\n";

    const int W = 65, H = 65;
    PixelBuffer buf(W, H, PixelColor::Black);

    // U-shaped concave polygon boundary:
    // Left prong: x in [10, 25], y in [10, 50]
    // Right prong: x in [35, 50], y in [10, 50]
    // Base connector: x in [10, 50], y in [10, 25]
    // Notch recess: x in [26, 34], y in [26, 50] (EXTERNAL TO SHAPE)
    std::vector<Point2D> uVerts = {
        { 10.0f, 10.0f },
        { 50.0f, 10.0f },
        { 50.0f, 50.0f },
        { 35.0f, 50.0f },
        { 35.0f, 25.0f },
        { 25.0f, 25.0f },
        { 25.0f, 50.0f },
        { 10.0f, 50.0f }
    };

    buf.drawPolygon(uVerts, PixelColor::White);

    // Seed inside the left prong at (15, 35)
    int filled = AreaFill::boundaryFill4(buf, 15, 35, PixelColor::Yellow, PixelColor::White);
    assert(filled > 0);

    // Verify fill propagated around the corner through the base to the right prong
    assert(buf.getPixel(15, 35) == PixelColor::Yellow); // Left prong
    assert(buf.getPixel(30, 18) == PixelColor::Yellow); // Base channel
    assert(buf.getPixel(45, 35) == PixelColor::Yellow); // Right prong

    // Verify notch recess (30, 35) remained outside and UNTOUCHED
    assert(buf.getPixel(30, 35) == PixelColor::Black);
    assert(buf.getPixel(5, 5) == PixelColor::Black);
    assert(buf.getPixel(55, 55) == PixelColor::Black);

    std::cout << " -> Concave interior traversed around corner to both prongs without leaking into recess.\n";
}

// ============================================================================
// TEST 4: INVALID SEED POINTS & SAFE BOUNDS HANDLING
// ============================================================================
void testInvalidSeedPointsAndEdgeCases() {
    std::cout << "\n[TEST 4] Area Fill: Invalid Seeds & Edge Cases...\n";

    const int W = 30, H = 30;
    PixelBuffer buf(W, H, PixelColor::Black);
    buf.drawRect(5, 5, 20, 20, PixelColor::White);

    // Case A: Negative coordinates
    int res1 = AreaFill::boundaryFill4(buf, -5, 10, PixelColor::Red, PixelColor::White);
    assert(res1 == 0);

    // Case B: Coordinates exceeding buffer dimensions
    int res2 = AreaFill::boundaryFill4(buf, 35, 10, PixelColor::Red, PixelColor::White);
    assert(res2 == 0);
    int res3 = AreaFill::boundaryFill4(buf, 10, 45, PixelColor::Red, PixelColor::White);
    assert(res3 == 0);

    // Case C: Seed directly on boundary pixel
    int res4 = AreaFill::boundaryFill4(buf, 5, 5, PixelColor::Red, PixelColor::White);
    assert(res4 == 0);

    // Case D: Seed on already filled pixel
    buf.setPixel(15, 15, PixelColor::Red);
    int res5 = AreaFill::boundaryFill4(buf, 15, 15, PixelColor::Red, PixelColor::White);
    assert(res5 == 0);

    // Case E: Fill color identical to boundary color
    int res6 = AreaFill::boundaryFill4(buf, 12, 12, PixelColor::White, PixelColor::White);
    assert(res6 == 0);

    // Case F: Flood-fill target color identical to fill color
    int res7 = AreaFill::floodFill4(buf, 0, 0, PixelColor::Black);
    assert(res7 == 0);

    std::cout << " -> All invalid seeds and edge configurations handled cleanly with 0 side-effects.\n";
}

// ============================================================================
// TEST 5: REGION TOUCHING IMAGE-BUFFER BOUNDARIES
// ============================================================================
void testBufferEdgeTouchingRegion() {
    std::cout << "\n[TEST 5] Area Fill: Region Touching Image-Buffer Boundary...\n";

    const int W = 40, H = 40;
    PixelBuffer buf(W, H, PixelColor::Black);

    // Draw an open L-shaped boundary: from (20, 0) to (20, 20) and (0, 20) to (20, 20)
    buf.drawLine(20, 0, 20, 20, PixelColor::White);
    buf.drawLine(0, 20, 20, 20, PixelColor::White);

    // The sub-region x in [0, 19], y in [0, 19] is bounded on top/right by White line,
    // and on bottom/left by the image buffer edges (x=0, y=0).
    int filled = AreaFill::boundaryFill4(buf, 5, 5, PixelColor::Green, PixelColor::White);
    assert(filled == 20 * 20); // Exactly 400 pixels

    // Verify all pixels in [0, 19] x [0, 19] are Green
    for (int y = 0; y < 20; ++y) {
        for (int x = 0; x < 20; ++x) {
            assert(buf.getPixel(x, y) == PixelColor::Green);
        }
    }

    // Verify boundary line remains White
    for (int y = 0; y <= 20; ++y) assert(buf.getPixel(20, y) == PixelColor::White);
    for (int x = 0; x <= 20; ++x) assert(buf.getPixel(x, 20) == PixelColor::White);

    // Verify other quadrants remained Black (no buffer overflow)
    assert(buf.getPixel(30, 30) == PixelColor::Black);
    assert(buf.getPixel(25, 5) == PixelColor::Black);
    assert(buf.getPixel(5, 25) == PixelColor::Black);

    std::cout << " -> Region touching buffer perimeter filled safely without out-of-bounds error (400 pixels).\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: CGL CO-3 AREA FILL TEST SUITE         \n";
    std::cout << "  (Iterative 4-Connected Flood-Fill & Boundary-Fill) \n";
    std::cout << "=====================================================\n\n";

    testRectangleFill();
    testCircleClosedBoundaryFill();
    testConcavePolygonFill();
    testInvalidSeedPointsAndEdgeCases();
    testBufferEdgeTouchingRegion();

    std::cout << "\n>>> ALL CGL CO-3 AREA FILL TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
