#!/bin/bash
echo "===================================================================="
echo "   AIR STRIKER: CGL (COMPUTER GRAPHICS LAB) TEST RUNNER             "
echo "   Testing CO-2 (Bresenham & Transformations) + CO-3 (Scanline Fill) "
echo "===================================================================="

mkdir -p bin

# 1. Compile and Run Bresenham Line & Circle Tests (CO-2)
echo ""
echo "[1/3] Compiling and running CGL Bresenham Tests..."
g++ -std=c++11 -I./CGL/src CGL/src/Bresenham.cpp CGL/tests/test_bresenham.cpp -o bin/test_bresenham -framework OpenGL
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_bresenham."
    exit 1
fi
./bin/test_bresenham
if [ $? -ne 0 ]; then
    echo "[ERROR] test_bresenham failed."
    exit 1
fi

# 2. Compile and Run Transformations Tests (CO-2)
echo ""
echo "[2/5] Compiling and running CGL Transformation Pipeline Tests..."
g++ -std=c++11 -I./CGL/src -I./PL/src CGL/src/Renderer.cpp CGL/src/Bresenham.cpp CGL/src/ScanLineFiller.cpp CGL/src/AreaFill.cpp CGL/src/Mesh3D.cpp PL/src/CombatEventStack.cpp PL/src/EnemySpawnQueue.cpp CGL/tests/test_transformations.cpp -o bin/test_transformations -framework OpenGL -framework GLUT
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_transformations."
    exit 1
fi
./bin/test_transformations
if [ $? -ne 0 ]; then
    echo "[ERROR] test_transformations failed."
    exit 1
fi

# 3. Compile and Run Scan-Line Polygon Fill Tests (CO-3)
echo ""
echo "[3/5] Compiling and running CGL Scan-Line Polygon Fill Tests..."
g++ -std=c++11 -I./CGL/src CGL/src/Bresenham.cpp CGL/src/ScanLineFiller.cpp CGL/tests/test_scanline_fill.cpp -o bin/test_scanline_fill -framework OpenGL
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_scanline_fill."
    exit 1
fi
./bin/test_scanline_fill
if [ $? -ne 0 ]; then
    echo "[ERROR] test_scanline_fill failed."
    exit 1
fi

# 4. Compile and Run Area-Fill Tests (Flood-Fill & Boundary-Fill) (CO-3)
echo ""
echo "[4/5] Compiling and running CGL Area Fill Tests (Flood-Fill & Boundary-Fill)..."
g++ -std=c++11 -I./CGL/src CGL/src/Bresenham.cpp CGL/src/AreaFill.cpp CGL/tests/test_area_fill.cpp -o bin/test_area_fill -framework OpenGL
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_area_fill."
    exit 1
fi
./bin/test_area_fill
if [ $? -ne 0 ]; then
    echo "[ERROR] test_area_fill failed."
    exit 1
fi

# 5. Compile and Run 3D Mesh Pipeline Tests (Spaceship, Asteroid, Transformations, Depth) (CO-3)
echo ""
echo "[5/5] Compiling and running CGL 3D Mesh Pipeline Tests..."
g++ -std=c++11 -I./CGL/src CGL/src/Mesh3D.cpp CGL/tests/test_3d_pipeline.cpp -o bin/test_3d_pipeline -framework OpenGL
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to compile test_3d_pipeline."
    exit 1
fi
./bin/test_3d_pipeline
if [ $? -ne 0 ]; then
    echo "[ERROR] test_3d_pipeline failed."
    exit 1
fi

echo ""
echo "===================================================================="
echo "   *** ALL 5 CGL TEST SUITES PASSED CLEANLY (CO-2 & CO-3) ***       "
echo "===================================================================="
