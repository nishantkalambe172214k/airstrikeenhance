#!/bin/bash
echo "===================================================================="
echo "   RUNNING PROGRAMMING LAB (PL) DATA STRUCTURE TESTS                "
echo "   CO-1 (Array Pool) + CO-2 (Linked List & 2D Matrix) + CO-3 (Stack/Queue)"
echo "===================================================================="

mkdir -p bin

echo ""
echo "--- [1/4] Compiling and Running Array Bullet Pool (CO-1) ---"
g++ -std=c++11 -I./PL/src PL/src/BulletPoolArray.cpp PL/tests/test_bullet_pool.cpp -o bin/test_bullet_pool
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_bullet_pool."
    exit 1
fi
./bin/test_bullet_pool
if [ $? -ne 0 ]; then
    echo "[ERROR] test_bullet_pool failed."
    exit 1
fi

echo ""
echo "--- [2/4] Compiling and Running Singly Linked List Enemy Manager (CO-2) ---"
g++ -std=c++11 -I./PL/src PL/src/EnemyLinkedList.cpp PL/tests/test_enemy_list.cpp -o bin/test_enemy_list
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_enemy_list."
    exit 1
fi
./bin/test_enemy_list
if [ $? -ne 0 ]; then
    echo "[ERROR] test_enemy_list failed."
    exit 1
fi

echo ""
echo "--- [3/4] Compiling and Running 2D Array Asteroid Grid Matrix (CO-2) ---"
g++ -std=c++11 -I./PL/src PL/src/AsteroidGridMatrix.cpp PL/tests/test_asteroid_grid.cpp -o bin/test_asteroid_grid
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_asteroid_grid."
    exit 1
fi
./bin/test_asteroid_grid
if [ $? -ne 0 ]; then
    echo "[ERROR] test_asteroid_grid failed."
    exit 1
fi

echo ""
echo "--- [4/4] Compiling and Running Stack & Queue (Combat History & Wave Spawning) (CO-3) ---"
g++ -std=c++11 -I./PL/src PL/src/CombatEventStack.cpp PL/src/EnemySpawnQueue.cpp PL/tests/test_stack_queue.cpp -o bin/test_stack_queue
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_stack_queue."
    exit 1
fi
./bin/test_stack_queue
if [ $? -ne 0 ]; then
    echo "[ERROR] test_stack_queue failed."
    exit 1
fi

echo ""
echo "===================================================================="
echo "   *** ALL 4 PL DATA STRUCTURE TEST SUITES PASSED CLEANLY! ***     "
echo "===================================================================="
