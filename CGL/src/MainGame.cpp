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
#include "../../COA/ALU/ALU80386.h"
#include "Renderer.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int TARGET_FPS = 60;
const int FRAME_DELAY_MS = 1000 / TARGET_FPS;

Renderer renderer;
BulletPoolArray bulletPool;
EnemyLinkedList enemyList;
ALU80386 alu;

float playerX = 100.0f;
float playerY = 300.0f;
float playerSpeed = 5.5f;
float playerAngle = 0.0f;
float playerScale = 1.0f;
int playerHealth = 100;
const int PLAYER_MAX_HEALTH = 100;
int playerShield = 50;
uint32_t playerScore = 0;
bool playerAlive = true;
bool isGameOver = false;

int fireCooldown = 0;
const int FIRE_COOLDOWN_TICKS = 9;

int currentWave = 1;
int nextEnemyId = 100;

bool keys[256] = { false };
bool specialKeys[256] = { false };

void spawnEnemyWave(int waveNumber) {
    enemyList.clear();
    int count = 3 + waveNumber;
    std::cout << "\n>>> [GAMEPLAY] INCOMING WAVE #" << waveNumber
              << " (" << count << " Alien Drones Detected!) <<<\n";

    for (int i = 0; i < count; ++i) {
        int id = nextEnemyId++;
        float x = 820.0f + (i * 70.0f);
        float y = 80.0f + static_cast<float>(rand() % (WINDOW_HEIGHT - 160));
        float vx = -(1.8f + static_cast<float>(rand() % 15) / 10.0f);
        float vy = (static_cast<float>(rand() % 20) - 10.0f) / 10.0f;
        enemyList.addEnemy(id, x, y, vx, vy, 40, 40, 100, 25, 0.0f);
    }
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

    bulletPool.reset();
    spawnEnemyWave(currentWave);
}

void handleInput() {
    if (isGameOver) return;

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

    if (enemyList.isEmpty()) {
        currentWave++;
        spawnEnemyWave(currentWave);
    }

    Bullet* bullets = bulletPool.getBullets();
    int capacity = bulletPool.getCapacity();

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
                    enemyList.removeEnemy(destroyedId);
                }
                break;
            }
            eNode = eNode->next;
        }
    }

    if (playerAlive) {
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
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    renderer.renderBackground(WINDOW_WIDTH, WINDOW_HEIGHT);

    const Bullet* bullets = bulletPool.getBullets();
    int capacity = bulletPool.getCapacity();
    for (int i = 0; i < capacity; ++i) {
        if (bullets[i].active) {
            renderer.renderLaserBolt(bullets[i].x, bullets[i].y, 16.0f, 3.5f, true);
        }
    }

    EnemyNode* eNode = enemyList.getHead();
    while (eNode != nullptr) {
        renderer.renderEnemyShip(eNode->data.x, eNode->data.y, 22.0f, eNode->data.angle, 1.0f);
        eNode = eNode->next;
    }

    renderer.renderPlayerShip(playerX, playerY, 26.0f, playerAlive, playerAngle, playerScale);

    renderer.renderHUD(playerHealth, PLAYER_MAX_HEALTH, playerShield, playerScore,
                      currentWave, bulletPool.getActiveCount(), bulletPool.getCapacity(),
                      WINDOW_WIDTH, WINDOW_HEIGHT);

    if (isGameOver) {
        renderer.renderGameOver(playerScore, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    glutSwapBuffers();
}

void timer(int) {
    handleInput();
    updateGame();
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
    std::cout << "  AIR STRIKER — 2D SPACE SHOOTER                       \n";
    std::cout << "=======================================================\n";
    std::cout << "  [PL/DS] Fixed Array Bullet Pool: Active (Cap: " << bulletPool.getCapacity() << ")\n";
    std::cout << "  [PL/DS] Singly Linked List Enemy Manager: Active\n";
    std::cout << "  [COA]   80386 ALU Simulator: Initialized\n";
    std::cout << "  [CGL]   OpenGL Geometric Primitive Renderer: Initialized\n";
    std::cout << "  [PSOOP] Game State & Domain Model: Initialized\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << "  CONTROLS:\n";
    std::cout << "    W / A / S / D or ARROWS = Move Spaceship\n";
    std::cout << "    SPACE                   = Fire Laser Cannon\n";
    std::cout << "    R                       = Restart after Game Over\n";
    std::cout << "    ESC                     = Exit Game\n";
    std::cout << "=======================================================\n\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(150, 100);
    glutCreateWindow("AirStriker — 2D Space Shooter (CO-1 Arcade Prototype)");

    renderer.initStarfield(90, WINDOW_WIDTH, WINDOW_HEIGHT);
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
