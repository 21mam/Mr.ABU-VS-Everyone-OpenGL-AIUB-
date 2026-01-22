#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <iostream>
#include <algorithm>

// ==========================================
// 1. UTILITY: CUSTOM RANDOM NUMBER GENERATOR
// ==========================================

unsigned long int nextRandom = 1;

int myRand()
{
    nextRandom = nextRandom * 1103515245 + 12345;
    return (unsigned int)(nextRandom / 65536) % 32768;
}

float myRandFloat()
{
    return (float)myRand() / 32768.0f;
}

// ==========================================
// 2. VISUAL CONFIGURATION
// ==========================================
const float PI = 3.14159265359f;

struct Color
{
    float r, g, b;
};

// --- WORLD COLORS ---
const Color COL_SKY = {0.42f, 0.81f, 0.98f};
const Color COL_CLOUD = {1.00f, 1.00f, 1.00f};
const Color COL_MTN_FAR = {0.55f, 0.83f, 0.98f};
const Color COL_MTN_NEAR = {0.40f, 0.75f, 0.95f};
const Color COL_GRASS_SHADOW = {0.35f, 0.68f, 0.35f};
const Color COL_GRASS_BODY = {0.45f, 0.78f, 0.45f};
const Color COL_GRASS_MID = {0.50f, 0.82f, 0.48f};
const Color COL_GRASS_HIGHLIGHT = {0.55f, 0.85f, 0.50f};
const Color COL_GRASS_TIP = {0.60f, 0.88f, 0.52f};
const Color COL_STONE_GAP = {0.15f, 0.18f, 0.22f};
const Color COL_STONE_BASE = {0.28f, 0.35f, 0.42f};
const Color COL_STONE_HIGHLIGHT = {0.32f, 0.39f, 0.46f};
const Color COL_OBSTACLE = {0.4f, 0.2f, 0.2f};

// --- ENEMY PALETTE ---
const Color CLR_SKIN = {0.25f, 0.25f, 0.3f};
const Color CLR_HOODIE = {0.20f, 0.20f, 0.25f};
const Color CLR_PANTS = {0.10f, 0.10f, 0.10f};
const Color CLR_EYES = {1.00f, 0.90f, 0.00f};

const float colTreePineLight[3] = {0.15f, 0.55f, 0.25f};
const float colTreePineDark[3] = {0.05f, 0.35f, 0.15f};
const float colTreeDecLight[3] = {0.30f, 0.70f, 0.35f};
const float colTreeDecDark[3] = {0.15f, 0.45f, 0.20f};
const float colTreeTrunk[3] = {0.35f, 0.25f, 0.15f};

GLuint listTreePine, listTreeDeciduous;

// ==========================================
// 3. GAME LOGIC CONFIGURATION
// ==========================================
const float LEVEL_START_X = -1.0f;
const float LEVEL_END_X = 14.0f;
const float PIT_DEPTH = -3.0f;

const float GRAVITY = 0.003f;
const float JUMP_FORCE = 0.065f;
const float MOVE_ACCEL = 0.005f;
const float MAX_SPEED = 0.040f;
const float CRAWL_SPEED = 0.015f;
const float FRICTION = 0.90f;
const float KNOCKBACK_FORCE = 0.04f;

int gameState = 0;
float cameraX = 0.0f;
float screenShake = 0.0f;
float redFlash = 0.0f;

float pX = 0.0f, pY = -0.5f;
float pVelX = 0.0f, pVelY = 0.0f;
bool isJumping = false;
bool facingRight = true;
float playerHealth = 100.0f;
int playerLives = 3;
int playerInvincibleTimer = 0;

float walkTimer = 0.0f;
bool isAttacking = false;
bool isCrouching = false;
float batAngle = 0.0f;
int attackFrame = 0;

bool keyA = false, keyD = false, keySpace = false, keyW = false, keyS = false;

// ==========================================
// 4. DATA STRUCTURES
// ==========================================

struct RectObj
{
    float x, y, w, h;
};
struct HitMarker
{
    float x, y;
    int life;
};
HitMarker hitEffect = {0, 0, 0};

struct Enemy
{
    float x, y;
    float velX;
    float startX, endX, speed;
    bool alive, facingRight;
    bool isBoss;
    int hp;
    int attackCooldown;
    bool isAttacking;
    int attackFrame;
};

// --- GROUND LOGIC ---
RectObj groundSegments[] = {
    {-5.0f, -1.2f, 12.0f, 0.6f},
    {9.5f, -1.2f, 15.5f, 0.6f}};
const int numGrounds = 2;

// --- OBSTACLE LOGIC ---
RectObj obstacles[] = {
    {4.0f, -0.35f, 1.5f, 1.0f}};
const int numObstacles = 1;

RectObj platforms[] = {
    {2.0f, -0.2f, 1.0f, 0.05f},
    {8.0f, -0.4f, 0.8f, 0.05f},
    {11.0f, -0.1f, 1.0f, 0.05f},
    {13.0f, -0.3f, 1.0f, 0.05f}};
const int numPlatforms = 4;

Enemy enemies[] = {
    {2.5f, -0.6f, 0.0f, 1.5f, 3.5f, 0.003f, true, true, false, 2, 0, false, 0},
    {6.0f, -0.6f, 0.0f, 5.0f, 6.5f, 0.004f, true, false, false, 2, 0, false, 0},
    {10.5f, -0.6f, 0.0f, 10.0f, 12.0f, 0.003f, true, true, false, 2, 0, false, 0},
    {13.0f, -0.6f, 0.0f, 11.5f, 13.8f, 0.005f, true, false, true, 8, 0, false, 0}};
const int numEnemies = 4;

bool checkOverlap(RectObj A, RectObj B)
{
    return (A.x < B.x + B.w && A.x + A.w > B.x &&
            A.y < B.y + B.h && A.y + A.h > B.y);
}

// ==========================================
// 5. DRAWING HELPERS
// ==========================================

void setColor(Color c) { glColor3f(c.r, c.g, c.b); }

void drawCircle(float cx, float cy, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++)
    {
        float theta = 2.0f * PI * float(i) / float(segments);
        glVertex2f(cx + radius * cosf(theta), cy + radius * sinf(theta));
    }
    glEnd();
}

void drawRoundedStone(float x, float y, float w, float h, float corner)
{
    glBegin(GL_POLYGON);
    glVertex2f(x + corner, y);
    glVertex2f(x + w - corner, y);
    glVertex2f(x + w, y + corner);
    glVertex2f(x + w, y + h - corner);
    glVertex2f(x + w - corner, y + h);
    glVertex2f(x + corner, y + h);
    glVertex2f(x, y + h - corner);
    glVertex2f(x, y + corner);
    glEnd();
}

// --- BOSS LIMBS ---
void drawBossLimb(float x1, float y1, float x2, float y2, float thickness, bool isArm = false, float rotation = 0.0f)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrt(dx * dx + dy * dy);

    if (length < 0.001f)
        return;

    dx /= length;
    dy /= length;

    float px = -dy * thickness * 0.5f;
    float py = dx * thickness * 0.5f;

    glBegin(GL_QUADS);
    glVertex2f(x1 + px, y1 + py);
    glVertex2f(x1 - px, y1 - py);
    glVertex2f(x2 - px, y2 - py);
    glVertex2f(x2 + px, y2 + py);
    glEnd();

    glPushMatrix();
    float jointX = x1 + dx * length * 0.6f;
    float jointY = y1 + dy * length * 0.6f;
    drawCircle(jointX, jointY, thickness * 1.2f, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x2, y2, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    if (isArm)
    {
        glBegin(GL_QUADS);
        glVertex2f(-thickness * 1.5f, -thickness * 0.5f);
        glVertex2f(thickness * 1.5f, -thickness * 0.5f);
        glVertex2f(thickness * 1.0f, thickness * 0.5f);
        glVertex2f(-thickness * 1.0f, thickness * 0.5f);
        glEnd();
    }
    else
    {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(-thickness * 1.5f, thickness * 0.8f);
        glVertex2f(0.0f, thickness * 1.5f);
        glVertex2f(thickness * 1.5f, thickness * 0.8f);
        glEnd();
    }
    glPopMatrix();
}

void drawBossKatana(float x, float y, float length, float rotation)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glColor3f(0.8f, 0.8f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(-length * 0.05f, 0.0f);
    glVertex2f(length * 0.05f, 0.0f);
    glVertex2f(length * 0.03f, -length);
    glVertex2f(-length * 0.03f, -length);
    glEnd();

    glColor3f(0.4f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-length * 0.08f, 0.0f);
    glVertex2f(length * 0.08f, 0.0f);
    glVertex2f(length * 0.08f, length * 0.25f);
    glVertex2f(-length * 0.08f, length * 0.25f);
    glEnd();
    glPopMatrix();
}

void createDisplayLists()
{
    listTreePine = glGenLists(1);
    glNewList(listTreePine, GL_COMPILE);
    glColor3fv(colTreeTrunk);
    glBegin(GL_POLYGON);
    glVertex2f(-0.03f, 0.0f);
    glVertex2f(0.03f, 0.0f);
    glVertex2f(0.01f, 0.6f);
    glVertex2f(-0.01f, 0.6f);
    glEnd();
    for (int i = 0; i < 3; i++)
    {
        float yBase = 0.15f + i * 0.15f;
        float width = 0.20f - i * 0.05f;
        float height = 0.25f;
        glBegin(GL_TRIANGLES);
        glColor3fv(colTreePineLight);
        glVertex2f(0.0f, yBase + height);
        glVertex2f(-width, yBase);
        glColor3fv(colTreePineDark);
        glVertex2f(0.0f, yBase);
        glColor3fv(colTreePineDark);
        glVertex2f(0.0f, yBase + height);
        glVertex2f(0.0f, yBase);
        glVertex2f(width, yBase);
        glEnd();
    }
    glEndList();

    listTreeDeciduous = glGenLists(1);
    glNewList(listTreeDeciduous, GL_COMPILE);
    glColor3fv(colTreeTrunk);
    glBegin(GL_POLYGON);
    glVertex2f(-0.04f, 0.0f);
    glVertex2f(0.04f, 0.0f);
    glVertex2f(0.02f, 0.25f);
    glVertex2f(-0.02f, 0.25f);
    glEnd();
    glColor3fv(colTreeDecDark);
    drawCircle(0.0f, 0.35f, 0.18f, 20);
    drawCircle(-0.12f, 0.28f, 0.12f, 15);
    drawCircle(0.12f, 0.28f, 0.12f, 15);
    glColor3fv(colTreeDecLight);
    drawCircle(0.0f, 0.38f, 0.14f, 20);
    drawCircle(-0.1f, 0.32f, 0.11f, 15);
    drawCircle(0.1f, 0.32f, 0.11f, 15);
    drawCircle(-0.05f, 0.42f, 0.09f, 15);
    drawCircle(0.05f, 0.40f, 0.08f, 15);
    glEndList();
}

// ==========================================
// 6. SCENE RENDERING
// ==========================================

void drawClouds()
{
    setColor(COL_CLOUD);
    for (int offset = -1; offset <= 3; offset++)
    {
        float xOff = offset * 4.0f;
        float clouds[][3] = {
            {-0.5f + xOff, 0.6f, 1.0f},
            {0.2f + xOff, 0.8f, 1.2f},
            {0.8f + xOff, 0.65f, 1.5f},
            {1.5f + xOff, 0.75f, 1.0f}};
        for (int i = 0; i < 4; i++)
        {
            float x = clouds[i][0];
            float y = clouds[i][1];
            float scale = clouds[i][2];
            drawCircle(x, y, 0.08f * scale, 20);
            drawCircle(x + 0.06f * scale, y + 0.02f * scale, 0.09f * scale, 20);
            drawCircle(x - 0.06f * scale, y + 0.01f * scale, 0.06f * scale, 20);
        }
    }
}

void drawMountains()
{
    float startX = -10.0f;
    float endX = 30.0f;

    setColor(COL_MTN_FAR);
    glBegin(GL_TRIANGLE_STRIP);
    for (float x = startX; x <= endX; x += 0.05f)
    {
        float y = -0.2f + sinf(x * 1.5f) * 0.5f + sinf(x * 3.0f) * 0.15f;
        glVertex2f(x, -1.2f);
        glVertex2f(x, y);
    }
    glEnd();

    setColor(COL_MTN_NEAR);
    glBegin(GL_TRIANGLE_STRIP);
    for (float x = startX; x <= endX; x += 0.05f)
    {
        float y = -0.4f + sinf(x * 2.5f + 2.0f) * 0.25f + sinf(x * 6.0f) * 0.05f;
        glVertex2f(x, -1.2f);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawWallAndGrass(float startX, float endX)
{
    unsigned long int oldSeed = nextRandom;
    nextRandom = (int)(startX * 100) + 555;

    float wallTop = -0.6f;
    float wallBottom = -1.2f;
    setColor(COL_STONE_GAP);
    glBegin(GL_QUADS);
    glVertex2f(startX, wallBottom);
    glVertex2f(endX, wallBottom);
    glVertex2f(endX, wallTop);
    glVertex2f(startX, wallTop);
    glEnd();

    float currentX = startX;
    float rowHeight = 0.2f;
    int rows = 3;
    for (int r = 0; r < rows; r++)
    {
        float yPos = wallTop - ((r + 1) * rowHeight);
        currentX = startX - (r % 2 == 0 ? 0.0f : 0.15f);
        while (currentX < endX)
        {
            float stoneW = 0.3f + (myRandFloat() * 0.1f);
            float stoneH = rowHeight - 0.02f;
            float gap = 0.01f;
            setColor(COL_STONE_BASE);
            drawRoundedStone(currentX + gap, yPos + gap, stoneW - gap * 2, stoneH - gap * 2, 0.02f);
            setColor(COL_STONE_HIGHLIGHT);
            drawRoundedStone(currentX + gap + 0.02f, yPos + gap + 0.02f, stoneW - gap * 2 - 0.04f, stoneH - gap * 2 - 0.04f, 0.01f);
            currentX += stoneW;
        }
    }

    setColor(COL_GRASS_BODY);
    glBegin(GL_QUADS);
    glVertex2f(startX, wallTop);
    glVertex2f(endX, wallTop);
    glVertex2f(endX, wallTop - 0.05f);
    glVertex2f(startX, wallTop - 0.05f);
    glEnd();

    glBegin(GL_TRIANGLES);
    for (float gx = startX; gx < endX; gx += 0.03f)
    {
        float h = 0.05f + (myRandFloat() * 0.1f);
        float w = 0.02f;
        setColor(COL_GRASS_TIP);
        glVertex2f(gx, wallTop);
        glVertex2f(gx + w, wallTop);
        glVertex2f(gx + w / 2, wallTop + h);
    }
    glEnd();

    nextRandom = oldSeed;
}

void drawSceneTrees(float startX, float endX)
{
    unsigned long int oldSeed = nextRandom;
    nextRandom = 999;

    for (float tx = startX; tx < endX; tx += 1.5f)
    {
        if (tx > 6.0f && tx < 10.0f)
            continue;

        float offset = myRandFloat() * 0.5f;
        float finalX = tx + offset;
        float scale = 0.8f + (myRandFloat() * 0.4f);

        glPushMatrix();
        glTranslatef(finalX, -0.6f, 0);
        glScalef(scale, scale, 1.0f);
        if (myRand() % 2 == 0)
            glCallList(listTreePine);
        else
            glCallList(listTreeDeciduous);
        glPopMatrix();
    }

    nextRandom = oldSeed;
}

// ==========================================
// 7. CHARACTER & UI DRAWING
// ==========================================

void drawRect(float x, float y, float w, float h, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawHitMarker()
{
    if (hitEffect.life > 0)
    {
        glColor3f(1.0f, 1.0f, 0.0f);
        glPushMatrix();
        glTranslatef(hitEffect.x, hitEffect.y, 0.0f);
        glRotatef(hitEffect.life * 25.0f, 0, 0, 1);
        float s = 0.08f;
        glBegin(GL_TRIANGLES);
        glVertex2f(-s, -s);
        glVertex2f(s, -s);
        glVertex2f(0, s);
        glVertex2f(-s, s);
        glVertex2f(s, s);
        glVertex2f(0, -s);
        glEnd();
        glPopMatrix();
        hitEffect.life--;
    }
}

// --- UNIFIED NINJA DRAWING FUNCTION ---
void drawNinjaEnemy(Enemy e)
{
    float s = e.isBoss ? 0.13f : 0.08f;
    Color headbandColor = e.isBoss ? (Color){0.9f, 0.1f, 0.1f} : (Color){0.2f, 0.2f, 0.8f};

    bool isMoving = (fabs(e.velX) > 0.001f);
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.01f;

    // Speeds up the walk cycle for small enemies
    if (!e.isBoss)
        time *= 1.5f;

    float legAngle = 0.0f;
    float headTilt = 0.0f;
    float footRotation = 0.0f;
    float armAngleRight = 0.0f;
    float armAngleLeft = 0.0f;
    float bodyYOffset = 0.0f;

    // --- ANIMATION STATE LOGIC ---
    if (e.isAttacking)
    {
        float progress = (float)e.attackFrame;
        if (progress < 5)
            armAngleRight = 120.0f + (progress * 10.0f); // Wind up
        else if (progress < 12)
            armAngleRight = 170.0f - ((progress - 5) * 25.0f); // Slash
        else
            armAngleRight = 0.0f; // Recover

        armAngleLeft = -30.0f;
        legAngle = 20.0f;
    }
    else if (isMoving)
    {
        float runCycle = time * 2.0f;
        legAngle = sin(runCycle) * 40.0f;
        headTilt = sin(runCycle * 0.5f) * 5.0f;
        footRotation = sin(runCycle) * 30.0f;
        float armBase = sin(runCycle) * 40.0f;
        armAngleRight = armBase;
        armAngleLeft = -armBase;
    }
    else
    {
        float breathe = sin(time);
        armAngleRight = 10.0f + breathe * 5.0f;
        armAngleLeft = -10.0f - breathe * 5.0f;
        headTilt = breathe * 2.0f;
    }

    float legRad = legAngle * PI / 180.0f;
    float legSwing = sin(legRad) * 0.4f;

    // --- DRAWING ---

    // 1. TORSO
    glPushMatrix();
    glTranslatef(0, -s * 1.5f + bodyYOffset, 0);

    glColor3f(0.05f, 0.05f, 0.08f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.6f, s * 0.1f);
    glVertex2f(s * 0.6f, s * 0.1f);
    glVertex2f(s * 0.4f, -s * 1.8f);
    glVertex2f(-s * 0.4f, -s * 1.8f);
    glEnd();

    // 2. BELT
    glColor3f(headbandColor.r, headbandColor.g, headbandColor.b);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.7f, -s * 1.0f);
    glVertex2f(s * 0.7f, -s * 1.0f);
    glVertex2f(s * 0.7f, -s * 1.2f);
    glVertex2f(-s * 0.7f, -s * 1.2f);
    glEnd();

    // 3. SCARF/ACCESSORY
    glColor3f(0.1f, 0.1f, 0.15f);
    glPushMatrix();
    glTranslatef(s * 0.3f, -s * 0.8f, 0.0f);
    glRotatef(10.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.1f, 0.0f);
    glVertex2f(s * 0.1f, 0.0f);
    glVertex2f(s * 0.06f, -s * 2.0f);
    glVertex2f(-s * 0.06f, -s * 2.0f);
    glEnd();
    glPopMatrix();
    glPopMatrix();

    // 4. LEGS
    glColor3f(0.08f, 0.08f, 0.12f);
    float hipY = -s * 2.2f + bodyYOffset;

    // Left Leg
    float leftLegRot = isMoving ? legSwing * 30.0f : 0.0f;
    drawBossLimb(-s * 0.3f, hipY, -s * 0.3f - sin(legRad * 0.5f) * s * 0.5f, hipY - s * 2.0f, s * 0.15f, false, leftLegRot + footRotation);

    // Right Leg
    float rightLegRot = isMoving ? -legSwing * 30.0f : 0.0f;
    drawBossLimb(s * 0.3f, hipY, s * 0.3f + sin(legRad * 0.5f + PI) * s * 0.5f, hipY - s * 2.0f, s * 0.15f, false, rightLegRot - footRotation);

    // 5. ARMS
    float shoulderY = -s * 1.6f + bodyYOffset;
    float armSwingRight = sin(armAngleRight * PI / 180.0f) * 0.3f;
    float armSwingLeft = sin(armAngleLeft * PI / 180.0f) * 0.3f;

    glColor3f(0.05f, 0.05f, 0.08f);

    // Left Arm (Back)
    drawBossLimb(s * 0.6f, shoulderY, s * 1.2f, shoulderY - s * 1.5f - armSwingLeft * s, s * 0.12f, true, armAngleLeft);

    // Right Arm (Front - Holds Katana)
    float handX = -s * 1.2f;
    float handY = shoulderY - s * 1.5f + armSwingRight * s;

    if (e.isAttacking)
    {
        handX = -s * 1.0f * cos(armAngleRight * PI / 180.0f);
        handY = shoulderY + s * 1.0f * sin(armAngleRight * PI / 180.0f);
    }

    drawBossLimb(-s * 0.6f, shoulderY, handX, handY, s * 0.12f, true, armAngleRight);

    // 6. KATANA
    float swordRot = -90.0f + armAngleRight;
    drawBossKatana(handX, handY, s * 2.5f, swordRot);

    // 7. HEAD
    glPushMatrix();
    glTranslatef(0.0f, -s * 1.4f + bodyYOffset, 0.0f);
    glRotatef(headTilt, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, s * 0.6f, 0.0f);

    // Face/Hood
    glColor3f(0.03f, 0.03f, 0.06f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, s * 0.5f);
    for (int i = 0; i <= 8; i++)
    {
        float angle = PI * 0.75f + i * PI / 4.0f;
        glVertex2f(cos(angle) * s * 0.9f, sin(angle) * s * 0.9f);
    }
    glEnd();

    // Skin area
    glColor3f(0.95f, 0.9f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.5f, s * 0.2f);
    glVertex2f(s * 0.5f, s * 0.2f);
    glVertex2f(s * 0.5f, -s * 0.2f);
    glVertex2f(-s * 0.5f, -s * 0.2f);
    glEnd();

    // Mask
    glColor3f(0.03f, 0.03f, 0.06f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.55f, -s * 0.05f);
    glVertex2f(s * 0.55f, -s * 0.05f);
    glVertex2f(s * 0.55f, -s * 0.25f);
    glVertex2f(-s * 0.55f, -s * 0.25f);
    glEnd();

    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-s * 0.25f, s * 0.02f, s * 0.08f, 10);
    drawCircle(s * 0.25f, s * 0.02f, s * 0.08f, 10);

    // Headband
    glColor3f(headbandColor.r, headbandColor.g, headbandColor.b);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-s * 0.85f, s * 0.3f);
    glVertex2f(s * 0.85f, s * 0.3f);
    glEnd();

    // Headband Tails
    glBegin(GL_TRIANGLES);
    glVertex2f(-s * 0.8f, s * 0.3f);
    glVertex2f(-s * 1.5f, s * 0.6f);
    glVertex2f(-s * 0.9f, s * 0.4f);
    glEnd();

    glPopMatrix();
}

void drawStudent()
{
    if (playerInvincibleTimer > 0 && (playerInvincibleTimer / 5) % 2 == 0)
        return;
    if (gameState == 2)
    {
        if (attackFrame < 20)
            attackFrame++;
        walkTimer += 0.2f;
    }

    glPushMatrix();
    glTranslatef(pX, pY, 0.0f);
    if (!facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);
    if (isCrouching)
        glScalef(1.0f, 0.6f, 1.0f);

    float bob = (fabs(pVelX) > 0.001f && !isJumping) ? sin(walkTimer) * 0.01f : 0.0f;

    drawRect(-0.03f, 0.0f, 0.03f, 0.12f, 0.1f, 0.1f, 0.1f);
    drawRect(0.01f, 0.0f, 0.03f, 0.12f, 0.1f, 0.1f, 0.1f);
    drawRect(-0.045f, 0.11f + bob, 0.10f, 0.16f, 0.15f, 0.2f, 0.3f);
    drawRect(-0.03f, 0.26f + bob, 0.07f, 0.07f, 0.8f, 0.7f, 0.6f);
    drawRect(0.01f, 0.27f + bob, 0.03f, 0.04f, 0.1f, 0.1f, 0.1f);

    glPushMatrix();
    glTranslatef(0.0f, 0.22f + bob, 0.0f);
    glRotatef(-20.0f - batAngle, 0.0f, 0.0f, 1.0f);
    drawRect(-0.015f, -0.08f, 0.03f, 0.09f, 0.15f, 0.2f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, -0.09f, 0.0f);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
    drawRect(-0.005f, 0.0f, 0.01f, 0.04f, 0.6f, 0.5f, 0.3f);
    drawRect(-0.01f, 0.04f, 0.02f, 0.18f, 0.4f, 0.25f, 0.1f);
    if (isAttacking && attackFrame > 8 && attackFrame < 12)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.1f, 0.2f);
        glVertex2f(0.2f, 0.1f);
        glEnd();
        glDisable(GL_BLEND);
    }
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

void drawVillain(Enemy e)
{
    if (!e.alive)
        return;
    glPushMatrix();

    // Position handling
    glTranslatef(e.x, e.y, 0.0f);

    // Damage Flash
    if (e.velX > 0.01f || e.velX < -0.01f)
        glColor3f(1.0f, 0.5f, 0.5f);
    else
        glColor3f(1, 1, 1);

    // Facing Direction
    if (!e.facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);

    if (e.isBoss)
    {
        // Boss Offset (Height adjustment for scale 0.13)
        glTranslatef(0.0f, 0.52f, 0.0f);
        drawNinjaEnemy(e);

        // HP Bar for Boss
        float hpPct = (float)e.hp / 8.0f;
        drawRect(-0.1f, 0.45f, 0.2f, 0.02f, 0.5f, 0.0f, 0.0f);
        drawRect(-0.1f, 0.45f, 0.2f * hpPct, 0.02f, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        // Small Enemy Offset (Height adjustment for scale 0.08)
        // 0.34f ensures the feet touch the ground instead of flying
        glTranslatef(0.0f, 0.34f, 0.0f);
        drawNinjaEnemy(e);
    }
    glPopMatrix();
}

void drawText(float x, float y, const char *string, void *font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    const char *c = string;
    while (*c != '\0')
    {
        glutBitmapCharacter(font, *c);
        c++;
    }
}

void drawHudStats(int lives, int distance)
{
    glRasterPos2f(20, 520);
    const char *label1 = "LIVES: ";
    for (const char *c = label1; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    char numBuf[16];
    int len = 0;
    int temp = lives;
    if (temp == 0)
        numBuf[len++] = '0';
    while (temp > 0)
    {
        numBuf[len++] = (temp % 10) + '0';
        temp /= 10;
    }
    for (int i = len - 1; i >= 0; i--)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, numBuf[i]);

    const char *label2 = "   BOSS DIST: ";
    for (const char *c = label2; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    temp = distance;
    len = 0;
    bool neg = false;
    if (temp < 0)
    {
        neg = true;
        temp = -temp;
    }
    if (temp == 0)
        numBuf[len++] = '0';
    while (temp > 0)
    {
        numBuf[len++] = (temp % 10) + '0';
        temp /= 10;
    }
    if (neg)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '-');
    for (int i = len - 1; i >= 0; i--)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, numBuf[i]);

    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'm');
}

void drawUI()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawRect(20, 550, 204, 24, 0.2f, 0.2f, 0.2f);
    drawRect(22, 552, 200, 20, 0.2f, 0.0f, 0.0f);
    if (playerHealth > 0)
        drawRect(22, 552, 200 * (playerHealth / 100.0f), 20, 0.0f, 0.8f, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawHudStats(playerLives, (int)(12.0f - pX));

    if (gameState == 1)
    {
        glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 600);
        glVertex2f(0, 600);
        glEnd();
        glDisable(GL_BLEND);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(350, 300, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(300, 270, "Press ENTER or SPACE to Restart");
    }

    if (gameState == 2)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 600);
        glVertex2f(0, 600);
        glEnd();
        glDisable(GL_BLEND);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawText(320, 350, "LEVEL COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    if (redFlash > 0.0f)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.0f, 0.0f, redFlash);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 600);
        glVertex2f(0, 600);
        glEnd();
        glDisable(GL_BLEND);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ==========================================
// 8. PHYSICS & MAIN
// ==========================================

void resetPlayer()
{
    pX = 0.0f;
    pY = -0.5f;
    pVelX = 0.0f;
    pVelY = 0.0f;
    playerHealth = 100.0f;
    screenShake = 0.0f;
    redFlash = 0.0f;
    isCrouching = false;
    playerInvincibleTimer = 0;
}

void fullReset()
{
    resetPlayer();
    playerLives = 3;
    gameState = 0;
    cameraX = 0.0f;
    for (int i = 0; i < numEnemies; i++)
    {
        enemies[i].alive = true;
        enemies[i].hp = (enemies[i].isBoss) ? 8 : 2;
        enemies[i].x = (enemies[i].startX + enemies[i].endX) / 2.0f;
        enemies[i].velX = 0;
        enemies[i].isAttacking = false;
        enemies[i].attackCooldown = 0;
    }
}

void takeDamage(float amount)
{
    if (gameState == 2 || playerInvincibleTimer > 0)
        return;
    playerHealth -= amount;
    screenShake = 0.1f;
    redFlash = 0.6f;
    playerInvincibleTimer = 60;
    pVelY = 0.03f;
    pVelX = (facingRight) ? -0.04f : 0.04f;
    if (playerHealth <= 0)
    {
        playerLives--;
        if (playerLives > 0)
        {
            pY = -0.5f;
            playerHealth = 100;
        }
        else
        {
            gameState = 1;
        }
    }
}

void timer(int value)
{
    if (gameState == 1 || gameState == 2)
    {
        if (gameState == 2)
            pX += 0.005f;
        glutPostRedisplay();
        glutTimerFunc(16, timer, 0);
        return;
    }

    if (screenShake > 0)
        screenShake -= 0.005f;
    else
        screenShake = 0;
    if (redFlash > 0)
        redFlash -= 0.05f;
    if (playerInvincibleTimer > 0)
        playerInvincibleTimer--;

    isCrouching = keyS && !isJumping;

    float currentAccel = MOVE_ACCEL;
    float currentMaxSpeed = MAX_SPEED;

    if (isCrouching)
    {
        currentAccel = MOVE_ACCEL * 0.5f;
        currentMaxSpeed = CRAWL_SPEED;
    }

    if (keyA)
    {
        pVelX -= currentAccel;
        facingRight = false;
        walkTimer += 0.3f;
    }
    if (keyD)
    {
        pVelX += currentAccel;
        facingRight = true;
        walkTimer += 0.3f;
    }

    if (pVelX > currentMaxSpeed)
        pVelX = currentMaxSpeed;
    if (pVelX < -currentMaxSpeed)
        pVelX = -currentMaxSpeed;

    if (!keyA && !keyD)
    {
        pVelX *= FRICTION;
        walkTimer = 0.0f;
    }

    float nextX = pX + pVelX;
    RectObj playerBoxFuture = {nextX - 0.03f, pY, 0.06f, isCrouching ? 0.15f : 0.3f};

    bool blocked = false;
    for (int i = 0; i < numObstacles; i++)
    {
        if (checkOverlap(playerBoxFuture, obstacles[i]))
        {
            blocked = true;
            pVelX = 0;
        }
    }

    if (!blocked)
    {
        pX += pVelX;
    }

    if (pX < LEVEL_START_X)
        pX = LEVEL_START_X;
    if (pX > LEVEL_END_X)
        pX = LEVEL_END_X;

    if (keySpace && !isJumping && !isCrouching)
    {
        pVelY = JUMP_FORCE;
        isJumping = true;
    }
    pY += pVelY;
    pVelY -= GRAVITY;

    bool onSolid = false;
    for (int i = 0; i < numGrounds; i++)
    {
        if (pX >= groundSegments[i].x && pX <= groundSegments[i].x + groundSegments[i].w)
        {
            float gTop = groundSegments[i].y + groundSegments[i].h;
            if (pY >= gTop - 0.1f && pY <= gTop + 0.1f && pVelY <= 0)
            {
                pY = gTop;
                pVelY = 0;
                isJumping = false;
                onSolid = true;
            }
        }
    }
    for (int i = 0; i < numPlatforms; i++)
    {
        RectObj plat = platforms[i];
        if (pVelY <= 0 && pY >= plat.y + plat.h - 0.1f && pY <= plat.y + plat.h + 0.1f && pX >= plat.x - 0.1f && pX <= plat.x + plat.w + 0.1f)
        {
            pY = plat.y + plat.h;
            pVelY = 0;
            isJumping = false;
            onSolid = true;
        }
    }
    if (!onSolid)
        isJumping = true;
    if (pY < PIT_DEPTH)
        takeDamage(100.0f);

    float targetCamX = pX;
    cameraX = targetCamX;
    if (cameraX < LEVEL_START_X + 1.0f)
        cameraX = LEVEL_START_X + 1.0f;
    if (cameraX > LEVEL_END_X - 1.0f)
        cameraX = LEVEL_END_X - 1.0f;

    RectObj playerBox = {pX - 0.03f, pY, 0.06f, 0.3f};
    if (isCrouching)
        playerBox.h = 0.15f;

    RectObj weaponBox = {0, 0, 0, 0};
    if (isAttacking && attackFrame > 8 && attackFrame < 15)
    {
        float reach = 0.35f;
        weaponBox.y = pY + 0.1f;
        weaponBox.h = 0.2f;
        weaponBox.w = reach;
        if (facingRight)
            weaponBox.x = pX;
        else
            weaponBox.x = pX - reach;
    }

    for (int i = 0; i < numEnemies; i++)
    {
        if (!enemies[i].alive)
            continue;
        enemies[i].x += enemies[i].velX;
        enemies[i].velX *= 0.9f;

        float eW = enemies[i].isBoss ? 0.15f : 0.08f;
        float eH = enemies[i].isBoss ? 0.5f : 0.3f;
        RectObj enemyBox = {enemies[i].x - eW / 2, enemies[i].y, eW, eH};
        float distX = enemies[i].x - pX;

        if (fabs(enemies[i].velX) < 0.001f && !enemies[i].isAttacking)
        {
            bool inAttackRange = (fabs(distX) < (enemies[i].isBoss ? 0.45f : 0.35f));
            if (inAttackRange && enemies[i].attackCooldown == 0)
            {
                enemies[i].facingRight = (distX < 0);
                enemies[i].isAttacking = true;
                enemies[i].attackFrame = 0;
            }
            else
            {
                if (enemies[i].facingRight)
                {
                    enemies[i].x += enemies[i].speed;
                    if (enemies[i].x > enemies[i].endX)
                        enemies[i].facingRight = false;
                }
                else
                {
                    enemies[i].x -= enemies[i].speed;
                    if (enemies[i].x < enemies[i].startX)
                        enemies[i].facingRight = true;
                }
            }
        }

        if (isAttacking && attackFrame > 8 && attackFrame < 15)
        {
            if (checkOverlap(weaponBox, enemyBox))
            {
                enemies[i].hp--;
                hitEffect.x = enemies[i].x;
                hitEffect.y = enemies[i].y + 0.2f;
                hitEffect.life = 5;
                enemies[i].velX = (facingRight ? KNOCKBACK_FORCE : -KNOCKBACK_FORCE);
                enemies[i].isAttacking = false;
                if (enemies[i].hp <= 0)
                {
                    enemies[i].alive = false;
                    screenShake = 0.05f;
                    if (enemies[i].isBoss)
                        gameState = 2;
                }
                isAttacking = false;
            }
        }

        if (enemies[i].isAttacking)
        {
            enemies[i].attackFrame++;
            if (enemies[i].attackFrame == 8)
            {
                float reach = enemies[i].isBoss ? 0.5f : 0.35f;
                RectObj eWeaponBox = {0, enemies[i].y + 0.1f, reach, 0.2f};
                if (enemies[i].facingRight)
                    eWeaponBox.x = enemies[i].x;
                else
                    eWeaponBox.x = enemies[i].x - reach;
                if (checkOverlap(eWeaponBox, playerBox))
                    takeDamage(enemies[i].isBoss ? 30.0f : 15.0f);
            }
            if (enemies[i].attackFrame > 20)
            {
                enemies[i].isAttacking = false;
                enemies[i].attackCooldown = 60;
            }
        }
        if (enemies[i].attackCooldown > 0)
            enemies[i].attackCooldown--;
    }

    if (isAttacking)
    {
        attackFrame++;
        if (attackFrame < 5)
            batAngle = -30.0f;
        else if (attackFrame < 12)
            batAngle = 100.0f;
        else if (attackFrame < 20)
            batAngle = (20 - attackFrame) * 10.0f;
        else
        {
            isAttacking = false;
            batAngle = 0.0f;
            attackFrame = 0;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void display()
{
    glClearColor(COL_SKY.r, COL_SKY.g, COL_SKY.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (screenShake > 0)
    {
        float rx = (myRand() % 100 / 5000.0f) * (screenShake * 10);
        float ry = (myRand() % 100 / 5000.0f) * (screenShake * 10);
        glTranslatef(rx, ry, 0);
    }

    glPushMatrix();
    glTranslatef(-cameraX * 0.9f, 0, 0);
    drawClouds();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cameraX * 0.5f, 0.0f, 0.0f);
    drawMountains();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cameraX, 0.0f, 0.0f);

    for (int i = 0; i < numGrounds; i++)
    {
        drawWallAndGrass(groundSegments[i].x, groundSegments[i].x + groundSegments[i].w);
    }

    drawSceneTrees(-2.0f, 16.0f);

    for (int i = 0; i < numPlatforms; i++)
    {
        drawRect(platforms[i].x, platforms[i].y, platforms[i].w, platforms[i].h, 0.25f, 0.18f, 0.15f);
        glColor3f(0.1f, 0.08f, 0.05f);
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        for (float r = platforms[i].x + 0.05f; r < platforms[i].x + platforms[i].w; r += 0.1f)
            glVertex2f(r, platforms[i].y + platforms[i].h / 2);
        glEnd();
    }

    for (int i = 0; i < numObstacles; i++)
    {
        drawRect(obstacles[i].x, obstacles[i].y, obstacles[i].w, obstacles[i].h, COL_OBSTACLE.r, COL_OBSTACLE.g, COL_OBSTACLE.b);
        glColor3f(COL_OBSTACLE.r * 0.8f, COL_OBSTACLE.g * 0.8f, COL_OBSTACLE.b * 0.8f);
        drawRect(obstacles[i].x + 0.1f, obstacles[i].y + 0.1f, obstacles[i].w - 0.2f, obstacles[i].h - 0.2f, COL_OBSTACLE.r * 0.8f, COL_OBSTACLE.g * 0.8f, COL_OBSTACLE.b * 0.8f);
    }

    for (int i = 0; i < numEnemies; i++)
        drawVillain(enemies[i]);
    drawStudent();
    drawHitMarker();

    glPopMatrix();
    drawUI();
    glutSwapBuffers();
}

void keyDown(unsigned char k, int x, int y)
{
    if (gameState == 1)
    {
        if (k == ' ' || k == 13)
            fullReset();
        return;
    }
    if (gameState == 2)
        return;
    if (k == ' ')
        keySpace = true;
    if (k == 'w')
    {
        if (!isAttacking)
        {
            isAttacking = true;
            attackFrame = 0;
        }
    }
    if (k == 'a')
        keyA = true;
    if (k == 'd')
        keyD = true;
    if (k == 's')
        keyS = true;
}

void keyUp(unsigned char k, int x, int y)
{
    if (k == ' ')
        keySpace = false;
    if (k == 'a')
        keyA = false;
    if (k == 'd')
        keyD = false;
    if (k == 's')
        keyS = false;
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("Student Detective: Unified Ninja Enemies");

    createDisplayLists();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMainLoop();
    return 0;
}