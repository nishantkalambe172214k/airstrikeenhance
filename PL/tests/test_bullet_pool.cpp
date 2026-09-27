#include <iostream>
#include <cassert>
#include "../src/BulletPoolArray.h"

void testFireTenSimultaneousBullets() {
    std::cout << "[TEST 1] Firing 10+ Simultaneous Bullets...\n";
    BulletPoolArray pool;

    int ids[15];
    for (int i = 0; i < 15; ++i) {
        ids[i] = pool.fireBullet(100.0f, 200.0f + (i * 10), 12.0f, 0.0f, 25);
        assert(ids[i] > 0);
    }

    assert(pool.getActiveCount() == 15);
    std::cout << " -> Successfully fired 15 simultaneous bullets into fixed array.\n";
    pool.displayActiveBullets();
}

void testUpdateBulletsMotion() {
    std::cout << "[TEST 2] Updating Bullets Kinematics...\n";
    BulletPoolArray pool;

    int bId = pool.fireBullet(50.0f, 50.0f, 10.0f, 5.0f, 20);
    int idx = pool.searchBullet(bId);
    assert(idx != -1);

    Bullet* b = &pool.getBullets()[idx];
    assert(b->x == 50.0f && b->y == 50.0f);

    pool.updateBullets(0.0f, 800.0f, 0.0f, 600.0f);
    assert(b->x == 60.0f && b->y == 55.0f);

    pool.updateBullets(0.0f, 800.0f, 0.0f, 600.0f);
    assert(b->x == 70.0f && b->y == 60.0f);
    std::cout << " -> Position updated accurately according to velocity vector.\n";
}

void testDeactivationAndSlotReuse() {
    std::cout << "[TEST 3] Slot Deactivation & Dynamic Slot Reuse...\n";
    BulletPoolArray pool;

    pool.fireBullet(10.0f, 10.0f, 5.0f, 0.0f, 10);
    pool.fireBullet(20.0f, 20.0f, 5.0f, 0.0f, 10);
    int b2 = pool.fireBullet(30.0f, 30.0f, 5.0f, 0.0f, 10);
    pool.fireBullet(40.0f, 40.0f, 5.0f, 0.0f, 10);
    pool.fireBullet(50.0f, 50.0f, 5.0f, 0.0f, 10);

    assert(pool.getActiveCount() == 5);
    int slotB2 = pool.searchBullet(b2);
    assert(slotB2 == 2);

    bool deactivated = pool.deactivateBullet(b2);
    assert(deactivated == true);
    assert(pool.getActiveCount() == 4);
    assert(pool.searchBullet(b2) == -1);

    int bNew = pool.fireBullet(99.0f, 99.0f, 8.0f, 0.0f, 50);
    int slotNew = pool.searchBullet(bNew);
    std::cout << " -> Deactivated slot was: " << slotB2 << ", Reused slot is: " << slotNew << "\n";
    assert(slotNew == slotB2);
    std::cout << " -> Inactive slot recycled without memory reallocation.\n";
}

void testBoundaryDeactivation() {
    std::cout << "[TEST 4] Screen Boundary Deactivation...\n";
    BulletPoolArray pool;

    int bId = pool.fireBullet(790.0f, 300.0f, 20.0f, 0.0f, 20);
    assert(pool.getActiveCount() == 1);

    pool.updateBullets(0.0f, 800.0f, 0.0f, 600.0f);

    assert(pool.getActiveCount() == 0);
    assert(pool.searchBullet(bId) == -1);
    std::cout << " -> Off-screen bullet automatically reclaimed by boundary check.\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  AIR STRIKER: PL CO-1 TEST SUITE (BulletPoolArray)  \n";
    std::cout << "=====================================================\n\n";

    testFireTenSimultaneousBullets();
    testUpdateBulletsMotion();
    testDeactivationAndSlotReuse();
    testBoundaryDeactivation();

    std::cout << "\n>>> ALL PL CO-1 BULLET POOL ARRAY TESTS PASSED! <<<\n";
    std::cout << "=====================================================\n";
    return 0;
}
