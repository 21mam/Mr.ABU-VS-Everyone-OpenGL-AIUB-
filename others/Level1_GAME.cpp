#include <GLUT/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define PI 3.14159265f

// ==========================================
// 1. CONFIGURATION
// ==========================================
const float LEVEL_START_X = -1.0f;
const float LEVEL_END_X = 14.0f;
const float PIT_DEPTH = -3.0f;

// Physics Tunings (Restored Heavy Feel)
const float GRAVITY = 0.003f;
const float JUMP_FORCE = 0.065f;
const float MOVE_ACCEL = 0.003f;
const float MAX_SPEED = 0.035f;
const float FRICTION = 0.85f;
const float KNOCKBACK_X = 0.06f;
const float KNOCKBACK_Y = 0.04f;

// Dimensions
const float PLAYER_W = 0.06f;
const float PLAYER_H_STAND = 0.3f;
const float PLAYER_H_CROUCH = 0.14f;

// State
int gameState = 0;
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
float batAngle = 0.0f;
int attackFrame = 0;
float cloudTime = 0.0f;

// Inputs
bool keyA = false, keyD = false, keySpace = false, keyW = false, keyS = false;

// ==========================================
// 2. DATA STRUCTURES
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

// Level Geometry
RectObj groundSegments[] = {
    {-1.0f, -0.6f, 3.0f, 0.1f},
    {2.5f, -0.8f, 2.0f, 0.1f},
    {5.0f, -0.6f, 1.5f, 0.1f},
    {7.0f, -0.6f, 7.0f, 0.1f}};
const int numGrounds = 4;

RectObj platforms[] = {
    {0.8f, -0.2f, 0.6f, 0.05f},
    {3.0f, -0.3f, 0.4f, 0.05f},
    {5.5f, 0.0f, 0.6f, 0.05f},
    {8.0f, 0.2f, 0.8f, 0.05f},
    {10.0f, -0.2f, 0.5f, 0.05f}};
const int numPlatforms = 5;

// --- OBSTACLES (REMOVED THE ONE BLOCKING THE BOSS) ---
RectObj lowBeams[] = {
    {4.0f, -0.32f, 1.0f, 0.2f}
    // REMOVED 2nd Beam
};
const int numBeams = 1;

RectObj spikes[] = {
    {2.5f, -0.6f, 0.4f, 0.12f},
    {6.0f, -0.6f, 0.6f, 0.12f}
    // REMOVED 3rd Spike
};
const int numSpikes = 2;

Enemy enemies[] = {
    {1.0f, -0.2f, 0.0f, 0.8f, 1.4f, 0.003f, true, true, false, 2, 0, false, 0},
    {3.2f, -0.3f, 0.0f, 3.0f, 3.4f, 0.004f, true, false, false, 2, 0, false, 0},
    {6.0f, 0.0f, 0.0f, 5.5f, 6.1f, 0.003f, true, true, false, 2, 0, false, 0},
    {8.5f, -0.6f, 0.0f, 7.5f, 9.5f, 0.006f, true, false, false, 3, 0, false, 0},
    {13.0f, -0.6f, 0.0f, 12.0f, 13.8f, 0.005f, true, false, true, 8, 0, false, 0}};
const int numEnemies = 5;

// Lamp Posts (Visual Light Sources Only - Non Colliding)
float lampPosts[] = {-0.5f, 4.0f, 7.5f, 10.0f, 13.0f};
const int numLamps = 5;

// Helper: Check AABB Overlap
bool checkOverlap(RectObj A, RectObj B)
{
    return (A.x < B.x + B.w && A.x + A.w > B.x &&
            A.y < B.y + B.h && A.y + A.h > B.y);
}

// ==========================================
// 3. DRAWING HELPERS
// ==========================================

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

void drawCircleBasic(float cx, float cy, float r, int segments)
{
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

// ==========================================
// 4. BACKGROUND DRAWING
// ==========================================

void drawBrickLayer()
{
    float brickW = 0.08f;
    float brickH = 0.05f;
    float startY = -0.6f;

    // Draw background wall
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

// ==========================================
// 5. GAME OBJECT DRAWING (With Lighting)
// ==========================================

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

void drawObstacles()
{
    // Beams (Solid)
    for (int i = 0; i < numBeams; i++)
    {
        drawLitRect(lowBeams[i].x, lowBeams[i].y, lowBeams[i].w, lowBeams[i].h, 0.2f, 0.2f, 0.22f);
        drawLitRect(lowBeams[i].x, lowBeams[i].y + 0.02f, lowBeams[i].w, 0.02f, 0.3f, 0.3f, 0.35f);
        drawLitRect(lowBeams[i].x, lowBeams[i].y + lowBeams[i].h - 0.04f, lowBeams[i].w, 0.02f, 0.1f, 0.1f, 0.12f);
    }

    // Spikes (Trigger)
    for (int i = 0; i < numSpikes; i++)
    {
        glColor3f(0.5f, 0.1f, 0.1f);
        glBegin(GL_TRIANGLES);
        glNormal3f(0, 0, 1);
        float sw = spikes[i].w;
        float sx = spikes[i].x;
        float sy = spikes[i].y;
        float sh = spikes[i].h;
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
    drawLitRect(-0.03f, 0.0f, 0.03f, 0.12f, 0.1f, 0.1f, 0.1f);
    drawLitRect(0.01f, 0.0f, 0.03f, 0.12f, 0.1f, 0.1f, 0.1f);
    drawLitRect(-0.045f, 0.11f + bob, 0.10f, 0.16f, 0.15f, 0.2f, 0.3f);
    drawLitRect(-0.03f, 0.26f + bob, 0.07f, 0.07f, 0.15f, 0.2f, 0.3f);
    drawLitRect(0.01f, 0.27f + bob, 0.03f, 0.04f, 0.8f, 0.7f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f + bob, 0.0f);
    glRotatef(-20.0f - batAngle, 0.0f, 0.0f, 1.0f);
    drawLitRect(-0.015f, -0.08f, 0.03f, 0.09f, 0.15f, 0.2f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, -0.09f, 0.0f);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
    drawLitRect(-0.005f, 0.0f, 0.01f, 0.04f, 0.6f, 0.5f, 0.3f);
    drawLitRect(-0.01f, 0.04f, 0.02f, 0.18f, 0.4f, 0.25f, 0.1f);
    if (isAttacking && attackFrame > 8 && attackFrame < 12)
    {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.1f, 0.2f);
        glVertex2f(0.2f, 0.1f);
        glEnd();
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
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
    glPopMatrix();
}

void setupLighting()
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

    for (int i = 0; i < numLamps; i++)
    {
        GLenum lid = GL_LIGHT1 + i;
        GLfloat lpos[] = {lampPosts[i], 0.6f, 0.5f, 1.0f};
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
// 6. CORE LOGIC (PHYSICS RE-WRITE)
// ==========================================

void takeDamage(float amount)
{
    if (gameState == 2 || playerInvincibleTimer > 0)
        return;
    playerHealth -= amount;
    screenShake = 0.1f;
    redFlash = 0.6f;
    playerInvincibleTimer = 60;
    // Bounce UP and AWAY
    pVelY = KNOCKBACK_Y;
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
            gameState = 1;
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

void timer(int value)
{
    cloudTime += 0.0003f;

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

    // --- 1. DETERMINE STATE (CROUCH) ---
    // Check if we are currently physically blocked from standing up
    RectObj standBox = {pX - 0.03f, pY, 0.06f, PLAYER_H_STAND};
    bool forcedCrouch = false;
    for (int i = 0; i < numBeams; i++)
    {
        if (checkOverlap(standBox, lowBeams[i]))
        {
            forcedCrouch = true; // Something is above our head!
        }
    }

    // Logic: You crouch if you hold S OR if you are forced to.
    // However, you cannot jump while holding S.
    bool wantsCrouch = keyS && !isJumping;
    isCrouching = wantsCrouch || forcedCrouch;

    // --- 2. APPLY X MOVEMENT ---
    if (isCrouching)
    {
        // Allow movement while crouched, but slower
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
        pVelX *= 0.8f; // Stronger friction
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

    pX += pVelX; // Move X First

    // Map limits
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

    // --- 3. CHECK X COLLISION (BEAMS/WALLS) ---
    float currentH = isCrouching ? PLAYER_H_CROUCH : PLAYER_H_STAND;
    RectObj pBox = {pX - 0.03f, pY, 0.06f, currentH};

    for (int i = 0; i < numBeams; i++)
    {
        if (checkOverlap(pBox, lowBeams[i]))
        {
            // Hit a wall/beam on X axis
            pX -= pVelX; // Undo move to prevent sticking
            pVelX = 0;
        }
    }

    // --- 4. APPLY Y MOVEMENT ---
    // Can only jump if not holding crouch key and not forced down
    if (keySpace && !isJumping && !wantsCrouch && !forcedCrouch)
    {
        pVelY = JUMP_FORCE;
        isJumping = true;
    }
    pY += pVelY;
    pVelY -= GRAVITY;

    // Update box for Y check
    pBox.y = pY;
    pBox.x = pX - 0.03f; // Update X

    // --- 5. CHECK Y COLLISION (HEAD BONK OR LANDING) ---
    bool onSolid = false;

    // Beam Y Collision
    for (int i = 0; i < numBeams; i++)
    {
        if (checkOverlap(pBox, lowBeams[i]))
        {
            if (pVelY > 0)
            {
                // Head Bonk
                pY -= pVelY;
                pVelY = 0;
            }
            else
            {
                // Landing on top
                // Only land if we were previously above it (simplification)
                if (pY - pVelY >= lowBeams[i].y + lowBeams[i].h)
                {
                    pY = lowBeams[i].y + lowBeams[i].h;
                    isJumping = false;
                    onSolid = true;
                    pVelY = 0;
                }
                else
                {
                    // We are overlapping but not landing? Push out Y?
                    // For this simple engine, just undo Y
                    pY -= pVelY;
                }
            }
        }
    }

    // Ground Collision
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
    // Platform Collision
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

    // Camera Lerp
    float targetCamX = pX - 0.0f;
    cameraX += (targetCamX - cameraX) * 0.1f; // Smooth camera
    if (cameraX < LEVEL_START_X + 1.0f)
        cameraX = LEVEL_START_X + 1.0f;
    if (cameraX > LEVEL_END_X - 1.0f)
        cameraX = LEVEL_END_X - 1.0f;

    // --- CHECK SPIKES (DAMAGE) ---
    // Update Hitbox again after corrections
    RectObj damageBox = {pX - 0.03f, pY, 0.06f, currentH};

    for (int i = 0; i < numSpikes; i++)
    {
        if (checkOverlap(damageBox, spikes[i]))
        {
            takeDamage(15.0f);
        }
    }

    // Enemies
    RectObj weaponBox = {0, 0, 0, 0};
    if (isAttacking && attackFrame > 8 && attackFrame < 15)
    {
        float reach = 0.35f;
        weaponBox.y = pY + 0.1f;
        weaponBox.h = 0.2f;
        weaponBox.w = reach;
        weaponBox.x = facingRight ? pX : pX - reach;
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
            if (fabs(distX) < (enemies[i].isBoss ? 0.45f : 0.35f) && enemies[i].attackCooldown == 0)
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
                // FIX: Use proper KNOCKBACK_X
                enemies[i].velX = (facingRight ? KNOCKBACK_X : -KNOCKBACK_X);
                if (enemies[i].hp <= 0)
                {
                    enemies[i].alive = false;
                    screenShake = 0.05f;
                    if (enemies[i].isBoss)
                        gameState = 2;
                }
            }
        }
        if (enemies[i].isAttacking)
        {
            enemies[i].attackFrame++;
            if (enemies[i].attackFrame == 8)
            {
                float reach = enemies[i].isBoss ? 0.5f : 0.35f;
                RectObj eWeaponBox = {0, enemies[i].y + 0.1f, reach, 0.2f};
                eWeaponBox.x = enemies[i].facingRight ? enemies[i].x : enemies[i].x - reach;
                if (checkOverlap(eWeaponBox, damageBox))
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

// ==========================================
// 7. DISPLAY & UI
// ==========================================

void drawUI()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    for (int i = 0; i < numLamps; i++)
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
    sprintf(livesStr, "LIVES: %d   BOSS: %dm", playerLives, (int)(12.0f - pX));
    drawText(20, 520, livesStr);

    if (gameState == 1)
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
    if (gameState == 2)
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
        drawText(320, 350, "LEVEL COMPLETE!");
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

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if (screenShake > 0)
        glTranslatef((rand() % 100 / 5000.0f) * screenShake * 10, (rand() % 100 / 5000.0f) * screenShake * 10, 0);

    // --- 1. FIXED BACKGROUND (Sky & Moon) ---
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

    // --- 2. PARALLAX LAYERS (Based on CameraX) ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clouds
    glPushMatrix();
    float cloudScroll = -cameraX * 0.05f - cloudTime;
    float wrappedCloud = fmod(cloudScroll, 2.0f);
    glTranslatef(wrappedCloud, 0, 0);
    drawCloudLayer();
    glTranslatef(2.0f, 0, 0);
    drawCloudLayer();
    glPopMatrix();

    // Far City
    float farCityWidth = 2.2f;
    int startChunk = (int)((cameraX * 0.5f) / farCityWidth) - 1;
    for (int i = startChunk; i < startChunk + 4; i++)
    {
        glPushMatrix();
        glTranslatef(i * farCityWidth - (cameraX * 0.5f), 0, 0);
        drawFarCityChunk();
        glPopMatrix();
    }

    // Background Pillars
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

    // Fog
    drawFog();

    // Brick Wall
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

    // --- 3. GAME WORLD (Lit) ---
    glPushMatrix();
    glTranslatef(-cameraX, 0.0f, 0.0f);

    setupLighting();

    // Draw Platform Tops (Ground segments) on top of brick wall
    for (int i = 0; i < numGrounds; i++)
        drawLitRect(groundSegments[i].x, groundSegments[i].y, groundSegments[i].w, groundSegments[i].h, 0.15f, 0.15f, 0.18f);
    for (int i = 0; i < numPlatforms; i++)
        drawLitRect(platforms[i].x, platforms[i].y, platforms[i].w, platforms[i].h, 0.25f, 0.18f, 0.15f);

    for (int i = 0; i < numLamps; i++)
        drawLampPost(lampPosts[i], -0.6f);
    drawObstacles();

    for (int i = 0; i < numEnemies; i++)
        drawVillain(enemies[i]);
    drawStudent();

    // Hit effects
    if (hitEffect.life > 0)
    {
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 0);
        glPushMatrix();
        glTranslatef(hitEffect.x, hitEffect.y, 0);
        glRotatef(hitEffect.life * 25, 0, 0, 1);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.08f, -0.08f);
        glVertex2f(0.08f, -0.08f);
        glVertex2f(0, 0.08f);
        glEnd();
        glPopMatrix();
        glEnable(GL_LIGHTING);
        hitEffect.life--;
    }
    glPopMatrix();

    drawUI();
    glutSwapBuffers();
}

// ==========================================
// 8. INPUT & MAIN
// ==========================================

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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 700);
    glutCreateWindow("Gothic Detective: Final Integration");
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMainLoop();
    return 0;
}