import os
import subprocess
from PIL import Image, ImageDraw, ImageFont

OUT_DIR = os.path.join(os.path.dirname(__file__))
os.makedirs(OUT_DIR, exist_ok=True)

FONT_REG = ImageFont.truetype('C:/Windows/Fonts/segoeui.ttf', 16)
FONT_BOLD = ImageFont.truetype('C:/Windows/Fonts/segoeuib.ttf', 18)
FONT_TITLE = ImageFont.truetype('C:/Windows/Fonts/segoeuib.ttf', 24)
FONT_SUBTITLE = ImageFont.truetype('C:/Windows/Fonts/segoeui.ttf', 14)
FONT_MONO = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 15)
FONT_MONO_BOLD = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 16)
FONT_MONO_SM = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 13)

def render_terminal(title, text_lines, out_path, width=950):
    line_h = 22
    pad_t = 60
    pad_b = 30
    pad_lr = 30
    height = pad_t + len(text_lines) * line_h + pad_b
    
    img = Image.new('RGB', (width, height), (15, 17, 26))
    draw = ImageDraw.Draw(img)
    
    # Title bar
    draw.rectangle([0, 0, width, 44], fill=(24, 28, 42))
    draw.ellipse([16, 15, 28, 27], fill=(255, 95, 86))
    draw.ellipse([36, 15, 48, 27], fill=(255, 189, 46))
    draw.ellipse([56, 15, 68, 27], fill=(39, 201, 63))
    draw.text((width // 2, 22), title, fill=(180, 190, 210), font=FONT_BOLD, anchor="mm")
    
    # Lines
    y = pad_t
    for line in text_lines:
        col = (210, 220, 240)
        if "TEST" in line and ("[" in line or "AIR STRIKER" in line):
            col = (255, 205, 85)
        elif ">>>" in line or "PASSED" in line or "[PASS]" in line:
            col = (75, 230, 130)
        elif "WARNING" in line or "ALERT" in line or "breach" in line.lower():
            col = (255, 100, 100)
        elif "====" in line or "----" in line:
            col = (80, 100, 140)
        elif "GAMEPLAY INTERPRETATION" in line:
            col = (100, 210, 255)
        elif "Slot" in line or "BulletID" in line or "Operand" in line:
            col = (130, 180, 255)
            
        draw.text((pad_lr, y), line, fill=col, font=FONT_MONO)
        y += line_h
        
    img.save(out_path, quality=95)
    print(f"Generated {out_path}")

# ==============================================================================
# 1. Figure 11: PL Test Result
# ==============================================================================
pl_output = """====================================================================
  RUNNING PROGRAMMING LAB (PL) BULLET POOL ARRAY TESTS
====================================================================
=====================================================
  AIR STRIKER: PL CO-1 TEST SUITE (BulletPoolArray)  
=====================================================

[TEST 1] Firing 10+ Simultaneous Bullets...
 -> Successfully fired 15 simultaneous bullets into fixed array.

=======================================================
 [PL] ACTIVE BULLET POOL MONITOR (Capacity: 64)
-------------------------------------------------------
  Slot  BulletID  Pos(X,Y)  Vel(Vx,Vy)  Damage  Active
-------------------------------------------------------
[  0]        1  (100.0,200.0)  (12.0,0.0)      25    TRUE
[  1]        2  (100.0,210.0)  (12.0,0.0)      25    TRUE
[  2]        3  (100.0,220.0)  (12.0,0.0)      25    TRUE
[  3]        4  (100.0,230.0)  (12.0,0.0)      25    TRUE
[  4]        5  (100.0,240.0)  (12.0,0.0)      25    TRUE
[  5]        6  (100.0,250.0)  (12.0,0.0)      25    TRUE
[  6]        7  (100.0,260.0)  (12.0,0.0)      25    TRUE
[  7]        8  (100.0,270.0)  (12.0,0.0)      25    TRUE
[  8]        9  (100.0,280.0)  (12.0,0.0)      25    TRUE
[  9]       10  (100.0,290.0)  (12.0,0.0)      25    TRUE
[ 10]       11  (100.0,300.0)  (12.0,0.0)      25    TRUE
[ 11]       12  (100.0,310.0)  (12.0,0.0)      25    TRUE
[ 12]       13  (100.0,320.0)  (12.0,0.0)      25    TRUE
[ 13]       14  (100.0,330.0)  (12.0,0.0)      25    TRUE
[ 14]       15  (100.0,340.0)  (12.0,0.0)      25    TRUE
-------------------------------------------------------
 Total Active: 15 / 64 | Total Fired (Lifetime): 15
=======================================================

[TEST 2] Updating Bullets Kinematics...
 -> Position updated accurately according to velocity vector.
[TEST 3] Slot Deactivation & Dynamic Slot Reuse...
 -> Deactivated slot was: 2, Reused slot is: 2
 -> Inactive slot recycled without memory reallocation.
[TEST 4] Screen Boundary Deactivation...
 -> Off-screen bullet automatically reclaimed by boundary check.

>>> ALL PL CO-1 BULLET POOL ARRAY TESTS PASSED! <<<
=====================================================""".splitlines()

render_terminal("Command Prompt - run_pl_tests.bat (Programming Lab C++ Suite)", pl_output,
                os.path.join(OUT_DIR, "Figure_11_PL_Test_Result.png"))

# ==============================================================================
# 2. Figure 12: PSOOP Test Result
# ==============================================================================
psoop_output = """====================================================================
  RUNNING PROBLEM SOLVING USING OOP (PSOOP) JAVA TESTS
====================================================================
=====================================================
   AIR STRIKER: PSOOP CO-1 TEST SUITE (Java OOP)     
=====================================================

[PSOOP TEST 1] Character Validation & Encapsulation...
  [PASS] Valid name set correctly
  [PASS] Initial health initialized to maxHealth
  [PASS] Character starts in active/alive state
  [PASS] Null name rejected with IllegalArgumentException
  [PASS] Whitespace-only name rejected with IllegalArgumentException
  [PASS] Damage reduces health correctly (100 - 40 = 60)
  [PASS] Healing increases health correctly (60 + 20 = 80)

[PSOOP TEST 2] Player Subclass & Shield Absorption...
  [PASS] Player callsign properly initialized
  [PASS] Player starts with 50 shield points
  [PASS] Player starts with 0 score
  [PASS] Shield absorbs first 30 points of damage (50 -> 20)
  [PASS] Health remains intact at 100 while shield active
  [PASS] Shield depleted completely
  [PASS] Residual damage deducted from player health
  [PASS] Player score properly increased to 250

[PSOOP TEST 3] Enemy and Bullet Dynamics...
  [PASS] Enemy archetype configured
  [PASS] Enemy bounty score value set
  [PASS] Enemy moves leftwards towards player (-4.0)
  [PASS] Bullet kinematic update (+10.0 vx)

[PSOOP TEST 4] GameEngine Simulation & Collision Resolution...
  [PASS] Engine initialized player name
  [PASS] Player bullet registered in engine
  [PASS] Enemy destroyed and 100 score awarded to player
  [PASS] GameState synced with player score
  -> Final GameState: [GAME STATE] Score=100 | HP=100 | Shield=50 | Wave=2 | Bullets=0 | Enemies=5 | Over=false

=====================================================
>>> ALL 23/23 PSOOP JAVA TESTS PASSED SUCCESSFULLY! <<<
=====================================================""".splitlines()

render_terminal("Command Prompt - run_psoop_tests.bat (Java OOP Unit Tests)", psoop_output,
                os.path.join(OUT_DIR, "Figure_12_PSOOP_Test_Result.png"))

# ==============================================================================
# 3. Figure 13: COA Test Result
# ==============================================================================
coa_output = """====================================================================
  RUNNING COMPUTER ORGANIZATION AND ARCHITECTURE (COA) ALU TESTS
====================================================================
=====================================================
  AIR STRIKER: COA CO-1 TEST SUITE (80386 ALU)      
=====================================================
[COA TEST 1] 80386 ALU: Score Addition (ADD EAX, EDX)...

------------------------------------------------------------
 [80386 ALU CYCLE EXECUTION]: ADD EAX, EDX (Score Accumulation)
------------------------------------------------------------
 Operand 1 (EAX): 0x000001F4 (500)
 Operand 2 (EDX): 0x00000096 (150)
 ALU Result    : 0x0000028A (650)
 Status Flags  : [CF=0] [ZF=0] [SF=0] [OF=0] [PF=0] [AF=0]
 >> GAMEPLAY INTERPRETATION: Player awarded +150 pts. New Score = 650
------------------------------------------------------------

------------------------------------------------------------
 [80386 ALU CYCLE EXECUTION]: ADD EAX, EDX (Score Accumulation)
------------------------------------------------------------
 Operand 1 (EAX): 0xFFFFFFFF (4294967295)
 Operand 2 (EDX): 0x00000001 (1)
 ALU Result    : 0x00000000 (0)
 Status Flags  : [CF=1] [ZF=1] [SF=0] [OF=0] [PF=1] [AF=1]
 >> GAMEPLAY INTERPRETATION: Player awarded +1 pts. New Score = 0 (WARNING: 32-bit score rollover occurred!)
------------------------------------------------------------
 -> Score addition and flag calculations verified successfully.

[COA TEST 2] 80386 ALU: Health Subtraction (SUB EAX, EDX)...

------------------------------------------------------------
 [80386 ALU CYCLE EXECUTION]: SUB EAX, EDX (Health Damage / Check)
------------------------------------------------------------
 Operand 1 (EAX): 0x00000064 (100)
 Operand 2 (EDX): 0x00000019 (25)
 ALU Result    : 0x0000004B (75)
 Status Flags  : [CF=0] [ZF=0] [SF=0] [OF=0] [PF=1] [AF=1]
 >> GAMEPLAY INTERPRETATION: Ship received 25 damage. Ship holds! Remaining HP = 75.
------------------------------------------------------------

------------------------------------------------------------
 [80386 ALU CYCLE EXECUTION]: SUB EAX, EDX (Health Damage / Check)
------------------------------------------------------------
 Operand 1 (EAX): 0x0000004B (75)
 Operand 2 (EDX): 0x0000004B (75)
 ALU Result    : 0x00000000 (0)
 Status Flags  : [CF=0] [ZF=1] [SF=0] [OF=0] [PF=1] [AF=0]
 >> GAMEPLAY INTERPRETATION: Ship received 75 damage. Zero Flag = 1: Exact lethal damage! Triggering JZ GameOver.
------------------------------------------------------------

------------------------------------------------------------
 [80386 ALU CYCLE EXECUTION]: SUB EAX, EDX (Health Damage / Check)
------------------------------------------------------------
 Operand 1 (EAX): 0x00000014 (20)
 Operand 2 (EDX): 0x00000032 (50)
 ALU Result    : 0xFFFFFFE2 (4294967266)
 Status Flags  : [CF=1] [ZF=0] [SF=1] [OF=0] [PF=1] [AF=0]
 >> GAMEPLAY INTERPRETATION: Ship received 50 damage. Sign/Carry Flag = 1: Fatal overkill! Triggering JS/JC GameOver.
------------------------------------------------------------
 -> Health damage, borrow, zero flag, and sign flag verified.

>>> ALL COA 80386 ALU ARITHMETIC TESTS PASSED! <<<
=====================================================""".splitlines()

render_terminal("Command Prompt - run_coa_tests.bat (80386 ALU Micro-Ops)", coa_output,
                os.path.join(OUT_DIR, "Figure_13_COA_Test_Result.png"))

# ==============================================================================
# 4. Figure 2: Four-Subject Integration Architecture Diagram
# ==============================================================================
def render_figure_2():
    w, h = 1100, 720
    img = Image.new('RGB', (w, h), (18, 22, 34))
    draw = ImageDraw.Draw(img)
    
    # Outer Border & Header
    draw.rectangle([20, 20, w-20, h-20], outline=(45, 60, 95), width=2)
    draw.rectangle([20, 20, w-20, 90], fill=(26, 32, 50))
    draw.text((w//2, 45), "AIRSTRIKER: FOUR-SUBJECT INTEGRATION ARCHITECTURE", fill=(100, 210, 255), font=FONT_TITLE, anchor="mm")
    draw.text((w//2, 73), "Unified Capstone Engineering Project Across Core Academic Disciplines", fill=(170, 185, 210), font=FONT_SUBTITLE, anchor="mm")
    
    # Central Core
    cx, cy = w // 2, 380
    core_w, core_h = 280, 140
    draw.rectangle([cx - core_w//2, cy - core_h//2, cx + core_w//2, cy + core_h//2], fill=(22, 40, 70), outline=(0, 200, 255), width=3)
    draw.text((cx, cy - 35), "CENTRAL GAME ENGINE", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
    draw.text((cx, cy - 10), "(MainGame.cpp / 60 FPS)", fill=(130, 200, 255), font=FONT_SUBTITLE, anchor="mm")
    draw.text((cx, cy + 18), "* Input Capture & Kinematics", fill=(200, 220, 245), font=FONT_MONO_SM, anchor="mm")
    draw.text((cx, cy + 38), "* AABB Collision Sweeps", fill=(200, 220, 245), font=FONT_MONO_SM, anchor="mm")
    draw.text((cx, cy + 58), "* Real-time Orchestration", fill=(200, 220, 245), font=FONT_MONO_SM, anchor="mm")
    
    # 4 Modules
    modules = [
        ("1. PROGRAMMING LAB (PL)", "Pure C++11 (Data Structures)", 
         ["* Fixed Array Pool: bullets[64]", "* Zero runtime malloc / new / delete", "* O(1) slot allocation & recycling", "* Kinematic coordinate updates"],
         (60, 130, 420, 310), (0, 180, 120)),
        ("2. COMPUTER GRAPHICS (CGL)", "C++ / Native OpenGL / FreeGLUT",
         ["* 800x600 Double-Buffered Window", "* Procedural Geometric Primitives", "* Parallax 3-Tier Starfield (GL_POINTS)", "* Dynamic HUD & Color Gauge"],
         (w - 480, 130, w - 60, 310), (180, 100, 255)),
        ("3. OBJECT ORIENTED (PSOOP)", "Java (JDK 26 OOP Models)",
         ["* Base Character Class Encapsulation", "* Player (Shield + Callsign Validation)", "* Enemy Drones & Bounty Scoring", "* GameEngine Service Architecture"],
         (60, 450, 420, 630), (255, 170, 40)),
        ("4. ARCHITECTURE (COA)", "YASMIN CPU-OS / 80386 ALU",
         ["* ALU Addition: ADD EAX, EDX (Score)", "* ALU Subtraction: SUB EAX, EDX (HP)", "* Status Flags: ZF=1 triggers Game Over", "* Register File Simulation: R00, R01"],
         (w - 480, 450, w - 60, 630), (255, 80, 80))
    ]
    
    for title, tech, points, box, color in modules:
        x1, y1, x2, y2 = box
        draw.rectangle([x1, y1, x2, y2], fill=(22, 28, 44), outline=color, width=2)
        draw.rectangle([x1, y1, x2, y1 + 38], fill=(30, 38, 60))
        draw.text((x1 + 15, y1 + 12), title, fill=color, font=FONT_BOLD)
        draw.text((x1 + 15, y1 + 27), tech, fill=(150, 165, 190), font=FONT_SUBTITLE)
        
        py = y1 + 52
        for pt in points:
            draw.text((x1 + 20, py), pt, fill=(220, 230, 245), font=FONT_REG)
            py += 26
            
        # Draw connector arrow to core
        bx = (x1 + x2) // 2
        by = (y1 + y2) // 2
        if x1 < cx:
            draw.line([(x2, by), (cx - core_w//2, cy)], fill=color, width=2)
        else:
            draw.line([(x1, by), (cx + core_w//2, cy)], fill=color, width=2)
            
    img.save(os.path.join(OUT_DIR, "Figure_02_Four_Subject_Integration_Architecture.png"), quality=95)
    print("Generated Figure 2")

render_figure_2()

# ==============================================================================
# 5. Figure 3: Overall CO-1 Software Architecture
# ==============================================================================
def render_figure_3():
    w, h = 1000, 750
    img = Image.new('RGB', (w, h), (16, 20, 30))
    draw = ImageDraw.Draw(img)
    
    draw.rectangle([20, 20, w-20, h-20], outline=(40, 55, 85), width=2)
    draw.rectangle([20, 20, w-20, 85], fill=(24, 30, 48))
    draw.text((w//2, 42), "AIRSTRIKER: OVERALL CO-1 SOFTWARE ARCHITECTURE", fill=(100, 210, 255), font=FONT_TITLE, anchor="mm")
    draw.text((w//2, 68), "Frame Cycle Flow, Subsystem Interfaces & Execution Pipeline", fill=(160, 175, 200), font=FONT_SUBTITLE, anchor="mm")
    
    boxes = [
        ("INPUT LAYER (Keyboard & Window Events)", 
         ["* Multi-key input buffer: keys[256], specialKeys[256]", "* Directional Vector: WASD / Arrows (dx, dy with normalization)", "* Trigger Actions: SPACE (Fire Laser), R (Reboot), ESC (Exit)"],
         (120, 115, w-120, 195), (60, 160, 240)),
         
        ("MAIN GAME LOOP (MainGame.cpp / 60 FPS Timer Callback)",
         ["1. Input Poll -> 2. Starfield Update -> 3. Bullet Kinematics -> 4. Enemy Drift",
          "5. AABB Hit Sweeps (Bullet vs Enemy & Player vs Enemy) -> 6. OpenGL Render Call"],
         (120, 230, w-120, 320), (255, 200, 60)),
         
        ("DATA STRUCTURES & POOLING (PL C++)",
         ["* BulletPoolArray (bullets[64])", "* O(1) Allocation: fireBullet()", "* Kinematics: updateBullets()", "* Slot Recycling without GC"],
         (120, 360, 520, 490), (0, 210, 130)),
         
        ("ARITHMETIC & STATE MACHINE (COA 80386)",
         ["* ALU80386 Class & Registers", "* ADD EAX, EDX -> Score Tally", "* SUB EAX, EDX -> Hull Damage", "* Flag Trigger: ZF=1 -> isGameOver=true"],
         (w-520, 360, w-120, 490), (255, 90, 90)),
         
        ("RENDERER PIPELINE (CGL OpenGL Native)",
         ["* double-buffered RGB setup (glutSwapBuffers)", "* Pure procedural primitives (GL_TRIANGLES, GL_QUADS, GL_POINTS)", "* Parallax multi-tier starfield, pulsating thruster fire, lasers, HUD"],
         (120, 530, 520, 660), (190, 110, 255)),
         
        ("JAVA OOP DOMAIN MIRROR (PSOOP)",
         ["* Character base class with strict encapsulation & callsign validator", "* Player subclass (Shield Absorption priority -> Hull damage pass-through)", "* GameEngine state synchronization & telemetry snapshot verification"],
         (w-520, 530, w-120, 660), (255, 160, 40))
    ]
    
    for title, lines, (x1, y1, x2, y2), col in boxes:
        draw.rectangle([x1, y1, x2, y2], fill=(22, 28, 44), outline=col, width=2)
        draw.rectangle([x1, y1, x2, y1 + 28], fill=(30, 38, 58))
        draw.text((x1 + 15, y1 + 6), title, fill=col, font=FONT_BOLD)
        
        ly = y1 + 36
        for l in lines:
            draw.text((x1 + 15, ly), l, fill=(215, 225, 240), font=FONT_REG)
            ly += 22
            
    # Draw vertical pipeline arrows
    draw.line([(w//2, 195), (w//2, 230)], fill=(100, 200, 255), width=3)
    draw.line([(320, 320), (320, 360)], fill=(0, 210, 130), width=3)
    draw.line([(w-320, 320), (w-320, 360)], fill=(255, 90, 90), width=3)
    draw.line([(320, 490), (320, 530)], fill=(190, 110, 255), width=3)
    draw.line([(w-320, 490), (w-320, 530)], fill=(255, 160, 40), width=3)
    
    img.save(os.path.join(OUT_DIR, "Figure_03_Overall_CO1_Software_Architecture.png"), quality=95)
    print("Generated Figure 3")

render_figure_3()

# ==============================================================================
# 6. Figure 5: Bullet Pool Architecture
# ==============================================================================
def render_figure_5():
    w, h = 1050, 680
    img = Image.new('RGB', (w, h), (18, 22, 32))
    draw = ImageDraw.Draw(img)
    
    draw.rectangle([20, 20, w-20, h-20], outline=(45, 60, 90), width=2)
    draw.rectangle([20, 20, w-20, 85], fill=(25, 32, 50))
    draw.text((w//2, 42), "FIGURE 5: FIXED-ARRAY BULLET POOL ARCHITECTURE (PL)", fill=(100, 210, 255), font=FONT_TITLE, anchor="mm")
    draw.text((w//2, 68), "Contiguous Static Memory Allocation: Bullet bullets[64] with Zero GC Pauses", fill=(170, 185, 210), font=FONT_SUBTITLE, anchor="mm")
    
    # Pool Slots Visualization
    start_x, start_y = 60, 120
    slot_w, slot_h = 135, 110
    cols = 6
    
    slots_data = [
        (0, 1, "(120, 250)", "ACTIVE", (0, 200, 120)),
        (1, 2, "(160, 250)", "ACTIVE", (0, 200, 120)),
        (2, 3, "(200, 250)", "RECYCLED", (255, 200, 50)),
        (3, "-", "FREE", "INACTIVE", (100, 110, 130)),
        (4, 4, "(320, 310)", "ACTIVE", (0, 200, 120)),
        (5, "-", "FREE", "INACTIVE", (100, 110, 130)),
    ]
    
    for i, (slot_idx, bid, pos, status, col) in enumerate(slots_data):
        sx = start_x + i * (slot_w + 20)
        sy = start_y
        draw.rectangle([sx, sy, sx + slot_w, sy + slot_h], fill=(24, 30, 48), outline=col, width=2)
        draw.rectangle([sx, sy, sx + slot_w, sy + 28], fill=(32, 40, 64))
        draw.text((sx + slot_w//2, sy + 14), f"SLOT [{slot_idx}]", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
        draw.text((sx + 10, sy + 38), f"BulletID: {bid}", fill=(200, 220, 245), font=FONT_MONO_SM)
        draw.text((sx + 10, sy + 58), f"Pos: {pos}", fill=(180, 200, 230), font=FONT_MONO_SM)
        draw.text((sx + 10, sy + 80), f"Status: {status}", fill=col, font=FONT_BOLD)
        
    draw.text((start_x + 6 * (slot_w + 20), start_y + slot_h//2), "... up to SLOT [63]", fill=(150, 170, 200), font=FONT_BOLD)
    
    # Feature Cards Below
    cards = [
        ("O(1) ACQUISITION (fireBullet)", 
         ["* Traverses contiguous buffer for active == false",
          "* Claims slot immediately, sets parameters",
          "* Returns unique bulletID without heap alloc",
          "* Worst case: bounded by fixed pool capacity (64)"],
         (60, 260, 490, 430), (0, 210, 130)),
         
        ("KINEMATIC STEP & AUTO-RECLAIM",
         ["* Sequential RAM sweep during updateBullets()",
          "* x += vx, y += vy with cache-friendly access",
          "* If (x < 0 || x > 800 || y < 0 || y > 600):",
          "    bullets[i].active = false (Instant slot reuse)"],
         (w-490, 260, w-60, 430), (100, 200, 255)),
         
        ("WHY FIXED POOLING IN AIRSTRIKER?",
         ["1. Eliminates std::vector reallocation and pointer invalidation.",
          "2. Prevents fragmentation from rapid 60 FPS projectile instantiation.",
          "3. Guarantees deterministic constant-time slot recycling.",
          "4. Directly demonstrates academic data structures syllabus (static contiguous arrays)."],
         (60, 460, w-60, 630), (255, 180, 50))
    ]
    
    for title, lines, (x1, y1, x2, y2), col in cards:
        draw.rectangle([x1, y1, x2, y2], fill=(22, 28, 44), outline=col, width=2)
        draw.rectangle([x1, y1, x2, y1 + 30], fill=(30, 38, 60))
        draw.text((x1 + 15, y1 + 6), title, fill=col, font=FONT_BOLD)
        
        ly = y1 + 40
        for l in lines:
            draw.text((x1 + 15, ly), l, fill=(220, 230, 245), font=FONT_REG)
            ly += 25
            
    img.save(os.path.join(OUT_DIR, "Figure_05_Bullet_Pool_Architecture.png"), quality=95)
    print("Generated Figure 5")

render_figure_5()

# ==============================================================================
# 7. Figure 6: PSOOP Class Diagram
# ==============================================================================
def render_figure_6():
    w, h = 1050, 720
    img = Image.new('RGB', (w, h), (18, 22, 32))
    draw = ImageDraw.Draw(img)
    
    draw.rectangle([20, 20, w-20, h-20], outline=(45, 60, 90), width=2)
    draw.rectangle([20, 20, w-20, 85], fill=(25, 32, 50))
    draw.text((w//2, 42), "FIGURE 6: PSOOP OBJECT-ORIENTED CLASS DIAGRAM (JAVA)", fill=(100, 210, 255), font=FONT_TITLE, anchor="mm")
    draw.text((w//2, 68), "Strict Encapsulation, Validation, Inheritance Hierarchy & Service Orchestration", fill=(170, 185, 210), font=FONT_SUBTITLE, anchor="mm")
    
    # Class 1: Character (Base)
    cx1, cy1, cx2, cy2 = w//2 - 190, 115, w//2 + 190, 310
    draw.rectangle([cx1, cy1, cx2, cy2], fill=(22, 28, 44), outline=(100, 200, 255), width=2)
    draw.rectangle([cx1, cy1, cx2, cy1 + 30], fill=(30, 42, 68))
    draw.text(((cx1+cx2)//2, cy1 + 15), "«abstract» Character", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
    
    char_fields = [
        "- name: String (validated 2..25 chars)",
        "- health: int",
        "- maxHealth: int",
        "- speed: double",
        "- active: boolean"
    ]
    char_methods = [
        "+ takeDamage(amount: int): void",
        "+ heal(amount: int): void",
        "+ move(dx: double, dy: double): void",
        "+ validated getters / setters"
    ]
    ly = cy1 + 38
    for f in char_fields:
        draw.text((cx1 + 10, ly), f, fill=(200, 220, 245), font=FONT_MONO_SM)
        ly += 18
    draw.line([(cx1, ly+2), (cx2, ly+2)], fill=(45, 60, 90), width=1)
    ly += 8
    for m in char_methods:
        draw.text((cx1 + 10, ly), m, fill=(130, 220, 160), font=FONT_MONO_SM)
        ly += 18
        
    # Class 2: Player (Subclass)
    px1, py1, px2, py2 = 60, 370, 460, 550
    draw.rectangle([px1, py1, px2, py2], fill=(22, 28, 44), outline=(0, 210, 130), width=2)
    draw.rectangle([px1, py1, px2, py1 + 30], fill=(26, 45, 45))
    draw.text(((px1+px2)//2, py1 + 15), "Player (extends Character)", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
    p_lines = [
        "- shield: int (default 50)",
        "- score: long",
        "- lives: int (default 3)",
        "------------------------------------",
        "+ takeDamage(amount: int): void",
        "   (Priority: Shield absorbs first,",
        "    residual damage hits health)",
        "+ addScore(points: int): void"
    ]
    ly = py1 + 36
    for l in p_lines:
        draw.text((px1 + 10, ly), l, fill=(210, 235, 220), font=FONT_MONO_SM)
        ly += 18
        
    # Class 3: Enemy (Subclass)
    ex1, ey1, ex2, ey2 = w - 460, 370, w - 60, 550
    draw.rectangle([ex1, ey1, ex2, ey2], fill=(22, 28, 44), outline=(255, 100, 100), width=2)
    draw.rectangle([ex1, ey1, ex2, ey1 + 30], fill=(50, 30, 35))
    draw.text(((ex1+ex2)//2, ey1 + 15), "Enemy (extends Character)", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
    e_lines = [
        "- archetype: String ('DRONE')",
        "- bountyScore: int (100 pts)",
        "- collisionDamage: int (25 HP)",
        "------------------------------------",
        "+ updateKinematics(): void",
        "+ getBountyScore(): int",
        "+ getCollisionDamage(): int"
    ]
    ly = ey1 + 36
    for l in e_lines:
        draw.text((ex1 + 10, ly), l, fill=(245, 210, 210), font=FONT_MONO_SM)
        ly += 18
        
    # Inheritance lines
    draw.line([(w//2, cy2), (w//2, cy2 + 30)], fill=(150, 180, 220), width=2)
    draw.line([(px1 + 200, cy2 + 30), (ex1 + 200, cy2 + 30)], fill=(150, 180, 220), width=2)
    draw.line([(px1 + 200, cy2 + 30), (px1 + 200, py1)], fill=(150, 180, 220), width=2)
    draw.line([(ex1 + 200, cy2 + 30), (ex1 + 200, ey1)], fill=(150, 180, 220), width=2)
    
    # Class 4: GameEngine Service
    gx1, gy1, gx2, gy2 = 180, 580, w - 180, 690
    draw.rectangle([gx1, gy1, gx2, gy2], fill=(22, 28, 44), outline=(255, 180, 50), width=2)
    draw.rectangle([gx1, gy1, gx2, gy1 + 26], fill=(45, 38, 25))
    draw.text(((gx1+gx2)//2, gy1 + 13), "GameEngine (Service Orchestrator & State Container)", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
    g_text = "Maintains: Player player, List<Enemy> enemies, List<Bullet> bullets | Methods: updateSimulation(), checkCollisions(), getGameState()"
    draw.text(((gx1+gx2)//2, gy1 + 55), g_text, fill=(230, 220, 180), font=FONT_MONO_SM, anchor="mm")
    
    img.save(os.path.join(OUT_DIR, "Figure_06_PSOOP_Class_Diagram.png"), quality=95)
    print("Generated Figure 6")

render_figure_6()

# ==============================================================================
# 8. Figure 7: COA ALU Architecture
# ==============================================================================
def render_figure_7():
    w, h = 1050, 720
    img = Image.new('RGB', (w, h), (18, 22, 32))
    draw = ImageDraw.Draw(img)
    
    draw.rectangle([20, 20, w-20, h-20], outline=(45, 60, 90), width=2)
    draw.rectangle([20, 20, w-20, 85], fill=(25, 32, 50))
    draw.text((w//2, 42), "FIGURE 7: COA 80386 ALU ARCHITECTURE & FLAG ROUTING", fill=(100, 210, 255), font=FONT_TITLE, anchor="mm")
    draw.text((w//2, 68), "Hardware Register Emulation, ADD/SUB Micro-Operations & Condition Branching", fill=(170, 185, 210), font=FONT_SUBTITLE, anchor="mm")
    
    # 32-bit Registers Box
    draw.rectangle([60, 120, 360, 300], fill=(22, 28, 44), outline=(100, 200, 255), width=2)
    draw.rectangle([60, 120, 360, 155], fill=(30, 42, 65))
    draw.text((210, 137), "32-BIT REGISTERS", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
    draw.text((80, 175), "EAX (Accumulator):", fill=(200, 220, 245), font=FONT_BOLD)
    draw.text((80, 198), "  Score / Current Hull Points", fill=(160, 180, 210), font=FONT_REG)
    draw.text((80, 230), "EDX (Data Register):", fill=(200, 220, 245), font=FONT_BOLD)
    draw.text((80, 253), "  Drone Bounty / Incoming Damage", fill=(160, 180, 210), font=FONT_REG)
    
    # ALU Core
    alux1, aluy1, alux2, aluy2 = w//2 - 130, 150, w//2 + 130, 340
    draw.rectangle([alux1, aluy1, alux2, aluy2], fill=(35, 25, 45), outline=(255, 100, 200), width=3)
    draw.rectangle([alux1, aluy1, alux2, aluy1 + 35], fill=(55, 35, 70))
    draw.text(((alux1+alux2)//2, aluy1 + 17), "80386 ALU CORE", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
    draw.text(((alux1+alux2)//2, aluy1 + 60), "Micro-Operations:", fill=(255, 200, 240), font=FONT_BOLD, anchor="mm")
    draw.text(((alux1+alux2)//2, aluy1 + 95), "ADD EAX, EDX", fill=(100, 255, 150), font=FONT_MONO_BOLD, anchor="mm")
    draw.text(((alux1+alux2)//2, aluy1 + 125), "(Score Accumulation)", fill=(180, 200, 220), font=FONT_SUBTITLE, anchor="mm")
    draw.text(((alux1+alux2)//2, aluy1 + 155), "SUB EAX, EDX", fill=(255, 120, 120), font=FONT_MONO_BOLD, anchor="mm")
    draw.text(((alux1+alux2)//2, aluy1 + 180), "(Hull Damage Deduction)", fill=(180, 200, 220), font=FONT_SUBTITLE, anchor="mm")
    
    # Connect registers to ALU
    draw.line([(360, 210), (alux1, 210)], fill=(100, 200, 255), width=3)
    draw.text((385, 190), "Operands", fill=(150, 180, 220), font=FONT_MONO_SM)
    
    # EFLAGS Status Register
    fx1, fy1, fx2, fy2 = w - 360, 120, w - 60, 300
    draw.rectangle([fx1, fy1, fx2, fy2], fill=(22, 28, 44), outline=(255, 180, 50), width=2)
    draw.rectangle([fx1, fy1, fx2, fy1 + 35], fill=(50, 40, 25))
    draw.text(((fx1+fx2)//2, fy1 + 17), "EFLAGS STATUS REGISTER", fill=(255, 255, 255), font=FONT_BOLD, anchor="mm")
    flags = [
        "ZF (Zero Flag): Set if Result == 0",
        "SF (Sign Flag): Set if Result < 0 (MSB=1)",
        "CF (Carry Flag): Set on Unsigned Borrow/Carry",
        "OF (Overflow): Set on Signed Overflow"
    ]
    ly = fy1 + 45
    for fl in flags:
        draw.text((fx1 + 12, ly), fl, fill=(240, 220, 180), font=FONT_MONO_SM)
        ly += 24
        
    # Connect ALU to EFLAGS
    draw.line([(alux2, 210), (fx1, 210)], fill=(255, 180, 50), width=3)
    draw.text((alux2 + 25, 190), "Status Flags", fill=(240, 200, 140), font=FONT_MONO_SM)
    
    # Game Logic Mapping Card Below
    draw.rectangle([60, 390, w - 60, 670], fill=(20, 26, 40), outline=(80, 120, 180), width=2)
    draw.rectangle([60, 390, w - 60, 425], fill=(30, 38, 60))
    draw.text((w//2, 407), "HARDWARE TO GAMEPLAY LOGIC MAPPING IN AIRSTRIKER", fill=(100, 210, 255), font=FONT_BOLD, anchor="mm")
    
    mapping_points = [
        ("Score Addition: ", "MOV #bounty, EDX -> ADD EAX, EDX. Result returned to playerScore telemetry."),
        ("Normal Damage:  ", "MOV #damage, EDX -> SUB EAX, EDX. If ZF=0 and SF=0: Ship integrity holds, game continues."),
        ("Exact Destruction:", "SUB EAX, EDX leaves 0 -> ZF = 1. Condition code JZ GameOver triggers mission failure."),
        ("Overkill Breach: ", "SUB EAX, EDX causes borrow -> SF = 1 or CF = 1. Condition code JS/JC GameOver triggers immediate breach."),
        ("College Lab Sync:", "Direct 1-to-1 equivalence with YASMIN CPU-OS Simulator Practical 1 (ADD R00, R01 & SUB R00, R01).")
    ]
    ly = 445
    for title, desc in mapping_points:
        draw.text((80, ly), title, fill=(255, 200, 80), font=FONT_BOLD)
        draw.text((270, ly), desc, fill=(220, 230, 245), font=FONT_REG)
        ly += 42
        
    img.save(os.path.join(OUT_DIR, "Figure_07_COA_ALU_Architecture.png"), quality=95)
    print("Generated Figure 7")

render_figure_7()

print("ALL DIAGRAMS SUCCESSFULLY GENERATED!")
