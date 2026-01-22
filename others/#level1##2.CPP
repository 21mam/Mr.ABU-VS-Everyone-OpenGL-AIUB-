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
    STATE_GAMEOVER,
    STATE_COMPLETE,
    STATE_LEVEL_COMPLETE
};

int currentLevel = 1;
GameState gameState = STATE_MENU;
float levelTransitionTimer = 0.0f;

// Window Dimensions (Used for Mouse Scaling)
int windowWidth = 1200;
int windowHeight = 700;

// ==========================================
// 2. COMMON CONFIGURATION
// ==========================================
const float LEVEL_START_X = -1.0f;
const float LEVEL_END_X = 14.0f;
const float PIT_DEPTH = -3.0f;

// BOSS CONFIGURATION ---
const int BOSS_MAX_HP = 80; // Increased Health (Harder)

// Physics Tunings
const float GRAVITY = 0.003f;
const float JUMP_FORCE = 0.065f;
const float MOVE_ACCEL = 0.003f;
const float MAX_SPEED = 0.035f;
const float FRICTION = 0.85f;
const float KNOCKBACK_X = 0.06f;
const float KNOCKBACK_Y = 0.04f;
const float KNOCKBACK_FORCE = 0.04f;

// --- CHANGED: Dimensions (Reduced by ~50% for small character) ---
const float PLAYER_W = 0.021f;
const float PLAYER_H_STAND = 0.075f;
const float PLAYER_H_CROUCH = 0.049f;
// ----------------------------------------------------------------

// Camera
float cameraX = 0.0f;
float screenShake = 0.0f;
float redFlash = 0.0f;

// Player
float pX = 0.0f, pY = -0.5f;
float pVelX = 0.0f, pVelY = 0.0f;
bool isJumping = false;
bool facingRight = true;
float playerHealth = 100.0f;
int playerLives = 3;
int playerInvincibleTimer = 0;

// Animation
float walkTimer = 0.0f;
bool isAttacking = false;
bool isCrouching = false;
float armAngle = 0.0f;
float swordAngle = 0.0f;
int attackFrame = 0;
float cloudTime = 0.0f;

// Inputs
bool keyA = false, keyD = false, keySpace = false, keyW = false, keyS = false;

// Mouse variables
int mouseX = 0, mouseY = 0;
bool mouseLeftDown = false;
bool mouseClicked = false;
bool mouseInLevel1Button = false;
bool mouseInLevel2Button = false;
bool mouseInStartButton = false;

// --- MENU LAYOUT CONSTANTS ---
const float LEVEL_BUTTON_X1 = 300.0f;
const float LEVEL_BUTTON_X2 = 500.0f;

// Order: Start (Top), Level 1 (Middle), Level 2 (Bottom)
const float START_BUTTON_X1 = 300.0f;
const float START_BUTTON_X2 = 500.0f;
const float START_BUTTON_Y1 = 360.0f; // Top
const float START_BUTTON_Y2 = 400.0f;

const float LEVEL1_BUTTON_Y1 = 290.0f; // Middle
const float LEVEL1_BUTTON_Y2 = 330.0f;

const float LEVEL2_BUTTON_Y1 = 220.0f; // Bottom
const float LEVEL2_BUTTON_Y2 = 260.0f;

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
    {13.0f, -0.6f, 0.0f, 12.0f, 13.8f, 0.005f, true, false, true, BOSS_MAX_HP, 0, false, 0}};
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
// 6. UTILITY FUNCTIONS
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
// 7. LEVEL 1 DRAWING FUNCTIONS
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

// Level 1 Lighting
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
// 8. LEVEL 2 DRAWING FUNCTIONS
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
// 9. COMMON CHARACTER DRAWING
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

    // --- CHANGED: Scaling down visually by 50% ---
    glScalef(0.5f, 0.5f, 1.0f);
    // ---------------------------------------------

    if (!facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);
    if (isCrouching)
        glScalef(1.0f, 0.6f, 1.0f);

    float bob = (fabs(pVelX) > 0.001f && !isJumping) ? sin(walkTimer) * 0.01f : 0.0f;

    // Skin Color Definition
    float skinR = 0.96f, skinG = 0.80f, skinB = 0.69f;

    // --- 1. BACK ARM (The arm behind the body) ---
    // Swings opposite to movement to look realistic
    float backArmSwing = sin(walkTimer) * 15.0f;
    glPushMatrix();
    glTranslatef(0.02f, 0.21f + bob, 0.0f);
    glRotatef(-backArmSwing, 0.0f, 0.0f, 1.0f);
    // Sleeve
    if (currentLevel == 1)
        drawLitRect(-0.01f, -0.09f, 0.03f, 0.10f, 0.65f, 0.50f, 0.35f);
    else
        drawRectBasic(-0.01f, -0.09f, 0.03f, 0.10f, 0.65f, 0.50f, 0.35f);
    // Hand
    drawCircleBasic(0.005f, -0.10f, 0.012f, 10, skinR - 0.1f, skinG - 0.1f, skinB - 0.1f);
    glPopMatrix();

    // --- 2. LEGS & SHOES ---
    // Left Leg (Pant)
    if (currentLevel == 1)
        drawLitRect(-0.035f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);
    else
        drawRectBasic(-0.035f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);
    // Right Leg (Pant)
    if (currentLevel == 1)
        drawLitRect(0.005f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);
    else
        drawRectBasic(0.005f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);

    // Shoes (Black leather dress shoes)
    // Left Shoe
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
    // Right Shoe
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

    // --- 3. TORSO (Coat & Suit) ---
    // Shirt (White) & Tie (Red)
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

    // Trench Coat Body
    drawTrapezoid(0.0f, 0.10f + bob, 0.10f, 0.14f, 0.19f, 0.76f, 0.60f, 0.42f);

    if (currentLevel == 1)
        drawLitRect(-0.045f, 0.16f + bob, 0.09f, 0.025f, 0.56f, 0.40f, 0.22f);
    else
        drawRectBasic(-0.045f, 0.16f + bob, 0.09f, 0.025f, 0.56f, 0.40f, 0.22f);

    // Coat Buttons
    glColor3f(0.3f, 0.2f, 0.1f);
    drawCircleBasic(-0.02f, 0.14f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircleBasic(-0.02f, 0.20f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircleBasic(0.02f, 0.14f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircleBasic(0.02f, 0.20f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);

    // Collar
    glColor3f(0.66f, 0.50f, 0.32f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.015f, 0.25f + bob);
    glVertex2f(-0.055f, 0.28f + bob);
    glVertex2f(-0.05f, 0.23f + bob);
    glVertex2f(0.015f, 0.25f + bob);
    glVertex2f(0.055f, 0.28f + bob);
    glVertex2f(0.05f, 0.23f + bob);
    glEnd();

    // --- 4. HEAD & FACE ---
    float headY = 0.29f + bob;

    // Neck
    if (currentLevel == 1)
        drawLitRect(-0.018f, 0.27f + bob, 0.036f, 0.04f, skinR, skinG, skinB);
    else
        drawRectBasic(-0.018f, 0.27f + bob, 0.036f, 0.04f, skinR, skinG, skinB);

    // Face Shape
    if (currentLevel == 1)
        drawLitRect(-0.038f, headY, 0.076f, 0.08f, skinR, skinG, skinB);
    else
        drawRectBasic(-0.038f, headY, 0.076f, 0.08f, skinR, skinG, skinB);

    // Ears
    drawCircleBasic(-0.038f, headY + 0.04f, 0.008f, 6, skinR, skinG, skinB);
    drawCircleBasic(0.038f, headY + 0.04f, 0.008f, 6, skinR - 0.1f, skinG - 0.1f, skinB - 0.1f);

    // --- FACE DETAILS ---
    float eyeY = headY + 0.045f;

    // Eyes (Whites)
    drawCircleBasic(-0.018f, eyeY, 0.011f, 10, 1.0f, 1.0f, 1.0f);
    drawCircleBasic(0.018f, eyeY, 0.011f, 10, 1.0f, 1.0f, 1.0f);
    // Irises (Blue)
    drawCircleBasic(-0.016f, eyeY, 0.005f, 8, 0.2f, 0.6f, 0.8f);
    drawCircleBasic(0.020f, eyeY, 0.005f, 8, 0.2f, 0.6f, 0.8f);

    // Nose (Shadow Triangle)
    glColor3f(skinR - 0.2f, skinG - 0.2f, skinB - 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, headY + 0.035f);
    glVertex2f(-0.005f, headY + 0.02f);
    glVertex2f(0.005f, headY + 0.02f);
    glEnd();

    // MOUTH (Line + Cigarette)
    float mouthY = headY + 0.012f;
    // The Mouth Line (Grimace)
    glLineWidth(2.0f);
    glColor3f(0.4f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(-0.01f, mouthY);
    glVertex2f(0.01f, mouthY - 0.002f);
    glEnd();

    // The Cigarette (White stick + orange tip)
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

    // Smoke
    if ((int)(walkTimer * 5) % 2 == 0)
    {
        glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
        drawCircleBasic(0.035f, mouthY + 0.01f + (float)(rand() % 10) / 500.0f, 0.004f, 6, 0.9f, 0.9f, 0.9f);
    }

    // --- FEDORA HAT ---
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

    // --- 5. FOREARM & SWORD HAND ---
    glPushMatrix();
    glTranslatef(0.0f, 0.22f + bob, 0.0f);
    glRotatef(armAngle, 0.0f, 0.0f, 1.0f);

    // Sleeve
    if (currentLevel == 1)
        drawLitRect(-0.02f, -0.09f, 0.04f, 0.11f, 0.70f, 0.55f, 0.38f);
    else
        drawRectBasic(-0.02f, -0.09f, 0.04f, 0.11f, 0.70f, 0.55f, 0.38f);

    // THE HAND (Fist shape)
    drawCircleBasic(0.0f, -0.10f, 0.018f, 10, skinR, skinG, skinB);

    // --- THE SWORD ---
    glPushMatrix();
    glTranslatef(0.0f, -0.10f, 0.0f);
    glRotatef(swordAngle + 15.0f, 0.0f, 0.0f, 1.0f);

    // Hilt/Handle
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

    // Blade
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

    // Tip
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.012f, 0.355f);
    glVertex2f(0.012f, 0.355f);
    glVertex2f(0.0f, 0.39f);
    glEnd();

    // Sword Trail Effect
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

    // THE THUMB
    drawCircleBasic(0.008f, -0.095f, 0.008f, 8, skinR - 0.05f, skinG - 0.05f, skinB - 0.05f);

    glPopMatrix();
    glPopMatrix();
}

// ==========================================
// 9.5 ENEMY DRAWING FUNCTIONS (SPLIT BY LEVEL)
// ==========================================

void drawVillainLevel1(Enemy e)
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
        // Scaled Down slightly (1.8 -> 1.35)
        glScalef(1.35f, 1.35f, 1.0f);

        // 1. LEGS (Suit Trousers - Black)
        drawLitRect(-0.14f, 0.0f, 0.12f, 0.25f, 0.05f, 0.05f, 0.05f); // L Leg
        drawLitRect(0.02f, 0.0f, 0.12f, 0.25f, 0.05f, 0.05f, 0.05f);  // R Leg

        // 2. TORSO (Broad Suit Jacket - Pitch Black)
        // V-Shape: Wider at shoulders
        drawLitRect(-0.20f, 0.22f, 0.40f, 0.38f, 0.02f, 0.02f, 0.02f);
        // Shirt & Blood Red Tie
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 1.0f, 1.0f); // White shirt
        glNormal3f(0, 0, 1);
        glVertex2f(-0.08f, 0.60f);
        glVertex2f(0.08f, 0.60f);
        glVertex2f(0.0f, 0.35f);
        glEnd();
        drawLitRect(-0.025f, 0.30f, 0.05f, 0.25f, 0.6f, 0.0f, 0.0f); // Red Tie

        // 3. HEAD (Pale Corpse Skin)
        drawLitRect(-0.09f, 0.58f, 0.18f, 0.20f, 0.85f, 0.85f, 0.9f);

        // 4. EYES (Glowing Red)
        float redGlow[] = {1.0f, 0.0f, 0.0f, 1.0f};
        float noGlow[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, redGlow);
        // Angled eyes for anger
        drawLitRect(-0.07f, 0.68f, 0.05f, 0.02f, 1.0f, 0.0f, 0.0f);
        drawLitRect(0.02f, 0.68f, 0.05f, 0.02f, 1.0f, 0.0f, 0.0f);
        glMaterialfv(GL_FRONT, GL_EMISSION, noGlow);

        // 5. HANDS (Pale)
        drawLitRect(-0.25f, 0.25f, 0.10f, 0.12f, 0.85f, 0.85f, 0.9f);
        drawLitRect(0.15f, 0.25f, 0.10f, 0.12f, 0.85f, 0.85f, 0.9f);

        // 6. HEALTH BAR (Above Head)
        drawLitRect(-0.2f, 0.85f, 0.4f, 0.03f, 0.3f, 0.0f, 0.0f);
        drawLitRect(-0.2f, 0.85f, 0.4f * ((float)e.hp / (float)BOSS_MAX_HP), 0.03f, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        // Minion
        drawLitRect(-0.04f, 0.0f, 0.03f, 0.13f, 0.1f, 0.1f, 0.1f);
        drawLitRect(0.01f, 0.0f, 0.03f, 0.13f, 0.15f, 0.15f, 0.15f);
        drawLitRect(-0.06f, 0.12f, 0.12f, 0.15f, 0.5f, 0.1f, 0.1f);
        drawLitRect(-0.04f, 0.26f, 0.08f, 0.07f, 0.2f, 0.05f, 0.05f);
    }

    // Weapon
    glPushMatrix();
    glTranslatef(0.0f, (e.isBoss ? 0.35f : 0.15f), 0.0f);
    float ang = -45.0f;
    if (e.isAttacking)
        ang = (e.attackFrame < 10) ? -45.0f + (e.attackFrame * 13.0f) : 85.0f - ((e.attackFrame - 10) * 10.0f);
    glRotatef(ang, 0.0f, 0.0f, 1.0f);

    if (e.isBoss)
    {
        // WEAPON: War Scepter (Resized)
        drawLitRect(-0.02f, -0.1f, 0.04f, 0.5f, 0.1f, 0.1f, 0.1f); // Black Shaft (Shorter)
        // Head
        glDisable(GL_LIGHTING);
        glColor3f(0.8f, 0.0f, 0.0f); // Blood Crystal
        glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.55f);
        glVertex2f(0.1f, 0.45f);
        glVertex2f(0.0f, 0.35f);
        glVertex2f(-0.1f, 0.45f);
        glEnd();
        glEnable(GL_LIGHTING);
    }
    else
    {
        drawLitRect(-0.01f, -0.05f, 0.02f, 0.06f, 0.2f, 0.2f, 0.2f);
        drawLitRect(0.0f, -0.05f, 0.015f, 0.18f, 0.6f, 0.6f, 0.65f);
    }
    glPopMatrix();
    glPopMatrix();
}

void drawVillainLevel2(Enemy e)
{
    if (!e.alive)
        return;
    glPushMatrix();
    glTranslatef(e.x, e.y, 0.0f);

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
    glPopMatrix();
}

void drawVillain(Enemy e)
{
    if (currentLevel == 1)
        drawVillainLevel1(e);
    else
        drawVillainLevel2(e);
}

// ==========================================
// 10. UI & MENU (UPDATED)
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

    // Background
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(800, 0);
    glVertex2f(800, 600);
    glVertex2f(0, 600);
    glEnd();

    // Title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(300, 500, "Teri ma k BHOSDA", GLUT_BITMAP_TIMES_ROMAN_24);

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

    // --- 2. LEVEL 1 BUTTON (MIDDLE) ---
    // If Level 1 is selected, color it Orange
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

    // --- 3. LEVEL 2 BUTTON (BOTTOM) ---
    // If Level 2 is selected, color it Orange
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

    // Instructions
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
    sprintf(livesStr, "LIVES: %d   LEVEL: %d", playerLives, currentLevel);
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

        if (currentLevel == 2)
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
// 11. GAME LOGIC
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

    // Reset Level 1 enemies
    for (int i = 0; i < numEnemies1; i++)
    {
        enemies1[i].alive = true;
        enemies1[i].hp = (enemies1[i].isBoss) ? BOSS_MAX_HP : 2;
        enemies1[i].x = (enemies1[i].startX + enemies1[i].endX) / 2.0f;
        enemies1[i].velX = 0;
        enemies1[i].isAttacking = false;
        enemies1[i].attackCooldown = 0;
    }

    // Reset Level 2 enemies
    for (int i = 0; i < numEnemies2; i++)
    {
        enemies2[i].alive = true;
        enemies2[i].hp = (enemies2[i].isBoss) ? 8 : 2;
        enemies2[i].x = (enemies2[i].startX + enemies2[i].endX) / 2.0f;
        enemies2[i].velX = 0;
        enemies2[i].isAttacking = false;
        enemies2[i].attackCooldown = 0;
    }
}

void loadLevel(int level)
{
    currentLevel = level;
    resetPlayer();
    cameraX = 0.0f;
    gameState = (level == 1) ? STATE_LEVEL1 : STATE_LEVEL2;
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

    if (currentLevel == 1)
        updateLevel1();
    else
        updateLevel2();

    if (isAttacking)
    {
        attackFrame++;
        // Smooth sword swing logic
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
// 12. DISPLAY
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
    else
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

    drawUI();
    glutSwapBuffers();
}

// ==========================================
// 13. INPUT HANDLING (UPDATED WITH CORRECT MOUSE SCALING)
// ==========================================
void keyDown(unsigned char k, int x, int y)
{
    if (gameState == STATE_MENU)
    {
        if (k == 13) // ENTER
        {
            loadLevel(currentLevel);
        }
        else if (k == 'w' || k == 'W')
        {
            currentLevel = 1;
        }
        else if (k == 's' || k == 'S')
        {
            currentLevel = 2;
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
        if (k == ' ' && currentLevel == 1)
        {
            loadLevel(2);
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
            currentLevel = 1;
        else if (key == GLUT_KEY_DOWN)
            currentLevel = 2;
    }
}

// --- UPDATED MOUSE FUNCTIONS (FIXED COORDINATE SCALING) ---
void handleMouseMotion(int x, int y)
{
    // Scale the X coordinate from window space to 800-width space
    mouseX = (int)(x * (800.0f / windowWidth));

    // Scale the Y coordinate (invert first, then scale)
    int invertedY = windowHeight - y;
    mouseY = (int)(invertedY * (600.0f / windowHeight));

    // Check Start Button (Top)
    mouseInStartButton = (mouseX >= START_BUTTON_X1 && mouseX <= START_BUTTON_X2 &&
                          mouseY >= START_BUTTON_Y1 && mouseY <= START_BUTTON_Y2);

    // Check Level 1 Button (Middle)
    mouseInLevel1Button = (mouseX >= LEVEL_BUTTON_X1 && mouseX <= LEVEL_BUTTON_X2 &&
                           mouseY >= LEVEL1_BUTTON_Y1 && mouseY <= LEVEL1_BUTTON_Y2);

    // Check Level 2 Button (Bottom)
    mouseInLevel2Button = (mouseX >= LEVEL_BUTTON_X1 && mouseX <= LEVEL_BUTTON_X2 &&
                           mouseY >= LEVEL2_BUTTON_Y1 && mouseY <= LEVEL2_BUTTON_Y2);

    glutPostRedisplay();
}

void handleMousePassiveMotion(int x, int y)
{
    handleMouseMotion(x, y);
}

void handleMouseClick(int button, int state, int x, int y)
{
    // Scale X
    mouseX = (int)(x * (800.0f / windowWidth));

    // Scale Y
    int invertedY = windowHeight - y;
    mouseY = (int)(invertedY * (600.0f / windowHeight));

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseLeftDown = true;
            mouseClicked = true;

            // 1. Clicked Start Game?
            if (mouseInStartButton)
            {
                loadLevel(currentLevel); // Start the currently selected level
            }
            // 2. Clicked Level 1?
            else if (mouseInLevel1Button)
            {
                currentLevel = 1; // Select Level 1
                glutPostRedisplay();
            }
            // 3. Clicked Level 2?
            else if (mouseInLevel2Button)
            {
                currentLevel = 2; // Select Level 2
                glutPostRedisplay();
            }
        }
        else if (state == GLUT_UP)
        {
            mouseLeftDown = false;
        }
    }

    // Reset mouse clicked flag
    if (state == GLUT_UP)
    {
        mouseClicked = false;
    }
}

void reshape(int w, int h)
{
    // Store new window dimensions for mouse scaling
    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

// ==========================================
// 14. MAIN FUNCTION (UPDATED)
// ==========================================
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 700); // Initial window size
    glutCreateWindow("Teri Ma Ka BHOSDA");
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

    // Register mouse functions
    glutMouseFunc(handleMouseClick);
    glutMotionFunc(handleMouseMotion);
    glutPassiveMotionFunc(handleMousePassiveMotion);

    glutMainLoop();
    return 0;
}