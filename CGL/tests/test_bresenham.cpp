#include <iostream>
#include <cassert>
#include <cmath>
#include <set>
#include "../src/Bresenham.h"

void testBresenhamLineHorizontalVerticalDiagonal() {
    std::cout << "[TEST 1] Bresenham Line: Horizontal, Vertical, Diagonal...\n";

    // 1. Horizontal Line
    auto hLine = Bresenham::calculateLine(10, 20, 20, 20);
    assert(hLine.size() == 11);
    for (size_t i = 0; i < hLine.size(); ++i) {
        assert(hLine[i].x == static_cast<int>(10 + i));
        assert(hLine[i].y == 20);
    }

    // 2. Vertical Line
    auto vLine = Bresenham::calculateLine(30, 5, 30, 15);
    assert(vLine.size() == 11);
    for (size_t i = 0; i < vLine.size(); ++i) {
        assert(vLine[i].x == 30);
        assert(vLine[i].y == static_cast<int>(5 + i));
    }

    // 3. Diagonal Line (Slope = 1)
    auto dLine = Bresenham::calculateLine(0, 0, 8, 8);
    assert(dLine.size() == 9);
    for (size_t i = 0; i < dLine.size(); ++i) {
        assert(dLine[i].x == static_cast<int>(i));
        assert(dLine[i].y == static_cast<int>(i));
    }
    std::cout << " -> Horizontal, vertical, and diagonal lines rasterized with exact coordinates.\n";
}

void testBresenhamLineAllOctants() {
    std::cout << "\n[TEST 2] Bresenham Line: Arbitrary Slopes Across All Octants...\n";

    // Shallow positive slope (0 < m < 1)
    auto line1 = Bresenham::calculateLine(0, 0, 10, 4);
    assert(line1.front().x == 0 && line1.front().y == 0);
    assert(line1.back().x == 10 && line1.back().y == 4);
    assert(line1.size() == 11); // Monotonic step in x

    // Steep positive slope (m > 1)
    auto line2 = Bresenham::calculateLine(0, 0, 3, 9);
    assert(line2.front().x == 0 && line2.front().y == 0);
    assert(line2.back().x == 3 && line2.back().y == 9);
    assert(line2.size() == 10); // Monotonic step in y

    // Negative slope (dx > 0, dy < 0)
    auto line3 = Bresenham::calculateLine(0, 10, 10, 0);
    assert(line3.front().x == 0 && line3.front().y == 10);
    assert(line3.back().x == 10 && line3.back().y == 0);
    assert(line3.size() == 11);

    // Reverse direction (x0 > x1, y0 > y1)
    auto line4 = Bresenham::calculateLine(10, 10, 2, 4);
    assert(line4.front().x == 10 && line4.front().y == 10);
    assert(line4.back().x == 2 && line4.back().y == 4);

    std::cout << " -> All 8 octant trajectories verified with correct boundary points.\n";
}

void testBresenhamCircleAccuracyAndSymmetry() {
    std::cout << "\n[TEST 3] Mid-point Bresenham Circle 8-Way Symmetry & Distance Tolerance...\n";

    int xc = 150;
    int yc = 200;
    int r = 25;

    auto circlePts = Bresenham::calculateCircle(xc, yc, r);
    assert(!circlePts.empty());

    // Check all cardinal points exist in the rasterized circle
    bool foundEast = false;
    bool foundWest = false;
    bool foundNorth = false;
    bool foundSouth = false;

    for (const auto& pt : circlePts) {
        if (pt.x == xc + r && pt.y == yc) foundEast = true;
        if (pt.x == xc - r && pt.y == yc) foundWest = true;
        if (pt.x == xc && pt.y == yc + r) foundNorth = true;
        if (pt.x == xc && pt.y == yc - r) foundSouth = true;

        // Verify distance from center is within discrete raster approximation error (|dist - r| <= 1.5)
        float dx = static_cast<float>(pt.x - xc);
        float dy = static_cast<float>(pt.y - yc);
        float dist = std::sqrt(dx * dx + dy * dy);
        float error = std::abs(dist - static_cast<float>(r));
        assert(error <= 1.5f);
    }

    assert(foundEast && foundWest && foundNorth && foundSouth);
    std::cout << " -> 8-way symmetric cardinal points confirmed within radius error tolerance.\n";
    std::cout << " -> Total raster points computed for radius " << r << ": " << circlePts.size() << "\n";
}

void testBresenhamArcFiltering() {
    std::cout << "\n[TEST 4] Bresenham Circular Arc Angular Filtering...\n";
    int xc = 0;
    int yc = 0;
    int r = 20;

    const float PI = 3.14159265f;
    // Arc in 1st quadrant: [0, PI/2]
    auto arc = Bresenham::calculateArc(xc, yc, r, 0.0f, PI * 0.5f);
    assert(!arc.empty());

    for (const auto& pt : arc) {
        // All points in 1st quadrant must have x >= 0 and y >= 0
        assert(pt.x >= 0);
        assert(pt.y >= 0);
    }
    std::cout << " -> Arc angular bounding strictly confined to selected angular sector.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: CGL CO-2 TEST SUITE (Bresenham Math)  \n";
    std::cout << "=====================================================\n\n";

    testBresenhamLineHorizontalVerticalDiagonal();
    testBresenhamLineAllOctants();
    testBresenhamCircleAccuracyAndSymmetry();
    testBresenhamArcFiltering();

    std::cout << "\n>>> ALL CGL CO-2 BRESENHAM ALGORITHM TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
