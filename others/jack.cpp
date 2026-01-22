#include <GLUT/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Animation states
enum AnimationState
{
    IDLE,
    WALKING,
    PUNCHING,
    KICKING,
    JUMPING,
    CROUCHING
};

// Player structure
struct Player
{
    float x, y;          // Position
    float width, height; // Dimensions
    float velocityX, velocityY;
    float scaleX; // For flipping sprite
    bool isGrounded;
    bool facingRight;
    AnimationState state;
    AnimationState prevState; // Track previous state for smooth transitions
    int currentFrame;
    int frameDelay;
    int frameCounter;
    float health;
    float punchDamage;
    float kickDamage;
    int attackCounter; // Separate counter for attack animations

    // Animation frame counts for each state
    int idleFrames;
    int walkFrames;
    int punchFrames;
    int kickFrames;
    int jumpFrames;
    int crouchFrames;
};

// Game structure
struct Game
{
    Player player;
    float groundLevel;
    bool keys[256];
};

Game game;

// Initialize player
void initPlayer()
{
    Player *p = &game.player;

    p->x = 100.0f;
    p->y = game.groundLevel;
    p->width = 70.0f;   // Jack is slightly broader
    p->height = 140.0f; // Jack is taller and more muscular
    p->velocityX = 0.0f;
    p->velocityY = 0.0f;
    p->scaleX = 1.0f;
    p->isGrounded = true;
    p->facingRight = true;
    p->state = IDLE;
    p->prevState = IDLE;
    p->currentFrame = 0;
    p->frameDelay = 5;
    p->frameCounter = 0;
    p->attackCounter = 0;
    p->health = 100.0f;
    p->punchDamage = 12.0f; // Jack hits harder
    p->kickDamage = 18.0f;  // Jack kicks harder

    // Initialize animation frame counts
    p->idleFrames = 4;
    p->walkFrames = 6;
    p->punchFrames = 4;
    p->kickFrames = 4;
    p->jumpFrames = 3;
    p->crouchFrames = 2;
}

// Initialize game
void initGame()
{
    game.groundLevel = 100.0f;

    // Initialize all keys to false
    for (int i = 0; i < 256; i++)
    {
        game.keys[i] = false;
    }

    initPlayer();
}

// Draw rectangle using modern OpenGL
void drawRect(float x, float y, float width, float height, float r, float g, float b)
{
    glColor3f(r, g, b);

    // Define vertices for a rectangle
    float vertices[] = {
        x, y,
        x + width, y,
        x + width, y + height,
        x, y + height};

    // Use vertex arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

// Draw a triangle (for hair/spikes)
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b)
{
    glColor3f(r, g, b);

    float vertices[] = {x1, y1, x2, y2, x3, y3};

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);
}

// Draw Jack Tenrec's signature vest
void drawVest(float x, float y, float width, float height, bool facingRight)
{
    // Vest base color (dark brown leather)
    float vestX = x + width * 0.1f;
    float vestWidth = width * 0.8f;
    float vestHeight = height * 0.5f;
    float vestY = y + height * 0.3f;

    drawRect(vestX, vestY, vestWidth, vestHeight, 0.3f, 0.2f, 0.1f);

    // Vest opening
    drawRect(x + width * 0.4f, vestY + vestHeight * 0.2f,
             width * 0.2f, vestHeight * 0.8f, 0.0f, 0.0f, 0.0f);

    // Vest straps
    drawRect(vestX + vestWidth * 0.1f, vestY + vestHeight * 0.8f,
             vestWidth * 0.8f, vestHeight * 0.1f, 0.5f, 0.35f, 0.1f); // Gold color
}

// Draw Jack's signature spiky hair
void drawHair(float x, float y, float width, float height, bool facingRight, int currentFrame)
{
    float hairBaseY = y + height * 0.85f;
    float hairBaseX = x + width * 0.3f;
    float hairWidth = width * 0.4f;

    // Hair base (brown)
    drawRect(hairBaseX, hairBaseY, hairWidth, height * 0.15f, 0.4f, 0.25f, 0.1f);

    // Spikes - different animation based on state
    float spikeOffset = sin(currentFrame * 0.3f) * 3.0f; // Subtle animation

    if (facingRight)
    {
        // Spikes pointing right
        drawTriangle(hairBaseX + hairWidth, hairBaseY,
                     hairBaseX + hairWidth + 10 + spikeOffset, hairBaseY + 5,
                     hairBaseX + hairWidth, hairBaseY + 10,
                     0.4f, 0.25f, 0.1f);

        drawTriangle(hairBaseX + hairWidth, hairBaseY + 5,
                     hairBaseX + hairWidth + 12 + spikeOffset, hairBaseY + 15,
                     hairBaseX + hairWidth, hairBaseY + 20,
                     0.4f, 0.25f, 0.1f);
    }
    else
    {
        // Spikes pointing left
        drawTriangle(hairBaseX, hairBaseY,
                     hairBaseX - 10 - spikeOffset, hairBaseY + 5,
                     hairBaseX, hairBaseY + 10,
                     0.4f, 0.25f, 0.1f);

        drawTriangle(hairBaseX, hairBaseY + 5,
                     hairBaseX - 12 - spikeOffset, hairBaseY + 15,
                     hairBaseX, hairBaseY + 20,
                     0.4f, 0.25f, 0.1f);
    }
}

// Draw Jack's bandana
void drawBandana(float x, float y, float width, float height, bool facingRight)
{
    float bandanaY = y + height * 0.8f;
    float bandanaHeight = height * 0.05f;

    // Red bandana
    drawRect(x + width * 0.25f, bandanaY, width * 0.5f, bandanaHeight, 0.8f, 0.1f, 0.1f);

    // Bandana knot
    if (facingRight)
    {
        drawRect(x + width * 0.7f, bandanaY, width * 0.1f, bandanaHeight * 2.0f, 0.8f, 0.1f, 0.1f);
    }
    else
    {
        drawRect(x + width * 0.2f, bandanaY, width * 0.1f, bandanaHeight * 2.0f, 0.8f, 0.1f, 0.1f);
    }
}

// Draw player
void drawPlayer()
{
    Player *p = &game.player;

    // Apply scale for direction - using matrix operations
    glPushMatrix();
    glTranslatef(p->x + p->width / 2, p->y, 0);
    glScalef(p->scaleX, 1.0f, 1.0f);
    glTranslatef(-(p->x + p->width / 2), -p->y, 0);

    // Draw different body parts based on animation state
    float armOffset = 0.0f;
    float legOffset = 0.0f;
    float bodyColorR = 0.9f; // Tan skin tone
    float bodyColorG = 0.7f;
    float bodyColorB = 0.5f;

    // Main body (muscular torso)
    float torsoWidth = p->width * 0.7f;
    float torsoHeight = p->height * 0.5f;
    float torsoX = p->x + (p->width - torsoWidth) / 2;
    float torsoY = p->y + p->height * 0.3f;

    drawRect(torsoX, torsoY, torsoWidth, torsoHeight, bodyColorR, bodyColorG, bodyColorB);

    // Draw Jack's signature vest
    drawVest(p->x, p->y, p->width, p->height, p->facingRight);

    // Draw head
    float headWidth = p->width * 0.4f;
    float headHeight = p->height * 0.3f;
    float headX = p->x + (p->width - headWidth) / 2;
    float headY = p->y + p->height * 0.7f;

    drawRect(headX, headY, headWidth, headHeight, bodyColorR, bodyColorG, bodyColorB);

    // Draw bandana
    drawBandana(p->x, p->y, p->width, p->height, p->facingRight);

    // Draw hair
    drawHair(p->x, p->y, p->width, p->height, p->facingRight, p->currentFrame);

    // Draw facial features
    // Eyes
    float eyeOffset = p->facingRight ? 5.0f : -5.0f;
    drawRect(headX + headWidth * 0.3f + eyeOffset, headY + headHeight * 0.6f,
             headWidth * 0.15f, headHeight * 0.15f, 0.0f, 0.0f, 0.0f);
    drawRect(headX + headWidth * 0.55f + eyeOffset, headY + headHeight * 0.6f,
             headWidth * 0.15f, headHeight * 0.15f, 0.0f, 0.0f, 0.0f);

    // Mouth (determined by state)
    if (p->state == PUNCHING || p->state == KICKING)
    {
        // Angry/shouting mouth
        drawRect(headX + headWidth * 0.35f, headY + headHeight * 0.25f,
                 headWidth * 0.3f, headHeight * 0.1f, 0.8f, 0.1f, 0.1f);
    }
    else
    {
        // Normal mouth
        drawRect(headX + headWidth * 0.4f, headY + headHeight * 0.3f,
                 headWidth * 0.2f, headHeight * 0.05f, 0.4f, 0.2f, 0.2f);
    }

    // Draw different body parts based on animation state
    switch (p->state)
    {
    case IDLE:
        // Arms at sides - muscular arms
        drawRect(p->x - 15.0f, torsoY, 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);
        drawRect(p->x + p->width, torsoY, 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);

        // Legs - jeans color
        drawRect(p->x + p->width * 0.2f, p->y, 25.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f);
        drawRect(p->x + p->width * 0.6f, p->y, 25.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f);

        // Boots
        drawRect(p->x + p->width * 0.18f, p->y - 5.0f, 29.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        drawRect(p->x + p->width * 0.58f, p->y - 5.0f, 29.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        break;

    case WALKING:
        // Animated walking arms
        armOffset = sin(p->currentFrame * 0.5f) * 20.0f;
        drawRect(p->x - 15.0f, torsoY + armOffset,
                 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);
        drawRect(p->x + p->width, torsoY - armOffset,
                 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);

        // Animated walking legs
        legOffset = sin(p->currentFrame * 0.5f + 1.57f) * 25.0f;
        drawRect(p->x + p->width * 0.2f, p->y + legOffset,
                 25.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f);
        drawRect(p->x + p->width * 0.6f, p->y - legOffset,
                 25.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f);

        // Boots
        drawRect(p->x + p->width * 0.18f, p->y + legOffset - 5.0f, 29.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        drawRect(p->x + p->width * 0.58f, p->y - legOffset - 5.0f, 29.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        break;

    case PUNCHING:
        // Extended punching arm (powerful punch)
        if (p->facingRight)
        {
            drawRect(p->x - 15.0f, torsoY, 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);
            drawRect(p->x + p->width, torsoY - 15.0f,
                     80.0f, 30.0f, bodyColorR, bodyColorG, bodyColorB); // Extended arm
        }
        else
        {
            drawRect(p->x - 65.0f, torsoY - 15.0f,
                     80.0f, 30.0f, bodyColorR, bodyColorG, bodyColorB); // Extended arm
            drawRect(p->x + p->width, torsoY, 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);
        }

        // Legs in fighting stance
        drawRect(p->x + p->width * 0.1f, p->y, 30.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f);
        drawRect(p->x + p->width * 0.7f, p->y, 30.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f);

        // Boots
        drawRect(p->x + p->width * 0.08f, p->y - 5.0f, 34.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        drawRect(p->x + p->width * 0.68f, p->y - 5.0f, 34.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        break;

    case KICKING:
        // High kick (Jack's signature move)
        if (p->facingRight)
        {
            drawRect(p->x + p->width * 0.3f, p->y - 60.0f,
                     30.0f, 80.0f, 0.1f, 0.2f, 0.5f);                                        // Kicking leg
            drawRect(p->x + p->width * 0.65f, p->y, 30.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f); // Standing leg

            // Boots
            drawRect(p->x + p->width * 0.28f, p->y - 65.0f, 34.0f, 15.0f, 0.2f, 0.2f, 0.2f);
            drawRect(p->x + p->width * 0.63f, p->y - 5.0f, 34.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        }
        else
        {
            drawRect(p->x + p->width * 0.4f, p->y - 60.0f,
                     30.0f, 80.0f, 0.1f, 0.2f, 0.5f);                                        // Kicking leg
            drawRect(p->x + p->width * 0.05f, p->y, 30.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f); // Standing leg

            // Boots
            drawRect(p->x + p->width * 0.38f, p->y - 65.0f, 34.0f, 15.0f, 0.2f, 0.2f, 0.2f);
            drawRect(p->x + p->width * 0.03f, p->y - 5.0f, 34.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        }

        // Arms in fighting stance
        drawRect(p->x - 15.0f, torsoY - 10.0f, 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);
        drawRect(p->x + p->width, torsoY - 10.0f, 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);
        break;

    case JUMPING:
        // Jumping pose - arms up
        drawRect(p->x - 10.0f, torsoY + 20.0f, 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);
        drawRect(p->x + p->width, torsoY + 20.0f, 15.0f, torsoHeight * 0.8f, bodyColorR, bodyColorG, bodyColorB);

        // Legs together in air
        drawRect(p->x + p->width * 0.3f, p->y, 25.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f);
        drawRect(p->x + p->width * 0.6f, p->y, 25.0f, torsoY - p->y, 0.1f, 0.2f, 0.5f);

        // Boots
        drawRect(p->x + p->width * 0.28f, p->y - 5.0f, 29.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        drawRect(p->x + p->width * 0.58f, p->y - 5.0f, 29.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        break;

    case CROUCHING:
        // Crouching pose - ready to spring
        drawRect(p->x - 10.0f, torsoY - 30.0f, 15.0f, torsoHeight * 0.6f, bodyColorR, bodyColorG, bodyColorB);
        drawRect(p->x + p->width, torsoY - 30.0f, 15.0f, torsoHeight * 0.6f, bodyColorR, bodyColorG, bodyColorB);

        // Crouched legs
        drawRect(p->x + p->width * 0.2f, p->y, 30.0f, 40.0f, 0.1f, 0.2f, 0.5f);
        drawRect(p->x + p->width * 0.6f, p->y, 30.0f, 40.0f, 0.1f, 0.2f, 0.5f);

        // Boots
        drawRect(p->x + p->width * 0.18f, p->y - 5.0f, 34.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        drawRect(p->x + p->width * 0.58f, p->y - 5.0f, 34.0f, 15.0f, 0.2f, 0.2f, 0.2f);
        break;
    }

    glPopMatrix();

    // Draw health bar with Jack's style
    // Health bar background
    drawRect(p->x - 10.0f, p->y + p->height + 10.0f,
             p->width + 20.0f, 15.0f, 0.3f, 0.1f, 0.1f); // Dark red

    // Health bar border
    drawRect(p->x - 12.0f, p->y + p->height + 8.0f,
             p->width + 24.0f, 19.0f, 0.5f, 0.35f, 0.1f); // Gold border

    // Health bar foreground
    drawRect(p->x - 10.0f, p->y + p->height + 10.0f,
             (p->width + 20.0f) * (p->health / 100.0f), 15.0f, 0.8f, 0.1f, 0.1f); // Bright red
}

// Update player
void updatePlayer()
{
    Player *p = &game.player;

    // Store previous state
    p->prevState = p->state;

    // Apply gravity
    if (!p->isGrounded)
    {
        p->velocityY -= 0.5f;
    }

    // Update position
    p->x += p->velocityX;
    p->y += p->velocityY;

    // Check ground collision
    if (p->y <= game.groundLevel)
    {
        p->y = game.groundLevel;
        p->velocityY = 0.0f;
        p->isGrounded = true;

        // Reset to idle if was jumping
        if (p->state == JUMPING)
        {
            p->state = IDLE;
            p->currentFrame = 0;
        }
    }

    // Boundary checking
    if (p->x < 0)
        p->x = 0;
    if (p->x > SCREEN_WIDTH - p->width)
        p->x = SCREEN_WIDTH - p->width;

    // Don't allow movement during attacks (except jumping)
    bool canMove = (p->state != PUNCHING && p->state != KICKING) || !p->isGrounded;

    // Handle input
    if ((game.keys['a'] || game.keys['A']) && canMove)
    {
        p->velocityX = -5.0f;
        p->facingRight = false;
        p->scaleX = -1.0f;
        if (p->isGrounded && p->state != JUMPING && p->state != CROUCHING)
        {
            p->state = WALKING;
        }
    }
    else if ((game.keys['d'] || game.keys['D']) && canMove)
    {
        p->velocityX = 5.0f;
        p->facingRight = true;
        p->scaleX = 1.0f;
        if (p->isGrounded && p->state != JUMPING && p->state != CROUCHING)
        {
            p->state = WALKING;
        }
    }
    else
    {
        p->velocityX = 0.0f;
        if (p->isGrounded && p->state == WALKING && p->state != CROUCHING)
        {
            p->state = IDLE;
        }
    }

    // Jump - only if not already attacking and grounded
    if ((game.keys['w'] || game.keys['W'] || game.keys[' ']) && p->isGrounded &&
        p->state != PUNCHING && p->state != KICKING)
    {
        p->velocityY = 15.0f;
        p->isGrounded = false;
        p->state = JUMPING;
        p->currentFrame = 0;
    }

    // Crouch - only if grounded
    if ((game.keys['s'] || game.keys['S']) && p->isGrounded &&
        p->state != PUNCHING && p->state != KICKING)
    {
        p->state = CROUCHING;
        p->height = 100.0f;
        p->y = game.groundLevel;
        p->velocityX = 0; // Stop movement while crouching
    }
    else if (p->state == CROUCHING && !(game.keys['s'] || game.keys['S']))
    {
        // Only return to idle if we were crouching and released crouch key
        p->state = IDLE;
        p->height = 140.0f;
        p->y = game.groundLevel - (140.0f - 100.0f); // Adjust position for height change
    }

    // Punch - only if not already attacking and not jumping
    if ((game.keys['j'] || game.keys['J']) && p->state != PUNCHING &&
        p->state != KICKING && p->isGrounded)
    {
        p->state = PUNCHING;
        p->currentFrame = 0;
        p->attackCounter = 0;
        p->velocityX = 0; // Stop movement during attack
    }

    // Kick - only if not already attacking and not jumping
    if ((game.keys['k'] || game.keys['K']) && p->state != PUNCHING &&
        p->state != KICKING && p->isGrounded)
    {
        p->state = KICKING;
        p->currentFrame = 0;
        p->attackCounter = 0;
        p->velocityX = 0; // Stop movement during attack
    }

    // Handle attack animations
    if (p->state == PUNCHING || p->state == KICKING)
    {
        p->attackCounter++;
        if (p->attackCounter >= 20) // Attack lasts 20 frames
        {
            // Return to previous state if it was walking or idle
            if (p->prevState == WALKING && (game.keys['a'] || game.keys['A'] || game.keys['d'] || game.keys['D']))
            {
                p->state = WALKING;
            }
            else
            {
                p->state = IDLE;
            }
            p->attackCounter = 0;
            p->currentFrame = 0;
        }
    }

    // Update animation frame (only if state changed or enough time passed)
    if (p->state == p->prevState)
    {
        p->frameCounter++;
        if (p->frameCounter >= p->frameDelay)
        {
            p->currentFrame++;
            p->frameCounter = 0;
        }
    }
    else
    {
        // State changed, reset animation
        p->currentFrame = 0;
        p->frameCounter = 0;
    }

    // Loop frames based on current state
    int maxFrames = 1;
    switch (p->state)
    {
    case IDLE:
        maxFrames = p->idleFrames;
        break;
    case WALKING:
        maxFrames = p->walkFrames;
        break;
    case PUNCHING:
        maxFrames = p->punchFrames;
        break;
    case KICKING:
        maxFrames = p->kickFrames;
        break;
    case JUMPING:
        maxFrames = p->jumpFrames;
        break;
    case CROUCHING:
        maxFrames = p->crouchFrames;
        break;
    }

    if (p->currentFrame >= maxFrames)
    {
        p->currentFrame = 0;
    }
}

// Display callback
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw post-apocalyptic background
    drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.3f, 0.25f, 0.2f); // Brownish wasteland sky

    // Draw ground with texture
    for (int i = 0; i < SCREEN_WIDTH; i += 50)
    {
        float shade = 0.3f + (sin(i * 0.01f) * 0.1f);
        drawRect(i, 0, 50, game.groundLevel,
                 shade, shade * 0.8f, shade * 0.6f); // Cracked earth
    }

    // Draw player
    drawPlayer();

    // Draw instructions
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, SCREEN_HEIGHT - 20);
    const char *instructions = "JACK TENREC - Controls: A/D=Move, W/Space=Jump, S=Crouch, J=Punch, K=Kick";
    const char *c;
    for (c = instructions; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }

    // Draw state info
    glRasterPos2f(10, SCREEN_HEIGHT - 40);
    const char *stateText = "State: ";
    for (c = stateText; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }

    // Append state string
    const char *stateStr;
    switch (game.player.state)
    {
    case IDLE:
        stateStr = "IDLE";
        break;
    case WALKING:
        stateStr = "WALKING";
        break;
    case PUNCHING:
        stateStr = "PUNCHING";
        break;
    case KICKING:
        stateStr = "KICKING";
        break;
    case JUMPING:
        stateStr = "JUMPING";
        break;
    case CROUCHING:
        stateStr = "CROUCHING";
        break;
    default:
        stateStr = "UNKNOWN";
    }

    for (c = stateStr; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }

    // Draw character info
    glRasterPos2f(10, SCREEN_HEIGHT - 60);
    const char *charInfo = "Character: Jack Tenrec - Cadillacs & Dinosaurs";
    for (c = charInfo; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }

    glutSwapBuffers();
}

// Keyboard down callback
void keyboardDown(unsigned char key, int x, int y)
{
    game.keys[key] = true;
}

// Keyboard up callback
void keyboardUp(unsigned char key, int x, int y)
{
    game.keys[key] = false;

    // Handle key releases specifically
    Player *p = &game.player;

    // If crouch key released and we're crouching
    if ((key == 's' || key == 'S') && p->state == CROUCHING)
    {
        p->state = IDLE;
        p->height = 140.0f;
        p->y = game.groundLevel;
    }
}

// Update game state
void update(int value)
{
    updatePlayer();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

// Reshape callback
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
// Silence deprecation warnings
#define GL_SILENCE_DEPRECATION
#define GLUT_SILENCE_DEPRECATION

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Cadillacs and Dinosaurs - Jack Tenrec Character");

    // Initialize game
    initGame();

    // Set up callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(0, update, 0);

    // Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable blending for transparency (if needed)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Start main loop
    glutMainLoop();

    return 0;
}