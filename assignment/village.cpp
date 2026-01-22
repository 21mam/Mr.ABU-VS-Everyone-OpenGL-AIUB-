/*
 * Asian Village Sunset Scene
 * Recreated in C++ using OpenGL (GLUT)
 * * Instructions:
 * - Compile with: g++ village.cpp -o village -lGL -lGLU -lglut
 * - Run: ./village
 */

#include <GLUT/glut.h>
#include <math.h>

// Constants for screen dimensions
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;

// PI for circle calculations
const float PI = 3.14159265f;

// --- Helper Functions for Shapes ---

// Draw a filled circle (for sun/lanterns)
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

// Draw a basic rectangle
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

// Draw a Trapezoid (used for roofs)
// x, y is the bottom-left corner of the base
void drawRoof(float x, float y, float width, float height, float overhang, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(x - overhang, y);                        // Bottom Left (flared out)
    glVertex2f(x + width + overhang, y);                // Bottom Right (flared out)
    glVertex2f(x + width - (overhang / 2), y + height); // Top Right
    glVertex2f(x + (overhang / 2), y + height);         // Top Left
    glEnd();

    // Optional: Draw a darker outline/ridge
    glColor3f(0.1f, 0.05f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x - overhang, y);
    glVertex2f(x + width + overhang, y);
    glEnd();
}

// Draw a glowing lantern
void drawLantern(float x, float y)
{
    // String
    glColor3f(0.2f, 0.1f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 15);
    glVertex2f(x, y);
    glEnd();

    // Lantern body
    drawCircle(x, y - 5, 8, 1.0f, 0.6f, 0.1f); // Orange glow
    drawCircle(x, y - 5, 4, 1.0f, 0.9f, 0.5f); // Bright center
}

// --- Specific Object Drawers ---

void drawPagoda(float x, float y)
{
    float baseW = 60;
    float floorH = 30;
    float roofH = 15;
    float overhang = 15;

    // Dark Brown color for wood
    float woodR = 0.25f, woodG = 0.15f, woodB = 0.1f;
    // Dark Grey/Brown for roof
    float roofR = 0.2f, roofG = 0.18f, roofB = 0.15f;

    // 3 Tiers
    for (int i = 0; i < 3; i++)
    {
        float currentY = y + (i * (floorH + roofH));
        float shrink = i * 5; // Get slightly narrower as we go up

        // Walls
        drawRect(x + shrink, currentY, baseW - (shrink * 2), floorH, woodR, woodG, woodB);

        // Windows/Details on walls
        drawRect(x + shrink + 5, currentY + 5, 10, 15, 0.1f, 0.05f, 0.0f);
        drawRect(x + baseW - shrink - 15, currentY + 5, 10, 15, 0.1f, 0.05f, 0.0f);

        // Roof
        drawRoof(x + shrink, currentY + floorH, baseW - (shrink * 2), roofH, overhang, roofR, roofG, roofB);

        // Lanterns hanging from roof corners
        if (i < 2)
        {
            drawLantern(x + shrink - overhang + 2, currentY + floorH);
            drawLantern(x + baseW - shrink + overhang - 2, currentY + floorH);
        }
    }

    // Spire on top
    float topY = y + (3 * (floorH + roofH));
    drawRect(x + (baseW / 2) - 3, topY, 6, 25, 0.1f, 0.1f, 0.1f);
    drawCircle(x + (baseW / 2), topY + 25, 5, 0.1f, 0.1f, 0.1f);
}

void drawTemple(float x, float y)
{
    float w = 100;
    float h = 50;

    // Main Hall
    drawRect(x, y, w, h, 0.3f, 0.2f, 0.15f);

    // Central Door
    drawRect(x + 35, y, 30, 35, 0.1f, 0.05f, 0.0f);

    // Windows
    drawRect(x + 10, y + 15, 15, 15, 0.1f, 0.05f, 0.0f);
    drawRect(x + w - 25, y + 15, 15, 15, 0.1f, 0.05f, 0.0f);

    // Massive Roof
    drawRoof(x - 10, y + h, w + 20, 30, 20, 0.25f, 0.2f, 0.18f);

    // Second smaller roof on top (pagoda style center)
    drawRect(x + 30, y + h + 30, 40, 15, 0.3f, 0.2f, 0.15f);
    drawRoof(x + 30, y + h + 30 + 15, 40, 15, 10, 0.25f, 0.2f, 0.18f);
}

void drawWatchTower(float x, float y)
{
    // Legs
    glColor3f(0.2f, 0.1f, 0.05f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y);           // Left leg bottom
    glVertex2f(x + 10, y + 80); // Left leg top
    glVertex2f(x + 40, y);      // Right leg bottom
    glVertex2f(x + 30, y + 80); // Right leg top

    // Cross bracing
    glVertex2f(x + 5, y + 40);
    glVertex2f(x + 35, y + 40);
    glVertex2f(x + 2, y + 20);
    glVertex2f(x + 38, y + 20);
    glEnd();

    // Platform hut
    drawRect(x + 5, y + 80, 30, 30, 0.25f, 0.15f, 0.1f);
    drawRect(x + 15, y + 90, 10, 10, 0.0f, 0.0f, 0.0f); // Window

    // Roof
    drawRoof(x + 5, y + 110, 30, 15, 10, 0.2f, 0.18f, 0.15f);
}

void drawBackgroundEnvironment()
{
    // 1. Sky Gradient (Golden Yellow to Orange)
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.8f, 0.2f); // Top - Bright Gold
    glVertex2f(0, WINDOW_HEIGHT);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);

    glColor3f(1.0f, 0.6f, 0.1f); // Bottom - Orange Sunset
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(0, 0);
    glEnd();

    // 2. Clouds (Simple alpha blended ellipses)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.9f, 0.6f, 0.4f);

    drawCircle(200, 500, 40, 1.0f, 0.9f, 0.7f);
    drawCircle(250, 500, 50, 1.0f, 0.9f, 0.7f);
    drawCircle(300, 500, 40, 1.0f, 0.9f, 0.7f);

    drawCircle(700, 450, 60, 1.0f, 0.9f, 0.7f);
    drawCircle(780, 450, 50, 1.0f, 0.9f, 0.7f);
    glDisable(GL_BLEND);

    // 3. Silhouette Hills (Back layer)
    glColor3f(0.6f, 0.45f, 0.2f); // Hazy brown
    glBegin(GL_POLYGON);
    glVertex2f(0, 150);
    glVertex2f(200, 250);
    glVertex2f(500, 180);
    glVertex2f(800, 300);
    glVertex2f(1000, 200);
    glVertex2f(1000, 0);
    glVertex2f(0, 0);
    glEnd();

    // 4. Forest Silhouette (Jagged triangles)
    glColor3f(0.25f, 0.15f, 0.1f); // Dark brown trees
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < WINDOW_WIDTH; i += 30)
    {
        float height = (i % 50) + 100; // Randomize height slightly
        glVertex2f(i, 80);
        glVertex2f(i + 15, 80 + height);
        glVertex2f(i + 30, 80);
    }
    glEnd();
}

void drawGround()
{
    // Main ground block
    drawRect(0, 0, WINDOW_WIDTH, 80, 0.35f, 0.3f, 0.25f);

    // Stones pattern (C-style loop, no vector)
    for (int i = 0; i < WINDOW_WIDTH; i += 40)
    {
        for (int j = 0; j < 80; j += 25)
        {
            float stoneColor = 0.3f + ((i % 3) * 0.01f); // Slight variation
            drawCircle(i + 20, j + 12, 10, stoneColor, stoneColor - 0.05f, stoneColor - 0.05f);
        }
    }

    // Top border of ground
    drawRect(0, 75, WINDOW_WIDTH, 10, 0.2f, 0.15f, 0.1f);
}

// --- Main Display Function ---
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // 1. Draw Environment
    drawBackgroundEnvironment();

    // 2. Draw Ground
    drawGround();

    // 3. Draw Buildings (Placed approximate to image)

    // Left: The Multi-tier Pagoda
    drawPagoda(100, 85);

    // Center-Left: Secondary Building
    drawPagoda(300, 85); // Reusing pagoda function but could be shorter

    // Center: Main Temple
    drawTemple(500, 85);

    // Right: Small hut
    drawRect(720, 85, 60, 40, 0.3f, 0.2f, 0.1f);
    drawRoof(720, 125, 60, 20, 10, 0.25f, 0.2f, 0.15f);
    drawLantern(750, 125); // Lantern on hut

    // Right Edge: Watchtower
    drawWatchTower(880, 85);

    glFlush();
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Ancient Village - OpenGL");

    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}