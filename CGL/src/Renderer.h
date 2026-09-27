#ifndef RENDERER_H
#define RENDERER_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <string>
#include <vector>

// Represents a 2D coordinate in world or screen space
struct Point2D {
    float x;
    float y;
};

// Represents an individual star in the multi-tier parallax starfield
struct Star {
    float x;
    float y;
    float speed;
    float size;
    float brightness;
};

// 2D OpenGL Graphic Renderer handling transformations, geometric primitives, and HUD
class Renderer {
private:
    std::vector<Star> stars;

public:
    Renderer();

    // 2D Geometric Transformations (TRS Pipeline)
    static Point2D translatePoint(Point2D p, float tx, float ty);
    static Point2D scalePoint(Point2D p, float sx, float sy);
    static Point2D rotatePoint(Point2D p, float angleRad);
    static Point2D transformPoint(Point2D p, float sx, float sy, float angleRad, float tx, float ty);

    // Parallax Starfield Management
    void initStarfield(int count, float width, float height);
    void updateStarfield(float width, float height, float delta);

    // OpenGL Camera & Viewport
    static void setup2DProjection(int width, int height);

    // Vector Primitive Rendering
    void renderBackground(float width, float height);
    void renderPlayerShip(float x, float y, float size, bool isAlive, float angle = 0.0f, float scale = 1.0f);
    void renderEnemyShip(float x, float y, float size, float angle = 0.0f, float scale = 1.0f);
    void renderLaserBolt(float x, float y, float length, float width, bool isPlayer = true, float angle = 0.0f);

    // Heads-Up Display (HUD) and Overlays
    void renderHUD(int health, int maxHealth, int shield, int score, int wave, int activeBullets, int maxBullets, float screenW, float screenH);
    void renderGameOver(int finalScore, float screenW, float screenH);
    static void drawBitmapString(float x, float y, void* font, const std::string& text, float r, float g, float b);
};

#endif // RENDERER_H
