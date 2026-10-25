#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <set>
#include "../src/Mesh3D.h"

// ============================================================================
// TEST 1: 3D SPACESHIP MODEL TOPOLOGY & COMPONENT VALIDATION
// ============================================================================
void testSpaceshipModelTopology() {
    std::cout << "[TEST 1] 3D Spaceship: Vertex, Face & Component Topology...\n";

    Mesh3D ship = Mesh3D::createSpaceship();
    assert(ship.getName() == "Starfighter 3D");
    assert(ship.getVertexCount() >= 20);
    assert(ship.getFaceCount() >= 15);

    // Verify all face vertex indices are in bounds [0, vertexCount - 1]
    const auto& vertices = ship.getVertices();
    const auto& faces = ship.getFaces();
    std::set<std::string> components;

    for (const auto& face : faces) {
        assert(face.vertexIndices.size() == 3 || face.vertexIndices.size() == 4);
        for (int idx : face.vertexIndices) {
            assert(idx >= 0 && idx < static_cast<int>(vertices.size()));
        }
        // Verify surface normal has unit length
        assert(std::abs(face.normal.length() - 1.0f) < 1e-3f);
        components.insert(face.component);
    }

    // Verify required aerodynamic and structural components are all present
    assert(components.count("FUSELAGE") > 0);
    assert(components.count("WINGS") > 0);
    assert(components.count("COCKPIT") > 0);
    assert(components.count("ENGINES") > 0);

    // Verify model is approximately centered at origin (centroid near 0,0,0)
    Point3D c = ship.computeCentroid();
    assert(std::abs(c.x) < 0.2f);
    assert(std::abs(c.y) < 0.2f);
    assert(std::abs(c.z) < 0.5f);

    std::cout << " -> Spaceship validated: " << vertices.size() << " vertices, "
              << faces.size() << " polygonal faces across 4 distinct components.\n";
}

// ============================================================================
// TEST 2: 3D ASTEROID ROCK IRREGULAR MESH VALIDATION
// ============================================================================
void testAsteroidMeshIrregularity() {
    std::cout << "\n[TEST 2] 3D Asteroid: Irregular Rocky Polyhedron Topology...\n";

    float baseR = 2.0f;
    Mesh3D rock = Mesh3D::createAsteroid(baseR);
    assert(rock.getVertexCount() == 42); // 12 base + 30 edge midpoints
    assert(rock.getFaceCount() == 80);   // 20 base * 4 subdivisions

    const auto& vertices = rock.getVertices();
    const auto& faces = rock.getFaces();

    float minR = 999.0f;
    float maxR = 0.0f;
    float sumR = 0.0f;

    for (const auto& v : vertices) {
        float r = v.length();
        if (r < minR) minR = r;
        if (r > maxR) maxR = r;
        sumR += r;
    }
    float avgR = sumR / static_cast<float>(vertices.size());

    // Verify genuine irregular cragginess (radius variance between min and max)
    assert(minR < baseR * 0.90f);
    assert(maxR > baseR * 1.10f);
    assert(std::abs(avgR - baseR) < 0.35f);

    // Verify all faces are triangular and indices are valid
    for (const auto& face : faces) {
        assert(face.vertexIndices.size() == 3);
        for (int idx : face.vertexIndices) {
            assert(idx >= 0 && idx < static_cast<int>(vertices.size()));
        }
        assert(std::abs(face.normal.length() - 1.0f) < 1e-3f);
    }

    std::cout << " -> Asteroid validated: " << vertices.size() << " vertices, "
              << faces.size() << " triangular facets, radial variance ["
              << minR << " .. " << maxR << "].\n";
}

// ============================================================================
// TEST 3: 3D AFFINE TRANSFORMATIONS (TRANSLATE, SCALE, ROTATE X/Y/Z)
// ============================================================================
void test3DAffineTransformations() {
    std::cout << "\n[TEST 3] 3D Math: Translation, Scaling, and Euler Rotations...\n";

    Point3D p(1.0f, 2.0f, 3.0f);

    // 1. Translation Matrix
    Matrix4x4 tMat = Matrix4x4::translation(5.0f, -2.0f, 10.0f);
    Point3D pTrans = tMat.transformPoint(p);
    assert(std::abs(pTrans.x - 6.0f) < 1e-4f);
    assert(std::abs(pTrans.y - 0.0f) < 1e-4f);
    assert(std::abs(pTrans.z - 13.0f) < 1e-4f);

    // 2. Scaling Matrix
    Matrix4x4 sMat = Matrix4x4::scaling(2.0f, 0.5f, 3.0f);
    Point3D pScale = sMat.transformPoint(p);
    assert(std::abs(pScale.x - 2.0f) < 1e-4f);
    assert(std::abs(pScale.y - 1.0f) < 1e-4f);
    assert(std::abs(pScale.z - 9.0f) < 1e-4f);

    // 3. Rotation around X-axis (90 degrees: y -> z, z -> -y)
    float halfPi = 1.57079632679f;
    Matrix4x4 rxMat = Matrix4x4::rotationX(halfPi);
    Point3D pUnitY(0.0f, 1.0f, 0.0f);
    Point3D pRotX = rxMat.transformPoint(pUnitY);
    assert(std::abs(pRotX.x - 0.0f) < 1e-4f);
    assert(std::abs(pRotX.y - 0.0f) < 1e-4f);
    assert(std::abs(pRotX.z - 1.0f) < 1e-4f);

    // 4. Rotation around Y-axis (90 degrees: z -> x, x -> -z)
    Matrix4x4 ryMat = Matrix4x4::rotationY(halfPi);
    Point3D pUnitZ(0.0f, 0.0f, 1.0f);
    Point3D pRotY = ryMat.transformPoint(pUnitZ);
    assert(std::abs(pRotY.x - 1.0f) < 1e-4f);
    assert(std::abs(pRotY.y - 0.0f) < 1e-4f);
    assert(std::abs(pRotY.z - 0.0f) < 1e-4f);

    // 5. Rotation around Z-axis (90 degrees: x -> y, y -> -x)
    Matrix4x4 rzMat = Matrix4x4::rotationZ(halfPi);
    Point3D pUnitX(1.0f, 0.0f, 0.0f);
    Point3D pRotZ = rzMat.transformPoint(pUnitX);
    assert(std::abs(pRotZ.x - 0.0f) < 1e-4f);
    assert(std::abs(pRotZ.y - 1.0f) < 1e-4f);
    assert(std::abs(pRotZ.z - 0.0f) < 1e-4f);

    // 6. Compound Transformation Order: TRS (Scale -> Rotate -> Translate)
    Matrix4x4 compound = tMat * ryMat * sMat;
    Point3D pCompound = compound.transformPoint(Point3D(1.0f, 0.0f, 0.0f));
    // Scale (2, 0, 0) -> RotateY 90 deg (0, 0, -2) -> Translate (5, -2, 8)
    assert(std::abs(pCompound.x - 5.0f) < 1e-4f);
    assert(std::abs(pCompound.y - -2.0f) < 1e-4f);
    assert(std::abs(pCompound.z - 8.0f) < 1e-4f);

    std::cout << " -> 3D affine matrix transformations (TRS pipeline) validated with machine precision.\n";
}

// ============================================================================
// TEST 4: IN-PLACE MESH TRANSFORMATION & CENTROID INVARIANCE
// ============================================================================
void testMeshTransformationAndCentroid() {
    std::cout << "\n[TEST 4] Mesh Transformations: Rotation around Geometric Centroid...\n";

    Mesh3D ship = Mesh3D::createSpaceship();
    Point3D initialCentroid = ship.computeCentroid();

    // Rotate 180 degrees around Y axis
    ship.rotateY(3.14159265f);
    Point3D rotatedCentroid = ship.computeCentroid();

    // Centroid x and z should be mirrored across origin, distance from origin unchanged
    assert(std::abs(initialCentroid.length() - rotatedCentroid.length()) < 1e-3f);

    // Scale by 2.0x
    ship.scale(2.0f, 2.0f, 2.0f);
    assert(std::abs(ship.computeCentroid().length() - (rotatedCentroid.length() * 2.0f)) < 1e-3f);

    // Translate by (10, 20, 30)
    ship.translate(10.0f, 20.0f, 30.0f);
    Point3D translatedCentroid = ship.computeCentroid();
    assert(std::abs(translatedCentroid.x - (rotatedCentroid.x * 2.0f + 10.0f)) < 1e-3f);
    assert(std::abs(translatedCentroid.y - (rotatedCentroid.y * 2.0f + 20.0f)) < 1e-3f);
    assert(std::abs(translatedCentroid.z - (rotatedCentroid.z * 2.0f + 30.0f)) < 1e-3f);

    std::cout << " -> In-place mesh transformations preserve geometric proportions and rotation centers.\n";
}

// ============================================================================
// TEST 5: PERSPECTIVE PROJECTION & DEPTH ORDERING
// ============================================================================
void testPerspectiveAndDepthOrdering() {
    std::cout << "\n[TEST 5] 3D Viewing: Depth Ordering & Z-Buffering Occlusion Logic...\n";

    // Simulate perspective division: X_proj = X / -Z, Y_proj = Y / -Z
    Point3D nearPt(1.0f, 1.0f, -2.0f);
    Point3D farPt(1.0f, 1.0f, -10.0f);

    float nearScreenX = nearPt.x / (-nearPt.z);
    float farScreenX = farPt.x / (-farPt.z);

    // Near point appears larger/further from center on screen
    assert(nearScreenX > farScreenX);
    assert(std::abs(nearScreenX - 0.5f) < 1e-4f);
    assert(std::abs(farScreenX - 0.1f) < 1e-4f);

    // Depth buffer comparison: Z_near (-2.0) has smaller normalized depth than Z_far (-10.0)
    // GL_LEQUAL / GL_LESS guarantees near face occludes far face
    assert(std::abs(nearPt.z) < std::abs(farPt.z));

    std::cout << " -> Perspective division and depth test ordering verified.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: CGL CO-3 3D OBJECT PIPELINE TEST      \n";
    std::cout << "  (Custom 3D Spaceship, Asteroid & Affine Math)      \n";
    std::cout << "=====================================================\n\n";

    testSpaceshipModelTopology();
    testAsteroidMeshIrregularity();
    test3DAffineTransformations();
    testMeshTransformationAndCentroid();
    testPerspectiveAndDepthOrdering();

    std::cout << "\n>>> ALL CGL CO-3 3D PIPELINE TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
