#include "Mesh3D.h"
#include <cmath>
#include <algorithm>
#include <map>

// ============================================================================
// [CGL CO-3] 4x4 AFFINE TRANSFORMATION MATRIX
// Column-Major layout matching OpenGL:
// [ m0  m4  m8  m12 ]   [ X ]
// [ m1  m5  m9  m13 ] * [ Y ]
// [ m2  m6  m10 m14 ]   [ Z ]
// [ m3  m7  m11 m15 ]   [ 1 ]
// ============================================================================
Matrix4x4::Matrix4x4() {
    for (int i = 0; i < 16; ++i) {
        m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
}

Matrix4x4 Matrix4x4::identity() {
    return Matrix4x4();
}

// [CGL CO-3] 3D Translation Matrix: T(tx, ty, tz)
Matrix4x4 Matrix4x4::translation(float tx, float ty, float tz) {
    Matrix4x4 res = identity();
    res.m[12] = tx;
    res.m[13] = ty;
    res.m[14] = tz;
    return res;
}

// [CGL CO-3] 3D Rotation Matrix around X-Axis (Pitch)
Matrix4x4 Matrix4x4::rotationX(float angleRad) {
    Matrix4x4 res = identity();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    res.m[5]  =  c;
    res.m[6]  =  s;
    res.m[9]  = -s;
    res.m[10] =  c;
    return res;
}

// [CGL CO-3] 3D Rotation Matrix around Y-Axis (Yaw)
Matrix4x4 Matrix4x4::rotationY(float angleRad) {
    Matrix4x4 res = identity();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    res.m[0]  =  c;
    res.m[2]  = -s;
    res.m[8]  =  s;
    res.m[10] =  c;
    return res;
}

// [CGL CO-3] 3D Rotation Matrix around Z-Axis (Roll)
Matrix4x4 Matrix4x4::rotationZ(float angleRad) {
    Matrix4x4 res = identity();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    res.m[0] =  c;
    res.m[1] =  s;
    res.m[4] = -s;
    res.m[5] =  c;
    return res;
}

// [CGL CO-3] 3D Scaling Matrix: S(sx, sy, sz)
Matrix4x4 Matrix4x4::scaling(float sx, float sy, float sz) {
    Matrix4x4 res = identity();
    res.m[0]  = sx;
    res.m[5]  = sy;
    res.m[10] = sz;
    return res;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& o) const {
    Matrix4x4 res;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += m[k * 4 + r] * o.m[c * 4 + k];
            }
            res.m[c * 4 + r] = sum;
        }
    }
    return res;
}

Point3D Matrix4x4::transformPoint(const Point3D& p) const {
    float x = m[0] * p.x + m[4] * p.y + m[8]  * p.z + m[12];
    float y = m[1] * p.x + m[5] * p.y + m[9]  * p.z + m[13];
    float z = m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14];
    float w = m[3] * p.x + m[7] * p.y + m[11] * p.z + m[15];
    if (std::abs(w) > 1e-6f && std::abs(w - 1.0f) > 1e-6f) {
        x /= w; y /= w; z /= w;
    }
    return Point3D(x, y, z);
}

Point3D Matrix4x4::transformVector(const Point3D& v) const {
    float x = m[0] * v.x + m[4] * v.y + m[8]  * v.z;
    float y = m[1] * v.x + m[5] * v.y + m[9]  * v.z;
    float z = m[2] * v.x + m[6] * v.y + m[10] * v.z;
    return Point3D(x, y, z);
}

// ============================================================================
// MESH3D IMPLEMENTATION
// ============================================================================
Mesh3D::Mesh3D(const std::string& meshName) : name(meshName) {}

int Mesh3D::addVertex(float x, float y, float z) {
    vertices.push_back(Point3D(x, y, z));
    return static_cast<int>(vertices.size() - 1);
}

int Mesh3D::addVertex(const Point3D& pt) {
    vertices.push_back(pt);
    return static_cast<int>(vertices.size() - 1);
}

void Mesh3D::addFace(const std::vector<int>& indices, const Color3D& color, const std::string& comp) {
    if (indices.size() < 3) return;
    Face3D face(indices, color, comp);

    // Compute face surface normal from first three vertices
    Point3D v0 = vertices[indices[0]];
    Point3D v1 = vertices[indices[1]];
    Point3D v2 = vertices[indices[2]];
    Point3D edge1 = v1 - v0;
    Point3D edge2 = v2 - v0;
    face.normal = Point3D::cross(edge1, edge2).normalized();

    faces.push_back(face);
}

// [CGL CO-3] Computes unit surface normal vector via vector cross product:
// N = ((v1 - v0) x (v2 - v0)) / ||(v1 - v0) x (v2 - v0)||
void Mesh3D::computeNormals() {
    for (auto& face : faces) {
        if (face.vertexIndices.size() >= 3) {
            Point3D v0 = vertices[face.vertexIndices[0]];
            Point3D v1 = vertices[face.vertexIndices[1]];
            Point3D v2 = vertices[face.vertexIndices[2]];
            Point3D edge1 = v1 - v0;
            Point3D edge2 = v2 - v0;
            face.normal = Point3D::cross(edge1, edge2).normalized();
        }
    }
}

// [CGL CO-3] Computes geometric center of mass: sum(vertices) / N
Point3D Mesh3D::computeCentroid() const {
    if (vertices.empty()) return Point3D(0.0f, 0.0f, 0.0f);
    Point3D sum(0.0f, 0.0f, 0.0f);
    for (const auto& v : vertices) {
        sum = sum + v;
    }
    float inv = 1.0f / static_cast<float>(vertices.size());
    return sum * inv;
}

// [CGL CO-3] In-place 3D Affine Transformations (pure mathematical vertex updates)
void Mesh3D::translate(float tx, float ty, float tz) {
    Matrix4x4 mat = Matrix4x4::translation(tx, ty, tz);
    for (auto& v : vertices) {
        v = mat.transformPoint(v);
    }
}

void Mesh3D::rotateX(float angleRad) {
    Matrix4x4 mat = Matrix4x4::rotationX(angleRad);
    for (auto& v : vertices) {
        v = mat.transformPoint(v);
    }
    for (auto& f : faces) {
        f.normal = mat.transformVector(f.normal).normalized();
    }
}

void Mesh3D::rotateY(float angleRad) {
    Matrix4x4 mat = Matrix4x4::rotationY(angleRad);
    for (auto& v : vertices) {
        v = mat.transformPoint(v);
    }
    for (auto& f : faces) {
        f.normal = mat.transformVector(f.normal).normalized();
    }
}

void Mesh3D::rotateZ(float angleRad) {
    Matrix4x4 mat = Matrix4x4::rotationZ(angleRad);
    for (auto& v : vertices) {
        v = mat.transformPoint(v);
    }
    for (auto& f : faces) {
        f.normal = mat.transformVector(f.normal).normalized();
    }
}

void Mesh3D::scale(float sx, float sy, float sz) {
    Matrix4x4 mat = Matrix4x4::scaling(sx, sy, sz);
    for (auto& v : vertices) {
        v = mat.transformPoint(v);
    }
    computeNormals();
}

// ============================================================================
// [CGL CO-3] PROCEDURAL 3D SPACESHIP GENERATOR
// Course Outcome 3: Develop 3D objects using polygons and algorithms.
// Constructs 4 distinct visual components: Fuselage, Cockpit, Wings, Engines.
// ============================================================================
Mesh3D Mesh3D::createSpaceship() {
    Mesh3D ship("Starfighter 3D");

    // [CGL CO-3] Component-specific distinct color palette
    Color3D cFuselage(0.20f, 0.28f, 0.42f);   // Metallic steel blue
    Color3D cFuselageDark(0.12f, 0.18f, 0.28f);
    Color3D cWings(0.32f, 0.45f, 0.70f);      // Cobalt blue
    Color3D cCockpit(0.15f, 0.90f, 1.0f);     // Cyan glass canopy
    Color3D cEngines(0.12f, 0.12f, 0.15f);    // Dark alloy housing
    Color3D cThrusters(1.0f, 0.55f, 0.10f);   // Glowing plasma amber

    // 1. Nose & Main Fuselage Vertices
    int vNose       = ship.addVertex( 0.0f,  0.0f,  2.6f);  // 0: Nose tip
    int vDorsalMid  = ship.addVertex( 0.0f,  0.55f, 0.6f);  // 1: Top ridge front
    int vDorsalAft  = ship.addVertex( 0.0f,  0.60f,-1.2f);  // 2: Top ridge rear
    int vVentralMid = ship.addVertex( 0.0f, -0.40f, 0.6f);  // 3: Bottom keel front
    int vVentralAft = ship.addVertex( 0.0f, -0.45f,-1.2f);  // 4: Bottom keel rear

    int vPortFront  = ship.addVertex(-0.65f, 0.0f,  0.8f);  // 5: Left flank mid
    int vPortRear   = ship.addVertex(-0.75f, 0.0f, -1.3f);  // 6: Left flank rear
    int vStbdFront  = ship.addVertex( 0.65f, 0.0f,  0.8f);  // 7: Right flank mid
    int vStbdRear   = ship.addVertex( 0.75f, 0.0f, -1.3f);  // 8: Right flank rear

    // Fuselage Faces (Triangles and Quads)
    // Nose cone facets
    ship.addFace({ vNose, vDorsalMid, vPortFront }, cFuselage, "FUSELAGE");
    ship.addFace({ vNose, vStbdFront, vDorsalMid }, cFuselage, "FUSELAGE");
    ship.addFace({ vNose, vPortFront, vVentralMid }, cFuselageDark, "FUSELAGE");
    ship.addFace({ vNose, vVentralMid, vStbdFront }, cFuselageDark, "FUSELAGE");

    // Fuselage flank quads
    ship.addFace({ vDorsalMid, vDorsalAft, vPortRear, vPortFront }, cFuselage, "FUSELAGE");
    ship.addFace({ vDorsalMid, vStbdFront, vStbdRear, vDorsalAft }, cFuselage, "FUSELAGE");
    ship.addFace({ vPortFront, vPortRear, vVentralAft, vVentralMid }, cFuselageDark, "FUSELAGE");
    ship.addFace({ vStbdFront, vVentralMid, vVentralAft, vStbdRear }, cFuselageDark, "FUSELAGE");

    // 2. Cockpit Canopy Vertices (Diamond raised canopy)
    int vCanopyApex  = ship.addVertex( 0.0f,  0.85f, 0.4f);  // 9: Canopy high point
    int vCanopyFront = ship.addVertex( 0.0f,  0.55f, 1.4f);  // 10: Canopy front slope
    int vCanopyPort  = ship.addVertex(-0.30f, 0.55f, 0.3f);  // 11: Canopy left
    int vCanopyStbd  = ship.addVertex( 0.30f, 0.55f, 0.3f);  // 12: Canopy right
    int vCanopyRear  = ship.addVertex( 0.0f,  0.55f,-0.4f);  // 13: Canopy rear slope

    // Canopy glass faces
    ship.addFace({ vCanopyFront, vCanopyApex, vCanopyPort }, cCockpit, "COCKPIT");
    ship.addFace({ vCanopyFront, vCanopyStbd, vCanopyApex }, cCockpit, "COCKPIT");
    ship.addFace({ vCanopyApex, vCanopyRear, vCanopyPort }, cCockpit, "COCKPIT");
    ship.addFace({ vCanopyApex, vCanopyStbd, vCanopyRear }, cCockpit, "COCKPIT");

    // 3. Swept-Back Wings Vertices
    int vWingPortTip     = ship.addVertex(-2.8f,  0.0f, -1.8f); // 14: Left wingtip
    int vWingPortWinglet = ship.addVertex(-2.8f,  0.65f,-1.8f); // 15: Left winglet
    int vWingStbdTip     = ship.addVertex( 2.8f,  0.0f, -1.8f); // 16: Right wingtip
    int vWingStbdWinglet = ship.addVertex( 2.8f,  0.65f,-1.8f); // 17: Right winglet

    // Wing surfaces (Top & Bottom quads)
    ship.addFace({ vPortFront, vWingPortTip, vPortRear }, cWings, "WINGS");
    ship.addFace({ vPortRear, vWingPortTip, vPortFront }, cFuselageDark, "WINGS");
    ship.addFace({ vStbdFront, vStbdRear, vWingStbdTip }, cWings, "WINGS");
    ship.addFace({ vStbdRear, vWingStbdTip, vStbdFront }, cFuselageDark, "WINGS");

    // Winglet vertical fins
    ship.addFace({ vWingPortTip, vWingPortWinglet, vPortRear }, cWings, "WINGS");
    ship.addFace({ vWingStbdTip, vStbdRear, vWingStbdWinglet }, cWings, "WINGS");

    // 4. Twin Rear Thruster Engines Vertices
    int vEngLeftCenter  = ship.addVertex(-0.35f, 0.0f, -1.35f); // 18
    int vEngRightCenter = ship.addVertex( 0.35f, 0.0f, -1.35f); // 19

    // Engine nozzles (exhaust quad plates)
    int vExhaust0 = ship.addVertex(-0.55f,  0.20f, -1.35f);
    int vExhaust1 = ship.addVertex(-0.15f,  0.20f, -1.35f);
    int vExhaust2 = ship.addVertex(-0.15f, -0.20f, -1.35f);
    int vExhaust3 = ship.addVertex(-0.55f, -0.20f, -1.35f);

    int vExhaust4 = ship.addVertex( 0.15f,  0.20f, -1.35f);
    int vExhaust5 = ship.addVertex( 0.55f,  0.20f, -1.35f);
    int vExhaust6 = ship.addVertex( 0.55f, -0.20f, -1.35f);
    int vExhaust7 = ship.addVertex( 0.15f, -0.20f, -1.35f);

    ship.addFace({ vExhaust0, vExhaust1, vExhaust2, vExhaust3 }, cThrusters, "ENGINES");
    ship.addFace({ vExhaust4, vExhaust5, vExhaust6, vExhaust7 }, cThrusters, "ENGINES");

    // Rear bulkhead cap
    ship.addFace({ vDorsalAft, vStbdRear, vVentralAft, vPortRear }, cEngines, "FUSELAGE");

    // Recenter model precisely at origin so rotations occur around its exact center
    Point3D center = ship.computeCentroid();
    ship.translate(-center.x, -center.y, -center.z);

    ship.computeNormals();
    return ship;
}

// ============================================================================
// [CGL CO-3] PROCEDURAL 3D ASTEROID GENERATOR (Irregular Perturbed Polyhedron)
// Course Outcome 3: Develop 3D objects using algorithms.
// - Starts from regular icosahedron (12 vertices, 20 triangular faces).
// - Subdivides each face into 4 sub-triangles (producing 42 vertices, 80 faces).
// - Perturbs vertices radially with harmonic pseudo-noise to form rocky silhouette.
// ============================================================================
Mesh3D Mesh3D::createAsteroid(float baseRadius, int) {
    Mesh3D rock("Asteroid Rock 3D");

    // Golden ratio for icosahedron vertices: t = (1 + sqrt(5)) / 2
    const float t = (1.0f + std::sqrt(5.0f)) * 0.5f;

    // 12 base vertices of an icosahedron
    std::vector<Point3D> baseVerts = {
        Point3D(-1.0f,  t, 0.0f),
        Point3D( 1.0f,  t, 0.0f),
        Point3D(-1.0f, -t, 0.0f),
        Point3D( 1.0f, -t, 0.0f),
        Point3D( 0.0f, -1.0f,  t),
        Point3D( 0.0f,  1.0f,  t),
        Point3D( 0.0f, -1.0f, -t),
        Point3D( 0.0f,  1.0f, -t),
        Point3D( t, 0.0f, -1.0f),
        Point3D( t, 0.0f,  1.0f),
        Point3D(-t, 0.0f, -1.0f),
        Point3D(-t, 0.0f,  1.0f)
    };

    // 20 triangular faces of an icosahedron
    std::vector<std::vector<int>> baseTriangles = {
        {0, 11, 5},  {0, 5, 1},   {0, 1, 7},   {0, 7, 10},  {0, 10, 11},
        {1, 5, 9},   {5, 11, 4},  {11, 10, 2}, {10, 7, 6},  {7, 1, 8},
        {3, 9, 4},   {3, 4, 2},   {3, 2, 6},   {3, 6, 8},   {3, 8, 9},
        {4, 9, 5},   {2, 4, 11},  {6, 2, 10},  {8, 6, 7},   {9, 8, 1}
    };

    // Deterministic pseudo-random irregularity generator
    auto pseudoNoise = [](float x, float y, float z) -> float {
        float val = std::sin(x * 12.9898f + y * 78.233f + z * 37.719f) * 43758.5453f;
        return val - std::floor(val);
    };

    // Subdivide icosahedron: create 42 vertices and 80 triangular facets
    std::vector<Point3D> subVerts = baseVerts;
    std::map<int64_t, int> midpointCache;

    auto getMidpoint = [&](int i1, int i2) -> int {
        int64_t key = (static_cast<int64_t>(std::min(i1, i2)) << 32) | static_cast<int64_t>(std::max(i1, i2));
        auto it = midpointCache.find(key);
        if (it != midpointCache.end()) {
            return it->second;
        }
        Point3D p1 = subVerts[i1];
        Point3D p2 = subVerts[i2];
        Point3D mid = (p1 + p2) * 0.5f;
        subVerts.push_back(mid);
        int idx = static_cast<int>(subVerts.size() - 1);
        midpointCache[key] = idx;
        return idx;
    };

    std::vector<std::vector<int>> fineTriangles;
    for (const auto& tri : baseTriangles) {
        int a = getMidpoint(tri[0], tri[1]);
        int b = getMidpoint(tri[1], tri[2]);
        int c = getMidpoint(tri[2], tri[0]);

        fineTriangles.push_back({ tri[0], a, c });
        fineTriangles.push_back({ tri[1], b, a });
        fineTriangles.push_back({ tri[2], c, b });
        fineTriangles.push_back({ a, b, c });
    }

    // Apply irregular radial displacements to produce craggy rocky surface
    for (size_t i = 0; i < subVerts.size(); ++i) {
        Point3D norm = subVerts[i].normalized();
        float n = pseudoNoise(norm.x * 2.5f, norm.y * 2.5f, norm.z * 2.5f);
        // Perturb radius between 75% and 125% of baseRadius
        float r = baseRadius * (0.80f + 0.38f * n);
        Point3D perturbed = norm * r;
        rock.addVertex(perturbed);
    }

    // Add faces with varying basalt/granite rock color palette
    for (size_t i = 0; i < fineTriangles.size(); ++i) {
        const auto& tri = fineTriangles[i];
        float shade = 0.40f + 0.35f * pseudoNoise(static_cast<float>(i), 3.14f, 1.41f);
        Color3D rockColor(
            0.45f * shade + 0.12f,
            0.38f * shade + 0.08f,
            0.30f * shade + 0.05f
        );
        rock.addFace(tri, rockColor, "ROCK");
    }

    // Recenter asteroid at exact origin
    Point3D center = rock.computeCentroid();
    rock.translate(-center.x, -center.y, -center.z);

    rock.computeNormals();
    return rock;
}

// ============================================================================
// [CGL CO-3] OPENGL 3D RENDERING IMPLEMENTATION
// ============================================================================

// [CGL CO-3] Solid Polygon Face Rendering with Lambertian Diffuse Lighting
// Shading formula: I = I_ambient + I_diffuse * max(0, N . L)
void Mesh3D::renderFilled(bool enableLighting) const {
    // Light direction vector (from front-top-right)
    Point3D lightDir = Point3D(0.5f, 0.7f, 0.6f).normalized();

    for (const auto& face : faces) {
        float diffuse = 1.0f;
        if (enableLighting) {
            float dotVal = Point3D::dot(face.normal, lightDir);
            diffuse = 0.35f + 0.65f * std::max(0.0f, dotVal); // 35% ambient + 65% diffuse
        }

        glColor4f(
            face.color.r * diffuse,
            face.color.g * diffuse,
            face.color.b * diffuse,
            face.color.a
        );

        if (face.vertexIndices.size() == 3) {
            glBegin(GL_TRIANGLES);
            glNormal3f(face.normal.x, face.normal.y, face.normal.z);
            for (int idx : face.vertexIndices) {
                const Point3D& v = vertices[idx];
                glVertex3f(v.x, v.y, v.z);
            }
            glEnd();
        } else if (face.vertexIndices.size() == 4) {
            glBegin(GL_QUADS);
            glNormal3f(face.normal.x, face.normal.y, face.normal.z);
            for (int idx : face.vertexIndices) {
                const Point3D& v = vertices[idx];
                glVertex3f(v.x, v.y, v.z);
            }
            glEnd();
        } else {
            glBegin(GL_POLYGON);
            glNormal3f(face.normal.x, face.normal.y, face.normal.z);
            for (int idx : face.vertexIndices) {
                const Point3D& v = vertices[idx];
                glVertex3f(v.x, v.y, v.z);
            }
            glEnd();
        }
    }
}

// [CGL CO-3] 3D Wireframe rendering via GL_LINE_LOOP on polygonal face boundaries
void Mesh3D::renderWireframe(float r, float g, float b, float lineWidth) const {
    glLineWidth(lineWidth);
    glColor3f(r, g, b);

    for (const auto& face : faces) {
        glBegin(GL_LINE_LOOP);
        for (int idx : face.vertexIndices) {
            const Point3D& v = vertices[idx];
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }
}

void Mesh3D::renderFilledWithWireframe(const Color3D& wireColor, float lineWidth) const {
    // 1. Draw solid filled faces with polygon offset to prevent Z-fighting
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    renderFilled(true);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // 2. Draw crisp wireframe contours
    renderWireframe(wireColor.r, wireColor.g, wireColor.b, lineWidth);
}

// ============================================================================
// [CGL CO-3] CAMERA3D PROJECTION UTILITIES
// ============================================================================

// [CGL CO-3] Configures 3D perspective viewing frustum and enables hardware depth test
void Camera3D::setupPerspective(int width, int height, float fovDeg, float nearZ, float farZ) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = static_cast<float>(width) / static_cast<float>(height > 0 ? height : 1);
    gluPerspective(fovDeg, aspect, nearZ, farZ);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // [CGL CO-3] Enable Depth Buffer testing for genuine 3D occlusion
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

// [CGL CO-3] Restores 2D orthographic projection and alpha blending for HUD/gameplay
void Camera3D::restore2DProjection(int width, int height) {
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<double>(width), 0.0, static_cast<double>(height));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Re-enable 2D alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
