#include <iostream>
#include <cassert>
#include <cmath>
#include "../src/Renderer.h"

bool isClose(float a, float b, float eps = 0.001f) {
    return std::abs(a - b) < eps;
}

void testTranslation() {
    std::cout << "[TEST 1] Geometric Translation (x' = x + tx, y' = y + ty)...\n";
    Point2D p = { 10.0f, 20.0f };
    Point2D res = Renderer::translatePoint(p, 5.0f, -10.0f);

    assert(isClose(res.x, 15.0f));
    assert(isClose(res.y, 10.0f));

    Point2D origin = { 0.0f, 0.0f };
    Point2D res2 = Renderer::translatePoint(origin, 100.0f, 300.0f);
    assert(isClose(res2.x, 100.0f));
    assert(isClose(res2.y, 300.0f));

    std::cout << " -> Translation correctly shifts local coordinates by translation vector.\n";
}

void testScaling() {
    std::cout << "\n[TEST 2] Geometric Scaling (x' = x * sx, y' = y * sy)...\n";
    Point2D p = { 10.0f, 20.0f };
    Point2D res = Renderer::scalePoint(p, 2.0f, 0.5f);

    assert(isClose(res.x, 20.0f));
    assert(isClose(res.y, 10.0f));

    Point2D p2 = { 4.0f, -6.0f };
    Point2D res2 = Renderer::scalePoint(p2, 1.5f, 1.5f);
    assert(isClose(res2.x, 6.0f));
    assert(isClose(res2.y, -9.0f));

    std::cout << " -> Scaling correctly resizes object vertices relative to origin.\n";
}

void testRotation() {
    std::cout << "\n[TEST 3] Geometric Rotation (x' = x*cos(theta) - y*sin(theta), y' = x*sin(theta) + y*cos(theta))...\n";
    const float PI = 3.14159265f;

    Point2D p = { 10.0f, 0.0f };
    Point2D res90 = Renderer::rotatePoint(p, PI * 0.5f);
    assert(isClose(res90.x, 0.0f));
    assert(isClose(res90.y, 10.0f));

    Point2D res180 = Renderer::rotatePoint(p, PI);
    assert(isClose(res180.x, -10.0f));
    assert(isClose(res180.y, 0.0f));

    Point2D p2 = { 0.0f, 10.0f };
    Point2D resNeg90 = Renderer::rotatePoint(p2, -PI * 0.5f);
    assert(isClose(resNeg90.x, 10.0f));
    assert(isClose(resNeg90.y, 0.0f));

    std::cout << " -> Rotation correctly computes trigonometric coordinates for arbitrary angles.\n";
}

void testCombinedTransformation() {
    std::cout << "\n[TEST 4] Combined Transformation Pipeline (Scale -> Rotate -> Translate)...\n";
    const float PI = 3.14159265f;

    Point2D local = { 10.0f, 0.0f };
    Point2D world = Renderer::transformPoint(local, 2.0f, 2.0f, PI * 0.5f, 50.0f, 100.0f);

    assert(isClose(world.x, 50.0f));
    assert(isClose(world.y, 120.0f));

    std::cout << " -> Full affine transformation (scale, rotate, translate) validated.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: CGL CO-2 TEST SUITE (Transformations) \n";
    std::cout << "=====================================================\n\n";

    testTranslation();
    testScaling();
    testRotation();
    testCombinedTransformation();

    std::cout << "\n>>> ALL CGL CO-2 GEOMETRIC TRANSFORMATION TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
