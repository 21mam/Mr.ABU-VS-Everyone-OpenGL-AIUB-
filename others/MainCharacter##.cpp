#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <iostream>

// ==========================================
// 1. GLOBAL VARIABLES
// ==========================================

float pX = 0.0f;
float pY = -0.4f;
float pVelX = 0.0f;
bool facingRight = true;
bool isCrouching = false;
bool isJumping = false;

float walkTimer = 0.0f;
bool isAttacking = false;
int attackFrame = 0;
float armAngle = 0.0f;
float swordAngle = 0.0f;

int gameState = 0;
int playerInvincibleTimer = 0;

// ==========================================
// 2. DRAWING HELPERS
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

void drawCircle(float cx, float cy, float r, int segments, float red, float green, float blue)
{
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
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
// 3. MAIN CHARACTER CODE (REALISTIC UPDATE)
// ==========================================

void drawDetective()
{
    if (playerInvincibleTimer > 0 && (playerInvincibleTimer / 5) % 2 == 0)
        return;

    glPushMatrix();
    glTranslatef(pX, pY, 0.0f);

    if (!facingRight)
        glScalef(-1.0f, 1.0f, 1.0f);
    if (isCrouching)
        glScalef(1.0f, 0.6f, 1.0f);

    float bob = (fabs(pVelX) > 0.001f && !isJumping) ? sin(walkTimer) * 0.01f : 0.0f;

    float skinR = 0.96f, skinG = 0.80f, skinB = 0.69f;

    // --- 1. BACK ARM (The arm behind the body) ---
    float backArmSwing = sin(walkTimer) * 15.0f;
    glPushMatrix();
    glTranslatef(0.02f, 0.21f + bob, 0.0f);
    glRotatef(-backArmSwing, 0.0f, 0.0f, 1.0f);
    drawRect(-0.01f, -0.09f, 0.03f, 0.10f, 0.65f, 0.50f, 0.35f);
    drawCircle(0.005f, -0.10f, 0.012f, 10, skinR - 0.1f, skinG - 0.1f, skinB - 0.1f);
    glPopMatrix();

    // --- 2. LEGS & SHOES ---
    drawRect(-0.035f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);
    drawRect(0.005f, 0.02f, 0.03f, 0.14f, 0.15f, 0.15f, 0.18f);

    drawRect(-0.04f, 0.0f, 0.04f, 0.02f, 0.05f, 0.05f, 0.05f);
    drawTrapezoid(-0.02f, 0.02f, 0.03f, 0.045f, 0.015f, 0.05f, 0.05f, 0.05f);
    drawRect(0.0f, 0.0f, 0.04f, 0.02f, 0.05f, 0.05f, 0.05f);
    drawTrapezoid(0.02f, 0.02f, 0.03f, 0.045f, 0.015f, 0.05f, 0.05f, 0.05f);

    // --- 3. TORSO (Coat & Suit) ---

    drawRect(-0.04f, 0.13f + bob, 0.08f, 0.15f, 0.9f, 0.9f, 0.9f);
    drawRect(-0.005f, 0.18f + bob, 0.015f, 0.08f, 0.6f, 0.1f, 0.1f);

    drawTrapezoid(0.0f, 0.10f + bob, 0.10f, 0.14f, 0.19f, 0.76f, 0.60f, 0.42f);
    drawRect(-0.045f, 0.16f + bob, 0.09f, 0.025f, 0.56f, 0.40f, 0.22f);

    glColor3f(0.3f, 0.2f, 0.1f);
    drawCircle(-0.02f, 0.14f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircle(-0.02f, 0.20f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircle(0.02f, 0.14f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);
    drawCircle(0.02f, 0.20f + bob, 0.003f, 6, 0.3f, 0.2f, 0.1f);

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

    drawRect(-0.018f, 0.27f + bob, 0.036f, 0.04f, skinR, skinG, skinB);

    drawRect(-0.038f, headY, 0.076f, 0.08f, skinR, skinG, skinB);

    drawCircle(-0.038f, headY + 0.04f, 0.008f, 6, skinR, skinG, skinB);
    drawCircle(0.038f, headY + 0.04f, 0.008f, 6, skinR - 0.1f, skinG - 0.1f, skinB - 0.1f);

    float eyeY = headY + 0.045f;

    drawCircle(-0.018f, eyeY, 0.011f, 10, 1.0f, 1.0f, 1.0f);
    drawCircle(0.018f, eyeY, 0.011f, 10, 1.0f, 1.0f, 1.0f);
    drawCircle(-0.016f, eyeY, 0.005f, 8, 0.2f, 0.6f, 0.8f);
    drawCircle(0.020f, eyeY, 0.005f, 8, 0.2f, 0.6f, 0.8f);

    glColor3f(skinR - 0.2f, skinG - 0.2f, skinB - 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, headY + 0.035f);
    glVertex2f(-0.005f, headY + 0.02f);
    glVertex2f(0.005f, headY + 0.02f);
    glEnd();

    // MOUTH (Line + Cigarette)
    float mouthY = headY + 0.012f;
    glLineWidth(2.0f);
    glColor3f(0.4f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(-0.01f, mouthY);
    glVertex2f(0.01f, mouthY - 0.002f);
    glEnd();

    drawRect(0.005f, mouthY - 0.003f, 0.02f, 0.003f, 1.0f, 1.0f, 1.0f);
    drawRect(0.025f, mouthY - 0.003f, 0.005f, 0.003f, 1.0f, 0.5f, 0.0f);
    if ((int)(walkTimer * 5) % 2 == 0)
    {
        glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
        drawCircle(0.035f, mouthY + 0.01f + (float)(rand() % 10) / 500.0f, 0.004f, 6, 0.9f, 0.9f, 0.9f);
    }

    drawRect(-0.07f, headY + 0.06f, 0.14f, 0.01f, 0.35f, 0.25f, 0.2f);
    drawTrapezoid(0.0f, headY + 0.065f, 0.07f, 0.09f, 0.05f, 0.35f, 0.25f, 0.2f);
    drawRect(-0.045f, headY + 0.07f, 0.09f, 0.012f, 0.1f, 0.1f, 0.1f);

    // --- 5. FOREARM & SWORD HAND ---
    glPushMatrix();
    glTranslatef(0.0f, 0.22f + bob, 0.0f);
    glRotatef(armAngle, 0.0f, 0.0f, 1.0f);

    drawRect(-0.02f, -0.09f, 0.04f, 0.11f, 0.70f, 0.55f, 0.38f);

    drawCircle(0.0f, -0.10f, 0.018f, 10, skinR, skinG, skinB);

    glPushMatrix();
    glTranslatef(0.0f, -0.10f, 0.0f);
    glRotatef(swordAngle + 15.0f, 0.0f, 0.0f, 1.0f);

    drawRect(-0.007f, -0.04f, 0.014f, 0.08f, 0.3f, 0.1f, 0.0f);
    drawRect(-0.035f, 0.04f, 0.07f, 0.015f, 0.8f, 0.7f, 0.1f);
    drawRect(-0.01f, -0.05f, 0.02f, 0.02f, 0.8f, 0.7f, 0.1f);

    drawRect(-0.012f, 0.055f, 0.024f, 0.30f, 0.85f, 0.85f, 0.9f);
    drawRect(-0.003f, 0.055f, 0.006f, 0.28f, 0.95f, 0.95f, 1.0f);

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

    drawCircle(0.008f, -0.095f, 0.008f, 8, skinR - 0.05f, skinG - 0.05f, skinB - 0.05f);

    glPopMatrix();
    glPopMatrix();
}

// ==========================================
// 4. TEST HARNESS
// ==========================================

void display()
{
    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawDetective();

    glColor3f(0.05f, 0.05f, 0.08f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, -0.4f);
    glVertex2f(-1.0f, -0.4f);
    glEnd();

    glutSwapBuffers();
}

void timer(int value)
{
    walkTimer += 0.3f;
    pVelX = 0.02f;

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
    else
    {
        if (rand() % 90 == 0)
        {
            isAttacking = true;
            attackFrame = 0;
        }
    }

    static int turnTimer = 0;
    turnTimer++;
    if (turnTimer > 150)
    {
        facingRight = !facingRight;
        turnTimer = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-0.4, 0.4, -0.45, 0.45);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Detective Zimam");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}