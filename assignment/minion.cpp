#define GL_SILENCE_DEPRECATION

#ifdef __APPLE__

// #include <GLUT/glut.h>  // use this

#else
// #include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>
#include <string.h>
#include <GLUT/glut.h>

const float PI = 3.14159f;

void drawMinionBody()
{

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.8f, -2.0f);
    glVertex2f(1.8f, -2.0f);
    glVertex2f(1.8f, 1.0f);
    glVertex2f(-1.8f, 1.0f);
    glEnd();

    glColor3f(0.0f, 0.6f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(-1.9f, -2.0f);
    glVertex2f(1.9f, -2.0f);
    glVertex2f(1.9f, -0.5f);
    glVertex2f(-1.9f, -0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(-0.7f, -0.5f);
    glVertex2f(0.7f, -0.5f);
    glVertex2f(0.7f, 0.6f);
    glVertex2f(-0.7f, 0.6f);
    glEnd();

    glColor3f(0.0f, 0.5f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(-0.4f, 0.0f);
    glVertex2f(0.4f, 0.0f);
    glVertex2f(0.4f, 0.4f);
    glVertex2f(-0.4f, 0.4f);
    glEnd();

    glColor3f(0.0f, 0.6f, 0.5f);
    glBegin(GL_TRIANGLES);

    glVertex2f(-0.4f, 0.6f);
    glVertex2f(-1.6f, 1.0f);
    glVertex2f(-1.0f, 0.6f);

    glVertex2f(0.4f, 0.6f);
    glVertex2f(1.6f, 1.0f);
    glVertex2f(1.0f, 0.6f);
    glEnd();

    glColor3f(0.1f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-1.0f, 0.6f);
    for (int i = 0; i <= 360; i += 30)
    {
        float a = i * PI / 180.0f;
        glVertex2f(-1.0f + std::cos(a) * 0.1f,
                   0.6f + std::sin(a) * 0.1f);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(1.0f, 0.6f);
    for (int i = 0; i <= 360; i += 30)
    {
        float a = i * PI / 180.0f;
        glVertex2f(1.0f + std::cos(a) * 0.1f,
                   0.6f + std::sin(a) * 0.1f);
    }
    glEnd();
}

void drawMinionHead()
{

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 2.5f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = std::cos(angle) * 1.8f;
        float y = 2.5f + std::sin(angle) * 1.8f;
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(0.35f, 0.25f, 0.18f);
    glBegin(GL_QUADS);
    glVertex2f(-1.6f, 2.9f);
    glVertex2f(1.6f, 2.9f);
    glVertex2f(1.6f, 2.7f);
    glVertex2f(-1.6f, 2.7f);
    glEnd();

    glColor3f(0.35f, 0.25f, 0.18f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-0.6f, 2.8f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = -0.6f + std::cos(angle) * 0.7f;
        float y = 2.8f + std::sin(angle) * 0.7f;
        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.6f, 2.8f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = 0.6f + std::cos(angle) * 0.7f;
        float y = 2.8f + std::sin(angle) * 0.7f;
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(0.25f, 0.18f, 0.12f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-0.6f, 2.8f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = -0.6f + std::cos(angle) * 0.6f;
        float y = 2.8f + std::sin(angle) * 0.6f;
        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.6f, 2.8f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = 0.6f + std::cos(angle) * 0.6f;
        float y = 2.8f + std::sin(angle) * 0.6f;
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-0.6f, 2.8f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = -0.6f + std::cos(angle) * 0.5f;
        float y = 2.8f + std::sin(angle) * 0.5f;
        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.6f, 2.8f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = 0.6f + std::cos(angle) * 0.5f;
        float y = 2.8f + std::sin(angle) * 0.5f;
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-0.7f, 2.8f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = -0.7f + std::cos(angle) * 0.2f;
        float y = 2.8f + std::sin(angle) * 0.2f;
        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.5f, 2.8f);
    for (int i = 0; i <= 360; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = 0.5f + std::cos(angle) * 0.2f;
        float y = 2.8f + std::sin(angle) * 0.2f;
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 210; i <= 330; i += 10)
    {
        float angle = i * PI / 180.0f;
        float x = std::cos(angle) * 0.8f;
        float y = 1.7f + std::sin(angle) * 0.6f;
        glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawMinionLimbs()
{

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);

    glVertex2f(-1.2f, 0.3f);
    glVertex2f(-1.7f, 0.3f);
    glVertex2f(-2.4f, -0.4f);
    glVertex2f(-1.9f, -0.4f);

    glVertex2f(1.2f, 0.3f);
    glVertex2f(1.7f, 0.3f);
    glVertex2f(1.9f, -0.4f);
    glVertex2f(2.4f, -0.4f);
    glEnd();

    glColor3f(0.35f, 0.20f, 0.05f);

    glBegin(GL_QUADS);
    glVertex2f(-2.6f, -0.4f);
    glVertex2f(-2.1f, -0.4f);
    glVertex2f(-2.1f, -0.9f);
    glVertex2f(-2.6f, -0.9f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(2.1f, -0.4f);
    glVertex2f(2.6f, -0.4f);
    glVertex2f(2.6f, -0.9f);
    glVertex2f(2.1f, -0.9f);
    glEnd();

    glBegin(GL_TRIANGLES);

    glVertex2f(-2.55f, -0.9f);
    glVertex2f(-2.45f, -1.2f);
    glVertex2f(-2.35f, -0.9f);

    glVertex2f(-2.35f, -0.9f);
    glVertex2f(-2.25f, -1.2f);
    glVertex2f(-2.15f, -0.9f);

    glVertex2f(-2.15f, -0.9f);
    glVertex2f(-2.05f, -1.2f);
    glVertex2f(-1.95f, -0.9f);

    glVertex2f(2.55f, -0.9f);
    glVertex2f(2.45f, -1.2f);
    glVertex2f(2.35f, -0.9f);

    glVertex2f(2.35f, -0.9f);
    glVertex2f(2.25f, -1.2f);
    glVertex2f(2.15f, -0.9f);

    glVertex2f(2.15f, -0.9f);
    glVertex2f(2.05f, -1.2f);
    glVertex2f(1.95f, -0.9f);
    glEnd();

    glColor3f(0.0f, 0.6f, 0.5f);
    glBegin(GL_QUADS);

    glVertex2f(-1.0f, -2.0f);
    glVertex2f(-0.5f, -2.0f);
    glVertex2f(-0.5f, -2.7f);
    glVertex2f(-1.0f, -2.7f);

    glVertex2f(0.5f, -2.0f);
    glVertex2f(1.0f, -2.0f);
    glVertex2f(1.0f, -2.7f);
    glVertex2f(0.5f, -2.7f);
    glEnd();

    glColor3f(0.25f, 0.15f, 0.05f);
    glBegin(GL_QUADS);

    glVertex2f(-1.1f, -2.7f);
    glVertex2f(-0.4f, -2.7f);
    glVertex2f(-0.4f, -3.1f);
    glVertex2f(-1.1f, -3.1f);

    glVertex2f(0.4f, -2.7f);
    glVertex2f(1.1f, -2.7f);
    glVertex2f(1.1f, -3.1f);
    glVertex2f(0.4f, -3.1f);
    glEnd();
}

void drawMinion()
{
    drawMinionBody();
    drawMinionHead();
    drawMinionLimbs();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    drawMinion();
    glFlush();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-4.0, 4.0, -4.0, 6.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 'Q' || key == 27)
    {
        exit(0);
    }
}

void init()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Minion");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
