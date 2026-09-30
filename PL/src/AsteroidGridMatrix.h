#ifndef ASTEROID_GRID_MATRIX_H
#define ASTEROID_GRID_MATRIX_H

#include <iostream>

const int GRID_ROWS = 6;
const int GRID_COLS = 8;
const int MAX_ASTEROIDS = GRID_ROWS * GRID_COLS;

struct Asteroid {
    int id;
    int row;
    int col;
    float x;
    float y;
    float radius;
    float vx;
    float vy;
    int health;
    int maxHealth;
    int scoreValue;
    int collisionDamage;
    bool active;
};

class AsteroidGridMatrix {
private:
    // 2D Array representing spatial sector matrix of obstacles
    Asteroid grid[GRID_ROWS][GRID_COLS];
    int nextAsteroidID;

public:
    AsteroidGridMatrix();

    // 2D Matrix Lifecycle & Generation
    void reset();
    void generateProceduralWave(int waveNumber, float worldWidth, float worldHeight, float startOffsetX = 850.0f);

    // 2D Element Access with Defensive Bounds Validation
    bool isValidCell(int row, int col) const;
    Asteroid& getAsteroid(int row, int col);
    const Asteroid& getAsteroid(int row, int col) const;
    bool setAsteroid(int row, int col, const Asteroid& asteroid);

    // Physics & Matrix Kinematics
    void updateField(float delta, float minX = -120.0f, float maxX = 1600.0f);
    bool checkPointCollision(float px, float py, int& hitRow, int& hitCol) const;
    bool checkCircleCollision(float cx, float cy, float radius, int& hitRow, int& hitCol) const;
    bool damageAsteroid(int row, int col, int damageAmount, bool& outDestroyed);
    void deactivate(int row, int col);

    // Diagnostics & Metrics
    int getActiveCount() const;
    int getRows() const;
    int getCols() const;
    int getCapacity() const;
    void displayGridAscii() const;
};

#endif // ASTEROID_GRID_MATRIX_H
