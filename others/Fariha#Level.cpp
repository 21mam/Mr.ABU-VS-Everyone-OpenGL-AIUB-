#include <GLUT/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// ==========================================
// 1. CONFIGURATION & GLOBALS
// ==========================================
const float LEVEL_START_X = -1.0f;
const float LEVEL_END_X = 14.0f;
const float PIT_DEPTH = -3.0f;

// Physics Tunings
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

// BOSS LASER
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

// Boss Constants
const int BOSS_DAMAGE = 30;
const float BOSS_REACH = 0.6f;
const int BOSS_MAX_HP = 25;

// Level Geometry
RectObj groundSegments[] = {
    {-1.0f, -0.6f, 3.0f, 0.1f},
    {2.5f, -0.8f, 2.0f, 0.1f},
    {5.0f, -0.6f, 1.5f, 0.1f},
    {7.0f, -0.6f, 7.0f, 0.1f}};
const int numGrounds = 4;

// Defined Platforms
RectObj platforms[] = {
    {0.8f, -0.2f, 0.6f, 0.05f}, // Platform 0
    {3.0f, -0.3f, 0.4f, 0.05f}, // Platform 1
    {5.5f, 0.0f, 0.6f, 0.05f},  // Platform 2
    {8.0f, 0.2f, 0.8f, 0.05f},  // Platform 3
    {10.0f, -0.2f, 0.5f, 0.05f} // Platform 4
};
const int numPlatforms = 5;

// OBSTACLES (Alien Tech)
RectObj alienBeams[] = {
    {2.0f, -0.32f, 1.0f, 0.2f}};
const int numBeams = 1;

RectObj mines[] = {
    {3.5f, -0.7f, 0.4f, 0.12f},
    {6.0f, -0.5f, 0.4f, 0.12f},
    {8.5f, 0.25f, 0.3f, 0.3f}};
const int numMines = 3;

// BOSS SPIKES (Crystal Traps)
RectObj spikeTraps[] = {
    {10.5f, -0.6f, 0.8f, 0.15f}, // Left of Arena
    {13.0f, -0.6f, 0.8f, 0.15f}  // Right of Arena
};
const int numSpikes = 2;

// --- ENEMIES ---
Enemy enemies[] = {
    {1.5f, -0.35f, 0.0f, 0.5f, 1.8f, 0.003f, true, true, false, 2, 0, false, 0},
    {1.0f, 0.0f, 0.0f, 0.8f, 1.4f, 0.002f, true, false, false, 2, 0, false, 0},
    {5.8f, 0.20f, 0.0f, 5.5f, 6.1f, 0.003f, true, true, false, 2, 0, false, 0},
    {8.4f, 0.40f, 0.0f, 8.0f, 8.8f, 0.004f, true, false, false, 3, 0, false, 0},
    // BOSS
    {12.0f, -0.5f, 0.0f, 10.5f, 13.5f, 0.008f, true, false, true, BOSS_MAX_HP, 0, false, 0}};
const int numEnemies = 5;

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

void drawText(float x, float y, const char *string)
{
    glRasterPos2f(x, y);
    int len = (int)strlen(string);
    for (int i = 0; i < len; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
}

// --- ALIEN GROUND ---
void drawAlienGround(RectObj r)
{
    // 1. Dark Violet Bio-Soil (Base)
    drawLitRect(r.x, r.y, r.w, r.h, 0.2f, 0.1f, 0.3f);

    // 2. Glowing Neon Top Layer (Bio-Moss)
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.8f, 0.2f); // Bright Green
    glBegin(GL_QUADS);
    glVertex2f(r.x, r.y + r.h - 0.02f);
    glVertex2f(r.x + r.w, r.y + r.h - 0.02f);
    glVertex2f(r.x + r.w, r.y + r.h);
    glVertex2f(r.x, r.y + r.h);
    glEnd();
    glEnable(GL_LIGHTING);
}

// --- ALIEN CRYSTAL SPIKES ---
void drawAlienSpikes()
{
    for (int i = 0; i < numSpikes; i++)
    {
        float sx = spikeTraps[i].x;
        float sy = spikeTraps[i].y;
        float sw = spikeTraps[i].w;
        float sh = spikeTraps[i].h;

        glDisable(GL_LIGHTING);
        // Draw clusters of jagged crystals
        for (float k = 0; k < sw; k += 0.08f)
        {
            float pulse = sin(walkTimer * 0.2f + k * 10.0f) * 0.02f;

            // Core (Cyan)
            glColor4f(0.0f, 1.0f, 1.0f, 0.9f);
            glBegin(GL_TRIANGLES);
            glVertex2f(sx + k, sy);
            glVertex2f(sx + k + 0.06f, sy);
            glVertex2f(sx + k + 0.03f, sy + sh + pulse);
            glEnd();

            // Inner Core (White/Bright)
            glColor4f(0.8f, 1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
            glVertex2f(sx + k + 0.03f, sy);
            glVertex2f(sx + k + 0.03f, sy + sh + pulse);
            glEnd();
        }
        glEnable(GL_LIGHTING);
    }
}

// --- BOSS LASER ---
void drawLaser()
{
    if (!bossLaser.active)
        return;
    glDisable(GL_LIGHTING);
    // Core (Red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(bossLaser.x, bossLaser.y);
    glVertex2f(bossLaser.x + bossLaser.w, bossLaser.y);
    glVertex2f(bossLaser.x + bossLaser.w, bossLaser.y + bossLaser.h);
    glVertex2f(bossLaser.x, bossLaser.y + bossLaser.h);
    glEnd();
    // Glow (Orange)
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

// ==========================================
// 4. BACKGROUND & ALIEN SUN
// ==========================================
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

// ==========================================
// 5. WALKER BOT & PLAYER
// ==========================================
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

void drawAlienObstacles()
{
    for (int i = 0; i < numBeams; i++)
    {
        drawLitRect(alienBeams[i].x, alienBeams[i].y, alienBeams[i].w, alienBeams[i].h, 0.2f, 0.0f, 0.3f);
        glDisable(GL_LIGHTING);
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(alienBeams[i].x, alienBeams[i].y + 0.1f);
        glVertex2f(alienBeams[i].x + alienBeams[i].w, alienBeams[i].y + 0.1f);
        glEnd();
        glEnable(GL_LIGHTING);
    }
    for (int i = 0; i < numMines; i++)
    {
        glPushMatrix();
        glTranslatef(mines[i].x + mines[i].w / 2, mines[i].y + mines[i].h / 2, 0);
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

void drawPlayer()
{
    if (playerInvincibleTimer > 0 && (playerInvincibleTimer / 5) % 2 == 0)
        return;
    glPushMatrix();
    glTranslatef(pX, pY, 0.0f);
    if (!facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);
    if (isCrouching)
        glScalef(1.0f, 0.6f, 1.0f);
    drawLitRect(-0.03f, 0.0f, 0.03f, 0.12f, 0.9f, 0.9f, 0.9f);
    drawLitRect(0.01f, 0.0f, 0.03f, 0.12f, 0.9f, 0.9f, 0.9f);
    drawLitRect(-0.045f, 0.11f, 0.10f, 0.16f, 1.0f, 1.0f, 1.0f);
    drawLitRect(-0.02f, 0.15f, 0.05f, 0.08f, 1.0f, 0.6f, 0.0f);
    glPushMatrix();
    glTranslatef(0.005f, 0.3f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidSphere(0.05f, 15, 15);
    glTranslatef(0.02f, 0.0f, 0.02f);
    glColor3f(0.8f, 0.6f, 0.0f);
    glutSolidSphere(0.03f, 10, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    glRotatef(-20.0f - batAngle, 0.0f, 0.0f, 1.0f);
    drawLitRect(-0.015f, -0.08f, 0.03f, 0.09f, 0.2f, 0.2f, 0.2f);
    drawLitRect(-0.01f, 0.04f, 0.02f, 0.25f, 0.0f, 1.0f, 1.0f);
    glPopMatrix();
    glPopMatrix();
}

void drawAlienEnemy(Enemy *e)
{
    if (!e->alive)
        return;
    glPushMatrix();
    glTranslatef(e->x, e->y, 0.0f);
    if (e->velX > 0.01f || e->velX < -0.01f)
        glColor3f(1.0f, 0.5f, 0.5f);
    else
        glColor3f(1, 1, 1);
    if (!e->facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);

    if (e->isBoss)
    {
        // --- BOSS: ALIEN WARLORD ---
        glScalef(1.5f, 1.5f, 1.0f);

        // --- SCARY BODY (Darker Colors & Spikes) ---
        // Dark Heavy Armor
        drawLitRect(-0.08f, 0.0f, 0.06f, 0.18f, 0.1f, 0.1f, 0.1f);   // Left Leg (Black)
        drawLitRect(0.02f, 0.0f, 0.06f, 0.18f, 0.1f, 0.1f, 0.1f);    // Right Leg (Black)
        drawLitRect(-0.1f, 0.18f, 0.2f, 0.25f, 0.05f, 0.05f, 0.05f); // Chest (Dark Grey)

        // Pulsing Energy Core
        float pulse = 0.5f + 0.5f * sin(walkTimer * 0.5f);
        drawLitRect(-0.03f, 0.28f, 0.06f, 0.06f, 1.0f * pulse, 0.0f, 0.0f);

        // Shoulder Spikes
        glDisable(GL_LIGHTING);
        glBegin(GL_TRIANGLES);
        glColor3f(0.8f, 0.0f, 0.0f); // Red Spikes
        // Left Spike
        glVertex2f(-0.1f, 0.4f);
        glVertex2f(-0.2f, 0.5f);
        glVertex2f(-0.12f, 0.35f);
        // Right Spike
        glVertex2f(0.1f, 0.4f);
        glVertex2f(0.2f, 0.5f);
        glVertex2f(0.12f, 0.35f);
        glEnd();
        glEnable(GL_LIGHTING);

        // Helmet Eyes (Glowing Red Slits)
        glPushMatrix();
        glTranslatef(0.0f, 0.48f, 0.0f);
        drawLitRect(-0.06f, 0.0f, 0.12f, 0.1f, 0.1f, 0.1f, 0.1f); // Helmet
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.0f, 0.0f); // Glowing Red Eyes
        drawRectBasic(-0.04f, 0.04f, 0.03f, 0.01f, 1, 0, 0);
        drawRectBasic(0.01f, 0.04f, 0.03f, 0.01f, 1, 0, 0);
        glEnable(GL_LIGHTING);
        glPopMatrix();

        // --- NEW: BOSS HEALTH BAR ---
        // Draw Health Bar Background (Dark Red)
        drawLitRect(-0.1f, 0.65f, 0.2f, 0.02f, 0.5f, 0.0f, 0.0f);
        // Draw Health Bar Foreground (Bright Red)
        drawLitRect(-0.1f, 0.65f, 0.2f * ((float)e->hp / (float)BOSS_MAX_HP), 0.02f, 1.0f, 0.0f, 0.0f);

        // Weapon Animation
        glPushMatrix();
        glTranslatef(0.0f, 0.15f, 0.0f);
        float ang = -45.0f;
        if (e->isAttacking)
            ang = (e->attackFrame < 10) ? -45.0f + (e->attackFrame * 13.0f) : 85.0f - ((e->attackFrame - 10) * 10.0f);
        glRotatef(ang, 0.0f, 0.0f, 1.0f);
        drawLitRect(0.0f, 0.0f, 0.02f, 0.35f, 1.0f, 0.0f, 1.0f); // Purple Blade
        glPopMatrix();
    }
    else
    {
        drawWalkerBot(e->isAttacking, walkTimer);
    }
    glPopMatrix();
}

// ==========================================
// 6. LIGHTING
// ==========================================
void setupLighting(float camX)
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
// 7. CORE LOGIC & PHYSICS
// ==========================================
void takeDamage(float amount)
{
    if (gameState == 2 || playerInvincibleTimer > 0)
        return;
    playerHealth -= amount;
    screenShake = 0.1f;
    redFlash = 0.6f;
    playerInvincibleTimer = 60;
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

void fullReset()
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
    playerLives = 3;
    gameState = 0;
    cameraX = 0.0f;
    bossLaser.active = false;
    for (int i = 0; i < numEnemies; i++)
    {
        enemies[i].alive = true;
        enemies[i].hp = (enemies[i].isBoss) ? BOSS_MAX_HP : 2;
        enemies[i].x = (enemies[i].startX + enemies[i].endX) / 2.0f;
        enemies[i].velX = 0;
        enemies[i].isAttacking = false;
        enemies[i].attackCooldown = 0;
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

    walkTimer += 0.1f;
    if (screenShake > 0)
        screenShake -= 0.005f;
    else
        screenShake = 0;
    if (redFlash > 0)
        redFlash -= 0.05f;
    if (playerInvincibleTimer > 0)
        playerInvincibleTimer--;

    // Crouch Logic
    bool forcedCrouch = false;
    RectObj standBox = {pX - 0.03f, pY, 0.06f, PLAYER_H_STAND};
    for (int i = 0; i < numBeams; i++)
    {
        if (checkOverlap(standBox, alienBeams[i]))
            forcedCrouch = true;
    }
    isCrouching = (keyS && !isJumping) || forcedCrouch;

    // X Movement
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

    // Bounds & Max Speed
    if (pVelX > MAX_SPEED)
    {
        pVelX = MAX_SPEED;
    }
    if (pVelX < -MAX_SPEED)
    {
        pVelX = -MAX_SPEED;
    }
    pX += pVelX;

    // Bounds & Collision X
    if (pX < LEVEL_START_X)
        pX = LEVEL_START_X;
    if (pX > LEVEL_END_X)
        pX = LEVEL_END_X;
    float currentH = isCrouching ? PLAYER_H_CROUCH : PLAYER_H_STAND;
    RectObj pBox = {pX - 0.03f, pY, 0.06f, currentH};
    for (int i = 0; i < numBeams; i++)
    {
        if (checkOverlap(pBox, alienBeams[i]))
        {
            pX -= pVelX;
            pVelX = 0;
        }
    }

    // Y Movement
    if (keySpace && !isJumping && !isCrouching && !forcedCrouch)
    {
        pVelY = JUMP_FORCE;
        isJumping = true;
    }
    pY += pVelY;
    pVelY -= GRAVITY;

    // Collision Y
    pBox.y = pY;
    pBox.x = pX - 0.03f;
    bool onSolid = false;
    for (int i = 0; i < numBeams; i++)
    {
        if (checkOverlap(pBox, alienBeams[i]))
        {
            if (pVelY > 0)
            {
                pY -= pVelY;
                pVelY = 0;
            }
            else if (pY - pVelY >= alienBeams[i].y + alienBeams[i].h)
            {
                pY = alienBeams[i].y + alienBeams[i].h;
                isJumping = false;
                onSolid = true;
                pVelY = 0;
            }
        }
    }
    // Ground
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
    // Platforms
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

    // Camera
    cameraX += (pX - cameraX) * 0.1f;
    if (cameraX < LEVEL_START_X + 1.0f)
        cameraX = LEVEL_START_X + 1.0f;
    if (cameraX > LEVEL_END_X - 1.0f)
        cameraX = LEVEL_END_X - 1.0f;

    // Hazards
    RectObj damageBox = {pX - 0.03f, pY, 0.06f, currentH};
    for (int i = 0; i < numMines; i++)
    {
        if (checkOverlap(damageBox, mines[i]))
            takeDamage(15.0f);
    }
    // Spikes
    for (int i = 0; i < numSpikes; i++)
    {
        if (checkOverlap(damageBox, spikeTraps[i]))
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

    for (int i = 0; i < numEnemies; i++)
    {
        if (!enemies[i].alive)
            continue;
        enemies[i].x += enemies[i].velX;
        enemies[i].velX *= 0.9f;

        float distX = enemies[i].x - pX;
        bool inRange = fabs(distX) < (enemies[i].isBoss ? 0.6f : 0.35f);

        // --- BOSS AI UPDATE ---
        if (enemies[i].isBoss && !enemies[i].isAttacking && enemies[i].attackCooldown == 0 && !bossLaser.active)
        {
            // RANGE CHECK: Only fire if distance is between 0.8 and 4.0 (CLOSE RANGE)
            if (fabs(distX) > 0.8f && fabs(distX) < 4.0f)
            {
                enemies[i].attackCooldown = 120; // 2 seconds cooldown
                bossLaser.active = true;
                bossLaser.w = 0.4f;
                bossLaser.h = 0.05f;
                bossLaser.y = enemies[i].y + 0.15f;
                if (enemies[i].x > pX)
                { // Fire Left
                    enemies[i].facingRight = false;
                    bossLaser.x = enemies[i].x - 0.4f;
                    bossLaser.velX = -0.1f; // Fast projectile
                }
                else
                { // Fire Right
                    enemies[i].facingRight = true;
                    bossLaser.x = enemies[i].x;
                    bossLaser.velX = 0.1f;
                }
            }
        }

        if (fabs(enemies[i].velX) < 0.001f && !enemies[i].isAttacking)
        {
            if (inRange && enemies[i].attackCooldown == 0)
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

        // Player hitting Enemy
        if (isAttacking && attackFrame > 8 && attackFrame < 15)
        {
            float eW = enemies[i].isBoss ? 0.2f : 0.1f;
            float eH = enemies[i].isBoss ? 0.4f : 0.2f;
            RectObj enemyBox = {enemies[i].x - eW / 2, enemies[i].y, eW, eH};

            if (checkOverlap(weaponBox, enemyBox))
            {
                enemies[i].hp--;
                hitEffect.x = enemies[i].x;
                hitEffect.y = enemies[i].y + 0.2f;
                hitEffect.life = 5;
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

        // Enemy hitting Player (Melee)
        if (enemies[i].isAttacking)
        {
            enemies[i].attackFrame++;
            if (enemies[i].attackFrame == 8)
            {
                float reach = enemies[i].isBoss ? 0.6f : 0.35f;
                RectObj eWeaponBox = {0, enemies[i].y + 0.1f, reach, 0.2f};
                eWeaponBox.x = enemies[i].facingRight ? enemies[i].x : enemies[i].x - reach;
                if (checkOverlap(eWeaponBox, damageBox))
                    takeDamage(enemies[i].isBoss ? BOSS_DAMAGE : 15.0f);
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
// 8. DISPLAY
// ==========================================
void display()
{
    glClearColor(0.2f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if (screenShake > 0)
        glTranslatef((rand() % 100 / 5000.0f) * screenShake * 10, (rand() % 100 / 5000.0f) * screenShake * 10, 0);

    // BACKGROUND
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

    // GAME WORLD
    glPushMatrix();
    glTranslatef(-cameraX, 0.0f, 0.0f);
    setupLighting(cameraX);

    // Use New Alien Ground Drawing for Ground Segments
    for (int i = 0; i < numGrounds; i++)
        drawAlienGround(groundSegments[i]);
    // Alien Platforms (Use same style)
    for (int i = 0; i < numPlatforms; i++)
        drawAlienGround(platforms[i]);

    drawAlienSpikes(); // Draw New Alien Spikes
    drawAlienObstacles();
    drawLaser(); // Draw Laser

    for (int i = 0; i < numEnemies; i++)
        drawAlienEnemy(&enemies[i]);
    drawPlayer();

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

    // UI
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
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

    // --- UPDATED UI: LIVES AND BOSS DISTANCE ---
    glColor3f(1.0f, 1.0f, 1.0f); // Set text color to White
    char livesStr[50];
    int bossDistance = (int)(12.0f - pX);
    if (bossDistance < 0)
        bossDistance = 0; // Clamp distance to 0 if passed
    sprintf(livesStr, "LIVES: %d   BOSS: %dm", playerLives, bossDistance);
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
        drawText(350, 300, "MISSION FAILED");
        glColor3f(1, 1, 1);
        drawText(300, 270, "Press SPACE to Restart");
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
        drawText(320, 350, "MARS SECURED!");
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
    glutSwapBuffers();
}

// ==========================================
// 9. MAIN
// ==========================================
void keyDown(unsigned char k, int x, int y)
{
    if (gameState == 1)
    {
        if (k == ' ')
            fullReset();
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
    glutCreateWindow("Mars Rescue Mission");
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