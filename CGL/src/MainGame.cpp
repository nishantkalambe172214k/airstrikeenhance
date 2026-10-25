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

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "../../PL/src/BulletPoolArray.h"
#include "../../PL/src/EnemyLinkedList.h"
#include "../../PL/src/AsteroidGridMatrix.h"
#include "../../COA/ALU/ALU80386.h"
#include "Renderer.h"
#include "Bresenham.h"
#include "NetworkBridge.h"
#include "AreaFill.h"
#include "Mesh3D.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int TARGET_FPS = 60;
const int FRAME_DELAY_MS = 1000 / TARGET_FPS;

Renderer renderer;
BulletPoolArray bulletPool;
EnemyLinkedList enemyList;
AsteroidGridMatrix asteroidGrid;
ALU80386 alu;
NetworkBridge psoopBridge;
RemoteGameState psoopState;
std::vector<RemoteEnemy> psoopEnemies;
bool psoopOnline = false;

float playerX = 100.0f;
float playerY = 300.0f;
float playerSpeed = 5.5f;
float playerAngle = 0.0f;
float playerScale = 1.0f;
int playerHealth = 100;
const int PLAYER_MAX_HEALTH = 100;
int playerShield = 50;
const int PLAYER_MAX_SHIELD = 50;
uint32_t playerScore = 0;
bool playerAlive = true;
bool isGameOver = false;

int fireCooldown = 0;
const int FIRE_COOLDOWN_TICKS = 9;

int currentWave = 1;
int nextEnemyId = 100;

float radarSweepAngle = 0.0f;
float lockOnProgress = 0.0f;
int targetEnemyId = -1;

bool keys[256] = { false };
bool specialKeys[256] = { false };

// ============================================================================
// [CGL CO-3] AREA-FILL ALGORITHM DEMONSTRATOR STATE
// Course Outcome 3: Develop 2D objects using various algorithms.
// - Interactive demonstrator for Iterative 4-connected Flood-Fill & Boundary-Fill.
// - Renders into off-screen PixelBuffer (120x120) with 2x display magnification.
// ============================================================================
bool isDemoModeActive = false;
int demoAlgorithmType = 0; // 0: Boundary-Fill, 1: Flood-Fill
int demoShapeType = 0;     // 0: Bresenham Circle Shield, 1: 10-Vertex Asteroid Rock
int demoColorIndex = 0;    // 0: Cyan, 1: Yellow, 2: Green, 3: Red
PixelBuffer demoBuffer(120, 120, PixelColor::Black);
int demoFilledPixels = 0;

void updateAreaFillDemo() {
    demoBuffer.clear(PixelColor::Black);
    int cx = 60, cy = 60;

    PixelColor fillColors[] = {
        PixelColor::Cyan,
        PixelColor::Yellow,
        PixelColor::Green,
        PixelColor::Red
    };
    PixelColor fillColor = fillColors[demoColorIndex % 4];

    if (demoShapeType == 0) {
        // Enclosed Shape 1: Bresenham Circle Shield (radius = 44)
        PixelColor boundaryColor = PixelColor::ShieldBlue;
        demoBuffer.drawCircle(cx, cy, 44, boundaryColor);

        if (demoAlgorithmType == 0) {
            demoFilledPixels = AreaFill::boundaryFill4(demoBuffer, cx, cy, fillColor, boundaryColor);
        } else {
            demoFilledPixels = AreaFill::floodFill4(demoBuffer, cx, cy, fillColor);
        }
    } else {
        // Enclosed Shape 2: 10-Vertex Irregular/Concave Asteroid Silhouette
        const int NUM_SECTORS = 10;
        const float angles[NUM_SECTORS] = {
            0.0f, 0.628f, 1.256f, 1.884f, 2.512f,
            3.141f, 3.769f, 4.397f, 5.025f, 5.653f
        };
        const float radiusRatios[NUM_SECTORS] = {
            1.0f, 0.82f, 1.08f, 0.90f, 1.15f,
            0.85f, 1.05f, 0.78f, 1.12f, 0.92f
        };
        float radius = 42.0f;
        std::vector<Point2D> rockVerts;
        for (int i = 0; i < NUM_SECTORS; ++i) {
            float r = radius * radiusRatios[i];
            float px = static_cast<float>(cx) + r * std::cos(angles[i]);
            float py = static_cast<float>(cy) + r * std::sin(angles[i]);
            rockVerts.push_back({ px, py });
        }
        PixelColor boundaryColor = PixelColor::AsteroidBrown;
        demoBuffer.drawPolygon(rockVerts, boundaryColor);

        if (demoAlgorithmType == 0) {
            demoFilledPixels = AreaFill::boundaryFill4(demoBuffer, cx, cy, fillColor, boundaryColor);
        } else {
            demoFilledPixels = AreaFill::floodFill4(demoBuffer, cx, cy, fillColor);
        }
    }
}

// ============================================================================
// [CGL CO-3] 3D OBJECT CONSTRUCTION & HANGAR STATE
// Course Outcome 3: Develop 3D objects using polygons and algorithms.
// - Dedicated 3D models: Custom Spaceship & Geodesic Asteroid meshes.
// - 3D Viewing Pipeline: Perspective projection (gluPerspective) + GL_DEPTH_TEST.
// ============================================================================
bool isHangar3DActive = false;
int hangarModelType = 0; // 0 = Spaceship, 1 = Asteroid
bool hangarWireframe = false;
bool hangarAutoSpin = true;
float hangarRotX = 15.0f;
float hangarRotY = 0.0f;
float hangarRotZ = 0.0f;
float hangarScale = 1.0f;
Mesh3D spaceship3D;
Mesh3D asteroid3D;

void spawnEnemyWave(int waveNumber) {
    enemyList.clear();

    if (psoopOnline) {
        psoopEnemies.clear();
        if (psoopBridge.spawnWave(waveNumber, psoopEnemies) && !psoopEnemies.empty()) {
            std::cout << "\n>>> [PSOOP JAVA] INCOMING WAVE #" << waveNumber
                      << " (" << psoopEnemies.size() << " Polymorphic Enemies: Drone/Fighter/Boss) <<<\n";
            for (const auto& re : psoopEnemies) {
                float vx = (re.type == "BOSS") ? -0.8f : ((re.type == "FIGHTER") ? -2.2f : -3.0f);
                float vy = 0.0f;
                enemyList.addEnemy(re.id, re.x, re.y, vx, vy, re.health, re.maxHealth, re.scoreValue, re.collisionDamage, 0.0f);
            }
            asteroidGrid.generateProceduralWave(waveNumber, WINDOW_WIDTH, WINDOW_HEIGHT, 850.0f);
            return;
        }
    }

    int count = 3 + waveNumber;
    std::cout << "\n>>> [GAMEPLAY] INCOMING WAVE #" << waveNumber
              << " (" << count << " Alien Drones & Asteroid Field Approaching!) <<<\n";

    for (int i = 0; i < count; ++i) {
        int id = nextEnemyId++;
        float x = 820.0f + (i * 70.0f);
        float y = 80.0f + static_cast<float>(rand() % (WINDOW_HEIGHT - 160));
        float vx = -(1.8f + static_cast<float>(rand() % 15) / 10.0f);
        float vy = (static_cast<float>(rand() % 20) - 10.0f) / 10.0f;
        enemyList.addEnemy(id, x, y, vx, vy, 40, 40, 100, 25, 0.0f);
    }

    // Generate 2D Matrix Asteroid Field (PL CO-2)
    asteroidGrid.generateProceduralWave(waveNumber, WINDOW_WIDTH, WINDOW_HEIGHT, 850.0f);
}

void restartGame() {
    std::cout << "\n>>> [SYSTEM] REBOOTING FLIGHT COMPUTER & RESTORING MISSION <<<\n";
    playerX = 100.0f;
    playerY = 300.0f;
    playerAngle = 0.0f;
    playerScale = 1.0f;
    playerHealth = PLAYER_MAX_HEALTH;
    playerShield = 50;
    playerScore = 0;
    playerAlive = true;
    isGameOver = false;
    currentWave = 1;
    fireCooldown = 0;

    if (psoopOnline) {
        psoopBridge.resetGame();
    }

    bulletPool.reset();
    spawnEnemyWave(currentWave);
}

void handleInput() {
    if (isGameOver || isHangar3DActive || isDemoModeActive) return;

    float dx = 0.0f;
    float dy = 0.0f;

    if (keys['w'] || keys['W'] || specialKeys[GLUT_KEY_UP])    dy += 1.0f;
    if (keys['s'] || keys['S'] || specialKeys[GLUT_KEY_DOWN])  dy -= 1.0f;
    if (keys['a'] || keys['A'] || specialKeys[GLUT_KEY_LEFT])  dx -= 1.0f;
    if (keys['d'] || keys['D'] || specialKeys[GLUT_KEY_RIGHT]) dx += 1.0f;

    if (dy > 0.0f) {
        playerAngle = 0.15f;
    } else if (dy < 0.0f) {
        playerAngle = -0.15f;
    } else {
        playerAngle = 0.0f;
    }

    if (dx != 0.0f && dy != 0.0f) {
        dx *= 0.7071f;
        dy *= 0.7071f;
    }

    playerX += dx * playerSpeed;
    playerY += dy * playerSpeed;

    if (playerX < 35.0f) playerX = 35.0f;
    if (playerX > WINDOW_WIDTH - 60.0f) playerX = WINDOW_WIDTH - 60.0f;
    if (playerY < 35.0f) playerY = 35.0f;
    if (playerY > WINDOW_HEIGHT - 75.0f) playerY = WINDOW_HEIGHT - 75.0f;

    if (fireCooldown > 0) {
        fireCooldown--;
    }

    if (keys[' '] && fireCooldown == 0 && playerAlive) {
        int bId = bulletPool.fireBullet(playerX + 28.0f, playerY, 13.0f, 0.0f, 25);
        if (bId != -1) {
            fireCooldown = FIRE_COOLDOWN_TICKS;
        }
    }
}

void updateGame() {
    if (isGameOver) return;

    float delta = 1.0f / TARGET_FPS;

    renderer.updateStarfield(WINDOW_WIDTH, WINDOW_HEIGHT, delta);
    bulletPool.updateBullets(0.0f, WINDOW_WIDTH, 0.0f, WINDOW_HEIGHT);
    asteroidGrid.updateField(delta, -100.0f, WINDOW_WIDTH + 400.0f);

    radarSweepAngle += 0.06f;
    if (radarSweepAngle > 6.2831853f) radarSweepAngle -= 6.2831853f;

    // Nearest enemy targeting logic for Bresenham reticle tracking
    float nearestDistSq = 999999.0f;
    EnemyNode* targetEnemy = nullptr;
    EnemyNode* trackNode = enemyList.getHead();
    while (trackNode != nullptr) {
        if (trackNode->data.x > playerX - 30.0f) {
            float dx = trackNode->data.x - playerX;
            float dy = trackNode->data.y - playerY;
            float dSq = dx * dx + dy * dy;
            if (dSq < nearestDistSq) {
                nearestDistSq = dSq;
                targetEnemy = trackNode;
            }
        }
        trackNode = trackNode->next;
    }

    if (targetEnemy != nullptr) {
        targetEnemyId = targetEnemy->data.id;
        if (lockOnProgress < 1.0f) lockOnProgress += 0.05f;
    } else {
        targetEnemyId = -1;
        lockOnProgress = 0.0f;
    }

    EnemyNode* eCurr = enemyList.getHead();
    while (eCurr != nullptr) {
        eCurr->data.x += eCurr->data.vx;
        eCurr->data.y += eCurr->data.vy;
        eCurr->data.angle += 0.03f;

        if (eCurr->data.y < 60.0f || eCurr->data.y > WINDOW_HEIGHT - 80.0f) {
            eCurr->data.vy = -eCurr->data.vy;
        }

        if (eCurr->data.x < -40.0f) {
            eCurr->data.x = WINDOW_WIDTH + 20.0f;
            eCurr->data.y = 80.0f + static_cast<float>(rand() % (WINDOW_HEIGHT - 160));
        }

        eCurr = eCurr->next;
    }

    if (psoopOnline) {
        psoopBridge.tickSimulation(0.0f, 0.0f, psoopState, psoopEnemies);
        for (const auto& re : psoopEnemies) {
            EnemyNode* cur = enemyList.getHead();
            while (cur != nullptr) {
                if (cur->data.id == re.id) {
                    cur->data.x = re.x;
                    cur->data.y = re.y;
                    break;
                }
                cur = cur->next;
            }
        }
    }

    if (enemyList.isEmpty()) {
        currentWave++;
        spawnEnemyWave(currentWave);
    }

    Bullet* bullets = bulletPool.getBullets();
    int capacity = bulletPool.getCapacity();

    // 1. Bullet vs Enemy Collisions
    for (int bIdx = 0; bIdx < capacity; ++bIdx) {
        if (!bullets[bIdx].active) continue;

        EnemyNode* eNode = enemyList.getHead();
        while (eNode != nullptr) {
            float distX = std::abs(bullets[bIdx].x - eNode->data.x);
            float distY = std::abs(bullets[bIdx].y - eNode->data.y);

            if (distX < 24.0f && distY < 20.0f) {
                bullets[bIdx].active = false;
                eNode->data.health -= bullets[bIdx].damage;

                if (eNode->data.health <= 0) {
                    int destroyedId = eNode->data.id;
                    int bounty = eNode->data.scoreValue;
                    playerScore = alu.computeScoreAdd(playerScore, bounty, true);
                    if (psoopOnline) {
                        psoopBridge.sendAluFeedback(0, bounty, alu.getEAX(), alu.getEDX(), alu.getZF(), alu.getSF(), alu.getCF());
                    }
                    enemyList.removeEnemy(destroyedId);
                }
                break;
            }
            eNode = eNode->next;
        }

        // 2. Bullet vs 2D Matrix Asteroid Field Collisions
        if (bullets[bIdx].active) {
            int hitR = -1, hitC = -1;
            if (asteroidGrid.checkPointCollision(bullets[bIdx].x, bullets[bIdx].y, hitR, hitC)) {
                bullets[bIdx].active = false;
                bool destroyed = false;
                const Asteroid& astRef = asteroidGrid.getAsteroid(hitR, hitC);
                int bounty = astRef.scoreValue;
                asteroidGrid.damageAsteroid(hitR, hitC, bullets[bIdx].damage, destroyed);
                if (destroyed) {
                    playerScore = alu.computeScoreAdd(playerScore, bounty, true);
                }
            }
        }
    }

    if (playerAlive) {
        // Player vs Enemy Collisions
        EnemyNode* eNode = enemyList.getHead();
        while (eNode != nullptr) {
            EnemyNode* nextNode = eNode->next;
            float distX = std::abs(playerX - eNode->data.x);
            float distY = std::abs(playerY - eNode->data.y);

            if (distX < 32.0f && distY < 24.0f) {
                int damage = eNode->data.collisionDamage;
                int hitEnemyId = eNode->data.id;
                enemyList.removeEnemy(hitEnemyId);

                if (playerShield > 0) {
                    if (playerShield >= damage) {
                        playerShield -= damage;
                        damage = 0;
                    } else {
                        damage -= playerShield;
                        playerShield = 0;
                    }
                }

                if (damage > 0) {
                    bool fatal = false;
                    playerHealth = static_cast<int>(alu.computeHealthDamage(
                        static_cast<uint32_t>(playerHealth),
                        static_cast<uint32_t>(damage),
                        fatal,
                        true
                    ));

                    if (psoopOnline) {
                        psoopBridge.sendAluFeedback(damage, 0, alu.getEAX(), alu.getEDX(), alu.getZF(), alu.getSF(), alu.getCF());
                    }

                    if (fatal || playerHealth <= 0) {
                        playerHealth = 0;
                        playerAlive = false;
                        isGameOver = true;
                        std::cout << "\n>>> [ALERT] CRITICAL SYSTEM BREACH: SHIP DESTROYED! <<<\n";
                    }
                }
            }
            eNode = nextNode;
        }

        // Player vs 2D Matrix Asteroid Collisions
        int hitR = -1, hitC = -1;
        if (asteroidGrid.checkCircleCollision(playerX, playerY, 20.0f, hitR, hitC)) {
            const Asteroid& hitAst = asteroidGrid.getAsteroid(hitR, hitC);
            int damage = hitAst.collisionDamage;
            asteroidGrid.deactivate(hitR, hitC);

            if (playerShield > 0) {
                if (playerShield >= damage) {
                    playerShield -= damage;
                    damage = 0;
                } else {
                    damage -= playerShield;
                    playerShield = 0;
                }
            }

            if (damage > 0) {
                bool fatal = false;
                playerHealth = static_cast<int>(alu.computeHealthDamage(
                    static_cast<uint32_t>(playerHealth),
                    static_cast<uint32_t>(damage),
                    fatal,
                    true
                ));

                if (fatal || playerHealth <= 0) {
                    playerHealth = 0;
                    playerAlive = false;
                    isGameOver = true;
                    std::cout << "\n>>> [ALERT] CRITICAL SYSTEM BREACH: SHIP DESTROYED BY ASTEROID IMPACT! <<<\n";
                }
            }
        }
    }
}

void display() {
    // CGL CO-3: 3D Hangar & Custom 3D Object Preview Scene (if active)
    if (isHangar3DActive) {
        renderer.render3DHangarScene(hangarModelType, hangarWireframe,
                                     hangarRotX, hangarRotY, hangarRotZ,
                                     hangarScale, spaceship3D, asteroid3D,
                                     WINDOW_WIDTH, WINDOW_HEIGHT);
        glutSwapBuffers();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    renderer.renderBackground(WINDOW_WIDTH, WINDOW_HEIGHT);

    // 1. Render 2D Matrix Asteroid Field (PL CO-2)
    for (int r = 0; r < asteroidGrid.getRows(); ++r) {
        for (int c = 0; c < asteroidGrid.getCols(); ++c) {
            const Asteroid& ast = asteroidGrid.getAsteroid(r, c);
            if (ast.active) {
                renderer.renderAsteroid(ast.x, ast.y, ast.radius, ast.health, ast.maxHealth);
            }
        }
    }

    // 2. Render Lasers
    const Bullet* bullets = bulletPool.getBullets();
    int capacity = bulletPool.getCapacity();
    for (int i = 0; i < capacity; ++i) {
        if (bullets[i].active) {
            renderer.renderLaserBolt(bullets[i].x, bullets[i].y, 16.0f, 3.5f, true);
        }
    }

    // 3. Render Enemies (Polymorphic Drone, Fighter, Boss from PSOOP)
    EnemyNode* eNode = enemyList.getHead();
    while (eNode != nullptr) {
        std::string type = "DRONE";
        if (eNode->data.maxHealth >= 200) type = "BOSS";
        else if (eNode->data.maxHealth >= 60) type = "FIGHTER";
        renderer.renderEnemyShip(eNode->data.x, eNode->data.y, 22.0f, eNode->data.angle, 1.0f, type);
        eNode = eNode->next;
    }

    // 4. Render Bresenham Targeting Reticle & Lock-on line on targeted enemy (CGL CO-2)
    if (playerAlive && targetEnemyId != -1) {
        EnemyNode* cur = enemyList.getHead();
        while (cur != nullptr) {
            if (cur->data.id == targetEnemyId) {
                renderer.renderTargetingReticle(cur->data.x, cur->data.y, 24.0f, lockOnProgress);
                renderer.renderLockLine(playerX + 25.0f, playerY, cur->data.x, cur->data.y,
                                        1.0f, 0.2f, 0.2f, 0.35f * lockOnProgress);
                break;
            }
            cur = cur->next;
        }
    }

    // 5. Render Player Ship & Bresenham Shield Ring (CGL CO-2)
    renderer.renderPlayerShip(playerX, playerY, 26.0f, playerAlive, playerAngle, playerScale);
    if (playerAlive) {
        renderer.renderShieldRing(playerX, playerY, 32.0f, playerShield, PLAYER_MAX_SHIELD);
    }

    // 6. Render HUD Telemetry
    renderer.renderHUD(playerHealth, PLAYER_MAX_HEALTH, playerShield, playerScore,
                      currentWave, bulletPool.getActiveCount(), bulletPool.getCapacity(),
                      WINDOW_WIDTH, WINDOW_HEIGHT);

    // 6b. Render 4-Subject Integration Banner
    if (psoopOnline) {
        Renderer::drawBitmapString(15.0f, WINDOW_HEIGHT - 40.0f, GLUT_BITMAP_HELVETICA_10,
                                   "[4-SUBJECT REAL-TIME INTEGRATION ACTIVE: PL + COA + CGL + PSOOP ONLINE]",
                                   0.2f, 0.95f, 0.4f);
        if (!psoopState.hudText.empty()) {
            Renderer::drawBitmapString(15.0f, WINDOW_HEIGHT - 54.0f, GLUT_BITMAP_HELVETICA_10,
                                       "JAVA TELEMETRY: " + psoopState.hudText,
                                       0.3f, 0.85f, 1.0f);
        }
    } else {
        Renderer::drawBitmapString(15.0f, WINDOW_HEIGHT - 40.0f, GLUT_BITMAP_HELVETICA_10,
                                   "[LOCAL MODE: PL + COA + CGL] Start PSOOPBridgeServer to link PSOOP",
                                   0.95f, 0.75f, 0.2f);
    }

    // 6c. Render CGL CO-3 Algorithm Telemetry Banner
    std::string fillStatus = (renderer.isScanLineFillEnabled() ? "[F] SCANLINE: ON" : "[F] SCANLINE: OFF") +
                             std::string("  |  [M] 2D FILL DEMO  |  [H] 3D HANGAR");
    Renderer::drawBitmapString(WINDOW_WIDTH - 420.0f, WINDOW_HEIGHT - 40.0f, GLUT_BITMAP_HELVETICA_10,
                               fillStatus, 0.3f, 0.95f, 0.95f);

    // 7. Render Bresenham Radar Minimap (CGL CO-2)
    std::vector<Point2D> enemyPos;
    EnemyNode* enm = enemyList.getHead();
    while (enm != nullptr) {
        enemyPos.push_back({ enm->data.x, enm->data.y });
        enm = enm->next;
    }

    std::vector<Point2D> astPos;
    for (int r = 0; r < asteroidGrid.getRows(); ++r) {
        for (int c = 0; c < asteroidGrid.getCols(); ++c) {
            const Asteroid& a = asteroidGrid.getAsteroid(r, c);
            if (a.active) {
                astPos.push_back({ a.x, a.y });
            }
        }
    }

    renderer.renderRadarMinimap(WINDOW_WIDTH - 65.0f, 65.0f, 45.0f, radarSweepAngle,
                                playerX, playerY, enemyPos, astPos,
                                WINDOW_WIDTH, WINDOW_HEIGHT);

    if (isGameOver) {
        renderer.renderGameOver(playerScore, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    // 8. Render CG CO-3 Area-Fill Demonstration Overlay (if active)
    if (isDemoModeActive) {
        renderer.renderAreaFillDemoOverlay(demoAlgorithmType, demoShapeType, demoFilledPixels,
                                           demoColorIndex, demoBuffer, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    glutSwapBuffers();
}

void timer(int) {
    handleInput();
    if (isHangar3DActive) {
        if (hangarAutoSpin) {
            hangarRotY += 1.2f;
            if (hangarRotY >= 360.0f) hangarRotY -= 360.0f;
        }
    } else if (!isDemoModeActive) {
        updateGame();
    }
    glutPostRedisplay();
    glutTimerFunc(FRAME_DELAY_MS, timer, 0);
}

void reshape(int w, int h) {
    renderer.setup2DProjection(w, h);
}

void keyDown(unsigned char key, int, int) {
    keys[key] = true;

    if (key == 27) {
        exit(0);
    }
    if ((key == 'r' || key == 'R') && isGameOver) {
        restartGame();
    }
    // [CGL CO-3] Toggle genuine Scan-Line ET/AET polygon filling on procedural asteroid rocks
    if (key == 'f' || key == 'F') {
        renderer.toggleScanLineFill();
        std::cout << "\n>>> [CGL CO-3] Scan-Line Polygon Fill toggled: "
                  << (renderer.isScanLineFillEnabled() ? "ACTIVE (Genuine ET/AET Algorithm)" : "OFF (OpenGL Built-in Fallback)")
                  << " <<<\n";
    }
    // [CGL CO-3] Toggle 3D Hangar Scene (Custom 3D Spaceship & Asteroid inspection)
    if (key == 'h' || key == 'H') {
        isHangar3DActive = !isHangar3DActive;
        if (isHangar3DActive) {
            isDemoModeActive = false; // prioritize 3D Hangar
            std::cout << "\n>>> [CGL CO-3] Opened 3D Object Construction Hangar (Spaceship / Asteroid Mesh) <<<\n";
        } else {
            renderer.setup2DProjection(WINDOW_WIDTH, WINDOW_HEIGHT);
            std::cout << "\n>>> [CGL CO-3] Restored 2D Orthographic Projection & Resumed Game <<<\n";
        }
    }
    // [CGL CO-3] Interactive 3D Hangar Controls (Pitch, Yaw, Roll, Scale, Wireframe)
    if (isHangar3DActive) {
        if (key == '1') {
            hangarModelType = 0; // Spaceship
        } else if (key == '2') {
            hangarModelType = 1; // Asteroid
        } else if (key == 'v' || key == 'V') {
            hangarWireframe = !hangarWireframe;
        } else if (key == 'p' || key == 'P') {
            hangarAutoSpin = !hangarAutoSpin;
        } else if (key == 'w' || key == 'W') {
            hangarRotX += 5.0f;
        } else if (key == 's' || key == 'S') {
            hangarRotX -= 5.0f;
        } else if (key == 'a' || key == 'A') {
            hangarRotY -= 5.0f;
        } else if (key == 'd' || key == 'D') {
            hangarRotY += 5.0f;
        } else if (key == 'q' || key == 'Q') {
            hangarRotZ -= 5.0f;
        } else if (key == 'e' || key == 'E') {
            hangarRotZ += 5.0f;
        } else if (key == '+' || key == '=') {
            hangarScale = std::min(hangarScale * 1.1f, 3.0f);
        } else if (key == '-' || key == '_') {
            hangarScale = std::max(hangarScale * 0.9f, 0.3f);
        } else if (key == 'r' || key == 'R') {
            hangarRotX = 15.0f;
            hangarRotY = 0.0f;
            hangarRotZ = 0.0f;
            hangarScale = 1.0f;
            hangarAutoSpin = true;
        }
        return;
    }
    // [CGL CO-3] Toggle 2D Area-Fill Demonstrator Overlay (Flood-Fill & Boundary-Fill)
    if (key == 'm' || key == 'M' || key == '\t') {
        isDemoModeActive = !isDemoModeActive;
        if (isDemoModeActive) {
            updateAreaFillDemo();
            std::cout << "\n>>> [CGL CO-3] Opened CG Algorithm Demonstrator (Flood-Fill & Boundary-Fill) <<<\n";
        } else {
            std::cout << "\n>>> [CGL CO-3] Resumed AirStriker 60 FPS Space Combat <<<\n";
        }
    }
    if (isDemoModeActive) {
        if (key == '1') {
            demoAlgorithmType = 0; // Boundary-Fill
            updateAreaFillDemo();
        } else if (key == '2') {
            demoAlgorithmType = 1; // Flood-Fill
            updateAreaFillDemo();
        } else if (key == '3') {
            demoShapeType = (demoShapeType + 1) % 2; // Toggle Circle / Asteroid
            updateAreaFillDemo();
        } else if (key == 'c' || key == 'C') {
            demoColorIndex = (demoColorIndex + 1) % 4; // Cycle palette
            updateAreaFillDemo();
        } else if (key == ' ') {
            updateAreaFillDemo(); // Re-fill
        }
    }
}

void keyUp(unsigned char key, int, int) {
    keys[key] = false;
}

void specialKeyDown(int key, int, int) {
    if (key >= 0 && key < 256) {
        specialKeys[key] = true;
    }
}

void specialKeyUp(int key, int, int) {
    if (key >= 0 && key < 256) {
        specialKeys[key] = false;
    }
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(NULL)));

    std::cout << "=======================================================\n";
    std::cout << "  AIR STRIKER — 2D SPACE SHOOTER (CO-2 EDITION)        \n";
    std::cout << "=======================================================\n";
    std::cout << "  [PL/DS] Fixed Array Bullet Pool: Active (Cap: " << bulletPool.getCapacity() << ")\n";
    std::cout << "  [PL/DS] Singly Linked List Enemy Manager: Active\n";
    std::cout << "  [PL/DS] 2D Array Asteroid Grid Matrix: Active (" << asteroidGrid.getRows() << "x" << asteroidGrid.getCols() << " Matrix)\n";
    std::cout << "  [COA]   80386 ALU Simulator: Initialized\n";
    std::cout << "  [CGL]   OpenGL Geometric Primitive Renderer: Initialized\n";
    std::cout << "  [CGL]   Mid-point Bresenham Line & Circle Algorithms: Active\n";
    std::cout << "  [CGL]   Scan-Line ET/AET, Flood-Fill & Boundary-Fill: Active (CO-3)\n";
    std::cout << "  [CGL]   3D Mesh Pipeline & Hangar (Spaceship / Asteroid): Active (CO-3)\n";
    std::cout << "  [PSOOP] Game State & Domain Model: Initialized\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << "  CONTROLS:\n";
    std::cout << "    W / A / S / D or ARROWS = Move Spaceship\n";
    std::cout << "    SPACE                   = Fire Laser Cannon\n";
    std::cout << "    F                       = Toggle Scan-Line Fill (Procedural Rocks)\n";
    std::cout << "    M / TAB                 = 2D Fill Demonstrator (Flood/Boundary)\n";
    std::cout << "    H                       = 3D Hangar (Custom Spaceship & Asteroid)\n";
    std::cout << "    R                       = Restart after Game Over\n";
    std::cout << "    ESC                     = Exit Game\n";
    std::cout << "=======================================================\n\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("AirStriker — 2D Space Shooter (CO-2/CO-3 Edition)");

    // Initialize CGL CO-3 Custom 3D Object Meshes
    spaceship3D = Mesh3D::createSpaceship();
    asteroid3D = Mesh3D::createAsteroid();
    std::cout << "  [CGL CO-3] Custom 3D Meshes Initialized:\n";
    std::cout << "             - Spaceship 3D (" << spaceship3D.getVertices().size() << " vertices, " << spaceship3D.getFaces().size() << " faces)\n";
    std::cout << "             - Asteroid 3D (" << asteroid3D.getVertices().size() << " vertices, " << asteroid3D.getFaces().size() << " faces)\n";

    renderer.initStarfield(90, WINDOW_WIDTH, WINDOW_HEIGHT);

    std::cout << "  Connecting to PSOOP Java Subsystem (127.0.0.1:9092)...\n";
    psoopOnline = psoopBridge.connectTo("127.0.0.1", 9092);
    if (psoopOnline) {
        std::cout << "  [PSOOP] LIVE CONNECTED! (4-Subject Real-time Integration Active)\n";
        psoopBridge.initGame("CadetPilot");
    } else {
        std::cout << "  [PSOOP] Server offline at 127.0.0.1:9092. Running in standalone fallback mode.\n";
    }

    restartGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutTimerFunc(FRAME_DELAY_MS, timer, 0);

    glutMainLoop();
    return 0;
}
