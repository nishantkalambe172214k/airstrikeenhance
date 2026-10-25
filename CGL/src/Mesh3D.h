#ifndef MESH3D_H
#define MESH3D_H

#include <vector>
#include <string>
#include <cmath>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// ============================================================================
// [CGL CO-3] 3D GEOMETRIC PRIMITIVES & VECTOR MATH
// Course Outcome 3: Develop 2D and 3D objects using various algorithms.
// ============================================================================
struct Point3D {
    float x;
    float y;
    float z;

    Point3D() : x(0.0f), y(0.0f), z(0.0f) {}
    Point3D(float px, float py, float pz) : x(px), y(py), z(pz) {}

    Point3D operator+(const Point3D& o) const { return Point3D(x + o.x, y + o.y, z + o.z); }
    Point3D operator-(const Point3D& o) const { return Point3D(x - o.x, y - o.y, z - o.z); }
    Point3D operator*(float s) const { return Point3D(x * s, y * s, z * s); }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Point3D normalized() const {
        float l = length();
        if (l < 1e-6f) return Point3D(0.0f, 0.0f, 1.0f);
        return Point3D(x / l, y / l, z / l);
    }

    static Point3D cross(const Point3D& a, const Point3D& b) {
        return Point3D(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    static float dot(const Point3D& a, const Point3D& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
};

struct Color3D {
    float r;
    float g;
    float b;
    float a;

    Color3D() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color3D(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
};

// [CGL CO-3] Polygonal Face: Indexed vertex list, surface normal vector, and RGBA color
struct Face3D {
    std::vector<int> vertexIndices;
    Point3D normal;
    Color3D color;
    std::string component; // e.g. "FUSELAGE", "WINGS", "COCKPIT", "ENGINES", "ROCK"

    Face3D() {}
    Face3D(const std::vector<int>& indices, const Color3D& col, const std::string& comp = "")
        : vertexIndices(indices), color(col), component(comp) {}
};

// ============================================================================
// [CGL CO-3] 4x4 HOMOGENEOUS AFFINE TRANSFORMATION MATRIX
// Column-major order: P' = M * P
// ============================================================================
struct Matrix4x4 {
    float m[16]; // Column-major order matching OpenGL standard

    Matrix4x4();
    static Matrix4x4 identity();
    static Matrix4x4 translation(float tx, float ty, float tz);
    static Matrix4x4 rotationX(float angleRad);
    static Matrix4x4 rotationY(float angleRad);
    static Matrix4x4 rotationZ(float angleRad);
    static Matrix4x4 scaling(float sx, float sy, float sz);

    Matrix4x4 operator*(const Matrix4x4& o) const;
    Point3D transformPoint(const Point3D& p) const;
    Point3D transformVector(const Point3D& v) const;
};

// ============================================================================
// [CGL CO-3] 3D MESH CLASS (Geometry Representation, Construction & Rendering)
// ============================================================================
class Mesh3D {
private:
    std::string name;
    std::vector<Point3D> vertices;
    std::vector<Face3D> faces;

public:
    Mesh3D(const std::string& meshName = "Mesh3D");

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }

    int addVertex(float x, float y, float z);
    int addVertex(const Point3D& pt);
    void addFace(const std::vector<int>& indices, const Color3D& color, const std::string& comp = "");

    void computeNormals();

    const std::vector<Point3D>& getVertices() const { return vertices; }
    const std::vector<Face3D>& getFaces() const { return faces; }
    size_t getVertexCount() const { return vertices.size(); }
    size_t getFaceCount() const { return faces.size(); }

    Point3D computeCentroid() const;

    // [CGL CO-3] In-place 3D Affine Transformations
    void translate(float tx, float ty, float tz);
    void rotateX(float angleRad);
    void rotateY(float angleRad);
    void rotateZ(float angleRad);
    void scale(float sx, float sy, float sz);

    // ========================================================================
    // [CGL CO-3] PROCEDURAL 3D OBJECT GENERATORS
    // ========================================================================
    static Mesh3D createSpaceship();
    static Mesh3D createAsteroid(float baseRadius = 2.0f, int subdivisions = 2);

    // ========================================================================
    // [CGL CO-3] OPENGL 3D RENDERING MODES
    // ========================================================================
    // Renders the mesh using flat-shaded or diffuse-lighted polygonal faces
    void renderFilled(bool enableLighting = true) const;

    // Renders the mesh in wireframe mode using Bresenham / OpenGL line edges
    void renderWireframe(float r = 0.0f, float g = 0.9f, float b = 1.0f, float lineWidth = 1.5f) const;

    // Combined rendering: solid interior with distinct wireframe contours
    void renderFilledWithWireframe(const Color3D& wireColor, float lineWidth = 1.5f) const;
};

// ============================================================================
// [CGL CO-3] 3D VIEWING & PROJECTION PIPELINE UTILITIES
// ============================================================================
class Camera3D {
public:
    // Configures perspective frustum (gluPerspective) and enables depth buffer (GL_DEPTH_TEST)
    static void setupPerspective(int width, int height, float fovDeg = 45.0f, float nearZ = 0.1f, float farZ = 1000.0f);
    // Disables depth testing and restores 2D orthographic projection for HUD/gameplay
    static void restore2DProjection(int width, int height);
};

#endif // MESH3D_H
