#include <GLUT/glut.h> // Use <GLUT/glut.h> on macOS, <GL/glut.h> on Windows/Linux
#include <cmath>
#include <cstdlib>
#include <vector>
#include <ctime>

// --- Configuration ---
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;
const float PI = 3.14159265359f;

// --- Display List IDs ---
GLuint listTreePine, listTreeDeciduous;

// --- Colors ---
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

// Tree Colors
const float colTreePineLight[3] = {0.15f, 0.55f, 0.25f};
const float colTreePineDark[3] = {0.05f, 0.35f, 0.15f};
const float colTreeDecLight[3] = {0.30f, 0.70f, 0.35f};
const float colTreeDecDark[3] = {0.15f, 0.45f, 0.20f};
const float colTreeTrunk[3] = {0.35f, 0.25f, 0.15f};

// --- Helpers ---
void setColor(Color c) { glColor3f(c.r, c.g, c.b); }

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

// --- Tree Display Lists ---
void createDisplayLists()
{
    // 1. Pine Tree
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

        glBegin(GL_TRIANGLES);
        glColor3fv(colTreePineDark);
        glVertex2f(-width / 2, yBase);
        glVertex2f(-width / 2, yBase - 0.03f);
        glVertex2f(-width / 2 + 0.02f, yBase);
        glVertex2f(width / 2, yBase);
        glVertex2f(width / 2, yBase - 0.03f);
        glVertex2f(width / 2 - 0.02f, yBase);
        glEnd();
    }
    glEndList();

    // 2. Deciduous Tree
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

    glColor3f(0.4f, 0.8f, 0.4f);
    for (int k = 0; k < 10; k++)
    {
        float rx = ((float)(rand() % 100) / 100.0f * 0.3f) - 0.15f;
        float ry = ((float)(rand() % 100) / 100.0f * 0.25f) + 0.25f;
        drawCircle(rx, ry, 0.02f, 8);
    }
    glEndList();
}

// --- Scene Objects ---

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
    drawCloudCluster(850, 450, 1.2f);
    drawCloudCluster(350, 520, 1.6f);
    drawCloudCluster(600, 490, 1.5f);
    drawCloudCluster(950, 540, 1.8f);
    drawCloudCluster(50, 550, 1.4f);
}

void drawMountains()
{
    setColor(COL_MTN_FAR);
    glBegin(GL_TRIANGLE_STRIP);
    for (int x = 0; x <= WINDOW_WIDTH; x += 10)
    {
        float y = 300 + sinf(x * 0.005f) * 150 + sinf(x * 0.01f) * 40;
        glVertex2f(x, 0);
        glVertex2f(x, y);
    }
    glEnd();

    setColor(COL_MTN_NEAR);
    glBegin(GL_TRIANGLE_STRIP);
    for (int x = 0; x <= WINDOW_WIDTH; x += 10)
    {
        float y = 220 + sinf(x * 0.008f + 2.0f) * 80 + sinf(x * 0.02f) * 20;
        glVertex2f(x, 0);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawTrees()
{
    // *** MODIFIED: Lowered ground level to match new wall height ***
    float groundLevel = 70.0f; // Was 140.0f

    srand(100);

    for (int i = 0; i < 12; i++)
    {
        float xPos = 30 + i * 90 + (rand() % 30 - 15);
        float scale = 180.0f + (rand() % 70);

        glPushMatrix();
        glTranslatef(xPos, groundLevel - 10, 0);
        glScalef(scale, scale, 1.0f);

        if (i % 2 == 0)
            glCallList(listTreePine);
        else
            glCallList(listTreeDeciduous);

        glPopMatrix();
    }
}

void drawWallAndGrass()
{
    // *** MODIFIED: Wall height reduced by half ***
    float wallHeight = 70.0f; // Was 140.0f
    float rowHeight = 70.0f;

    // Grout
    setColor(COL_STONE_GAP);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, wallHeight);
    glVertex2f(0, wallHeight);
    glEnd();

    // Stones
    // *** MODIFIED: Reduced to 1 row to fit height ***
    int rows = 1; // Was 2

    float currentY = 0;
    srand(999);
    for (int r = 0; r < rows; r++)
    {
        float currentX = (r % 2 == 0) ? -40 : 0;
        while (currentX < WINDOW_WIDTH)
        {
            float brickW = 110 + (rand() % 40);
            float gap = 6.0f;
            float drawX = currentX + gap, drawY = currentY + gap;
            float drawW = brickW - gap * 2, drawH = rowHeight - gap * 2;

            setColor(COL_STONE_BASE);
            drawRoundedStone(drawX, drawY, drawW, drawH, 10.0f);
            setColor(COL_STONE_HIGHLIGHT);
            drawRoundedStone(drawX + 5, drawY + 5, drawW - 10, drawH - 10, 8.0f);
            currentX += brickW;
        }
        currentY += rowHeight;
    }

    // Grass
    float grassBaseY = wallHeight;
    glBegin(GL_QUADS);
    setColor(COL_GRASS_SHADOW);
    glVertex2f(0, grassBaseY);
    glVertex2f(WINDOW_WIDTH, grassBaseY);
    setColor(COL_GRASS_BODY);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 15);
    glVertex2f(0, grassBaseY + 15);

    setColor(COL_GRASS_BODY);
    glVertex2f(0, grassBaseY + 15);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 15);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 25);
    glVertex2f(0, grassBaseY + 25);

    setColor(COL_GRASS_MID);
    glVertex2f(0, grassBaseY + 25);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 25);
    setColor(COL_GRASS_HIGHLIGHT);
    glVertex2f(WINDOW_WIDTH, grassBaseY + 30);
    glVertex2f(0, grassBaseY + 30);
    glEnd();

    // Grass Blades
    srand(12345);
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

            if (bladeHeight > 15)
                setColor(COL_GRASS_TIP);
            else if (bladeHeight > 10)
                setColor(COL_GRASS_HIGHLIGHT);
            else
                setColor(COL_GRASS_MID);

            float midX = x + (tipX - x) * 0.3f;
            float midY = grassBaseY + 25 + bladeHeight * 0.4f;

            glVertex2f(x, grassBaseY + 25 + layerYOffset);
            glVertex2f(midX, midY);
            glVertex2f(tipX - bladeWidth / 2, grassBaseY + 25 + bladeHeight);
            glVertex2f(x + bladeWidth, grassBaseY + 25 + layerYOffset);
            glVertex2f(midX, midY);
            glVertex2f(tipX + bladeWidth / 2, grassBaseY + 25 + bladeHeight);
        }
        glEnd();
    }
}

void display()
{
    glClearColor(COL_SKY.r, COL_SKY.g, COL_SKY.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawMountains();
    drawTrees();
    drawWallAndGrass();
    drawClouds();

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
    glutCreateWindow("Game Background - Short Ground");

    srand(time(NULL));
    createDisplayLists();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}