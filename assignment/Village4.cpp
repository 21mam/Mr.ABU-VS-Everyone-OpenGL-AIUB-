#include <GLUT/glut.h> // Use <GLUT/glut.h> if on macOS, <GL/glut.h> on Windows/Linux
#include <cmath>
#include <cstdlib>
#include <vector>

// --- Configuration ---
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;
const float PI = 3.14159265359f;

// --- Color Palette (Enhanced for realism) ---
struct Color
{
    float r, g, b;
};

const Color COL_SKY = {0.42f, 0.81f, 0.98f};             // Bright Blue Sky
const Color COL_CLOUD = {1.00f, 1.00f, 1.00f};           // Pure White Clouds
const Color COL_MTN_FAR = {0.55f, 0.83f, 0.98f};         // Faded Blue Mountain
const Color COL_MTN_NEAR = {0.40f, 0.75f, 0.95f};        // Vibrant Blue Mountain
const Color COL_BUSH_DARK = {0.18f, 0.42f, 0.38f};       // Dark Teal/Green Foliage
const Color COL_BUSH_MID = {0.25f, 0.52f, 0.45f};        // Medium bush color
const Color COL_BUSH_LIGHT = {0.32f, 0.58f, 0.50f};      // Light bush highlight
const Color COL_GRASS_SHADOW = {0.35f, 0.68f, 0.35f};    // Shadow under grass
const Color COL_GRASS_BODY = {0.45f, 0.78f, 0.45f};      // Main Grass Color
const Color COL_GRASS_MID = {0.50f, 0.82f, 0.48f};       // Mid grass tone
const Color COL_GRASS_HIGHLIGHT = {0.55f, 0.85f, 0.50f}; // Top Grass Edge
const Color COL_GRASS_TIP = {0.60f, 0.88f, 0.52f};       // Very light grass tips
const Color COL_STONE_GAP = {0.15f, 0.18f, 0.22f};       // Dark gap between stones
const Color COL_STONE_BASE = {0.28f, 0.35f, 0.42f};      // Blue-Grey Stone
const Color COL_STONE_HIGHLIGHT = {0.32f, 0.39f, 0.46f}; // Slight highlight on stone

// --- Utility Functions ---

void setColor(Color c)
{
    glColor3f(c.r, c.g, c.b);
}

void drawEllipse(float cx, float cy, float rx, float ry, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= segments; i++)
    {
        float theta = 2.0f * PI * float(i) / float(segments);
        float x = rx * cosf(theta);
        float y = ry * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawCircle(float cx, float cy, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++)
    {
        float theta = 2.0f * PI * float(i) / float(segments);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

// Draws a custom polygon for the stones
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

// --- Layer Functions ---

void drawClouds()
{
    setColor(COL_CLOUD);
    auto drawCloudCluster = [](float x, float y, float scale)
    {
        drawCircle(x, y, 30 * scale, 20);
        drawCircle(x + 25 * scale, y + 10 * scale, 35 * scale, 20);
        drawCircle(x - 25 * scale, y + 5 * scale, 25 * scale, 20);
        drawCircle(x + 10 * scale, y + 20 * scale, 30 * scale, 20);
    };

    drawCloudCluster(150, 480, 1.0f);
    drawCloudCluster(500, 520, 0.8f);
    drawCloudCluster(850, 450, 1.2f);
}

void drawMountains()
{
    setColor(COL_MTN_FAR);
    glBegin(GL_TRIANGLE_STRIP);
    for (int x = 0; x <= WINDOW_WIDTH; x += 10)
    {
        float y = 200 + sinf(x * 0.005f) * 80 + sinf(x * 0.01f) * 20;
        glVertex2f(x, 0);
        glVertex2f(x, y);
    }
    glEnd();

    setColor(COL_MTN_NEAR);
    glBegin(GL_TRIANGLE_STRIP);
    for (int x = 0; x <= WINDOW_WIDTH; x += 10)
    {
        float y = 150 + sinf(x * 0.008f + 2.0f) * 60 + sinf(x * 0.02f) * 10;
        glVertex2f(x, 0);
        glVertex2f(x, y);
    }
    glEnd();
}

// *** ENHANCED GRASS FUNCTION ***
void drawWallAndGrass()
{
    float wallHeight = 140.0f;
    float rowHeight = 70.0f;

    // Background Grout
    setColor(COL_STONE_GAP);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, wallHeight);
    glVertex2f(0, wallHeight);
    glEnd();

    // Stones
    int rows = 2;
    float currentY = 0;
    srand(999);

    for (int r = 0; r < rows; r++)
    {
        float currentX = (r % 2 == 0) ? -40 : 0;
        while (currentX < WINDOW_WIDTH)
        {
            float brickW = 110 + (rand() % 40);
            float gap = 6.0f;
            float drawX = currentX + gap;
            float drawY = currentY + gap;
            float drawW = brickW - (gap * 2);
            float drawH = rowHeight - (gap * 2);

            setColor(COL_STONE_BASE);
            drawRoundedStone(drawX, drawY, drawW, drawH, 10.0f);

            setColor(COL_STONE_HIGHLIGHT);
            drawRoundedStone(drawX + 5, drawY + 5, drawW - 10, drawH - 10, 8.0f);

            currentX += brickW;
        }
        currentY += rowHeight;
    }

    // Enhanced Grass
    float grassBaseY = wallHeight;

    // Grass base with subtle gradient
    glBegin(GL_QUADS);
    // Bottom shadow
    setColor(COL_GRASS_SHADOW);
    glVertex2f(0, grassBaseY);
    glVertex2f(WINDOW_WIDTH, grassBaseY);
    // Top highlight
    setColor(COL_GRASS_BODY);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 15);
    glVertex2f(0, grassBaseY + 15);
    glEnd();

    // Main grass body
    setColor(COL_GRASS_BODY);
    glBegin(GL_QUADS);
    glVertex2f(0, grassBaseY + 15);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 15);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 25);
    glVertex2f(0, grassBaseY + 25);
    glEnd();

    // Grass top with highlight
    glBegin(GL_QUADS);
    setColor(COL_GRASS_MID);
    glVertex2f(0, grassBaseY + 25);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 25);
    setColor(COL_GRASS_HIGHLIGHT);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 30);
    glVertex2f(0, grassBaseY + 30);
    glEnd();

    // Detailed grass blades with varying heights and colors
    srand(12345); // Fixed seed for consistent grass

    // Draw multiple layers of grass blades for depth
    for (int layer = 0; layer < 3; layer++)
    {
        float layerYOffset = layer * 0.5f;
        float density = (layer == 0) ? 8 : 12;

        glBegin(GL_TRIANGLES);
        for (int x = 0; x <= WINDOW_WIDTH; x += density)
        {
            float bladeHeight = 8.0f + (rand() % 20) - layer * 3;
            if (bladeHeight < 5)
                bladeHeight = 5;

            float tipX = x + (rand() % 12) - 6;
            float bladeWidth = 2.0f + (rand() % 4) / 2.0f;

            // Color variation based on height
            if (bladeHeight > 15)
                setColor(COL_GRASS_TIP);
            else if (bladeHeight > 10)
                setColor(COL_GRASS_HIGHLIGHT);
            else
                setColor(COL_GRASS_MID);

            // Curved grass blades (two triangles for slight curve)
            float midX = x + (tipX - x) * 0.3f;
            float midY = grassBaseY + 25 + bladeHeight * 0.4f;

            // Left side of blade
            glVertex2f(x, grassBaseY + 25 + layerYOffset);
            glVertex2f(midX, midY);
            glVertex2f(tipX - bladeWidth / 2, grassBaseY + 25 + bladeHeight);

            // Right side of blade
            glVertex2f(x + bladeWidth, grassBaseY + 25 + layerYOffset);
            glVertex2f(midX, midY);
            glVertex2f(tipX + bladeWidth / 2, grassBaseY + 25 + bladeHeight);
        }
        glEnd();
    }

    // Add some tall grass clumps for variety
    srand(54321);
    for (int i = 0; i < 30; i++)
    {
        float clusterX = rand() % WINDOW_WIDTH;
        float clusterHeight = 15.0f + rand() % 20;

        glBegin(GL_TRIANGLES);
        for (int j = 0; j < 5; j++)
        {
            float x = clusterX + (rand() % 10) - 5;
            float height = clusterHeight * (0.7f + (rand() % 30) / 100.0f);
            float tipX = x + (rand() % 8) - 4;

            setColor(COL_GRASS_TIP);

            glVertex2f(x, grassBaseY + 25);
            glVertex2f(x + 3, grassBaseY + 25);
            glVertex2f(tipX, grassBaseY + 25 + height);
        }
        glEnd();
    }
}

// --- Main Display ---

void display()
{
    glClearColor(COL_SKY.r, COL_SKY.g, COL_SKY.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Order: Far to Near
    drawMountains();
    drawClouds();
    drawWallAndGrass(); // Enhanced realistic grass (bushes removed)

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Game Background - Enhanced Realism (No Bushes)");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}