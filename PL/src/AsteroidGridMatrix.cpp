#include "AsteroidGridMatrix.h"
#include <cmath>
#include <cstdlib>

AsteroidGridMatrix::AsteroidGridMatrix() : nextAsteroidID(500) {
    reset();
}

void AsteroidGridMatrix::reset() {
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            grid[r][c].id = 0;
            grid[r][c].row = r;
            grid[r][c].col = c;
            grid[r][c].x = 0.0f;
            grid[r][c].y = 0.0f;
            grid[r][c].radius = 18.0f;
            grid[r][c].vx = 0.0f;
            grid[r][c].vy = 0.0f;
            grid[r][c].health = 0;
            grid[r][c].maxHealth = 0;
            grid[r][c].scoreValue = 50;
            grid[r][c].collisionDamage = 20;
            grid[r][c].active = false;
        }
    }
}

void AsteroidGridMatrix::generateProceduralWave(int waveNumber, float worldWidth, float worldHeight, float startOffsetX) {
    reset();

    float topMargin = 70.0f;
    float bottomMargin = 50.0f;
    float playableHeight = worldHeight - topMargin - bottomMargin;
    float laneHeight = playableHeight / static_cast<float>(GRID_ROWS);
    float colSpacing = 110.0f;

    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            // Procedural sparsity pattern: create navigable gaps through the asteroid belt
            // Deterministic pattern based on row, column, and wave
            bool spawn = ((r + c + waveNumber) % 3 != 0) && ((r * 2 + c) % 5 != 0);

            if (spawn) {
                float baseX = startOffsetX + (c * colSpacing);
                float baseY = bottomMargin + (r * laneHeight) + (laneHeight * 0.5f);

                // Slight natural jitter
                float jitterY = ((r * 17 + c * 31) % 15) - 7.0f;
                float sizeVar = 14.0f + static_cast<float>((r * 11 + c * 7 + waveNumber) % 14);

                grid[r][c].id = nextAsteroidID++;
                grid[r][c].row = r;
                grid[r][c].col = c;
                grid[r][c].x = baseX;
                grid[r][c].y = baseY + jitterY;
                grid[r][c].radius = sizeVar;
                grid[r][c].vx = -1.2f - (0.15f * waveNumber);
                grid[r][c].vy = 0.0f;
                grid[r][c].maxHealth = static_cast<int>(sizeVar * 2.0f);
                grid[r][c].health = grid[r][c].maxHealth;
                grid[r][c].scoreValue = 50 + static_cast<int>(sizeVar);
                grid[r][c].collisionDamage = 15 + static_cast<int>(sizeVar * 0.5f);
                grid[r][c].active = true;
            } else {
                grid[r][c].active = false;
            }
        }
    }
}

bool AsteroidGridMatrix::isValidCell(int row, int col) const {
    return (row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS);
}

Asteroid& AsteroidGridMatrix::getAsteroid(int row, int col) {
    if (!isValidCell(row, col)) {
        static Asteroid dummy;
        dummy.active = false;
        return dummy;
    }
    return grid[row][col];
}

const Asteroid& AsteroidGridMatrix::getAsteroid(int row, int col) const {
    if (!isValidCell(row, col)) {
        static Asteroid dummy;
        dummy.active = false;
        return dummy;
    }
    return grid[row][col];
}

bool AsteroidGridMatrix::setAsteroid(int row, int col, const Asteroid& asteroid) {
    if (!isValidCell(row, col)) {
        return false;
    }
    grid[row][col] = asteroid;
    grid[row][col].row = row;
    grid[row][col].col = col;
    return true;
}

void AsteroidGridMatrix::updateField(float delta, float minX, float maxX) {
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            if (grid[r][c].active) {
                grid[r][c].x += grid[r][c].vx * delta * 60.0f;
                grid[r][c].y += grid[r][c].vy * delta * 60.0f;

                // Off-screen left boundary deactivation
                if (grid[r][c].x < minX) {
                    grid[r][c].active = false;
                }
            }
        }
    }
}

bool AsteroidGridMatrix::checkPointCollision(float px, float py, int& hitRow, int& hitCol) const {
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            if (grid[r][c].active) {
                float dx = px - grid[r][c].x;
                float dy = py - grid[r][c].y;
                float distSq = (dx * dx) + (dy * dy);
                float radSq = grid[r][c].radius * grid[r][c].radius;

                if (distSq <= radSq) {
                    hitRow = r;
                    hitCol = c;
                    return true;
                }
            }
        }
    }
    hitRow = -1;
    hitCol = -1;
    return false;
}

bool AsteroidGridMatrix::checkCircleCollision(float cx, float cy, float radius, int& hitRow, int& hitCol) const {
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            if (grid[r][c].active) {
                float dx = cx - grid[r][c].x;
                float dy = cy - grid[r][c].y;
                float distSq = (dx * dx) + (dy * dy);
                float combinedRad = radius + grid[r][c].radius;

                if (distSq <= (combinedRad * combinedRad)) {
                    hitRow = r;
                    hitCol = c;
                    return true;
                }
            }
        }
    }
    hitRow = -1;
    hitCol = -1;
    return false;
}

bool AsteroidGridMatrix::damageAsteroid(int row, int col, int damageAmount, bool& outDestroyed) {
    outDestroyed = false;
    if (!isValidCell(row, col) || !grid[row][col].active) {
        return false;
    }

    grid[row][col].health -= damageAmount;
    if (grid[row][col].health <= 0) {
        grid[row][col].health = 0;
        grid[row][col].active = false;
        outDestroyed = true;
    }
    return true;
}

void AsteroidGridMatrix::deactivate(int row, int col) {
    if (isValidCell(row, col)) {
        grid[row][col].active = false;
    }
}

int AsteroidGridMatrix::getActiveCount() const {
    int count = 0;
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            if (grid[r][c].active) {
                count++;
            }
        }
    }
    return count;
}

int AsteroidGridMatrix::getRows() const {
    return GRID_ROWS;
}

int AsteroidGridMatrix::getCols() const {
    return GRID_COLS;
}

int AsteroidGridMatrix::getCapacity() const {
    return MAX_ASTEROIDS;
}

void AsteroidGridMatrix::displayGridAscii() const {
    std::cout << "\n--- [2D ASTEROID MATRIX STATE (" << GRID_ROWS << "x" << GRID_COLS << ")] ---\n";
    std::cout << "     ";
    for (int c = 0; c < GRID_COLS; ++c) {
        std::cout << "C" << c << " ";
    }
    std::cout << "\n";

    for (int r = 0; r < GRID_ROWS; ++r) {
        std::cout << "[R" << r << "]  ";
        for (int c = 0; c < GRID_COLS; ++c) {
            if (grid[r][c].active) {
                if (grid[r][c].health < grid[r][c].maxHealth) {
                    std::cout << "*  "; // Damaged
                } else {
                    std::cout << "O  "; // Intact
                }
            } else {
                std::cout << ".  "; // Empty sector
            }
        }
        std::cout << "\n";
    }
    std::cout << "Active Sectors: " << getActiveCount() << " / " << MAX_ASTEROIDS << "\n";
}
