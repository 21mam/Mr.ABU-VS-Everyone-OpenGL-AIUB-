#include <GLUT/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// ==========================================
// 1. CONFIGURATION
// ==========================================
const float LEVEL_START_X = -1.0f;
const float LEVEL_END_X = 14.0f;
const float PIT_DEPTH = -3.0f;

// Physics Constants
const float GRAVITY = 0.003f;
const float JUMP_FORCE = 0.065f;
const float MOVE_ACCEL = 0.003f;
const float MAX_SPEED = 0.035f;
const float FRICTION = 0.85f;
const float KNOCKBACK_FORCE = 0.04f;

// Game States: 0 = Playing, 1 = Game Over, 2 = Level Complete
int gameState = 0;

// Camera & Effects
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

// Animation / Actions
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

// Level Data
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

Enemy enemies[] = {
    {1.0f, -0.2f, 0.0f, 0.8f, 1.4f, 0.003f, true, true, false, 2, 0, false, 0},
    {3.2f, -0.3f, 0.0f, 3.0f, 3.4f, 0.004f, true, false, false, 2, 0, false, 0},
    {6.0f, 0.0f, 0.0f, 5.5f, 6.1f, 0.003f, true, true, false, 2, 0, false, 0},
    {8.5f, -0.6f, 0.0f, 7.5f, 9.5f, 0.006f, true, false, false, 3, 0, false, 0},
    {12.0f, -0.6f, 0.0f, 10.5f, 13.5f, 0.005f, true, false, true, 8, 0, false, 0}};
const int numEnemies = 5;

// ==========================================
// 3. BACKGROUND DRAWING FUNCTIONS (YOUR CODE)
// ==========================================

const float PI = 3.14159265f;

void drawCircle(float x, float y, float radius, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++)
    {
        float theta = i * PI / 180;
        glVertex2f(x + radius * cos(theta), y + radius * sin(theta));
    }
    glEnd();
}

void drawRect(float x, float y, float width, float height, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawRoof(float x, float y, float width, float height, float overhang, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(x - overhang, y);
    glVertex2f(x + width + overhang, y);
    glVertex2f(x + width - (overhang / 2), y + height);
    glVertex2f(x + (overhang / 2), y + height);
    glEnd();
}

void drawLantern(float x, float y)
{
    // String
    glColor3f(0.2f, 0.1f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 0.015f);
    glVertex2f(x, y);
    glEnd();

    // Lantern body
    drawCircle(x, y - 0.005f, 0.008f, 1.0f, 0.6f, 0.1f);
    drawCircle(x, y - 0.005f, 0.004f, 1.0f, 0.9f, 0.5f);
}

void drawPagoda(float x, float y)
{
    float baseW = 0.06f;
    float floorH = 0.03f;
    float roofH = 0.015f;
    float overhang = 0.015f;

    float woodR = 0.25f, woodG = 0.15f, woodB = 0.1f;
    float roofR = 0.2f, roofG = 0.18f, roofB = 0.15f;

    for (int i = 0; i < 3; i++)
    {
        float currentY = y + (i * (floorH + roofH));
        float shrink = i * 0.005f;

        // Walls
        drawRect(x + shrink, currentY, baseW - (shrink * 2), floorH, woodR, woodG, woodB);

        // Windows/Details
        drawRect(x + shrink + 0.005f, currentY + 0.005f, 0.01f, 0.015f, 0.1f, 0.05f, 0.0f);
        drawRect(x + baseW - shrink - 0.015f, currentY + 0.005f, 0.01f, 0.015f, 0.1f, 0.05f, 0.0f);

        // Roof
        drawRoof(x + shrink, currentY + floorH, baseW - (shrink * 2), roofH, overhang, roofR, roofG, roofB);

        // Lanterns
        if (i < 2)
        {
            drawLantern(x + shrink - overhang + 0.002f, currentY + floorH);
            drawLantern(x + baseW - shrink + overhang - 0.002f, currentY + floorH);
        }
    }

    // Spire on top
    float topY = y + (3 * (floorH + roofH));
    drawRect(x + (baseW / 2) - 0.003f, topY, 0.006f, 0.025f, 0.1f, 0.1f, 0.1f);
    drawCircle(x + (baseW / 2), topY + 0.025f, 0.005f, 0.1f, 0.1f, 0.1f);
}

void drawTemple(float x, float y)
{
    float w = 0.1f;
    float h = 0.05f;

    // Main Hall
    drawRect(x, y, w, h, 0.3f, 0.2f, 0.15f);

    // Central Door
    drawRect(x + 0.035f, y, 0.03f, 0.035f, 0.1f, 0.05f, 0.0f);

    // Windows
    drawRect(x + 0.01f, y + 0.015f, 0.015f, 0.015f, 0.1f, 0.05f, 0.0f);
    drawRect(x + w - 0.025f, y + 0.015f, 0.015f, 0.015f, 0.1f, 0.05f, 0.0f);

    // Massive Roof
    drawRoof(x - 0.01f, y + h, w + 0.02f, 0.03f, 0.02f, 0.25f, 0.2f, 0.18f);

    // Second smaller roof
    drawRect(x + 0.03f, y + h + 0.03f, 0.04f, 0.015f, 0.3f, 0.2f, 0.15f);
    drawRoof(x + 0.03f, y + h + 0.045f, 0.04f, 0.015f, 0.01f, 0.25f, 0.2f, 0.18f);
}

void drawWatchTower(float x, float y)
{
    // Legs
    glColor3f(0.2f, 0.1f, 0.05f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + 0.01f, y + 0.08f);
    glVertex2f(x + 0.04f, y);
    glVertex2f(x + 0.03f, y + 0.08f);

    // Cross bracing
    glVertex2f(x + 0.005f, y + 0.04f);
    glVertex2f(x + 0.035f, y + 0.04f);
    glVertex2f(x + 0.002f, y + 0.02f);
    glVertex2f(x + 0.038f, y + 0.02f);
    glEnd();

    // Platform hut
    drawRect(x + 0.005f, y + 0.08f, 0.03f, 0.03f, 0.25f, 0.15f, 0.1f);
    drawRect(x + 0.015f, y + 0.09f, 0.01f, 0.01f, 0.0f, 0.0f, 0.0f); // Window

    // Roof
    drawRoof(x + 0.005f, y + 0.11f, 0.03f, 0.015f, 0.01f, 0.2f, 0.18f, 0.15f);
}

void drawAsianVillageBackground(float cameraX)
{
    // Sky Gradient (Sunset colors)
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.8f, 0.2f); // Top - Bright Gold
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);

    glColor3f(1.0f, 0.6f, 0.1f); // Bottom - Orange Sunset
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Sun (right side of sky)
    drawCircle(0.7f, 0.7f, 0.15f, 1.0f, 0.9f, 0.3f);

    // Clouds (with parallax)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.9f, 0.7f, 0.4f);

    float cloudSpeed = 0.3f;
    for (int i = -2; i < 4; i++)
    {
        float cloudX = fmod((i * 0.6f) + cameraX * cloudSpeed, 2.0f) - 1.0f;
        if (cloudX > -1.2f && cloudX < 1.2f)
        {
            drawCircle(cloudX, 0.8f, 0.05f, 1.0f, 0.9f, 0.7f);
            drawCircle(cloudX + 0.06f, 0.8f, 0.07f, 1.0f, 0.9f, 0.7f);
            drawCircle(cloudX + 0.12f, 0.8f, 0.05f, 1.0f, 0.9f, 0.7f);
        }
    }
    glDisable(GL_BLEND);

    // Distant Hills (silhouette)
    glColor3f(0.6f, 0.45f, 0.2f);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f, -0.3f);
    glVertex2f(-0.5f, -0.1f);
    glVertex2f(0.0f, -0.2f);
    glVertex2f(0.5f, 0.0f);
    glVertex2f(1.0f, -0.1f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Village buildings (with parallax)
    float buildingSpeed = 0.2f;

    // Draw multiple copies for seamless scrolling
    for (int i = -2; i < 4; i++)
    {
        float buildingX = fmod((i * 1.5f) + cameraX * buildingSpeed, 3.0f) - 1.5f;

        if (buildingX > -1.5f && buildingX < 1.5f)
        {
            // Pagoda on left
            drawPagoda(buildingX - 0.7f, -0.55f);

            // Temple in middle
            drawTemple(buildingX - 0.1f, -0.6f);

            // Hut on right
            drawRect(buildingX + 0.6f, -0.55f, 0.06f, 0.04f, 0.3f, 0.2f, 0.1f);
            drawRoof(buildingX + 0.6f, -0.51f, 0.06f, 0.02f, 0.01f, 0.25f, 0.2f, 0.15f);
            drawLantern(buildingX + 0.63f, -0.51f);

            // Watchtower
            drawWatchTower(buildingX + 0.85f, -0.55f);
        }
    }

    // Forest silhouette (jagged trees)
    glColor3f(0.25f, 0.15f, 0.1f);
    glBegin(GL_TRIANGLES);
    for (int i = -20; i < 20; i++)
    {
        float treeX = (i * 0.1f) + fmod(cameraX * 0.1f, 0.2f);
        if (treeX > -1.0f && treeX < 1.0f)
        {
            float height = 0.1f + ((i % 5) * 0.02f);
            glVertex2f(treeX, -0.4f);
            glVertex2f(treeX + 0.05f, -0.4f + height);
            glVertex2f(treeX + 0.1f, -0.4f);
        }
    }
    glEnd();
}

// ==========================================
// 4. HELPER FUNCTIONS
// ==========================================

bool checkOverlap(RectObj A, RectObj B)
{
    return (A.x < B.x + B.w && A.x + A.w > B.x &&
            A.y < B.y + B.h && A.y + A.h > B.y);
}

// ==========================================
// 5. PLAYER & ENEMY DRAWING
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
    drawRect(-0.03f, 0.26f + bob, 0.07f, 0.07f, 0.15f, 0.2f, 0.3f);
    drawRect(0.01f, 0.27f + bob, 0.03f, 0.04f, 0.8f, 0.7f, 0.6f);

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
        drawRect(-0.05f, 0.0f, 0.04f, 0.15f, 0.1f, 0.1f, 0.1f);
        drawRect(0.01f, 0.0f, 0.04f, 0.15f, 0.1f, 0.1f, 0.1f);
        drawRect(-0.07f, 0.15f, 0.14f, 0.18f, 0.8f, 0.6f, 0.1f);
        drawRect(-0.05f, 0.32f, 0.1f, 0.09f, 0.1f, 0.1f, 0.1f);
        drawRect(-0.1f, 0.45f, 0.2f, 0.02f, 0.5f, 0.0f, 0.0f);
        float hpPct = (float)e.hp / 8.0f;
        drawRect(-0.1f, 0.45f, 0.2f * hpPct, 0.02f, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        drawRect(-0.04f, 0.0f, 0.03f, 0.13f, 0.1f, 0.1f, 0.1f);
        drawRect(0.01f, 0.0f, 0.03f, 0.13f, 0.15f, 0.15f, 0.15f);
        drawRect(-0.06f, 0.12f, 0.12f, 0.15f, 0.5f, 0.1f, 0.1f);
        drawRect(-0.04f, 0.26f, 0.08f, 0.07f, 0.2f, 0.05f, 0.05f);
    }

    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    float enemyBatAngle = -45.0f;
    if (e.isAttacking)
    {
        if (e.attackFrame < 10)
            enemyBatAngle = -45.0f + (e.attackFrame * 13.0f);
        else
            enemyBatAngle = 85.0f - ((e.attackFrame - 10) * 10.0f);
    }
    glRotatef(enemyBatAngle, 0.0f, 0.0f, 1.0f);
    drawRect(-0.01f, -0.05f, 0.02f, 0.06f, 0.2f, 0.2f, 0.2f);
    drawRect(0.0f, -0.05f, 0.015f, 0.18f, 0.6f, 0.6f, 0.65f);
    glPopMatrix();
    glPopMatrix();
}

// ==========================================
// 6. GAME LOGIC
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

    if (isCrouching)
    {
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
    }

    if (pVelX > MAX_SPEED)
        pVelX = MAX_SPEED;
    if (pVelX < -MAX_SPEED)
        pVelX = -MAX_SPEED;

    if (!keyA && !keyD)
    {
        pVelX *= FRICTION;
        walkTimer = 0.0f;
    }
    pX += pVelX;

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

    float targetCamX = pX - 0.0f;
    cameraX = targetCamX;
    if (cameraX < LEVEL_START_X + 1.0f)
        cameraX = LEVEL_START_X + 1.0f;
    if (cameraX > LEVEL_END_X - 1.0f)
        cameraX = LEVEL_END_X - 1.0f;

    // Hitbox Logic
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
                {
                    takeDamage(enemies[i].isBoss ? 30.0f : 15.0f);
                }
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
// 7. UI & MAIN DISPLAY
// ==========================================

void drawText(float x, float y, const char *string, void *font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    int len = (int)strlen(string);
    for (int i = 0; i < len; i++)
        glutBitmapCharacter(font, string[i]);
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
    char livesStr[50];
    sprintf(livesStr, "LIVES: %d   BOSS DIST: %dm", playerLives, (int)(12.0f - pX));
    drawText(20, 520, livesStr);

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

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (screenShake > 0)
    {
        glTranslatef((rand() % 100 / 5000.0f) * (screenShake * 10),
                     (rand() % 100 / 5000.0f) * (screenShake * 10), 0);
    }

    glPushMatrix();
    glTranslatef(-cameraX, 0.0f, 0.0f);

    // Draw your Asian Village background
    drawAsianVillageBackground(cameraX);

    // Draw game platforms and ground
    for (int i = 0; i < numGrounds; i++)
    {
        drawRect(groundSegments[i].x, groundSegments[i].y, groundSegments[i].w, groundSegments[i].h, 0.15f, 0.15f, 0.18f);
        drawRect(groundSegments[i].x, groundSegments[i].y + groundSegments[i].h - 0.02f, groundSegments[i].w, 0.02f, 0.3f, 0.3f, 0.35f);
    }

    for (int i = 0; i < numPlatforms; i++)
    {
        drawRect(platforms[i].x, platforms[i].y, platforms[i].w, platforms[i].h, 0.25f, 0.18f, 0.15f);
        glColor3f(0.1f, 0.08f, 0.05f);
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        for (float r = platforms[i].x + 0.05f; r < platforms[i].x + platforms[i].w; r += 0.1f)
        {
            glVertex2f(r, platforms[i].y + platforms[i].h / 2);
        }
        glEnd();
    }

    // Draw pit
    drawRect(LEVEL_START_X, PIT_DEPTH, LEVEL_END_X - LEVEL_START_X, 1.0f, 0.3f, 0.15f, 0.1f);

    // Draw enemies
    for (int i = 0; i < numEnemies; i++)
        drawVillain(enemies[i]);

    // Draw player
    drawStudent();

    // Draw hit effects
    drawHitMarker();

    glPopMatrix();

    // Draw UI
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
    glutInitWindowSize(1200, 700);
    glutCreateWindow("Student Detective: Street Fighter - Asian Village Edition");
    glDisable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMainLoop();
    return 0;
}