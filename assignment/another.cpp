#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <cmath>
#include <iostream>
const float PI = 3.14159265f;

void drawCircle(float cx, float cy, float r, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++)
    {
        float theta = 2.0f * PI * float(i) / float(segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void drawEllipse(float cx, float cy, float rx, float ry, int segments)
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

void drawStripe(float x, float y, float width, float height, float angle)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(angle, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2f(-width / 2, -height / 2);
    glVertex2f(width / 2, -height / 2);
    glVertex2f(width / 2, height / 2);
    glVertex2f(-width / 2, height / 2);
    glEnd();
    glPopMatrix();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Orange color
    glColor3f(0.95f, 0.55f, 0.1f);

    // Main head (circle)
    drawCircle(0.0f, 0.0f, 0.5f, 50);

    // Ears
    glColor3f(0.95f, 0.55f, 0.1f);
    drawCircle(-0.35f, 0.35f, 0.18f, 30);
    drawCircle(0.35f, 0.35f, 0.18f, 30);

    // Inner ears (darker)
    glColor3f(0.3f, 0.15f, 0.05f);
    drawCircle(-0.35f, 0.35f, 0.10f, 30);
    drawCircle(0.35f, 0.35f, 0.10f, 30);

    // Black stripes on head
    glColor3f(0.1f, 0.05f, 0.0f);

    // Top stripes
    drawStripe(-0.15f, 0.3f, 0.06f, 0.25f, -20);
    drawStripe(0.15f, 0.3f, 0.06f, 0.25f, 20);
    drawStripe(-0.3f, 0.2f, 0.05f, 0.2f, -35);
    drawStripe(0.3f, 0.2f, 0.05f, 0.2f, 35);

    // Side stripes
    drawStripe(-0.4f, 0.0f, 0.05f, 0.18f, -10);
    drawStripe(0.4f, 0.0f, 0.05f, 0.18f, 10);

    // White muzzle area
    glColor3f(0.95f, 0.95f, 0.9f);
    drawEllipse(0.0f, -0.15f, 0.3f, 0.25f, 40);

    // Orange spots on white area
    glColor3f(0.85f, 0.5f, 0.1f);
    drawCircle(-0.15f, -0.05f, 0.05f, 20);
    drawCircle(0.15f, -0.05f, 0.05f, 20);
    drawCircle(-0.08f, -0.18f, 0.04f, 20);
    drawCircle(0.08f, -0.18f, 0.04f, 20);

    // Black dots on orange spots
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-0.15f, -0.05f, 0.015f, 10);
    drawCircle(0.15f, -0.05f, 0.015f, 10);

    // Nose (black triangle)
    glColor3f(0.1f, 0.05f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, -0.1f);
    glVertex2f(-0.04f, -0.15f);
    glVertex2f(0.04f, -0.15f);
    glEnd();

    // Mouth
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, -0.15f);
    glVertex2f(0.0f, -0.25f);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.12f, -0.25f);
    glVertex2f(0.0f, -0.25f);
    glVertex2f(0.12f, -0.25f);
    glEnd();

    // Whisker dots
    drawCircle(-0.2f, -0.18f, 0.01f, 10);
    drawCircle(-0.25f, -0.15f, 0.01f, 10);
    drawCircle(-0.25f, -0.21f, 0.01f, 10);
    drawCircle(0.2f, -0.18f, 0.01f, 10);
    drawCircle(0.25f, -0.15f, 0.01f, 10);
    drawCircle(0.25f, -0.21f, 0.01f, 10);

    // White eye patches
    glColor3f(0.95f, 0.95f, 0.9f);
    drawEllipse(-0.18f, 0.08f, 0.13f, 0.15f, 30);
    drawEllipse(0.18f, 0.08f, 0.13f, 0.15f, 30);

    // Eyes (white)
    glColor3f(1.0f, 1.0f, 1.0f);
    drawEllipse(-0.18f, 0.08f, 0.09f, 0.12f, 30);
    drawEllipse(0.18f, 0.08f, 0.09f, 0.12f, 30);

    // Eye outlines
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 30; i++)
    {
        float theta = 2.0f * PI * float(i) / 30.0f;
        float x = 0.09f * cosf(theta);
        float y = 0.12f * sinf(theta);
        glVertex2f(x - 0.18f, y + 0.08f);
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 30; i++)
    {
        float theta = 2.0f * PI * float(i) / 30.0f;
        float x = 0.09f * cosf(theta);
        float y = 0.12f * sinf(theta);
        glVertex2f(x + 0.18f, y + 0.08f);
    }
    glEnd();

    // Pupils (yellow-green)
    glColor3f(0.8f, 0.85f, 0.3f);
    drawEllipse(-0.18f, 0.08f, 0.05f, 0.08f, 30);
    drawEllipse(0.18f, 0.08f, 0.05f, 0.08f, 30);

    // Black pupils (vertical slits)
    glColor3f(0.0f, 0.0f, 0.0f);
    drawEllipse(-0.18f, 0.08f, 0.012f, 0.06f, 30);
    drawEllipse(0.18f, 0.08f, 0.012f, 0.06f, 30);

    // Eye highlights
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(-0.19f, 0.11f, 0.015f, 10);
    drawCircle(0.17f, 0.11f, 0.015f, 10);

    // Eyebrows
    glColor3f(0.1f, 0.05f, 0.0f);
    drawStripe(-0.15f, 0.22f, 0.12f, 0.04f, -15);
    drawStripe(0.15f, 0.22f, 0.12f, 0.04f, 15);

    glFlush();
}

void init()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tiger Head - OpenGL");

    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}