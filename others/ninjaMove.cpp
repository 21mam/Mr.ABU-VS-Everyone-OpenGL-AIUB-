#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

// --- CONSTANTS ---
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float PI = 3.14159265359f;

// --- ENUMS & STRUCTS ---

enum AnimationState
{
    IDLE,
    WALKING,
    PUNCHING, // Uses Katana
    KICKING,
    JUMPING,
    CROUCHING
};

// The Ninja now has the exact same physical properties as Jack
struct Ninja
{
    float x, y;
    float width, height;
    float velocityX, velocityY;
    float scaleX; // For facing direction
    bool isGrounded;
    bool facingRight;
    AnimationState state;
    AnimationState prevState;
    int currentFrame;
    int frameDelay;
    int frameCounter;
    float health;
    int attackCounter;

    // Animation frame counts
    int idleFrames;
    int walkFrames;
    int punchFrames;
    int kickFrames;
    int jumpFrames;
    int crouchFrames;
};

struct Game
{
    Ninja ninja;
    float groundLevel;
    bool keys[256];
};

Game game;

// --- INITIALIZATION ---

void initNinja()
{
    Ninja *n = &game.ninja;

    n->x = 100.0f;
    n->y = game.groundLevel;
    // Ninja is slightly slimmer/shorter than Jack, but hitbox is similar
    n->width = 60.0f;
    n->height = 130.0f;
    n->velocityX = 0.0f;
    n->velocityY = 0.0f;
    n->scaleX = 1.0f;
    n->isGrounded = true;
    n->facingRight = true;
    n->state = IDLE;
    n->prevState = IDLE;
    n->currentFrame = 0;
    n->frameDelay = 5;
    n->frameCounter = 0;
    n->attackCounter = 0;
    n->health = 100.0f;

    // Frame counts for the state machine
    n->idleFrames = 4;
    n->walkFrames = 6;
    n->punchFrames = 5; // Matches the katana swing logic
    n->kickFrames = 4;
    n->jumpFrames = 1;
    n->crouchFrames = 1;
}

void initGame()
{
    game.groundLevel = 100.0f;
    for (int i = 0; i < 256; i++)
    {
        game.keys[i] = false;
    }
    initNinja();
}

// --- BASIC DRAWING HELPERS ---

void drawCircle(float cx, float cy, float r, int num_segments)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++)
    {
        float theta = 2.0f * PI * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
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

// --- NINJA SPECIFIC DRAWING (OUTFIT PRESERVED) ---

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

    // Limb segment
    glBegin(GL_QUADS);
    glVertex2f(x1 + px, y1 + py);
    glVertex2f(x1 - px, y1 - py);
    glVertex2f(x2 - px, y2 - py);
    glVertex2f(x2 + px, y2 + py);
    glEnd();

    // Joint
    glPushMatrix();
    float jointX = x1 + dx * length * 0.6f;
    float jointY = y1 + dy * length * 0.6f;
    drawCircle(jointX, jointY, thickness * 1.2f, 12);
    glPopMatrix();

    // Hand/Foot
    glPushMatrix();
    glTranslatef(x2, y2, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    if (isArm)
    {
        glBegin(GL_QUADS); // Hand
        glVertex2f(-thickness * 1.5f, -thickness * 0.5f);
        glVertex2f(thickness * 1.5f, -thickness * 0.5f);
        glVertex2f(thickness * 1.0f, thickness * 0.5f);
        glVertex2f(-thickness * 1.0f, thickness * 0.5f);
        glEnd();
    }
    else
    {
        glBegin(GL_TRIANGLE_FAN); // Foot
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

    // Blade
    glColor3f(0.8f, 0.8f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(-length * 0.05f, 0.0f);
    glVertex2f(length * 0.05f, 0.0f);
    glVertex2f(length * 0.03f, -length);
    glVertex2f(-length * 0.03f, -length);
    glEnd();

    // Handle
    glColor3f(0.4f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-length * 0.08f, 0.0f);
    glVertex2f(length * 0.08f, 0.0f);
    glVertex2f(length * 0.08f, length * 0.25f);
    glVertex2f(-length * 0.08f, length * 0.25f);
    glEnd();
    glPopMatrix();
}

// --- NINJA MAIN DRAW ---
// This function now uses the Ninja struct (Jack's logic) to determine angles
void drawNinja()
{
    Ninja *n = &game.ninja;

    // Apply global transformations for position and facing direction
    glPushMatrix();
    glTranslatef(n->x + n->width / 2, n->y, 0);
    glScalef(n->scaleX, 1.0f, 1.0f); // Flip horizontally if facing left
    // We draw relative to (0,0) being the center bottom of the ninja inside this pushMatrix

    // Scale factor for the drawing primitives
    float s = 40.0f; // Adjusted scale to fit ~130px height

    // Calculate animation variables based on State
    float legAngle = 0.0f;
    float headTilt = 0.0f;
    float footRotation = 0.0f;
    float armAngleRight = 0.0f;
    float armAngleLeft = 0.0f;
    float bodyYOffset = 0.0f;

    // State Machine for Visuals
    switch (n->state)
    {
    case IDLE:
    {
        float breathe = sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f);
        armAngleRight = 10.0f + breathe * 5.0f;
        armAngleLeft = -10.0f - breathe * 5.0f;
        headTilt = breathe * 2.0f;
        break;
    }
    case WALKING:
    {
        // Sync with currentFrame instead of generic time for tight control
        float runCycle = n->currentFrame * 0.8f;
        legAngle = sin(runCycle) * 40.0f;
        headTilt = sin(runCycle * 0.5f) * 5.0f;
        footRotation = sin(runCycle) * 30.0f;

        float armBase = sin(runCycle) * 40.0f;
        armAngleRight = armBase;
        armAngleLeft = -armBase;
        break;
    }
    case PUNCHING: // Katana Slash
    {
        float progress = (float)n->attackCounter;
        // Map the attack counter (0-20) to arm angles
        if (progress < 5)
            armAngleRight = 120.0f + (progress * 10.0f); // Wind up
        else if (progress < 12)
            armAngleRight = 170.0f - ((progress - 5) * 25.0f); // Slash down
        else
            armAngleRight = 0.0f; // Recover

        armAngleLeft = -30.0f;
        legAngle = 20.0f; // Wide stance
        break;
    }
    case KICKING:
    {
        armAngleRight = -45.0f;
        armAngleLeft = 45.0f;
        // High kick logic
        legAngle = 90.0f; // Used as a flag for the limb drawer below
        break;
    }
    case JUMPING:
    {
        legAngle = 10.0f;
        footRotation = 20.0f;
        armAngleRight = 140.0f; // Arms up
        armAngleLeft = 140.0f;
        bodyYOffset = 20.0f; // Tuck legs
        break;
    }
    case CROUCHING:
    {
        bodyYOffset = -30.0f; // Lower body
        legAngle = 45.0f;     // Bend knees
        armAngleRight = 20.0f;
        armAngleLeft = -20.0f;
        break;
    }
    }

    float legRad = legAngle * PI / 180.0f;
    float legSwing = sin(legRad) * 0.4f;

    // --- BODY DRAWING (Outfit Colors Preserved) ---

    glPushMatrix();
    glTranslatef(0, 80 + bodyYOffset, 0); // Center of torso

    // Torso (Dark Blue)
    glColor3f(0.05f, 0.05f, 0.08f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.6f, s * 0.1f);
    glVertex2f(s * 0.6f, s * 0.1f);
    glVertex2f(s * 0.4f, -s * 1.8f);
    glVertex2f(-s * 0.4f, -s * 1.8f);
    glEnd();

    // Belt/Sash (Red)
    glColor3f(0.7f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.7f, -s * 1.0f);
    glVertex2f(s * 0.7f, -s * 1.0f);
    glVertex2f(s * 0.7f, -s * 1.2f);
    glVertex2f(-s * 0.7f, -s * 1.2f);
    glEnd();

    // Scarf/Accessory
    glColor3f(0.1f, 0.1f, 0.15f);
    glPushMatrix();
    glTranslatef(s * 0.3f, -s * 0.8f, 0.0f);
    glRotatef(10.0f, 0.0f, 0.0f, 1.0f);
    // Scarf tails
    glBegin(GL_QUADS);
    glVertex2f(-s * 0.1f, 0.0f);
    glVertex2f(s * 0.1f, 0.0f);
    glVertex2f(s * 0.06f, -s * 2.0f);
    glVertex2f(-s * 0.06f, -s * 2.0f);
    glEnd();
    glPopMatrix();

    glPopMatrix(); // End Body Frame

    // --- LEGS ---
    glColor3f(0.08f, 0.08f, 0.12f);
    float hipY = 20.0f + bodyYOffset;

    if (n->state == KICKING)
    {
        // Special drawing for High Kick
        // Right Leg (Kicking)
        drawBossLimb(s * 0.3f, hipY, s * 1.5f, hipY + s * 1.5f, s * 0.15f, false, 45.0f);
        // Left Leg (Standing)
        drawBossLimb(-s * 0.3f, hipY, -s * 0.3f, 0, s * 0.15f, false, 0.0f);
    }
    else if (n->state == CROUCHING)
    {
        // Squatting legs
        drawBossLimb(-s * 0.3f, hipY, -s * 0.8f, 0, s * 0.15f, false, 0.0f);
        drawBossLimb(s * 0.3f, hipY, s * 0.8f, 0, s * 0.15f, false, 0.0f);
    }
    else
    {
        // Walking / Idle / Jumping
        float leftLegRot = legSwing * 30.0f;
        float rightLegRot = -legSwing * 30.0f;

        // Left Leg
        drawBossLimb(-s * 0.3f, hipY, -s * 0.3f - sin(legRad * 0.5f) * s * 0.5f, 0.0f + (n->state == JUMPING ? 20 : 0), s * 0.15f, false, leftLegRot + footRotation);
        // Right Leg
        drawBossLimb(s * 0.3f, hipY, s * 0.3f + sin(legRad * 0.5f + PI) * s * 0.5f, 0.0f + (n->state == JUMPING ? 30 : 0), s * 0.15f, false, rightLegRot - footRotation);
    }

    // --- ARMS ---
    float shoulderY = 80.0f + bodyYOffset;
    float armSwingRight = sin(armAngleRight * PI / 180.0f) * 0.3f;
    float armSwingLeft = sin(armAngleLeft * PI / 180.0f) * 0.3f;

    glColor3f(0.05f, 0.05f, 0.08f);

    // Left Arm (Back)
    drawBossLimb(s * 0.6f, shoulderY, s * 1.2f, shoulderY - s * 1.5f - armSwingLeft * s, s * 0.12f, true, armAngleLeft);

    // Right Arm (Front/Holding Sword)
    float handX = -s * 1.2f - (n->state == PUNCHING ? 0.2f : 0.0f);
    float handY = shoulderY - s * 1.5f + armSwingRight * s;

    // Override Hand position for specific animations if needed
    if (n->state == PUNCHING)
    {
        handX = -s * 1.0f * cos(armAngleRight * PI / 180.0f);
        handY = shoulderY + s * 1.0f * sin(armAngleRight * PI / 180.0f);
    }

    drawBossLimb(-s * 0.6f, shoulderY, handX, handY, s * 0.12f, true, armAngleRight);

    // --- KATANA ---
    // Only draw katana in hand
    float swordRot = -90.0f + armAngleRight;
    drawBossKatana(handX, handY, s * 2.5f, swordRot);

    // --- HEAD ---
    glPushMatrix();
    glTranslatef(0.0f, 95.0f + bodyYOffset, 0.0f);
    glRotatef(headTilt, 0.0f, 0.0f, 1.0f);

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

    // Red Headband
    glColor3f(0.9f, 0.1f, 0.1f);
    glLineWidth(4.0f);
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

    glPopMatrix(); // End Head

    glPopMatrix(); // End Global Ninja Transform
}

// --- LOGIC / PHYSICS (EXACTLY LIKE JACK) ---

void updateNinja()
{
    Ninja *n = &game.ninja;

    n->prevState = n->state;

    // Apply gravity
    if (!n->isGrounded)
    {
        n->velocityY -= 0.5f;
    }

    n->x += n->velocityX;
    n->y += n->velocityY;

    // Ground Collision
    if (n->y <= game.groundLevel)
    {
        n->y = game.groundLevel;
        n->velocityY = 0.0f;
        n->isGrounded = true;

        if (n->state == JUMPING)
        {
            n->state = IDLE;
            n->currentFrame = 0;
        }
    }

    // Boundary checking
    if (n->x < 0)
        n->x = 0;
    if (n->x > SCREEN_WIDTH - n->width)
        n->x = SCREEN_WIDTH - n->width;

    bool canMove = (n->state != PUNCHING && n->state != KICKING) || !n->isGrounded;

    // Input Handling
    if ((game.keys['a'] || game.keys['A']) && canMove)
    {
        n->velocityX = -5.0f;
        n->facingRight = false;
        n->scaleX = -1.0f;
        if (n->isGrounded && n->state != JUMPING && n->state != CROUCHING)
        {
            n->state = WALKING;
        }
    }
    else if ((game.keys['d'] || game.keys['D']) && canMove)
    {
        n->velocityX = 5.0f;
        n->facingRight = true;
        n->scaleX = 1.0f;
        if (n->isGrounded && n->state != JUMPING && n->state != CROUCHING)
        {
            n->state = WALKING;
        }
    }
    else
    {
        n->velocityX = 0.0f;
        if (n->isGrounded && n->state == WALKING && n->state != CROUCHING)
        {
            n->state = IDLE;
        }
    }

    // Jump
    if ((game.keys['w'] || game.keys['W'] || game.keys[' ']) && n->isGrounded &&
        n->state != PUNCHING && n->state != KICKING)
    {
        n->velocityY = 15.0f;
        n->isGrounded = false;
        n->state = JUMPING;
        n->currentFrame = 0;
    }

    // Crouch
    if ((game.keys['s'] || game.keys['S']) && n->isGrounded &&
        n->state != PUNCHING && n->state != KICKING)
    {
        n->state = CROUCHING;
        n->velocityX = 0;
    }
    else if (n->state == CROUCHING && !(game.keys['s'] || game.keys['S']))
    {
        n->state = IDLE;
    }

    // Attack 1 (Katana) - Mapped to J
    if ((game.keys['j'] || game.keys['J']) && n->state != PUNCHING &&
        n->state != KICKING && n->isGrounded)
    {
        n->state = PUNCHING;
        n->currentFrame = 0;
        n->attackCounter = 0;
        n->velocityX = 0;
    }

    // Attack 2 (Kick) - Mapped to K
    if ((game.keys['k'] || game.keys['K']) && n->state != PUNCHING &&
        n->state != KICKING && n->isGrounded)
    {
        n->state = KICKING;
        n->currentFrame = 0;
        n->attackCounter = 0;
        n->velocityX = 0;
    }

    // Attack Logic
    if (n->state == PUNCHING || n->state == KICKING)
    {
        n->attackCounter++;
        if (n->attackCounter >= 20)
        {
            n->state = IDLE;
            n->attackCounter = 0;
            n->currentFrame = 0;
        }
    }

    // Animation Frame Update
    if (n->state == n->prevState)
    {
        n->frameCounter++;
        if (n->frameCounter >= n->frameDelay)
        {
            n->currentFrame++;
            n->frameCounter = 0;
        }
    }
    else
    {
        n->currentFrame = 0;
        n->frameCounter = 0;
    }
}

// --- GLUT CALLBACKS ---

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Setup Ortho
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Background (Dark Dojos style)
    drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.1f, 0.1f, 0.12f);
    // Floor
    drawRect(0, 0, SCREEN_WIDTH, game.groundLevel, 0.2f, 0.2f, 0.25f);

    drawNinja();

    // UI Info
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, SCREEN_HEIGHT - 20);
    const char *text = "NINJA CONTROLS (JACK STYLE): WASD to Move, J=Slash, K=Kick";
    for (const char *c = text; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

    glutSwapBuffers();
}

void update(int value)
{
    updateNinja();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboardDown(unsigned char key, int x, int y)
{
    game.keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y)
{
    game.keys[key] = false;
    // Immediate release logic for crouch
    if ((key == 's' || key == 'S') && game.ninja.state == CROUCHING)
    {
        game.ninja.state = IDLE;
    }
}

int main(int argc, char **argv)
{
#define GL_SILENCE_DEPRECATION
#define GLUT_SILENCE_DEPRECATION

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Ninja with Jack's Movement");

    initGame();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(0, update, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glutMainLoop();
    return 0;
}