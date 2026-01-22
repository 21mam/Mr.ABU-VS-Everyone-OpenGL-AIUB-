#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <cmath>
#include <iostream>

const float PI = 3.14159265358979323846f;

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

void drawFilledCircle(float cx, float cy, float r, int segments)
{
    drawFilledEllipse(cx, cy, r, r, segments);
}

void drawFilledPolygon(float *vertices, int num_vertices)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_vertices * 2; i += 2)
    {
        glVertex2f(vertices[i], vertices[i + 1]);
    }
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    float tigressOrangeR = 0.9f, tigressOrangeG = 0.5f, tigressOrangeB = 0.05f;
    float tigressWhiteR = 0.98f, tigressWhiteG = 0.98f, tigressWhiteB = 0.98f;
    float tigressBlackR = 0.1f, tigressBlackG = 0.08f, tigressBlackB = 0.08f;
    float tigressEyeYellowR = 1.0f, tigressEyeYellowG = 0.85f, tigressEyeYellowB = 0.15f;
    float tigressInnerEarR = 0.8f, tigressInnerEarG = 0.4f, tigressInnerEarB = 0.0f;

    glColor3f(tigressOrangeR, tigressOrangeG, tigressOrangeB);
    glPushMatrix();
    glTranslatef(-0.35f, 0.45f, 0.0f);
    glRotatef(20, 0.0f, 0.0f, 1.0f);
    drawFilledEllipse(0.0f, 0.0f, 0.15f, 0.18f, 50);
    glColor3f(tigressInnerEarR, tigressInnerEarG, tigressInnerEarB);
    drawFilledEllipse(0.0f, 0.0f, 0.08f, 0.12f, 50);
    glPopMatrix();

    glColor3f(tigressOrangeR, tigressOrangeG, tigressOrangeB);
    glPushMatrix();
    glTranslatef(0.35f, 0.45f, 0.0f);
    glRotatef(-20, 0.0f, 0.0f, 1.0f);
    drawFilledEllipse(0.0f, 0.0f, 0.15f, 0.18f, 50);
    glColor3f(tigressInnerEarR, tigressInnerEarG, tigressInnerEarB);
    drawFilledEllipse(0.0f, 0.0f, 0.08f, 0.12f, 50);
    glPopMatrix();

    glColor3f(tigressOrangeR, tigressOrangeG, tigressOrangeB);
    drawFilledEllipse(0.0f, 0.1f, 0.48f, 0.45f, 100);

    glColor3f(tigressWhiteR, tigressWhiteG, tigressWhiteB);
    drawFilledEllipse(0.0f, -0.15f, 0.25f, 0.2f, 80);
    drawFilledEllipse(-0.3f, 0.0f, 0.18f, 0.22f, 50);
    drawFilledEllipse(0.3f, 0.0f, 0.18f, 0.22f, 50);

    glColor3f(tigressBlackR, tigressBlackG, tigressBlackB);

    float forehead_center_stripe[] = {
        0.0f, 0.48f,
        -0.08f, 0.35f,
        0.0f, 0.25f,
        0.08f, 0.35f};
    drawFilledPolygon(forehead_center_stripe, 4);

    float l_stripe1[] = {-0.4f, 0.38f, -0.2f, 0.35f, -0.38f, 0.45f};
    drawFilledPolygon(l_stripe1, 3);
    float l_stripe2[] = {-0.45f, 0.25f, -0.25f, 0.22f, -0.42f, 0.33f};
    drawFilledPolygon(l_stripe2, 3);
    float l_stripe3[] = {-0.48f, 0.1f, -0.28f, 0.08f, -0.45f, 0.18f};
    drawFilledPolygon(l_stripe3, 3);

    float r_stripe1[] = {0.4f, 0.38f, 0.2f, 0.35f, 0.38f, 0.45f};
    drawFilledPolygon(r_stripe1, 3);
    float r_stripe2[] = {0.45f, 0.25f, 0.25f, 0.22f, 0.42f, 0.33f};
    drawFilledPolygon(r_stripe2, 3);
    float r_stripe3[] = {0.48f, 0.1f, 0.28f, 0.08f, 0.45f, 0.18f};
    drawFilledPolygon(r_stripe3, 3);

    float lc_stripe1[] = {-0.3f, -0.05f, -0.15f, -0.1f, -0.28f, 0.0f};
    drawFilledPolygon(lc_stripe1, 3);
    float lc_stripe2[] = {-0.35f, -0.2f, -0.2f, -0.25f, -0.32f, -0.15f};
    drawFilledPolygon(lc_stripe2, 3);

    float rc_stripe1[] = {0.3f, -0.05f, 0.15f, -0.1f, 0.28f, 0.0f};
    drawFilledPolygon(rc_stripe1, 3);
    float rc_stripe2[] = {0.35f, -0.2f, 0.2f, -0.25f, 0.32f, -0.15f};
    drawFilledPolygon(rc_stripe2, 3);

    glColor3f(tigressBlackR, tigressBlackG, tigressBlackB);
    glBegin(GL_POLYGON);
    glVertex2f(-0.08f, -0.1f);
    glVertex2f(0.08f, -0.1f);
    glVertex2f(0.05f, -0.18f);
    glVertex2f(-0.05f, -0.18f);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.0f, -0.1f);
    glVertex2f(0.0f, -0.05f);
    glEnd();

    glColor3f(tigressBlackR, tigressBlackG, tigressBlackB);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.05f, -0.22f);
    glVertex2f(0.05f, -0.22f);
    glEnd();
    glLineWidth(1.0f);

    glPushMatrix();
    glTranslatef(-0.18f, 0.08f, 0.0f);
    glRotatef(-15, 0.0f, 0.0f, 1.0f);
    glColor3f(tigressBlackR, tigressBlackG, tigressBlackB);
    drawFilledEllipse(0.0f, 0.0f, 0.14f, 0.09f, 40);
    glColor3f(tigressEyeYellowR, tigressEyeYellowG, tigressEyeYellowB);
    drawFilledEllipse(0.0f, 0.0f, 0.12f, 0.08f, 40);
    glColor3f(tigressBlackR, tigressBlackG, tigressBlackB);
    drawFilledEllipse(0.0f, 0.0f, 0.04f, 0.07f, 40);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledCircle(0.03f, 0.03f, 0.02f, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.18f, 0.08f, 0.0f);
    glRotatef(15, 0.0f, 0.0f, 1.0f);
    glColor3f(tigressBlackR, tigressBlackG, tigressBlackB);
    drawFilledEllipse(0.0f, 0.0f, 0.14f, 0.09f, 40);
    glColor3f(tigressEyeYellowR, tigressEyeYellowG, tigressEyeYellowB);
    drawFilledEllipse(0.0f, 0.0f, 0.12f, 0.08f, 40);
    glColor3f(tigressBlackR, tigressBlackG, tigressBlackB);
    drawFilledEllipse(0.0f, 0.0f, 0.04f, 0.07f, 40);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledCircle(0.03f, 0.03f, 0.02f, 20);
    glPopMatrix();

    glFlush();
}

void init()
{
    glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-0.7, 0.7, -0.7, 0.7);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tigress ");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
