#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define PI 3.14159265f

// ==========================================
// 1. GAME STATE & LEVEL MANAGEMENT
// ==========================================
enum GameState
{
    STATE_MENU,
    STATE_LEVEL1,
    STATE_LEVEL2,
    STATE_LEVEL3,
    STATE_GAMEOVER,
    STATE_COMPLETE,
    STATE_LEVEL_COMPLETE
};

int currentLevel = 1;
GameState gameState = STATE_MENU;
float levelTransitionTimer = 0.0f;

int windowWidth = 1200;
int windowHeight = 700;

// ==========================================
// 2. COMMON CONFIGURATION
// ==========================================
const float LEVEL_START_X = -1.0f;
const float LEVEL_END_X = 14.0f;
const float PIT_DEPTH = -3.0f;

const float GRAVITY = 0.003f;
const float JUMP_FORCE = 0.065f;
const float MOVE_ACCEL = 0.003f;
const float MAX_SPEED = 0.035f;
const float FRICTION = 0.85f;
const float KNOCKBACK_X = 0.06f;
const float KNOCKBACK_Y = 0.04f;
const float KNOCKBACK_FORCE = 0.04f;

const float PLAYER_W = 0.021f;
const float PLAYER_H_STAND = 0.075f;
const float PLAYER_H_CROUCH = 0.049f;

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
float armAngle = 0.0f;
float swordAngle = 0.0f;
int attackFrame = 0;
float cloudTime = 0.0f;

bool keyA = false, keyD = false, keySpace = false, keyW = false, keyS = false;

// Mouse variables
int mouseX = 0, mouseY = 0;
bool mouseLeftDown = false;
bool mouseClicked = false;
bool mouseInLevel1Button = false;
bool mouseInLevel2Button = false;
bool mouseInLevel3Button = false;
bool mouseInStartButton = false;

// --- MENU LAYOUT CONSTANTS ---
const float LEVEL_BUTTON_X1 = 300.0f;
const float LEVEL_BUTTON_X2 = 500.0f;

const float START_BUTTON_X1 = 300.0f;
const float START_BUTTON_X2 = 500.0f;
const float START_BUTTON_Y1 = 430.0f;
const float START_BUTTON_Y2 = 470.0f;

const float LEVEL1_BUTTON_Y1 = 360.0f;
const float LEVEL1_BUTTON_Y2 = 400.0f;

const float LEVEL2_BUTTON_Y1 = 290.0f;
const float LEVEL2_BUTTON_Y2 = 330.0f;

const float LEVEL3_BUTTON_Y1 = 220.0f;
const float LEVEL3_BUTTON_Y2 = 260.0f;

// ==========================================
// 3. DATA STRUCTURES
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

// BOSS LASER (Level 3)
struct Projectile
{
    float x, y, velX, w, h;
    bool active;
};
Projectile bossLaser = {0, 0, 0, 0.4f, 0.05f, false};

struct Enemy
{
    float x, y, velX, startX, endX, speed;
    bool alive, facingRight, isBoss;
    int hp, attackCooldown;
    bool isAttacking;
    int attackFrame;
};

// Custom Random for Level 2
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

// Level 3 Boss Constants
const int BOSS_DAMAGE = 30;
const float BOSS_REACH = 0.6f;
const int BOSS_MAX_HP = 25;

// ==========================================
// 4. LEVEL 1 DATA
// ==========================================
RectObj groundSegments1[] = {
    {-1.0f, -0.6f, 3.0f, 0.1f},
    {2.5f, -0.8f, 2.0f, 0.1f},
    {5.0f, -0.6f, 1.5f, 0.1f},
    {7.0f, -0.6f, 7.0f, 0.1f}};
const int numGrounds1 = 4;

RectObj platforms1[] = {
    {0.8f, -0.2f, 0.6f, 0.05f},
    {3.0f, -0.3f, 0.4f, 0.05f},
    {5.5f, 0.0f, 0.6f, 0.05f},
    {8.0f, 0.2f, 0.8f, 0.05f},
    {10.0f, -0.2f, 0.5f, 0.05f}};
const int numPlatforms1 = 5;

RectObj lowBeams1[] = {
    {4.0f, -0.32f, 1.0f, 0.2f}};
const int numBeams1 = 1;

RectObj spikes1[] = {
    {2.5f, -0.6f, 0.4f, 0.12f},
    {6.0f, -0.6f, 0.6f, 0.12f}};
const int numSpikes1 = 2;

Enemy enemies1[] = {
    {1.0f, -0.2f, 0.0f, 0.8f, 1.4f, 0.003f, true, true, false, 2, 0, false, 0},
    {3.2f, -0.3f, 0.0f, 3.0f, 3.4f, 0.004f, true, false, false, 2, 0, false, 0},
    {6.0f, 0.0f, 0.0f, 5.5f, 6.1f, 0.003f, true, true, false, 2, 0, false, 0},
    {8.5f, -0.6f, 0.0f, 7.5f, 9.5f, 0.006f, true, false, false, 3, 0, false, 0},
    {13.0f, -0.6f, 0.0f, 12.0f, 13.8f, 0.005f, true, false, true, 8, 0, false, 0}};
const int numEnemies1 = 5;

float lampPosts1[] = {-0.5f, 4.0f, 7.5f, 10.0f, 13.0f};
const int numLamps1 = 5;

// ==========================================
// 5. LEVEL 2 DATA
// ==========================================
struct Color
{
    float r, g, b;
};

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

// Level 2 Geometry
RectObj groundSegments2[] = {
    {-5.0f, -1.2f, 12.0f, 0.6f},
    {9.5f, -1.2f, 15.5f, 0.6f}};
const int numGrounds2 = 2;

RectObj obstacles2[] = {
    {4.0f, -0.35f, 1.5f, 1.0f}};
const int numObstacles2 = 1;

RectObj platforms2[] = {
    {2.0f, -0.2f, 1.0f, 0.05f},
    {8.0f, -0.4f, 0.8f, 0.05f},
    {11.0f, -0.1f, 1.0f, 0.05f},
    {13.0f, -0.3f, 1.0f, 0.05f}};
const int numPlatforms2 = 4;

Enemy enemies2[] = {
    {2.5f, -0.6f, 0.0f, 1.5f, 3.5f, 0.003f, true, true, false, 2, 0, false, 0},
    {6.0f, -0.6f, 0.0f, 5.0f, 6.5f, 0.004f, true, false, false, 2, 0, false, 0},
    {10.5f, -0.6f, 0.0f, 10.0f, 12.0f, 0.003f, true, true, false, 2, 0, false, 0},
    {13.0f, -0.6f, 0.0f, 11.5f, 13.8f, 0.005f, true, false, true, 8, 0, false, 0}};
const int numEnemies2 = 4;

// Tree display lists for Level 2
GLuint listTreePine, listTreeDeciduous;

// ==========================================
// 6. LEVEL 3 DATA (MARS/ALIEN THEME)
// ==========================================
RectObj groundSegments3[] = {
    {-1.0f, -0.6f, 3.0f, 0.1f},
    {2.5f, -0.8f, 2.0f, 0.1f},
    {5.0f, -0.6f, 1.5f, 0.1f},
    {7.0f, -0.6f, 7.0f, 0.1f}};
const int numGrounds3 = 4;

RectObj platforms3[] = {
    {0.8f, -0.2f, 0.6f, 0.05f},
    {3.0f, -0.3f, 0.4f, 0.05f},
    {5.5f, 0.0f, 0.6f, 0.05f},
    {8.0f, 0.2f, 0.8f, 0.05f},
    {10.0f, -0.2f, 0.5f, 0.05f}};
const int numPlatforms3 = 5;

RectObj alienBeams3[] = {
    {2.0f, -0.32f, 1.0f, 0.2f}};
const int numBeams3 = 1;

RectObj mines3[] = {
    {3.5f, -0.7f, 0.4f, 0.12f},
    {6.0f, -0.5f, 0.4f, 0.12f},
    {8.5f, 0.25f, 0.3f, 0.3f}};
const int numMines3 = 3;

RectObj spikeTraps3[] = {
    {10.5f, -0.6f, 0.8f, 0.15f},
    {13.0f, -0.6f, 0.8f, 0.15f}};
const int numSpikes3 = 2;

Enemy enemies3[] = {
    {1.5f, -0.35f, 0.0f, 0.5f, 1.8f, 0.003f, true, true, false, 2, 0, false, 0},
    {1.0f, 0.0f, 0.0f, 0.8f, 1.4f, 0.002f, true, false, false, 2, 0, false, 0},
    {5.8f, 0.20f, 0.0f, 5.5f, 6.1f, 0.003f, true, true, false, 2, 0, false, 0},
    {8.4f, 0.40f, 0.0f, 8.0f, 8.8f, 0.004f, true, false, false, 3, 0, false, 0},
    {12.0f, -0.5f, 0.0f, 10.5f, 13.5f, 0.008f, true, false, true, BOSS_MAX_HP, 0, false, 0}};
const int numEnemies3 = 5;

// ==========================================
// 7. UTILITY FUNCTIONS
// ==========================================
bool checkOverlap(RectObj A, RectObj B)
{
    return (A.x < B.x + B.w && A.x + A.w > B.x &&
            A.y < B.y + B.h && A.y + A.h > B.y);
}

void setColor(Color c) { glColor3f(c.r, c.g, c.b); }

void drawRectBasic(float x, float y, float w, float h, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawCircleBasic(float cx, float cy, float r, int segments, float red = 1.0f, float green = 1.0f, float blue = 1.0f)
{
    glColor3f(red, green, blue);
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++)
    {
        float theta = 2.0f * PI * (float)i / (float)segments;
        glVertex2f(r * cosf(theta) + cx, r * sinf(theta) + cy);
    }
    glEnd();
}

void drawText(float x, float y, const char *string, void *font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    int len = (int)strlen(string);
    for (int i = 0; i < len; i++)
        glutBitmapCharacter(font, string[i]);
}

void drawTrapezoid(float x, float y, float topW, float botW, float h, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x - topW / 2, y + h);
    glVertex2f(x + topW / 2, y + h);
    glVertex2f(x + botW / 2, y);
    glVertex2f(x - botW / 2, y);
    glEnd();
}

// ==========================================
// 8. LEVEL 1 DRAWING FUNCTIONS
// ==========================================
void drawBrickLayer()
{
    float brickW = 0.08f;
    float brickH = 0.05f;
    float startY = -0.6f;

    drawRectBasic(-1.0f, -1.0f, 2.0f, startY + 1.0f, 0.12f, 0.05f, 0.05f);

    glBegin(GL_QUADS);
    bool stagger = false;
    for (float y = -1.0f; y < startY; y += (brickH + 0.005f))
    {
        float xStart = stagger ? -1.0f - (brickW / 2) : -1.0f;
        for (float x = xStart; x < 1.0f; x += (brickW + 0.005f))
        {
            if ((int)(x * 100) % 5 == 0)
                glColor3f(0.25f, 0.1f, 0.1f);
            else
                glColor3f(0.3f, 0.15f, 0.15f);
            glVertex2f(x, y);
            glVertex2f(x + brickW, y);
            glVertex2f(x + brickW, y + brickH);
            glVertex2f(x, y + brickH);
        }
        stagger = !stagger;
    }
    glEnd();
}

void drawIndustrialTank(float x)
{
    float width = 0.35f, height = 0.8f;
    glColor3f(0.02f, 0.02f, 0.02f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x, -1.0f);
    glVertex2f(x + 0.05f, -1.0f + height);
    glVertex2f(x + width, -1.0f);
    glVertex2f(x + width - 0.05f, -1.0f + height);
    glVertex2f(x, -1.0f);
    glVertex2f(x + width - 0.05f, -1.0f + height);
    glVertex2f(x + width, -1.0f);
    glVertex2f(x + 0.05f, -1.0f + height);
    glEnd();
    glLineWidth(1.0f);
    float tankY = -1.0f + height, tankH = 0.25f;
    drawRectBasic(x - 0.02f, tankY, width + 0.04f, tankH, 0.10f, 0.05f, 0.05f);
    glBegin(GL_QUADS);
    glColor3f(0.08f, 0.04f, 0.04f);
    glVertex2f(x - 0.02f, tankY + tankH);
    glVertex2f(x + width + 0.02f, tankY + tankH);
    glVertex2f(x + width - 0.1f, tankY + tankH + 0.1f);
    glVertex2f(x + 0.1f, tankY + tankH + 0.1f);
    glEnd();
}

void drawGothicSpire(float x)
{
    float w = 0.3f, baseH = 1.0f;
    drawRectBasic(x, -1.0f, w, baseH, 0.08f, 0.08f, 0.12f);
    glColor3f(0.02f, 0.02f, 0.05f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (float i = x + 0.05f; i < x + w; i += 0.05f)
    {
        glVertex2f(i, -1.0f);
        glVertex2f(i, -1.0f + baseH + 0.2f);
    }
    glEnd();
    glBegin(GL_TRIANGLES);
    glColor3f(0.05f, 0.05f, 0.08f);
    glVertex2f(x - 0.05f, -1.0f + baseH);
    glVertex2f(x + w + 0.05f, -1.0f + baseH);
    glVertex2f(x + w / 2, -1.0f + baseH + 0.8f);
    glEnd();
    glColor3f(0.9f, 0.8f, 0.5f);
    drawCircleBasic(x + w / 2, -1.0f + baseH + 0.2f, 0.04f, 8);
}

void drawSkyscraper(float x, float w, float h)
{
    drawRectBasic(x, -1.0f, w, h, 0.05f, 0.05f, 0.1f);
    float winSize = 0.01f;
    glColor3f(0.6f, 0.5f, 0.3f);
    glBegin(GL_QUADS);
    for (float wy = -0.9f; wy < (-1.0f + h - 0.05f); wy += 0.04f)
    {
        for (float wx = x + 0.02f; wx < (x + w - 0.02f); wx += 0.03f)
        {
            int hash = (int)(wx * 900) + (int)(wy * 900);
            if ((hash % 100) < 40)
            {
                glVertex2f(wx, wy);
                glVertex2f(wx + winSize, wy);
                glVertex2f(wx + winSize, wy + winSize);
                glVertex2f(wx, wy + winSize);
            }
        }
    }
    glEnd();
}

void drawCloud(float x, float y, float scale)
{
    glColor4f(0.15f, 0.15f, 0.20f, 0.6f);
    drawCircleBasic(x, y, scale, 20);
    drawCircleBasic(x + scale * 0.8f, y - scale * 0.2f, scale * 0.7f, 20);
    drawCircleBasic(x - scale * 0.8f, y - scale * 0.2f, scale * 0.7f, 20);
}

void drawCloudLayer()
{
    drawCloud(-0.8f, 0.7f, 0.15f);
    drawCloud(-0.2f, 0.85f, 0.18f);
    drawCloud(0.4f, 0.65f, 0.14f);
    drawCloud(0.9f, 0.8f, 0.20f);
}

void drawFog()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(-1.0f, 0.2f);
    glVertex2f(1.0f, 0.2f);
    glColor4f(0.05f, 0.05f, 0.08f, 0.8f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();
    glDisable(GL_BLEND);
}

void drawFarCityChunk()
{
    drawSkyscraper(-1.1f, 0.25f, 1.4f);
    drawIndustrialTank(-0.7f);
    drawSkyscraper(-0.2f, 0.35f, 0.9f);
    drawGothicSpire(0.3f);
    drawSkyscraper(0.7f, 0.25f, 1.3f);
    drawSkyscraper(1.0f, 0.20f, 1.5f);
}

void drawPillar(float x)
{
    float w = 0.25f;
    drawRectBasic(x, -1.0f, w, 2.0f, 0.15f, 0.15f, 0.18f);
    glColor3f(0.08f, 0.08f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (float lx = x + 0.02f; lx < x + w; lx += 0.03f)
    {
        glVertex2f(lx, -1.0f);
        glVertex2f(lx, 1.0f);
    }
    glEnd();
    glLineWidth(1.0f);
    drawRectBasic(x - 0.02f, -1.0f, w + 0.04f, 0.4f, 0.12f, 0.12f, 0.15f);
}

void drawLitRect(float x, float y, float w, float h, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawLampPost(float x, float y)
{
    drawLitRect(x - 0.02f, y, 0.04f, 0.8f, 0.1f, 0.1f, 0.1f);
    drawLitRect(x - 0.1f, y + 0.7f, 0.2f, 0.03f, 0.1f, 0.1f, 0.1f);
    float emission[] = {1.0f, 0.9f, 0.4f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glColor3f(1.0f, 1.0f, 0.5f);
    glBegin(GL_POLYGON);
    glNormal3f(0, 0, 1);
    for (int i = 0; i < 10; i++)
    {
        float t = 2 * PI * i / 10;
        glVertex2f(x - 0.075f + 0.04f * cos(t), y + 0.64f + 0.04f * sin(t));
    }
    glEnd();
    glBegin(GL_POLYGON);
    glNormal3f(0, 0, 1);
    for (int i = 0; i < 10; i++)
    {
        float t = 2 * PI * i / 10;
        glVertex2f(x + 0.075f + 0.04f * cos(t), y + 0.64f + 0.04f * sin(t));
    }
    glEnd();
    float noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
}

void drawObstacles1()
{
    for (int i = 0; i < numBeams1; i++)
    {
        drawLitRect(lowBeams1[i].x, lowBeams1[i].y, lowBeams1[i].w, lowBeams1[i].h, 0.2f, 0.2f, 0.22f);
        drawLitRect(lowBeams1[i].x, lowBeams1[i].y + 0.02f, lowBeams1[i].w, 0.02f, 0.3f, 0.3f, 0.35f);
        drawLitRect(lowBeams1[i].x, lowBeams1[i].y + lowBeams1[i].h - 0.04f, lowBeams1[i].w, 0.02f, 0.1f, 0.1f, 0.12f);
    }

    for (int i = 0; i < numSpikes1; i++)
    {
        glColor3f(0.5f, 0.1f, 0.1f);
        glBegin(GL_TRIANGLES);
        glNormal3f(0, 0, 1);
        float sw = spikes1[i].w;
        float sx = spikes1[i].x;
        float sy = spikes1[i].y;
        float sh = spikes1[i].h;
        int numTeeth = (int)(sw * 15);
        float toothW = sw / numTeeth;
        for (int t = 0; t < numTeeth; t++)
        {
            glVertex2f(sx + t * toothW, sy);
            glVertex2f(sx + (t + 1) * toothW, sy);
            glVertex2f(sx + t * toothW + toothW / 2, sy + sh);
        }
        glEnd();
    }
}

void setupLighting1()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat light0_amb[] = {0.15f, 0.15f, 0.25f, 1.0f};
    GLfloat light0_dif[] = {0.5f, 0.5f, 0.6f, 1.0f};
    GLfloat light0_spec[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat light0_pos[] = {0.0f, 10.0f, 5.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_spec);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glEnable(GL_LIGHT0);

    GLfloat lamp_amb[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat lamp_dif[] = {0.9f, 0.7f, 0.2f, 1.0f};
    GLfloat lamp_spec[] = {1.0f, 1.0f, 1.0f, 1.0f};

    for (int i = 0; i < numLamps1; i++)
    {
        GLenum lid = GL_LIGHT1 + i;
        GLfloat lpos[] = {lampPosts1[i], 0.6f, 0.5f, 1.0f};
        glLightfv(lid, GL_AMBIENT, lamp_amb);
        glLightfv(lid, GL_DIFFUSE, lamp_dif);
        glLightfv(lid, GL_SPECULAR, lamp_spec);
        glLightfv(lid, GL_POSITION, lpos);
        glLightf(lid, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(lid, GL_LINEAR_ATTENUATION, 0.1f);
        glLightf(lid, GL_QUADRATIC_ATTENUATION, 0.05f);
        glEnable(lid);
    }
}

// ==========================================
// 9. LEVEL 2 DRAWING FUNCTIONS
// ==========================================
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

void drawClouds2()
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
    float colTreePineLight[3] = {0.15f, 0.55f, 0.25f};
    float colTreePineDark[3] = {0.05f, 0.35f, 0.15f};
    float colTreeDecLight[3] = {0.30f, 0.70f, 0.35f};
    float colTreeDecDark[3] = {0.15f, 0.45f, 0.20f};
    float colTreeTrunk[3] = {0.35f, 0.25f, 0.15f};

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

void drawNinjaEnemy(Enemy e)
{
    float s = e.isBoss ? 0.13f : 0.08f;
    Color headbandColor = e.isBoss ? (Color){0.9f, 0.1f, 0.1f} : (Color){0.2f, 0.2f, 0.8f};

    bool isMoving = (fabs(e.velX) > 0.001f);
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.01f;

    if (!e.isBoss)
        time *= 1.5f;

    float legAngle = 0.0f;
    float headTilt = 0.0f;
    float footRotation = 0.0f;
    float armAngleRight = 0.0f;
    float armAngleLeft = 0.0f;
    float bodyYOffset = 0.0f;

    if (e.isAttacking)
    {
        float progress = (float)e.attackFrame;
        if (progress < 5)
            armAngleRight = 120.0f + (progress * 10.0f);
        else if (progress < 12)
            armAngleRight = 170.0f - ((progress - 5) * 25.0f);
        else
            armAngleRight = 0.0f;

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

    glPushMatrix();
    glTranslatef(0, -s * 1.5f + bodyYOffset, 0);

    glColor3f(0.05f, 0.05f, 0.08f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.6f, s * 0.1f);
    glVertex2f(s * 0.6f, s * 0.1f);
    glVertex2f(s * 0.4f, -s * 1.8f);
    glVertex2f(-s * 0.4f, -s * 1.8f);
    glEnd();

    glColor3f(headbandColor.r, headbandColor.g, headbandColor.b);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.7f, -s * 1.0f);
    glVertex2f(s * 0.7f, -s * 1.0f);
    glVertex2f(s * 0.7f, -s * 1.2f);
    glVertex2f(-s * 0.7f, -s * 1.2f);
    glEnd();

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

    glColor3f(0.08f, 0.08f, 0.12f);
    float hipY = -s * 2.2f + bodyYOffset;

    float leftLegRot = isMoving ? legSwing * 30.0f : 0.0f;
    drawBossLimb(-s * 0.3f, hipY, -s * 0.3f - sin(legRad * 0.5f) * s * 0.5f, hipY - s * 2.0f, s * 0.15f, false, leftLegRot + footRotation);

    float rightLegRot = isMoving ? -legSwing * 30.0f : 0.0f;
    drawBossLimb(s * 0.3f, hipY, s * 0.3f + sin(legRad * 0.5f + PI) * s * 0.5f, hipY - s * 2.0f, s * 0.15f, false, rightLegRot - footRotation);

    float shoulderY = -s * 1.6f + bodyYOffset;
    float armSwingRight = sin(armAngleRight * PI / 180.0f) * 0.3f;
    float armSwingLeft = sin(armAngleLeft * PI / 180.0f) * 0.3f;

    glColor3f(0.05f, 0.05f, 0.08f);

    drawBossLimb(s * 0.6f, shoulderY, s * 1.2f, shoulderY - s * 1.5f - armSwingLeft * s, s * 0.12f, true, armAngleLeft);

    float handX = -s * 1.2f;
    float handY = shoulderY - s * 1.5f + armSwingRight * s;

    if (e.isAttacking)
    {
        handX = -s * 1.0f * cos(armAngleRight * PI / 180.0f);
        handY = shoulderY + s * 1.0f * sin(armAngleRight * PI / 180.0f);
    }

    drawBossLimb(-s * 0.6f, shoulderY, handX, handY, s * 0.12f, true, armAngleRight);

    float swordRot = -90.0f + armAngleRight;
    drawBossKatana(handX, handY, s * 2.5f, swordRot);

    glPushMatrix();
    glTranslatef(0.0f, -s * 1.4f + bodyYOffset, 0.0f);
    glRotatef(headTilt, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, s * 0.6f, 0.0f);

    glColor3f(0.03f, 0.03f, 0.06f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, s * 0.5f);
    for (int i = 0; i <= 8; i++)
    {
        float angle = PI * 0.75f + i * PI / 4.0f;
        glVertex2f(cos(angle) * s * 0.9f, sin(angle) * s * 0.9f);
    }
    glEnd();

    glColor3f(0.95f, 0.9f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.5f, s * 0.2f);
    glVertex2f(s * 0.5f, s * 0.2f);
    glVertex2f(s * 0.5f, -s * 0.2f);
    glVertex2f(-s * 0.5f, -s * 0.2f);
    glEnd();

    glColor3f(0.03f, 0.03f, 0.06f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.55f, -s * 0.05f);
    glVertex2f(s * 0.55f, -s * 0.05f);
    glVertex2f(s * 0.55f, -s * 0.25f);
    glVertex2f(-s * 0.55f, -s * 0.25f);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-s * 0.25f, s * 0.02f, s * 0.08f, 10);
    drawCircle(s * 0.25f, s * 0.02f, s * 0.08f, 10);

    glColor3f(headbandColor.r, headbandColor.g, headbandColor.b);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-s * 0.85f, s * 0.3f);
    glVertex2f(s * 0.85f, s * 0.3f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(-s * 0.8f, s * 0.3f);
    glVertex2f(-s * 1.5f, s * 0.6f);
    glVertex2f(-s * 0.9f, s * 0.4f);
    glEnd();

    glPopMatrix();
}

// ==========================================
// 10. LEVEL 3 DRAWING FUNCTIONS
// ==========================================
void drawAlienGround(RectObj r)
{
    drawRectBasic(r.x, r.y, r.w, r.h, 0.2f, 0.1f, 0.3f);

    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(r.x, r.y + r.h - 0.02f);
    glVertex2f(r.x + r.w, r.y + r.h - 0.02f);
    glVertex2f(r.x + r.w, r.y + r.h);
    glVertex2f(r.x, r.y + r.h);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawAlienSpikes()
{
    for (int i = 0; i < numSpikes3; i++)
    {
        float sx = spikeTraps3[i].x;
        float sy = spikeTraps3[i].y;
        float sw = spikeTraps3[i].w;
        float sh = spikeTraps3[i].h;

        glDisable(GL_LIGHTING);
        for (float k = 0; k < sw; k += 0.08f)
        {
            float pulse = sin(walkTimer * 0.2f + k * 10.0f) * 0.02f;

            glColor4f(0.0f, 1.0f, 1.0f, 0.9f);
            glBegin(GL_TRIANGLES);
            glVertex2f(sx + k, sy);
            glVertex2f(sx + k + 0.06f, sy);
            glVertex2f(sx + k + 0.03f, sy + sh + pulse);
            glEnd();

            glColor4f(0.8f, 1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
            glVertex2f(sx + k + 0.03f, sy);
            glVertex2f(sx + k + 0.03f, sy + sh + pulse);
            glEnd();
        }
        glEnable(GL_LIGHTING);
    }
}

void drawLaser()
{
    if (!bossLaser.active)
        return;
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(bossLaser.x, bossLaser.y);
    glVertex2f(bossLaser.x + bossLaser.w, bossLaser.y);
    glVertex2f(bossLaser.x + bossLaser.w, bossLaser.y + bossLaser.h);
    glVertex2f(bossLaser.x, bossLaser.y + bossLaser.h);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0f, 0.5f, 0.0f, 0.5f);
    float glow = 0.05f;
    glBegin(GL_QUADS);
    glVertex2f(bossLaser.x - glow, bossLaser.y - glow);
    glVertex2f(bossLaser.x + bossLaser.w + glow, bossLaser.y - glow);
    glVertex2f(bossLaser.x + bossLaser.w + glow, bossLaser.y + bossLaser.h + glow);
    glVertex2f(bossLaser.x - glow, bossLaser.y + bossLaser.h + glow);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawAlienSun(float cameraX)
{
    float sunX = -0.6f;
    float sunY = 0.7f;
    float sunRadius = 0.15f;
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(cameraX * 0.9f, 0, 0);
    glTranslatef(sunX, sunY, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (int i = 0; i < 10; i++)
    {
        float s = 1.0f + (i * 0.1f);
        glColor4f(1.0f, 0.5f, 0.0f, 0.05f);
        glPushMatrix();
        glScalef(s, s, 1);
        glutSolidSphere(sunRadius, 30, 30);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
    glColor3f(1.0f, 0.9f, 0.6f);
    glutSolidSphere(sunRadius, 30, 30);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawStars(float scrollOffset)
{
    glDisable(GL_LIGHTING);
    glPointSize(1.5);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    float stars[30][2] = {
        {-0.9, 0.8}, {-0.8, 0.4}, {-0.7, 0.6}, {-0.6, 0.9}, {-0.5, 0.2}, {-0.4, 0.7}, {-0.3, 0.5}, {-0.2, 0.8}, {-0.1, 0.3}, {0.0, 0.6}, {0.1, 0.9}, {0.2, 0.4}, {0.3, 0.7}, {0.4, 0.2}, {0.5, 0.8}, {0.6, 0.5}, {0.7, 0.9}, {0.8, 0.3}, {0.9, 0.6}, {1.0, 0.8}, {-0.95, 0.55}, {-0.75, 0.25}, {-0.45, 0.95}, {-0.15, 0.15}, {0.15, 0.85}, {0.35, 0.35}, {0.55, 0.65}, {0.75, 0.15}, {0.85, 0.55}, {0.95, 0.95}};
    for (int i = 0; i < 30; i++)
    {
        float x = stars[i][0] + scrollOffset;
        while (x < -1.0f)
            x += 2.0f;
        while (x > 1.0f)
            x -= 2.0f;
        glVertex2f(x, stars[i][1]);
    }
    glEnd();
}

void drawMarsHills(float scrollFactor, float r, float g, float b, float yOffset)
{
    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f, -1.0f);
    for (float x = -1.0f; x <= 1.0f; x += 0.1f)
    {
        float h = sinf((x + scrollFactor) * 3.0f) * 0.15f + sinf((x + scrollFactor) * 7.5f) * 0.05f;
        glVertex2f(x, yOffset + h);
    }
    glVertex2f(1.0f, -1.0f);
    glEnd();
}

void drawAlienObstacles()
{
    for (int i = 0; i < numBeams3; i++)
    {
        drawRectBasic(alienBeams3[i].x, alienBeams3[i].y, alienBeams3[i].w, alienBeams3[i].h, 0.2f, 0.0f, 0.3f);
        glDisable(GL_LIGHTING);
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(alienBeams3[i].x, alienBeams3[i].y + 0.1f);
        glVertex2f(alienBeams3[i].x + alienBeams3[i].w, alienBeams3[i].y + 0.1f);
        glEnd();
        glEnable(GL_LIGHTING);
    }
    for (int i = 0; i < numMines3; i++)
    {
        glPushMatrix();
        glTranslatef(mines3[i].x + mines3[i].w / 2, mines3[i].y + mines3[i].h / 2, 0);
        float pulse = 1.0f + 0.2f * sin(walkTimer * 0.5f);
        glScalef(pulse, pulse, 1);
        glColor3f(1.0f, 0.2f, 0.0f);
        glutSolidSphere(0.1f, 10, 10);
        glColor3f(0.5f, 0.5f, 0.5f);
        for (int j = 0; j < 8; j++)
        {
            glPushMatrix();
            glRotatef(j * 45 + walkTimer * 2.0f, 0, 0, 1);
            glTranslatef(0.1f, 0, 0);
            glScalef(2, 0.5, 0.5);
            glutSolidCube(0.05f);
            glPopMatrix();
        }
        glPopMatrix();
    }
}

void drawWalkerBot(bool isAttacking, float walkTime)
{
    glColor3f(0.2f, 0.8f, 0.2f);
    glutSolidSphere(0.08f, 15, 15);
    glPushMatrix();
    glTranslatef(0.05f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.03f, 10, 10);
    glPopMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.08f);
    glVertex2f(0.0f, 0.15f);
    glEnd();
    glPushMatrix();
    glTranslatef(0.0f, -0.1f, 0.0f);
    glColor3f(0.3f, 0.3f, 0.35f);
    glScalef(1.0f, 1.5f, 1.0f);
    glutSolidCube(0.08f);
    glPopMatrix();
    float handBob = sin(walkTime * 10.0f) * 0.02f;
    glPushMatrix();
    glTranslatef(0.1f, -0.1f + handBob, 0.05f);
    if (isAttacking)
    {
        glColor3f(1.0f, 0.0f, 1.0f);
        glScalef(1.0f, 0.1f, 0.1f);
        glutSolidCube(0.2f);
    }
    else
    {
        glColor3f(0.2f, 0.2f, 0.2f);
        glutSolidSphere(0.03f, 8, 8);
    }
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.05f, -0.1f - handBob, -0.05f);
    glColor3f(0.2f, 0.2f, 0.2f);
    glutSolidSphere(0.03f, 8, 8);
    glPopMatrix();
    float legSwing = sin(walkTime * 8.0f) * 20.0f;
    glPushMatrix();
    glTranslatef(0.0f, -0.15f, 0.05f);
    glRotatef(legSwing, 0, 0, 1);
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(0, -0.15f);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, -0.15f, -0.05f);
    glRotatef(-legSwing, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(0, -0.15f);
    glEnd();
    glPopMatrix();
}

void drawAlienEnemy(Enemy e)
{
    if (!e.alive)
        return;
    glPushMatrix();
    glTranslatef(e.x, e.y, 0.0f);
    if (e.velX > 0.01f || e.velX < -0.01f)
        glColor3f(1.0f, 0.5f, 0.5f);
    else
        glColor3f(1, 1, 1);
    if (!e.facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);

    if (e.isBoss)
    {
        glScalef(1.5f, 1.5f, 1.0f);

        drawRectBasic(-0.08f, 0.0f, 0.06f, 0.18f, 0.1f, 0.1f, 0.1f);
        drawRectBasic(0.02f, 0.0f, 0.06f, 0.18f, 0.1f, 0.1f, 0.1f);
        drawRectBasic(-0.1f, 0.18f, 0.2f, 0.25f, 0.05f, 0.05f, 0.05f);

        float pulse = 0.5f + 0.5f * sin(walkTimer * 0.5f);
        drawRectBasic(-0.03f, 0.28f, 0.06f, 0.06f, 1.0f * pulse, 0.0f, 0.0f);

        glDisable(GL_LIGHTING);
        glBegin(GL_TRIANGLES);
        glColor3f(0.8f, 0.0f, 0.0f);
        glVertex2f(-0.1f, 0.4f);
        glVertex2f(-0.2f, 0.5f);
        glVertex2f(-0.12f, 0.35f);
        glVertex2f(0.1f, 0.4f);
        glVertex2f(0.2f, 0.5f);
        glVertex2f(0.12f, 0.35f);
        glEnd();
        glEnable(GL_LIGHTING);

        glPushMatrix();
        glTranslatef(0.0f, 0.48f, 0.0f);
        drawRectBasic(-0.06f, 0.0f, 0.12f, 0.1f, 0.1f, 0.1f, 0.1f);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawRectBasic(-0.04f, 0.04f, 0.03f, 0.01f, 1, 0, 0);
        drawRectBasic(0.01f, 0.04f, 0.03f, 0.01f, 1, 0, 0);
        glEnable(GL_LIGHTING);
        glPopMatrix();

        drawRectBasic(-0.1f, 0.65f, 0.2f, 0.02f, 0.5f, 0.0f, 0.0f);
        drawRectBasic(-0.1f, 0.65f, 0.2f * ((float)e.hp / (float)BOSS_MAX_HP), 0.02f, 1.0f, 0.0f, 0.0f);

        glPushMatrix();
        glTranslatef(0.0f, 0.15f, 0.0f);
        float ang = -45.0f;
        if (e.isAttacking)
            ang = (e.attackFrame < 10) ? -45.0f + (e.attackFrame * 13.0f) : 85.0f - ((e.attackFrame - 10) * 10.0f);
        glRotatef(ang, 0.0f, 0.0f, 1.0f);
        drawRectBasic(0.0f, 0.0f, 0.02f, 0.35f, 1.0f, 0.0f, 1.0f);
        glPopMatrix();
    }
    else
    {
        drawWalkerBot(e.isAttacking, walkTimer);
    }
    glPopMatrix();
}

void setupLighting3(float camX)
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    GLfloat sun_amb[] = {0.2f, 0.1f, 0.1f, 1.0f};
    GLfloat sun_dif[] = {1.0f, 0.8f, 0.6f, 1.0f};
    GLfloat sun_spec[] = {1.0f, 1.0f, 0.8f, 1.0f};
    GLfloat sun_pos[] = {-0.6f, 0.7f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_spec);
    glLightfv(GL_LIGHT0, GL_POSITION, sun_pos);
    glEnable(GL_LIGHT0);
}

// ==========================================
// 11. COMMON CHARACTER DRAWING
// ==========================================
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

void drawDetective()
{
    if (playerInvincibleTimer > 0 && (playerInvincibleTimer / 5) % 2 == 0)
        return;
    if (gameState == STATE_LEVEL_COMPLETE)
    {
        if (attackFrame < 20)
            attackFrame++;
        walkTimer += 0.2f;
    }

    glPushMatrix();
    glTranslatef(pX, pY, 0.0f);

    glScalef(0.5f, 0.5f, 1.0f);

    if (!facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);
    if (isCrouching)
        glScalef(1.0f, 0.6f, 1.0f);

    float bob = (fabs(pVelX) > 0.001f && !isJumping) ? sin(walkTimer) * 0.01f : 0.0f;

    float skinR = 0.96f, skinG = 0.80f, skinB = 0.69f;

    // --- 1. BACK ARM ---
    float backArmSwing = sin(walkTimer) * 15.0f;
    glPushMatrix();
    glTranslatef(0.02f, 0.21f + bob, 0.0f);
    glRotatef(-backArmSwing, 0.0f, 0.0f, 1.0f);
    if (currentLevel == 1)
        drawLitRect(-0.01f, -0.09f, 0.03f, 0.10f, 0.65f, 0.50f, 0.35f);
    else
        drawRectBasic(-0.01f, -0.09f, 0.03f, 0.10f, 0.65f, 0.50f, 0.35f);
    drawCircleBasic(0.005f, -0.10f, 0.012f, 10, skinR - 0.1f, skinG - 0.1f, skinB - 0.1f);
    glPopMatrix();

    // --- 2. LEGS & SHOES ---
    if (currentLevel == 1)
        drawLitRect(-0.035f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);
    else
        drawRectBasic(-0.035f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);

    if (currentLevel == 1)
        drawLitRect(0.005f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);
    else
        drawRectBasic(0.005f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);

    if (currentLevel == 1)
    {
        drawLitRect(-0.04f, 0.0f, 0.04f, 0.02f, 0.05f, 0.05f, 0.05f);
        drawTrapezoid(-0.02f, 0.02f, 0.03f, 0.045f, 0.015f, 0.05f, 0.05f, 0.05f);
    }
    else
    {
        drawRectBasic(-0.04f, 0.0f, 0.04f, 0.02f, 0.05f, 0.05f, 0.05f);
        drawTrapezoid(-0.02f, 0.02f, 0.03f, 0.045f, 0.015f, 0.05f, 0.05f, 0.05f);
    }
    if (currentLevel == 1)
    {
        drawLitRect(0.0f, 0.0f, 0.04f, 0.02f, 0.05f, 0.05f, 0.05f);
        drawTrapezoid(0.02f, 0.02f, 0.03f, 0.045f, 0.015f, 0.05f, 0.05f, 0.05f);
    }
    else
    {
        drawRectBasic(0.0f, 0.0f, 0.04f, 0.02f, 0.05f, 0.05f, 0.05f);
        drawTrapezoid(0.02f, 0.02f, 0.03f, 0.045f, 0.015f, 0.05f, 0.05f, 0.05f);
    }

    // --- 3. TORSO ---
    if (currentLevel == 1)
    {
        drawLitRect(-0.04f, 0.13f + bob, 0.08f, 0.15f, 0.9f, 0.9f, 0.9f);
        drawLitRect(-0.005f, 0.18f + bob, 0.015f, 0.08f, 0.6f, 0.1f, 0.1f);
    }
    else
    {
        drawRectBasic(-0.04f, 0.13f + bob, 0.08f, 0.15f, 0.9f, 0.9f, 0.9f);
        drawRectBasic(-0.005f, 0.18f + bob, 0.015f, 0.08f, 0.6f, 0.1f, 0.1f);
    }

    drawTrapezoid(0.0f, 0.10f + bob, 0.10f, 0.14f, 0.19f, 0.76f, 0.60f, 0.42f);

    if (currentLevel == 1)
        drawLitRect(-0.045f, 0.16f + bob, 0.09f, 0.025f, 0.56f, 0.40f, 0.22f);
    else
        drawRectBasic(-0.045f, 0.16f + bob, 0.09f, 0.025f, 0.56f, 0.40f, 0.22f);

    glColor3f(0.3f, 0.2f, 0.1f);
    drawCircleBasic(-0.02f, 0.14f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircleBasic(-0.02f, 0.20f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircleBasic(0.02f, 0.14f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircleBasic(0.02f, 0.20f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);

    glColor3f(0.66f, 0.50f, 0.32f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.015f, 0.25f + bob);
    glVertex2f(-0.055f, 0.28f + bob);
    glVertex2f(-0.05f, 0.23f + bob);
    glVertex2f(0.015f, 0.25f + bob);
    glVertex2f(0.055f, 0.28f + bob);
    glVertex2f(0.05f, 0.23f + bob);
    glEnd();

    // --- 4. HEAD ---
    float headY = 0.29f + bob;

    if (currentLevel == 1)
        drawLitRect(-0.018f, 0.27f + bob, 0.036f, 0.04f, skinR, skinG, skinB);
    else
        drawRectBasic(-0.018f, 0.27f + bob, 0.036f, 0.04f, skinR, skinG, skinB);

    if (currentLevel == 1)
        drawLitRect(-0.038f, headY, 0.076f, 0.08f, skinR, skinG, skinB);
    else
        drawRectBasic(-0.038f, headY, 0.076f, 0.08f, skinR, skinG, skinB);

    drawCircleBasic(-0.038f, headY + 0.04f, 0.008f, 6, skinR, skinG, skinB);
    drawCircleBasic(0.038f, headY + 0.04f, 0.008f, 6, skinR - 0.1f, skinG - 0.1f, skinB - 0.1f);

    float eyeY = headY + 0.045f;
    drawCircleBasic(-0.018f, eyeY, 0.011f, 10, 1.0f, 1.0f, 1.0f);
    drawCircleBasic(0.018f, eyeY, 0.011f, 10, 1.0f, 1.0f, 1.0f);
    drawCircleBasic(-0.016f, eyeY, 0.005f, 8, 0.2f, 0.6f, 0.8f);
    drawCircleBasic(0.020f, eyeY, 0.005f, 8, 0.2f, 0.6f, 0.8f);

    glColor3f(skinR - 0.2f, skinG - 0.2f, skinB - 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, headY + 0.035f);
    glVertex2f(-0.005f, headY + 0.02f);
    glVertex2f(0.005f, headY + 0.02f);
    glEnd();

    float mouthY = headY + 0.012f;
    glLineWidth(2.0f);
    glColor3f(0.4f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(-0.01f, mouthY);
    glVertex2f(0.01f, mouthY - 0.002f);
    glEnd();

    if (currentLevel == 1)
    {
        drawLitRect(0.005f, mouthY - 0.003f, 0.02f, 0.003f, 1.0f, 1.0f, 1.0f);
        drawLitRect(0.025f, mouthY - 0.003f, 0.005f, 0.003f, 1.0f, 0.5f, 0.0f);
    }
    else
    {
        drawRectBasic(0.005f, mouthY - 0.003f, 0.02f, 0.003f, 1.0f, 1.0f, 1.0f);
        drawRectBasic(0.025f, mouthY - 0.003f, 0.005f, 0.003f, 1.0f, 0.5f, 0.0f);
    }

    if ((int)(walkTimer * 5) % 2 == 0)
    {
        glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
        drawCircleBasic(0.035f, mouthY + 0.01f + (float)(rand() % 10) / 500.0f, 0.004f, 6, 0.9f, 0.9f, 0.9f);
    }

    if (currentLevel == 1)
    {
        drawLitRect(-0.07f, headY + 0.06f, 0.14f, 0.01f, 0.35f, 0.25f, 0.2f);
        drawTrapezoid(0.0f, headY + 0.065f, 0.07f, 0.09f, 0.05f, 0.35f, 0.25f, 0.2f);
        drawLitRect(-0.045f, headY + 0.07f, 0.09f, 0.012f, 0.1f, 0.1f, 0.1f);
    }
    else
    {
        drawRectBasic(-0.07f, headY + 0.06f, 0.14f, 0.01f, 0.35f, 0.25f, 0.2f);
        drawTrapezoid(0.0f, headY + 0.065f, 0.07f, 0.09f, 0.05f, 0.35f, 0.25f, 0.2f);
        drawRectBasic(-0.045f, headY + 0.07f, 0.09f, 0.012f, 0.1f, 0.1f, 0.1f);
    }

    // --- 5. FOREARM & SWORD ---
    glPushMatrix();
    glTranslatef(0.0f, 0.22f + bob, 0.0f);
    glRotatef(armAngle, 0.0f, 0.0f, 1.0f);

    if (currentLevel == 1)
        drawLitRect(-0.02f, -0.09f, 0.04f, 0.11f, 0.70f, 0.55f, 0.38f);
    else
        drawRectBasic(-0.02f, -0.09f, 0.04f, 0.11f, 0.70f, 0.55f, 0.38f);

    drawCircleBasic(0.0f, -0.10f, 0.018f, 10, skinR, skinG, skinB);

    glPushMatrix();
    glTranslatef(0.0f, -0.10f, 0.0f);
    glRotatef(swordAngle + 15.0f, 0.0f, 0.0f, 1.0f);

    if (currentLevel == 1)
    {
        drawLitRect(-0.007f, -0.04f, 0.014f, 0.08f, 0.3f, 0.1f, 0.0f);
        drawLitRect(-0.035f, 0.04f, 0.07f, 0.015f, 0.8f, 0.7f, 0.1f);
        drawLitRect(-0.01f, -0.05f, 0.02f, 0.02f, 0.8f, 0.7f, 0.1f);
    }
    else
    {
        drawRectBasic(-0.007f, -0.04f, 0.014f, 0.08f, 0.3f, 0.1f, 0.0f);
        drawRectBasic(-0.035f, 0.04f, 0.07f, 0.015f, 0.8f, 0.7f, 0.1f);
        drawRectBasic(-0.01f, -0.05f, 0.02f, 0.02f, 0.8f, 0.7f, 0.1f);
    }

    if (currentLevel == 1)
    {
        drawLitRect(-0.012f, 0.055f, 0.024f, 0.30f, 0.85f, 0.85f, 0.9f);
        drawLitRect(-0.003f, 0.055f, 0.006f, 0.28f, 0.95f, 0.95f, 1.0f);
    }
    else
    {
        drawRectBasic(-0.012f, 0.055f, 0.024f, 0.30f, 0.85f, 0.85f, 0.9f);
        drawRectBasic(-0.003f, 0.055f, 0.006f, 0.28f, 0.95f, 0.95f, 1.0f);
    }

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.012f, 0.355f);
    glVertex2f(0.012f, 0.355f);
    glVertex2f(0.0f, 0.39f);
    glEnd();

    if (isAttacking && attackFrame > 4 && attackFrame < 12)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.8f, 0.9f, 1.0f, 0.4f);
        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.2f, 0.35f);
        glVertex2f(0.4f, 0.1f);
        glEnd();
        glDisable(GL_BLEND);
    }
    glPopMatrix();

    drawCircleBasic(0.008f, -0.095f, 0.008f, 8, skinR - 0.05f, skinG - 0.05f, skinB - 0.05f);

    glPopMatrix();
    glPopMatrix();
}

void drawSpaceHero()
{
    if (playerInvincibleTimer > 0 && (playerInvincibleTimer / 5) % 2 == 0)
        return;

    glPushMatrix();
    glTranslatef(pX, pY, 0.0f);

    glScalef(0.5f, 0.5f, 1.0f);

    if (!facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);
    if (isCrouching)
        glScalef(1.0f, 0.6f, 1.0f);

    float bob = (fabs(pVelX) > 0.001f && !isJumping) ? sin(walkTimer) * 0.01f : 0.0f;

    // --- 1. OXYGEN TANK (Backpack) ---
    drawRectBasic(-0.06f, 0.15f + bob, 0.03f, 0.12f, 0.6f, 0.6f, 0.65f);

    // --- 2. BACK ARM ---
    float backArmSwing = sin(walkTimer) * 15.0f;
    glPushMatrix();
    glTranslatef(0.02f, 0.21f + bob, 0.0f);
    glRotatef(-backArmSwing, 0.0f, 0.0f, 1.0f);
    drawRectBasic(-0.01f, -0.09f, 0.035f, 0.10f, 0.8f, 0.8f, 0.85f);
    drawCircleBasic(0.005f, -0.10f, 0.015f, 8, 0.2f, 0.2f, 0.2f);
    glPopMatrix();

    // --- 3. LEGS & BOOTS ---
    drawRectBasic(-0.035f, 0.02f, 0.035f, 0.14f, 0.8f, 0.8f, 0.85f);
    drawRectBasic(0.005f, 0.02f, 0.035f, 0.14f, 0.8f, 0.8f, 0.85f);

    glColor3f(0.3f, 0.3f, 0.35f);
    drawRectBasic(-0.04f, 0.0f, 0.045f, 0.04f, 0.3f, 0.3f, 0.35f);
    drawRectBasic(0.0f, 0.0f, 0.045f, 0.04f, 0.3f, 0.3f, 0.35f);

    // --- 4. TORSO (Space Suit) ---
    // Main Body
    drawRectBasic(-0.04f, 0.13f + bob, 0.085f, 0.16f, 0.9f, 0.9f, 0.95f);

    drawRectBasic(-0.02f, 0.18f + bob, 0.04f, 0.06f, 0.2f, 0.2f, 0.25f);
    float lifePulse = sin(walkTimer * 0.5f);
    drawRectBasic(-0.01f, 0.20f + bob, 0.02f, 0.02f, 0.0f, 1.0f, 1.0f);

    drawRectBasic(-0.04f, 0.13f + bob, 0.085f, 0.02f, 0.2f, 0.2f, 0.2f);

    // --- 5. HELMET ---
    float headY = 0.29f + bob;

    drawCircleBasic(0.0f, headY + 0.04f, 0.06f, 15, 0.9f, 0.9f, 0.95f);

    drawCircleBasic(0.015f, headY + 0.04f, 0.035f, 10, 1.0f, 0.7f, 0.0f);
    drawCircleBasic(0.025f, headY + 0.05f, 0.01f, 6, 1.0f, 1.0f, 0.8f);

    // --- 6. FRONT ARM & LASER SWORD ---
    glPushMatrix();
    glTranslatef(0.0f, 0.22f + bob, 0.0f);
    glRotatef(armAngle, 0.0f, 0.0f, 1.0f);

    drawRectBasic(-0.02f, -0.09f, 0.04f, 0.11f, 0.85f, 0.85f, 0.9f);

    drawCircleBasic(0.0f, -0.10f, 0.018f, 10, 0.2f, 0.2f, 0.2f);

    glPushMatrix();
    glTranslatef(0.0f, -0.10f, 0.0f);
    glRotatef(swordAngle + 15.0f, 0.0f, 0.0f, 1.0f);

    drawRectBasic(-0.01f, -0.05f, 0.02f, 0.10f, 0.1f, 0.1f, 0.1f);
    drawRectBasic(-0.015f, 0.04f, 0.03f, 0.01f, 0.5f, 0.5f, 0.5f);

    drawRectBasic(-0.008f, 0.05f, 0.016f, 0.35f, 0.5f, 1.0f, 1.0f);
    drawRectBasic(-0.003f, 0.05f, 0.006f, 0.33f, 1.0f, 1.0f, 1.0f);

    if (isAttacking && attackFrame > 4 && attackFrame < 12)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(0.0f, 1.0f, 1.0f, 0.6f);
        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.2f, 0.35f);
        glVertex2f(0.4f, 0.1f);
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

    if (currentLevel == 3)
    {
        drawAlienEnemy(e);
        return;
    }

    glPushMatrix();
    glTranslatef(e.x, e.y, 0.0f);

    if (e.velX > 0.01f || e.velX < -0.01f)
        glColor3f(1.0f, 0.5f, 0.5f);
    else
        glColor3f(1, 1, 1);

    if (!e.facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);

    if (currentLevel == 1)
    {
        if (e.isBoss)
        {
            glScalef(1.5f, 1.5f, 1.0f);
            drawLitRect(-0.05f, 0.0f, 0.04f, 0.15f, 0.1f, 0.1f, 0.1f);
            drawLitRect(0.01f, 0.0f, 0.04f, 0.15f, 0.1f, 0.1f, 0.1f);
            drawLitRect(-0.07f, 0.15f, 0.14f, 0.18f, 0.8f, 0.6f, 0.1f);
            drawLitRect(-0.05f, 0.32f, 0.1f, 0.09f, 0.1f, 0.1f, 0.1f);
            drawLitRect(-0.1f, 0.45f, 0.2f, 0.02f, 0.5f, 0.0f, 0.0f);
            drawLitRect(-0.1f, 0.45f, 0.2f * ((float)e.hp / 8.0f), 0.02f, 1.0f, 0.0f, 0.0f);
        }
        else
        {
            drawLitRect(-0.04f, 0.0f, 0.03f, 0.13f, 0.1f, 0.1f, 0.1f);
            drawLitRect(0.01f, 0.0f, 0.03f, 0.13f, 0.15f, 0.15f, 0.15f);
            drawLitRect(-0.06f, 0.12f, 0.12f, 0.15f, 0.5f, 0.1f, 0.1f);
            drawLitRect(-0.04f, 0.26f, 0.08f, 0.07f, 0.2f, 0.05f, 0.05f);
        }
        glPushMatrix();
        glTranslatef(0.0f, 0.15f, 0.0f);
        float ang = -45.0f;
        if (e.isAttacking)
            ang = (e.attackFrame < 10) ? -45.0f + (e.attackFrame * 13.0f) : 85.0f - ((e.attackFrame - 10) * 10.0f);
        glRotatef(ang, 0.0f, 0.0f, 1.0f);
        drawLitRect(-0.01f, -0.05f, 0.02f, 0.06f, 0.2f, 0.2f, 0.2f);
        drawLitRect(0.0f, -0.05f, 0.015f, 0.18f, 0.6f, 0.6f, 0.65f);
        glPopMatrix();
    }
    else if (currentLevel == 2)
    {
        if (e.isBoss)
        {
            glTranslatef(0.0f, 0.52f, 0.0f);
            drawNinjaEnemy(e);
            drawRectBasic(-0.1f, 0.45f, 0.2f, 0.02f, 0.5f, 0.0f, 0.0f);
            drawRectBasic(-0.1f, 0.45f, 0.2f * ((float)e.hp / 8.0f), 0.02f, 1.0f, 0.0f, 0.0f);
        }
        else
        {
            glTranslatef(0.0f, 0.34f, 0.0f);
            drawNinjaEnemy(e);
        }
    }
    glPopMatrix();
}

// ==========================================
// 12. UI & MENU (UPDATED WITH LEVEL 3)
// ==========================================
void drawMenu()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.1f, 0.2f, 0.6f, 0.8f); // Blue: R=0.1, G=0.2, B=0.6, Alpha=0.8
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(800, 0);
    glVertex2f(800, 600);
    glVertex2f(0, 600);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(300, 500, "Mr.ABU  VS  Everyone", GLUT_BITMAP_TIMES_ROMAN_24);

    // --- 1. START GAME BUTTON (TOP) ---
    if (mouseInStartButton)
        glColor3f(0.0f, 0.8f, 0.0f);
    else
        glColor3f(0.0f, 0.6f, 0.0f);

    drawRectBasic(START_BUTTON_X1, START_BUTTON_Y1, 200, 40, 0.2f, 0.2f, 0.2f);

    if (mouseInStartButton)
    {
        glColor3f(0.0f, 1.0f, 0.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(START_BUTTON_X1, START_BUTTON_Y1);
        glVertex2f(START_BUTTON_X2, START_BUTTON_Y1);
        glVertex2f(START_BUTTON_X2, START_BUTTON_Y2);
        glVertex2f(START_BUTTON_X1, START_BUTTON_Y2);
        glEnd();
        glLineWidth(1.0f);
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(370, START_BUTTON_Y1 + 15, "START GAME");

    // --- 2. LEVEL 1 BUTTON (UPPER MIDDLE) ---
    if (currentLevel == 1 || mouseInLevel1Button)
        glColor3f(1.0f, 0.5f, 0.0f);
    else
        glColor3f(0.6f, 0.6f, 0.6f);

    drawRectBasic(LEVEL_BUTTON_X1, LEVEL1_BUTTON_Y1, 200, 40, 0.3f, 0.3f, 0.3f);

    if (mouseInLevel1Button)
    {
        glColor3f(1.0f, 0.7f, 0.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(LEVEL_BUTTON_X1, LEVEL1_BUTTON_Y1);
        glVertex2f(LEVEL_BUTTON_X2, LEVEL1_BUTTON_Y1);
        glVertex2f(LEVEL_BUTTON_X2, LEVEL1_BUTTON_Y2);
        glVertex2f(LEVEL_BUTTON_X1, LEVEL1_BUTTON_Y2);
        glEnd();
        glLineWidth(1.0f);
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(370, LEVEL1_BUTTON_Y1 + 15, "LEVEL 1");

    // --- 3. LEVEL 2 BUTTON (MIDDLE) ---
    if (currentLevel == 2 || mouseInLevel2Button)
        glColor3f(1.0f, 0.5f, 0.0f);
    else
        glColor3f(0.6f, 0.6f, 0.6f);

    drawRectBasic(LEVEL_BUTTON_X1, LEVEL2_BUTTON_Y1, 200, 40, 0.3f, 0.3f, 0.3f);

    if (mouseInLevel2Button)
    {
        glColor3f(1.0f, 0.7f, 0.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(LEVEL_BUTTON_X1, LEVEL2_BUTTON_Y1);
        glVertex2f(LEVEL_BUTTON_X2, LEVEL2_BUTTON_Y1);
        glVertex2f(LEVEL_BUTTON_X2, LEVEL2_BUTTON_Y2);
        glVertex2f(LEVEL_BUTTON_X1, LEVEL2_BUTTON_Y2);
        glEnd();
        glLineWidth(1.0f);
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(370, LEVEL2_BUTTON_Y1 + 15, "LEVEL 2");

    // --- 4. LEVEL 3 BUTTON (BOTTOM) ---
    if (currentLevel == 3 || mouseInLevel3Button)
        glColor3f(1.0f, 0.5f, 0.0f);
    else
        glColor3f(0.6f, 0.6f, 0.6f);

    drawRectBasic(LEVEL_BUTTON_X1, LEVEL3_BUTTON_Y1, 200, 40, 0.3f, 0.3f, 0.3f);

    if (mouseInLevel3Button)
    {
        glColor3f(1.0f, 0.7f, 0.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(LEVEL_BUTTON_X1, LEVEL3_BUTTON_Y1);
        glVertex2f(LEVEL_BUTTON_X2, LEVEL3_BUTTON_Y1);
        glVertex2f(LEVEL_BUTTON_X2, LEVEL3_BUTTON_Y2);
        glVertex2f(LEVEL_BUTTON_X1, LEVEL3_BUTTON_Y2);
        glEnd();
        glLineWidth(1.0f);
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(370, LEVEL3_BUTTON_Y1 + 15, "LEVEL 3");

    glColor3f(0.7f, 0.7f, 0.7f);
    drawText(310, 150, "Select Level, then Start");

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawUI()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    for (int i = 0; i < numLamps1; i++)
        glDisable(GL_LIGHT1 + i);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawRectBasic(20, 550, 204, 24, 0.2f, 0.2f, 0.2f);
    drawRectBasic(22, 552, 200, 20, 0.2f, 0.0f, 0.0f);
    if (playerHealth > 0)
        drawRectBasic(22, 552, 200 * (playerHealth / 100.0f), 20, 0.0f, 0.8f, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    char livesStr[50];
    if (currentLevel == 3)
    {
        int bossDistance = (int)(12.0f - pX);
        if (bossDistance < 0)
            bossDistance = 0;
        sprintf(livesStr, "LIVES: %d   BOSS: %dm   LEVEL: %d", playerLives, bossDistance, currentLevel);
    }
    else
    {
        sprintf(livesStr, "LIVES: %d   LEVEL: %d", playerLives, currentLevel);
    }
    drawText(20, 520, livesStr);

    if (gameState == STATE_GAMEOVER)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0, 0, 0, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 600);
        glVertex2f(0, 600);
        glEnd();
        glColor3f(1, 0, 0);
        drawText(350, 300, "GAME OVER");
        glColor3f(1, 1, 1);
        drawText(300, 270, "Press ENTER to Restart");
        glDisable(GL_BLEND);
    }

    if (gameState == STATE_LEVEL_COMPLETE)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1, 1, 1, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 600);
        glVertex2f(0, 600);
        glEnd();
        glColor3f(0, 1, 0);

        if (currentLevel == 3)
        {
            drawText(320, 350, "GAME COMPLETE!");
            drawText(310, 320, "All levels finished!");
        }
        else
        {
            drawText(320, 350, "LEVEL COMPLETE!");
            drawText(300, 320, "Press SPACE for next level");
        }

        glDisable(GL_BLEND);
    }

    if (redFlash > 0.0f)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1, 0, 0, redFlash);
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
// 13. GAME LOGIC
// ==========================================
void takeDamage(float amount)
{
    if (gameState == STATE_LEVEL_COMPLETE || playerInvincibleTimer > 0)
        return;
    playerHealth -= amount;
    screenShake = 0.1f;
    redFlash = 0.6f;
    playerInvincibleTimer = 60;
    pVelY = (currentLevel == 1) ? KNOCKBACK_Y : 0.03f;
    pVelX = (facingRight) ? -KNOCKBACK_X : KNOCKBACK_X;
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
            gameState = STATE_GAMEOVER;
        }
    }
}

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
    gameState = STATE_MENU;
    cameraX = 0.0f;
    bossLaser.active = false;

    for (int i = 0; i < numEnemies1; i++)
    {
        enemies1[i].alive = true;
        enemies1[i].hp = (enemies1[i].isBoss) ? 8 : 2;
        enemies1[i].x = (enemies1[i].startX + enemies1[i].endX) / 2.0f;
        enemies1[i].velX = 0;
        enemies1[i].isAttacking = false;
        enemies1[i].attackCooldown = 0;
    }

    for (int i = 0; i < numEnemies2; i++)
    {
        enemies2[i].alive = true;
        enemies2[i].hp = (enemies2[i].isBoss) ? 8 : 2;
        enemies2[i].x = (enemies2[i].startX + enemies2[i].endX) / 2.0f;
        enemies2[i].velX = 0;
        enemies2[i].isAttacking = false;
        enemies2[i].attackCooldown = 0;
    }

    for (int i = 0; i < numEnemies3; i++)
    {
        enemies3[i].alive = true;
        enemies3[i].hp = (enemies3[i].isBoss) ? BOSS_MAX_HP : 2;
        enemies3[i].x = (enemies3[i].startX + enemies3[i].endX) / 2.0f;
        enemies3[i].velX = 0;
        enemies3[i].isAttacking = false;
        enemies3[i].attackCooldown = 0;
    }
}

void loadLevel(int level)
{
    currentLevel = level;
    resetPlayer();
    cameraX = 0.0f;
    bossLaser.active = false;

    switch (level)
    {
    case 1:
        gameState = STATE_LEVEL1;
        break;
    case 2:
        gameState = STATE_LEVEL2;
        break;
    case 3:
        gameState = STATE_LEVEL3;
        break;
    }
}

void updateLevel1()
{
    if (screenShake > 0)
        screenShake -= 0.005f;
    else
        screenShake = 0;
    if (redFlash > 0)
        redFlash -= 0.05f;
    if (playerInvincibleTimer > 0)
        playerInvincibleTimer--;

    RectObj standBox = {pX - 0.03f, pY, 0.06f, PLAYER_H_STAND};
    bool forcedCrouch = false;
    for (int i = 0; i < numBeams1; i++)
    {
        if (checkOverlap(standBox, lowBeams1[i]))
        {
            forcedCrouch = true;
        }
    }

    bool wantsCrouch = keyS && !isJumping;
    isCrouching = wantsCrouch || forcedCrouch;

    if (isCrouching)
    {
        if (keyA)
        {
            pVelX -= MOVE_ACCEL * 0.6f;
            facingRight = false;
            walkTimer += 0.2f;
        }
        if (keyD)
        {
            pVelX += MOVE_ACCEL * 0.6f;
            facingRight = true;
            walkTimer += 0.2f;
        }
        pVelX *= 0.8f;
    }
    else
    {
        if (keyA)
        {
            pVelX -= MOVE_ACCEL;
            facingRight = false;
            walkTimer += 0.3f;
        }
        if (keyD)
        {
            pVelX += MOVE_ACCEL;
            facingRight = true;
            walkTimer += 0.3f;
        }
        if (!keyA && !keyD)
            pVelX *= FRICTION;
    }

    if (pVelX > MAX_SPEED)
        pVelX = MAX_SPEED;
    if (pVelX < -MAX_SPEED)
        pVelX = -MAX_SPEED;

    pX += pVelX;

    if (pX < LEVEL_START_X)
    {
        pX = LEVEL_START_X;
        pVelX = 0;
    }
    if (pX > LEVEL_END_X)
    {
        pX = LEVEL_END_X;
        pVelX = 0;
    }

    float currentH = isCrouching ? PLAYER_H_CROUCH : PLAYER_H_STAND;
    RectObj pBox = {pX - 0.03f, pY, 0.06f, currentH};

    for (int i = 0; i < numBeams1; i++)
    {
        if (checkOverlap(pBox, lowBeams1[i]))
        {
            pX -= pVelX;
            pVelX = 0;
        }
    }

    if (keySpace && !isJumping && !wantsCrouch && !forcedCrouch)
    {
        pVelY = JUMP_FORCE;
        isJumping = true;
    }
    pY += pVelY;
    pVelY -= GRAVITY;

    pBox.y = pY;
    pBox.x = pX - 0.03f;

    bool onSolid = false;

    for (int i = 0; i < numBeams1; i++)
    {
        if (checkOverlap(pBox, lowBeams1[i]))
        {
            if (pVelY > 0)
            {
                pY -= pVelY;
                pVelY = 0;
            }
            else
            {
                if (pY - pVelY >= lowBeams1[i].y + lowBeams1[i].h)
                {
                    pY = lowBeams1[i].y + lowBeams1[i].h;
                    isJumping = false;
                    onSolid = true;
                    pVelY = 0;
                }
                else
                {
                    pY -= pVelY;
                }
            }
        }
    }

    for (int i = 0; i < numGrounds1; i++)
    {
        if (pX >= groundSegments1[i].x && pX <= groundSegments1[i].x + groundSegments1[i].w)
        {
            float gTop = groundSegments1[i].y + groundSegments1[i].h;
            if (pY >= gTop - 0.1f && pY <= gTop + 0.1f && pVelY <= 0)
            {
                pY = gTop;
                pVelY = 0;
                isJumping = false;
                onSolid = true;
            }
        }
    }

    for (int i = 0; i < numPlatforms1; i++)
    {
        RectObj plat = platforms1[i];
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

    float targetCamX = pX - 0.0f;
    cameraX += (targetCamX - cameraX) * 0.1f;
    if (cameraX < LEVEL_START_X + 1.0f)
        cameraX = LEVEL_START_X + 1.0f;
    if (cameraX > LEVEL_END_X - 1.0f)
        cameraX = LEVEL_END_X - 1.0f;

    RectObj damageBox = {pX - 0.03f, pY, 0.06f, currentH};

    for (int i = 0; i < numSpikes1; i++)
    {
        if (checkOverlap(damageBox, spikes1[i]))
        {
            takeDamage(15.0f);
        }
    }

    RectObj weaponBox = {0, 0, 0, 0};
    if (isAttacking && attackFrame > 8 && attackFrame < 15)
    {
        float reach = 0.35f;
        weaponBox.y = pY + 0.1f;
        weaponBox.h = 0.2f;
        weaponBox.w = reach;
        weaponBox.x = facingRight ? pX : pX - reach;
    }

    for (int i = 0; i < numEnemies1; i++)
    {
        if (!enemies1[i].alive)
            continue;
        enemies1[i].x += enemies1[i].velX;
        enemies1[i].velX *= 0.9f;
        float eW = enemies1[i].isBoss ? 0.15f : 0.08f;
        float eH = enemies1[i].isBoss ? 0.5f : 0.3f;
        RectObj enemyBox = {enemies1[i].x - eW / 2, enemies1[i].y, eW, eH};
        float distX = enemies1[i].x - pX;

        if (fabs(enemies1[i].velX) < 0.001f && !enemies1[i].isAttacking)
        {
            if (fabs(distX) < (enemies1[i].isBoss ? 0.45f : 0.35f) && enemies1[i].attackCooldown == 0)
            {
                enemies1[i].facingRight = (distX < 0);
                enemies1[i].isAttacking = true;
                enemies1[i].attackFrame = 0;
            }
            else
            {
                if (enemies1[i].facingRight)
                {
                    enemies1[i].x += enemies1[i].speed;
                    if (enemies1[i].x > enemies1[i].endX)
                        enemies1[i].facingRight = false;
                }
                else
                {
                    enemies1[i].x -= enemies1[i].speed;
                    if (enemies1[i].x < enemies1[i].startX)
                        enemies1[i].facingRight = true;
                }
            }
        }

        if (isAttacking && attackFrame > 8 && attackFrame < 15)
        {
            if (checkOverlap(weaponBox, enemyBox))
            {
                enemies1[i].hp--;
                hitEffect.x = enemies1[i].x;
                hitEffect.y = enemies1[i].y + 0.2f;
                hitEffect.life = 5;
                enemies1[i].velX = (facingRight ? KNOCKBACK_X : -KNOCKBACK_X);
                if (enemies1[i].hp <= 0)
                {
                    enemies1[i].alive = false;
                    screenShake = 0.05f;
                    if (enemies1[i].isBoss)
                        gameState = STATE_LEVEL_COMPLETE;
                }
            }
        }

        if (enemies1[i].isAttacking)
        {
            enemies1[i].attackFrame++;
            if (enemies1[i].attackFrame == 8)
            {
                float reach = enemies1[i].isBoss ? 0.5f : 0.35f;
                RectObj eWeaponBox = {0, enemies1[i].y + 0.1f, reach, 0.2f};
                eWeaponBox.x = enemies1[i].facingRight ? enemies1[i].x : enemies1[i].x - reach;
                if (checkOverlap(eWeaponBox, damageBox))
                    takeDamage(enemies1[i].isBoss ? 30.0f : 15.0f);
            }
            if (enemies1[i].attackFrame > 20)
            {
                enemies1[i].isAttacking = false;
                enemies1[i].attackCooldown = 60;
            }
        }
        if (enemies1[i].attackCooldown > 0)
            enemies1[i].attackCooldown--;
    }
}

void updateLevel2()
{
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
        currentMaxSpeed = MAX_SPEED * 0.5f;
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
    for (int i = 0; i < numObstacles2; i++)
    {
        if (checkOverlap(playerBoxFuture, obstacles2[i]))
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
    for (int i = 0; i < numGrounds2; i++)
    {
        if (pX >= groundSegments2[i].x && pX <= groundSegments2[i].x + groundSegments2[i].w)
        {
            float gTop = groundSegments2[i].y + groundSegments2[i].h;
            if (pY >= gTop - 0.1f && pY <= gTop + 0.1f && pVelY <= 0)
            {
                pY = gTop;
                pVelY = 0;
                isJumping = false;
                onSolid = true;
            }
        }
    }

    for (int i = 0; i < numPlatforms2; i++)
    {
        RectObj plat = platforms2[i];
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

    for (int i = 0; i < numEnemies2; i++)
    {
        if (!enemies2[i].alive)
            continue;
        enemies2[i].x += enemies2[i].velX;
        enemies2[i].velX *= 0.9f;

        float eW = enemies2[i].isBoss ? 0.15f : 0.08f;
        float eH = enemies2[i].isBoss ? 0.5f : 0.3f;
        RectObj enemyBox = {enemies2[i].x - eW / 2, enemies2[i].y, eW, eH};
        float distX = enemies2[i].x - pX;

        if (fabs(enemies2[i].velX) < 0.001f && !enemies2[i].isAttacking)
        {
            bool inAttackRange = (fabs(distX) < (enemies2[i].isBoss ? 0.45f : 0.35f));
            if (inAttackRange && enemies2[i].attackCooldown == 0)
            {
                enemies2[i].facingRight = (distX < 0);
                enemies2[i].isAttacking = true;
                enemies2[i].attackFrame = 0;
            }
            else
            {
                if (enemies2[i].facingRight)
                {
                    enemies2[i].x += enemies2[i].speed;
                    if (enemies2[i].x > enemies2[i].endX)
                        enemies2[i].facingRight = false;
                }
                else
                {
                    enemies2[i].x -= enemies2[i].speed;
                    if (enemies2[i].x < enemies2[i].startX)
                        enemies2[i].facingRight = true;
                }
            }
        }

        if (isAttacking && attackFrame > 8 && attackFrame < 15)
        {
            if (checkOverlap(weaponBox, enemyBox))
            {
                enemies2[i].hp--;
                hitEffect.x = enemies2[i].x;
                hitEffect.y = enemies2[i].y + 0.2f;
                hitEffect.life = 5;
                enemies2[i].velX = (facingRight ? KNOCKBACK_FORCE : -KNOCKBACK_FORCE);
                enemies2[i].isAttacking = false;
                if (enemies2[i].hp <= 0)
                {
                    enemies2[i].alive = false;
                    screenShake = 0.05f;
                    if (enemies2[i].isBoss)
                        gameState = STATE_LEVEL_COMPLETE;
                }
                isAttacking = false;
            }
        }

        if (enemies2[i].isAttacking)
        {
            enemies2[i].attackFrame++;
            if (enemies2[i].attackFrame == 8)
            {
                float reach = enemies2[i].isBoss ? 0.5f : 0.35f;
                RectObj eWeaponBox = {0, enemies2[i].y + 0.1f, reach, 0.2f};
                if (enemies2[i].facingRight)
                    eWeaponBox.x = enemies2[i].x;
                else
                    eWeaponBox.x = enemies2[i].x - reach;
                if (checkOverlap(eWeaponBox, playerBox))
                    takeDamage(enemies2[i].isBoss ? 30.0f : 15.0f);
            }
            if (enemies2[i].attackFrame > 20)
            {
                enemies2[i].isAttacking = false;
                enemies2[i].attackCooldown = 60;
            }
        }
        if (enemies2[i].attackCooldown > 0)
            enemies2[i].attackCooldown--;
    }
}

void updateLevel3()
{
    if (screenShake > 0)
        screenShake -= 0.005f;
    else
        screenShake = 0;
    if (redFlash > 0)
        redFlash -= 0.05f;
    if (playerInvincibleTimer > 0)
        playerInvincibleTimer--;

    walkTimer += 0.1f;

    bool forcedCrouch = false;
    RectObj standBox = {pX - 0.03f, pY, 0.06f, PLAYER_H_STAND};
    for (int i = 0; i < numBeams3; i++)
    {
        if (checkOverlap(standBox, alienBeams3[i]))
            forcedCrouch = true;
    }
    isCrouching = (keyS && !isJumping) || forcedCrouch;

    if (isCrouching)
    {
        if (keyA)
        {
            pVelX -= MOVE_ACCEL * 0.6f;
            facingRight = false;
        }
        if (keyD)
        {
            pVelX += MOVE_ACCEL * 0.6f;
            facingRight = true;
        }
        pVelX *= 0.8f;
    }
    else
    {
        if (keyA)
        {
            pVelX -= MOVE_ACCEL;
            facingRight = false;
        }
        if (keyD)
        {
            pVelX += MOVE_ACCEL;
            facingRight = true;
        }
        if (!keyA && !keyD)
            pVelX *= FRICTION;
    }

    if (pVelX > MAX_SPEED)
        pVelX = MAX_SPEED;
    if (pVelX < -MAX_SPEED)
        pVelX = -MAX_SPEED;

    pX += pVelX;

    if (pX < LEVEL_START_X)
        pX = LEVEL_START_X;
    if (pX > LEVEL_END_X)
        pX = LEVEL_END_X;

    float currentH = isCrouching ? PLAYER_H_CROUCH : PLAYER_H_STAND;
    RectObj pBox = {pX - 0.03f, pY, 0.06f, currentH};

    for (int i = 0; i < numBeams3; i++)
    {
        if (checkOverlap(pBox, alienBeams3[i]))
        {
            pX -= pVelX;
            pVelX = 0;
        }
    }

    if (keySpace && !isJumping && !isCrouching && !forcedCrouch)
    {
        pVelY = JUMP_FORCE;
        isJumping = true;
    }
    pY += pVelY;
    pVelY -= GRAVITY;

    pBox.y = pY;
    pBox.x = pX - 0.03f;
    bool onSolid = false;

    for (int i = 0; i < numBeams3; i++)
    {
        if (checkOverlap(pBox, alienBeams3[i]))
        {
            if (pVelY > 0)
            {
                pY -= pVelY;
                pVelY = 0;
            }
            else if (pY - pVelY >= alienBeams3[i].y + alienBeams3[i].h)
            {
                pY = alienBeams3[i].y + alienBeams3[i].h;
                isJumping = false;
                onSolid = true;
                pVelY = 0;
            }
        }
    }

    for (int i = 0; i < numGrounds3; i++)
    {
        if (pX >= groundSegments3[i].x && pX <= groundSegments3[i].x + groundSegments3[i].w)
        {
            float gTop = groundSegments3[i].y + groundSegments3[i].h;
            if (pY >= gTop - 0.1f && pY <= gTop + 0.1f && pVelY <= 0)
            {
                pY = gTop;
                pVelY = 0;
                isJumping = false;
                onSolid = true;
            }
        }
    }

    for (int i = 0; i < numPlatforms3; i++)
    {
        RectObj plat = platforms3[i];
        if (pVelY <= 0 && pY >= plat.y + plat.h - 0.1f && pY <= plat.y + plat.h + 0.1f &&
            pX >= plat.x - 0.1f && pX <= plat.x + plat.w + 0.1f)
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

    cameraX += (pX - cameraX) * 0.1f;
    if (cameraX < LEVEL_START_X + 1.0f)
        cameraX = LEVEL_START_X + 1.0f;
    if (cameraX > LEVEL_END_X - 1.0f)
        cameraX = LEVEL_END_X - 1.0f;

    RectObj damageBox = {pX - 0.03f, pY, 0.06f, currentH};
    for (int i = 0; i < numMines3; i++)
    {
        if (checkOverlap(damageBox, mines3[i]))
            takeDamage(15.0f);
    }

    for (int i = 0; i < numSpikes3; i++)
    {
        if (checkOverlap(damageBox, spikeTraps3[i]))
            takeDamage(20.0f);
    }

    // Laser Logic
    if (bossLaser.active)
    {
        bossLaser.x += bossLaser.velX;
        RectObj laserBox = {bossLaser.x, bossLaser.y, bossLaser.w, bossLaser.h};
        if (checkOverlap(laserBox, damageBox))
        {
            takeDamage(25.0f);
            bossLaser.active = false;
        }
        if (bossLaser.x < cameraX - 10.0f || bossLaser.x > cameraX + 10.0f)
            bossLaser.active = false;
    }

    // Enemy Logic
    RectObj weaponBox = {0, 0, 0, 0};
    if (isAttacking && attackFrame > 8 && attackFrame < 15)
    {
        float reach = 0.35f;
        weaponBox.y = pY + 0.1f;
        weaponBox.h = 0.2f;
        weaponBox.w = reach;
        weaponBox.x = facingRight ? pX : pX - reach;
    }

    for (int i = 0; i < numEnemies3; i++)
    {
        if (!enemies3[i].alive)
            continue;
        enemies3[i].x += enemies3[i].velX;
        enemies3[i].velX *= 0.9f;

        float distX = enemies3[i].x - pX;
        bool inRange = fabs(distX) < (enemies3[i].isBoss ? 0.6f : 0.35f);

        if (enemies3[i].isBoss && !enemies3[i].isAttacking && enemies3[i].attackCooldown == 0 && !bossLaser.active)
        {
            if (fabs(distX) > 0.8f && fabs(distX) < 4.0f)
            {
                enemies3[i].attackCooldown = 120;
                bossLaser.active = true;
                bossLaser.w = 0.4f;
                bossLaser.h = 0.05f;
                bossLaser.y = enemies3[i].y + 0.15f;
                if (enemies3[i].x > pX)
                {
                    enemies3[i].facingRight = false;
                    bossLaser.x = enemies3[i].x - 0.4f;
                    bossLaser.velX = -0.1f;
                }
                else
                {
                    enemies3[i].facingRight = true;
                    bossLaser.x = enemies3[i].x;
                    bossLaser.velX = 0.1f;
                }
            }
        }

        if (fabs(enemies3[i].velX) < 0.001f && !enemies3[i].isAttacking)
        {
            if (inRange && enemies3[i].attackCooldown == 0)
            {
                enemies3[i].facingRight = (distX < 0);
                enemies3[i].isAttacking = true;
                enemies3[i].attackFrame = 0;
            }
            else
            {
                if (enemies3[i].facingRight)
                {
                    enemies3[i].x += enemies3[i].speed;
                    if (enemies3[i].x > enemies3[i].endX)
                        enemies3[i].facingRight = false;
                }
                else
                {
                    enemies3[i].x -= enemies3[i].speed;
                    if (enemies3[i].x < enemies3[i].startX)
                        enemies3[i].facingRight = true;
                }
            }
        }

        if (isAttacking && attackFrame > 8 && attackFrame < 15)
        {
            float eW = enemies3[i].isBoss ? 0.2f : 0.1f;
            float eH = enemies3[i].isBoss ? 0.4f : 0.2f;
            RectObj enemyBox = {enemies3[i].x - eW / 2, enemies3[i].y, eW, eH};

            if (checkOverlap(weaponBox, enemyBox))
            {
                enemies3[i].hp--;
                hitEffect.x = enemies3[i].x;
                hitEffect.y = enemies3[i].y + 0.2f;
                hitEffect.life = 5;
                enemies3[i].velX = (facingRight ? KNOCKBACK_X : -KNOCKBACK_X);
                if (enemies3[i].hp <= 0)
                {
                    enemies3[i].alive = false;
                    screenShake = 0.05f;
                    if (enemies3[i].isBoss)
                        gameState = STATE_LEVEL_COMPLETE;
                }
            }
        }

        if (enemies3[i].isAttacking)
        {
            enemies3[i].attackFrame++;
            if (enemies3[i].attackFrame == 8)
            {
                float reach = enemies3[i].isBoss ? 0.6f : 0.35f;
                RectObj eWeaponBox = {0, enemies3[i].y + 0.1f, reach, 0.2f};
                eWeaponBox.x = enemies3[i].facingRight ? enemies3[i].x : enemies3[i].x - reach;
                if (checkOverlap(eWeaponBox, damageBox))
                    takeDamage(enemies3[i].isBoss ? BOSS_DAMAGE : 15.0f);
            }
            if (enemies3[i].attackFrame > 20)
            {
                enemies3[i].isAttacking = false;
                enemies3[i].attackCooldown = 60;
            }
        }
        if (enemies3[i].attackCooldown > 0)
            enemies3[i].attackCooldown--;
    }
}

void timer(int value)
{
    cloudTime += 0.0003f;

    if (gameState == STATE_GAMEOVER || gameState == STATE_LEVEL_COMPLETE)
    {
        if (gameState == STATE_LEVEL_COMPLETE)
            pX += 0.005f;
        glutPostRedisplay();
        glutTimerFunc(16, timer, 0);
        return;
    }

    if (gameState == STATE_MENU)
    {
        glutPostRedisplay();
        glutTimerFunc(16, timer, 0);
        return;
    }

    switch (currentLevel)
    {
    case 1:
        updateLevel1();
        break;
    case 2:
        updateLevel2();
        break;
    case 3:
        updateLevel3();
        break;
    }

    if (isAttacking)
    {
        attackFrame++;
        if (attackFrame < 5)
        {
            armAngle = -45.0f;
            swordAngle = -45.0f;
        }
        else if (attackFrame < 12)
        {
            armAngle = 0.0f;
            swordAngle = 90.0f;
        }
        else if (attackFrame < 25)
        {
            float t = (float)(attackFrame - 12) / 13.0f;
            armAngle = -20.0f * (1.0f - t);
            swordAngle = 90.0f * (1.0f - t);
        }
        else
        {
            isAttacking = false;
            attackFrame = 0;
            armAngle = 0.0f;
            swordAngle = 0.0f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ==========================================
// 14. DISPLAY
// ==========================================
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (gameState == STATE_MENU)
    {
        drawMenu();
        glutSwapBuffers();
        return;
    }

    if (screenShake > 0)
        glTranslatef((rand() % 100 / 5000.0f) * screenShake * 10, (rand() % 100 / 5000.0f) * screenShake * 10, 0);

    if (currentLevel == 1)
    {
        glDisable(GL_LIGHTING);
        glBegin(GL_QUADS);
        glColor3f(0.02f, 0.02f, 0.05f);
        glVertex2f(-1, 1);
        glColor3f(0.02f, 0.02f, 0.05f);
        glVertex2f(1, 1);
        glColor3f(0.1f, 0.1f, 0.2f);
        glVertex2f(1, -0.2f);
        glColor3f(0.1f, 0.1f, 0.2f);
        glVertex2f(-1, -0.2f);
        glEnd();

        glPushMatrix();
        glTranslatef(0.6f, 0.6f, 0.0f);
        glColor3f(0.9f, 0.9f, 0.8f);
        drawCircleBasic(0, 0, 0.12f, 40);
        glPopMatrix();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPushMatrix();
        float cloudScroll = -cameraX * 0.05f - cloudTime;
        float wrappedCloud = fmod(cloudScroll, 2.0f);
        glTranslatef(wrappedCloud, 0, 0);
        drawCloudLayer();
        glTranslatef(2.0f, 0, 0);
        drawCloudLayer();
        glPopMatrix();

        float farCityWidth = 2.2f;
        int startChunk = (int)((cameraX * 0.5f) / farCityWidth) - 1;
        for (int i = startChunk; i < startChunk + 4; i++)
        {
            glPushMatrix();
            glTranslatef(i * farCityWidth - (cameraX * 0.5f), 0, 0);
            drawFarCityChunk();
            glPopMatrix();
        }

        float pillarWidth = 2.0f;
        int pillarChunk = (int)((cameraX * 0.7f) / pillarWidth) - 1;
        for (int i = pillarChunk; i < pillarChunk + 4; i++)
        {
            glPushMatrix();
            glTranslatef(i * pillarWidth - (cameraX * 0.7f), 0, 0);
            drawPillar(-0.8f);
            drawPillar(0.8f);
            glPopMatrix();
        }

        drawFog();

        float nearWallWidth = 2.0f;
        int wallChunk = (int)((cameraX * 0.8f) / nearWallWidth) - 1;
        for (int i = wallChunk; i < wallChunk + 4; i++)
        {
            glPushMatrix();
            glTranslatef(i * nearWallWidth - (cameraX * 0.8f), 0, 0);
            drawBrickLayer();
            glPopMatrix();
        }
        glDisable(GL_BLEND);

        glPushMatrix();
        glTranslatef(-cameraX, 0.0f, 0.0f);

        setupLighting1();

        for (int i = 0; i < numGrounds1; i++)
            drawLitRect(groundSegments1[i].x, groundSegments1[i].y, groundSegments1[i].w, groundSegments1[i].h, 0.15f, 0.15f, 0.18f);
        for (int i = 0; i < numPlatforms1; i++)
            drawLitRect(platforms1[i].x, platforms1[i].y, platforms1[i].w, platforms1[i].h, 0.25f, 0.18f, 0.15f);

        for (int i = 0; i < numLamps1; i++)
            drawLampPost(lampPosts1[i], -0.6f);
        drawObstacles1();

        for (int i = 0; i < numEnemies1; i++)
            drawVillain(enemies1[i]);
        drawDetective();

        drawHitMarker();

        glPopMatrix();
    }
    else if (currentLevel == 2)
    {
        glClearColor(COL_SKY.r, COL_SKY.g, COL_SKY.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glTranslatef(-cameraX * 0.9f, 0, 0);
        drawClouds2();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-cameraX * 0.5f, 0.0f, 0.0f);
        drawMountains();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-cameraX, 0.0f, 0.0f);

        for (int i = 0; i < numGrounds2; i++)
        {
            drawWallAndGrass(groundSegments2[i].x, groundSegments2[i].x + groundSegments2[i].w);
        }

        drawSceneTrees(-2.0f, 16.0f);

        for (int i = 0; i < numPlatforms2; i++)
        {
            drawRectBasic(platforms2[i].x, platforms2[i].y, platforms2[i].w, platforms2[i].h, 0.25f, 0.18f, 0.15f);
            glColor3f(0.1f, 0.08f, 0.05f);
            glPointSize(4.0f);
            glBegin(GL_POINTS);
            for (float r = platforms2[i].x + 0.05f; r < platforms2[i].x + platforms2[i].w; r += 0.1f)
                glVertex2f(r, platforms2[i].y + platforms2[i].h / 2);
            glEnd();
        }

        for (int i = 0; i < numObstacles2; i++)
        {
            drawRectBasic(obstacles2[i].x, obstacles2[i].y, obstacles2[i].w, obstacles2[i].h, COL_OBSTACLE.r, COL_OBSTACLE.g, COL_OBSTACLE.b);
            glColor3f(COL_OBSTACLE.r * 0.8f, COL_OBSTACLE.g * 0.8f, COL_OBSTACLE.b * 0.8f);
            drawRectBasic(obstacles2[i].x + 0.1f, obstacles2[i].y + 0.1f, obstacles2[i].w - 0.2f, obstacles2[i].h - 0.2f, COL_OBSTACLE.r * 0.8f, COL_OBSTACLE.g * 0.8f, COL_OBSTACLE.b * 0.8f);
        }

        for (int i = 0; i < numEnemies2; i++)
            drawVillain(enemies2[i]);
        drawDetective();
        drawHitMarker();

        glPopMatrix();
    }
    else if (currentLevel == 3)
    {
        glClearColor(0.2f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
        glTranslatef(-cameraX * 0.05f, 0, 0);
        drawStars(-cameraX * 0.002f);
        drawAlienSun(cameraX * 0.05f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-cameraX * 0.2f, 0, 0);
        drawMarsHills(-cameraX * 0.01f, 0.4f, 0.15f, 0.1f, -0.2f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-cameraX * 0.5f, 0, 0);
        drawMarsHills(-cameraX * 0.05f, 0.5f, 0.2f, 0.1f, -0.5f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-cameraX, 0.0f, 0.0f);
        setupLighting3(cameraX);

        for (int i = 0; i < numGrounds3; i++)
            drawAlienGround(groundSegments3[i]);
        for (int i = 0; i < numPlatforms3; i++)
            drawAlienGround(platforms3[i]);

        drawAlienSpikes();
        drawAlienObstacles();
        drawLaser();

        for (int i = 0; i < numEnemies3; i++)
            drawVillain(enemies3[i]);

        drawSpaceHero();

        if (hitEffect.life > 0)
        {
            glDisable(GL_LIGHTING);
            glColor3f(0, 1, 0);
            glPushMatrix();
            glTranslatef(hitEffect.x, hitEffect.y, 0);
            glRotatef(hitEffect.life * 25, 0, 0, 1);
            glBegin(GL_TRIANGLES);
            glVertex2f(-0.08f, -0.08f);
            glVertex2f(0.08f, -0.08f);
            glVertex2f(0, 0.08f);
            glEnd();
            glPopMatrix();
            hitEffect.life--;
        }
        glPopMatrix();
    }

    drawUI();
    glutSwapBuffers();
}

// ==========================================
// 15. INPUT HANDLING (UPDATED WITH LEVEL 3)
// ==========================================
void keyDown(unsigned char k, int x, int y)
{
    if (gameState == STATE_MENU)
    {
        if (k == 13)
        {
            loadLevel(currentLevel);
        }
        else if (k == '1')
        {
            currentLevel = 1;
        }
        else if (k == '2')
        {
            currentLevel = 2;
        }
        else if (k == '3')
        {
            currentLevel = 3;
        }
        return;
    }

    if (gameState == STATE_GAMEOVER)
    {
        if (k == ' ' || k == 13)
            fullReset();
        return;
    }

    if (gameState == STATE_LEVEL_COMPLETE)
    {
        if (k == ' ')
        {
            if (currentLevel == 1)
                loadLevel(2);
            else if (currentLevel == 2)
                loadLevel(3);
            else if (currentLevel == 3)
                fullReset();
        }
        else if (k == 13)
        {
            fullReset();
        }
        return;
    }

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

void specialKey(int key, int x, int y)
{
    if (gameState == STATE_MENU)
    {
        if (key == GLUT_KEY_UP)
        {
            currentLevel--;
            if (currentLevel < 1)
                currentLevel = 3;
        }
        else if (key == GLUT_KEY_DOWN)
        {
            currentLevel++;
            if (currentLevel > 3)
                currentLevel = 1;
        }
    }
}

void handleMouseMotion(int x, int y)
{
    mouseX = (int)(x * (800.0f / windowWidth));

    int invertedY = windowHeight - y;
    mouseY = (int)(invertedY * (600.0f / windowHeight));

    mouseInStartButton = (mouseX >= START_BUTTON_X1 && mouseX <= START_BUTTON_X2 &&
                          mouseY >= START_BUTTON_Y1 && mouseY <= START_BUTTON_Y2);

    mouseInLevel1Button = (mouseX >= LEVEL_BUTTON_X1 && mouseX <= LEVEL_BUTTON_X2 &&
                           mouseY >= LEVEL1_BUTTON_Y1 && mouseY <= LEVEL1_BUTTON_Y2);

    mouseInLevel2Button = (mouseX >= LEVEL_BUTTON_X1 && mouseX <= LEVEL_BUTTON_X2 &&
                           mouseY >= LEVEL2_BUTTON_Y1 && mouseY <= LEVEL2_BUTTON_Y2);

    mouseInLevel3Button = (mouseX >= LEVEL_BUTTON_X1 && mouseX <= LEVEL_BUTTON_X2 &&
                           mouseY >= LEVEL3_BUTTON_Y1 && mouseY <= LEVEL3_BUTTON_Y2);

    glutPostRedisplay();
}

void handleMousePassiveMotion(int x, int y)
{
    handleMouseMotion(x, y);
}

void handleMouseClick(int button, int state, int x, int y)
{

    mouseX = (int)(x * (800.0f / windowWidth));

    int invertedY = windowHeight - y;
    mouseY = (int)(invertedY * (600.0f / windowHeight));

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseLeftDown = true;
            mouseClicked = true;

            if (mouseInStartButton)
            {
                loadLevel(currentLevel);
            }
            else if (mouseInLevel1Button)
            {
                currentLevel = 1;
                glutPostRedisplay();
            }
            else if (mouseInLevel2Button)
            {
                currentLevel = 2;
                glutPostRedisplay();
            }
            else if (mouseInLevel3Button)
            {
                currentLevel = 3;
                glutPostRedisplay();
            }
        }
        else if (state == GLUT_UP)
        {
            mouseLeftDown = false;
        }
    }

    if (state == GLUT_UP)
    {
        mouseClicked = false;
    }
}

void reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

// ==========================================
// 16. MAIN FUNCTION
// ==========================================
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 700);
    glutCreateWindow("Mr.ABU  VS  Everyone");
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);

    createDisplayLists();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKey);

    glutMouseFunc(handleMouseClick);
    glutMotionFunc(handleMouseMotion);
    glutPassiveMotionFunc(handleMousePassiveMotion);

    glutMainLoop();
    return 0;
}