#include <iostream>
#include <cassert>
#include "../src/AsteroidGridMatrix.h"

void testGridMatrixInitialization() {
    std::cout << "[TEST 1] 2D Array Matrix Dimensions & Initialization...\n";
    AsteroidGridMatrix matrix;

    assert(matrix.getRows() == 6);
    assert(matrix.getCols() == 8);
    assert(matrix.getCapacity() == 48);
    assert(matrix.getActiveCount() == 0);

    // Bounds checking
    assert(matrix.isValidCell(0, 0) == true);
    assert(matrix.isValidCell(5, 7) == true);
    assert(matrix.isValidCell(-1, 0) == false);
    assert(matrix.isValidCell(6, 0) == false);
    assert(matrix.isValidCell(0, 8) == false);

    for (int r = 0; r < matrix.getRows(); ++r) {
        for (int c = 0; c < matrix.getCols(); ++c) {
            const Asteroid& ast = matrix.getAsteroid(r, c);
            assert(!ast.active);
            assert(ast.row == r);
            assert(ast.col == c);
        }
    }
    std::cout << " -> 2D Matrix memory initialized to 6x8 grid (48 cells) with valid indexing.\n";
}

void testProceduralWaveGeneration() {
    std::cout << "\n[TEST 2] Procedural 2D Matrix Generation Across Lanes...\n";
    AsteroidGridMatrix matrix;
    matrix.generateProceduralWave(1, 800.0f, 600.0f, 850.0f);

    int activeCount = matrix.getActiveCount();
    assert(activeCount > 0);
    assert(activeCount <= matrix.getCapacity());

    std::cout << " -> Wave 1 generated " << activeCount << " active obstacles across 2D matrix.\n";
    matrix.displayGridAscii();

    // Verify row lane distribution
    int rowWithActive = 0;
    for (int r = 0; r < matrix.getRows(); ++r) {
        bool rowHasActive = false;
        for (int c = 0; c < matrix.getCols(); ++c) {
            if (matrix.getAsteroid(r, c).active) {
                rowHasActive = true;
                break;
            }
        }
        if (rowHasActive) rowWithActive++;
    }
    assert(rowWithActive >= 4);
    std::cout << " -> Multi-lane vertical spread verified across " << rowWithActive << " rows.\n";
}

void testKinematicsAndBoundaryDeactivation() {
    std::cout << "\n[TEST 3] Matrix Kinematics & Left Boundary Deactivation...\n";
    AsteroidGridMatrix matrix;
    matrix.reset();

    Asteroid ast;
    ast.id = 999;
    ast.x = 10.0f;
    ast.y = 300.0f;
    ast.vx = -15.0f;
    ast.vy = 0.0f;
    ast.radius = 20.0f;
    ast.active = true;

    matrix.setAsteroid(2, 3, ast);
    assert(matrix.getActiveCount() == 1);
    assert(matrix.getAsteroid(2, 3).active == true);

    // Delta 1.0 (approx 60 ticks): x becomes 10 + (-15 * 60) = -890 (past minX of -120.0f)
    matrix.updateField(1.0f, -120.0f, 1600.0f);
    assert(matrix.getAsteroid(2, 3).active == false);
    assert(matrix.getActiveCount() == 0);
    std::cout << " -> Kinematic step correctly deactivated off-screen sector.\n";
}

void test2DCollisionDetectionAndDamage() {
    std::cout << "\n[TEST 4] 2D Spatial Collision Detection & Cell Damage...\n";
    AsteroidGridMatrix matrix;
    matrix.reset();

    Asteroid target;
    target.id = 777;
    target.x = 400.0f;
    target.y = 250.0f;
    target.radius = 25.0f;
    target.health = 50;
    target.maxHealth = 50;
    target.active = true;

    matrix.setAsteroid(3, 4, target);
    assert(matrix.getActiveCount() == 1);

    // Test point collision
    int hitR = -1, hitC = -1;
    bool hitPoint = matrix.checkPointCollision(405.0f, 252.0f, hitR, hitC);
    assert(hitPoint == true);
    assert(hitR == 3 && hitC == 4);

    bool missPoint = matrix.checkPointCollision(100.0f, 100.0f, hitR, hitC);
    assert(missPoint == false);

    // Test circle collision
    bool hitCircle = matrix.checkCircleCollision(420.0f, 250.0f, 10.0f, hitR, hitC);
    assert(hitCircle == true);
    assert(hitR == 3 && hitC == 4);

    // Damage without destruction
    bool destroyed = false;
    matrix.damageAsteroid(3, 4, 30, destroyed);
    assert(destroyed == false);
    assert(matrix.getAsteroid(3, 4).health == 20);
    assert(matrix.getAsteroid(3, 4).active == true);

    // Lethal damage
    matrix.damageAsteroid(3, 4, 25, destroyed);
    assert(destroyed == true);
    assert(matrix.getAsteroid(3, 4).health == 0);
    assert(matrix.getAsteroid(3, 4).active == false);
    assert(matrix.getActiveCount() == 0);
    std::cout << " -> 2D sector collision and damage state transitions verified.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: PL CO-2 TEST SUITE (AsteroidGridMatrix) \n";
    std::cout << "=====================================================\n\n";

    testGridMatrixInitialization();
    testProceduralWaveGeneration();
    testKinematicsAndBoundaryDeactivation();
    test2DCollisionDetectionAndDamage();

    std::cout << "\n>>> ALL PL CO-2 2D ARRAY MATRIX TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
