#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <cmath>
#include <iostream>
// --- CONSTANTS & MATH ---
const float PI = 3.14159265358979323846f;

// --- HELPER FUNCTIONS ---

// Draw a filled ellipse
void drawFilledEllipse(float cx, float cy, float rx, float ry, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++)
    {
        float theta = 2.0f * PI * float(i) / float(segments);
        float x = rx * cosf(theta);
        float y = ry * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

// Draw arbitrary polygon
void drawPolygon(float *coords, int num_points)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_points * 2; i += 2)
    {
        glVertex2f(coords[i], coords[i + 1]);
    }
    glEnd();
}

// Draw a simple leaf shape (for the vest pattern)
void drawLeaf(float x, float y, float scaleX, float scaleY, float rotation)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    glScalef(scaleX, scaleY, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= 10; i++)
    {
        float t = (float)i / 10.0f * PI;
        glVertex2f(sinf(t) * 0.5f, cosf(t) * 1.0f);
    }
    glEnd();
    glPopMatrix();
}

// --- DRAWING COMPONENTS ---

void drawHead()
{
    // Head Color Palette
    float orangeR = 0.9f, orangeG = 0.5f, orangeB = 0.05f;
    float whiteR = 0.95f, whiteG = 0.95f, whiteB = 0.95f;
    float blackR = 0.1f, blackG = 0.1f, blackB = 0.1f;
    float yellowR = 1.0f, yellowG = 0.85f, yellowB = 0.15f;

    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f); // Move head up
    float scale = 0.8f;              // Scale head to fit body
    glScalef(scale, scale, 1.0f);

    // 1. Ears
    glColor3f(orangeR, orangeG, orangeB);
    // Left Ear
    glPushMatrix();
    glTranslatef(-0.35f, 0.4f, 0.0f);
    glRotatef(25, 0, 0, 1);
    drawFilledEllipse(0, 0, 0.15, 0.18, 40);
    glColor3f(whiteR, whiteG, whiteB);
    drawFilledEllipse(0, 0, 0.08, 0.12, 40);
    glPopMatrix();
    // Right Ear
    glColor3f(orangeR, orangeG, orangeB);
    glPushMatrix();
    glTranslatef(0.35f, 0.4f, 0.0f);
    glRotatef(-25, 0, 0, 1);
    drawFilledEllipse(0, 0, 0.15, 0.18, 40);
    glColor3f(whiteR, whiteG, whiteB);
    drawFilledEllipse(0, 0, 0.08, 0.12, 40);
    glPopMatrix();

    // 2. Face Base
    glColor3f(orangeR, orangeG, orangeB);
    drawFilledEllipse(0.0f, 0.05f, 0.46f, 0.42f, 80);

    // 3. Muzzle/Cheeks (White)
    glColor3f(whiteR, whiteG, whiteB);
    drawFilledEllipse(0.0f, -0.2f, 0.24f, 0.18f, 60);  // Chin
    drawFilledEllipse(-0.3f, -0.05f, 0.15f, 0.2f, 50); // Left cheek
    drawFilledEllipse(0.3f, -0.05f, 0.15f, 0.2f, 50);  // Right cheek

    // 4. Face Stripes (Black)
    glColor3f(blackR, blackG, blackB);
    // Forehead Diamond
    float diamond[] = {0.0f, 0.45f, -0.08f, 0.35f, 0.0f, 0.25f, 0.08f, 0.35f};
    drawPolygon(diamond, 4);
    // Side Stripes (Simple triangles)
    float l_stripe[] = {-0.42f, 0.3f, -0.25f, 0.28f, -0.4f, 0.38f};
    drawPolygon(l_stripe, 3);
    float r_stripe[] = {0.42f, 0.3f, 0.25f, 0.28f, 0.4f, 0.38f};
    drawPolygon(r_stripe, 3);
    float l_cheek_s[] = {-0.35f, -0.1f, -0.2f, -0.15f, -0.32f, -0.02f};
    drawPolygon(l_cheek_s, 3);
    float r_cheek_s[] = {0.35f, -0.1f, 0.2f, -0.15f, 0.32f, -0.02f};
    drawPolygon(r_cheek_s, 3);

    // 5. Facial Features
    // Nose
    glColor3f(blackR, blackG, blackB);
    float nose[] = {-0.1f, -0.12f, 0.1f, -0.12f, 0.0f, -0.22f};
    drawPolygon(nose, 3);

    // Eyes
    // Left
    glPushMatrix();
    glTranslatef(-0.18f, 0.05f, 0.0f);
    glRotatef(-15, 0, 0, 1);
    glColor3f(blackR, blackG, blackB);
    drawFilledEllipse(0, 0, 0.13, 0.09, 40);
    glColor3f(yellowR, yellowG, yellowB);
    drawFilledEllipse(0, 0, 0.11, 0.07, 40);
    glColor3f(blackR, blackG, blackB);
    drawFilledEllipse(0, 0, 0.04, 0.06, 40);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledEllipse(0.03, 0.03, 0.02, 0.02, 20);
    glPopMatrix();
    // Right
    glPushMatrix();
    glTranslatef(0.18f, 0.05f, 0.0f);
    glRotatef(15, 0, 0, 1);
    glColor3f(blackR, blackG, blackB);
    drawFilledEllipse(0, 0, 0.13, 0.09, 40);
    glColor3f(yellowR, yellowG, yellowB);
    drawFilledEllipse(0, 0, 0.11, 0.07, 40);
    glColor3f(blackR, blackG, blackB);
    drawFilledEllipse(0, 0, 0.04, 0.06, 40);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledEllipse(0.03, 0.03, 0.02, 0.02, 20);
    glPopMatrix();

    glPopMatrix(); // End Head Transform
}

void drawBody()
{
    // Colors
    float vestGoldR = 0.85f, vestGoldG = 0.75f, vestGoldB = 0.2f;
    float vestRedR = 0.6f, vestRedG = 0.1f, vestRedB = 0.1f;
    float blackR = 0.1f, blackG = 0.1f, blackB = 0.1f;
    float furR = 0.9f, furG = 0.5f, furB = 0.05f;

    // --- 1. VEST / TORSO ---
    glColor3f(vestGoldR, vestGoldG, vestGoldB);
    // Main chest block
    glBegin(GL_POLYGON);
    glVertex2f(-0.35f, 0.1f); // Top Left Shoulder
    glVertex2f(0.35f, 0.1f);  // Top Right Shoulder
    glVertex2f(0.3f, -0.6f);  // Bottom Right
    glVertex2f(-0.3f, -0.6f); // Bottom Left
    glEnd();

    // --- 2. VEST PATTERN (Red Leaves) ---
    glColor3f(vestRedR, vestRedG, vestRedB);
    drawLeaf(-0.2f, -0.1f, 0.1f, 0.15f, 45);
    drawLeaf(0.2f, -0.1f, 0.1f, 0.15f, -45);
    drawLeaf(0.0f, -0.2f, 0.08f, 0.12f, 180);
    drawLeaf(-0.15f, -0.4f, 0.1f, 0.15f, 60);
    drawLeaf(0.15f, -0.4f, 0.1f, 0.15f, -60);

    // --- 3. COLLAR (Black V-Neck) ---
    glColor3f(blackR, blackG, blackB);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.35f, 0.1f);
    glVertex2f(0.35f, 0.1f);
    glVertex2f(0.0f, -0.1f); // Deep V point
    glEnd();

    // Restore Gold Color for neck area inside V (optional, but simplifies layering)
    glColor3f(vestGoldR, vestGoldG, vestGoldB);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.25f, 0.1f);
    glVertex2f(0.25f, 0.1f);
    glVertex2f(0.0f, -0.05f);
    glEnd();

    // --- 4. ARMS (Crossed) ---
    // Basic geometric approximation of crossed arms

    // Right Arm (Viewer's Left side) - Bicep
    glColor3f(furR, furG, furB);
    glPushMatrix();
    glTranslatef(-0.35f, -0.15f, 0.0f);
    glRotatef(20, 0, 0, 1);
    drawFilledEllipse(0.0f, 0.0f, 0.12f, 0.25f, 40);
    // Arm Stripe
    glColor3f(blackR, blackG, blackB);
    drawFilledEllipse(0.0f, 0.0f, 0.13f, 0.05f, 40);
    glPopMatrix();

    // Left Arm (Viewer's Right side) - Bicep
    glColor3f(furR, furG, furB);
    glPushMatrix();
    glTranslatef(0.35f, -0.15f, 0.0f);
    glRotatef(-20, 0, 0, 1);
    drawFilledEllipse(0.0f, 0.0f, 0.12f, 0.25f, 40);
    // Arm Stripe
    glColor3f(blackR, blackG, blackB);
    drawFilledEllipse(0.0f, 0.0f, 0.13f, 0.05f, 40);
    glPopMatrix();

    // Forearms Crossed in front
    // Forearm 1 (Left to Right)
    glColor3f(furR, furG, furB);
    glPushMatrix();
    glTranslatef(0.0f, -0.35f, 0.0f);
    drawFilledEllipse(0.0f, 0.0f, 0.35f, 0.1f, 40);
    // Stripes on forearm
    glColor3f(blackR, blackG, blackB);
    drawFilledEllipse(-0.15f, 0.0f, 0.03f, 0.09f, 30);
    drawFilledEllipse(0.15f, 0.0f, 0.03f, 0.09f, 30);
    glPopMatrix();

    // Paws (Subtle circles at ends of folded arms)
    glColor3f(0.8f, 0.4f, 0.05f);                        // Slightly darker paws
    drawFilledEllipse(-0.28f, -0.32f, 0.08f, 0.08f, 30); // Left paw tuck
    drawFilledEllipse(0.28f, -0.32f, 0.08f, 0.08f, 30);  // Right paw tuck
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawBody();
    drawHead(); // Draw head last so it appears on top of the body/collar

    glFlush();
}

void init()
{
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // Grey background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Zoom out slightly (Ortho from -1 to 1) to fit head and body
    gluOrtho2D(-0.8, 0.8, -0.8, 0.8);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 600); // Taller window for body
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tigress Full Body - OpenGL");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}