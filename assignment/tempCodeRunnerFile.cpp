#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <iostream>

using namespace std;

int windowWidth = 800;
int windowHeight = 600;

void init()
{
    glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
}

void drawBench(int x, int y)
{
    glColor3f(0.56f, 0.37f, 0.18f);
    glBegin(GL_QUADS);
    glVertex2f(x - 70, y + 38);
    glVertex2f(x + 70, y + 38);
    glVertex2f(x + 70, y + 48);
    glVertex2f(x - 70, y + 48);
    glEnd();

    glColor3f(0.60f, 0.40f, 0.20f);
    glBegin(GL_QUADS);
    glVertex2f(x - 70, y + 48);
    glVertex2f(x + 70, y + 48);
    glVertex2f(x + 68, y + 50);
    glVertex2f(x - 68, y + 50);
    glEnd();

    glColor3f(0.46f, 0.28f, 0.14f);
    glBegin(GL_QUADS);
    glVertex2f(x - 70, y + 35);
    glVertex2f(x + 70, y + 35);
    glVertex2f(x + 70, y + 38);
    glVertex2f(x - 70, y + 38);
    glEnd();

    glColor3f(0.58f, 0.39f, 0.19f);
    glBegin(GL_QUADS);
    glVertex2f(x - 70, y + 50);
    glVertex2f(x + 70, y + 50);
    glVertex2f(x + 70, y + 95);
    glVertex2f(x - 70, y + 95);
    glEnd();

    glColor3f(0.62f, 0.42f, 0.21f);
    glBegin(GL_QUADS);
    glVertex2f(x - 70, y + 95);
    glVertex2f(x + 70, y + 95);
    glVertex2f(x + 68, y + 98);
    glVertex2f(x - 68, y + 98);
    glEnd();

    glColor3f(0.46f, 0.28f, 0.14f);
    glLineWidth(2.8);
    for (int i = 0; i < 5; i++)
    {
        int slaty = y + 58 + i * 9;
        glBegin(GL_LINES);
        glVertex2f(x - 68, slaty);
        glVertex2f(x + 68, slaty);
        glEnd();
    }

    glColor3f(0.56f, 0.37f, 0.18f);
    for (int i = -1; i <= 1; i += 2)
    {
        glBegin(GL_QUADS);
        glVertex2f(x + i * 65, y + 38);
        glVertex2f(x + i * 75, y + 38);
        glVertex2f(x + i * 75, y + 72);
        glVertex2f(x + i * 65, y + 72);
        glEnd();

        glColor3f(0.60f, 0.40f, 0.20f);
        glBegin(GL_QUADS);
        glVertex2f(x + i * 65, y + 72);
        glVertex2f(x + i * 75, y + 72);
        glVertex2f(x + i * 75, y + 78);
        glVertex2f(x + i * 65, y + 78);
        glEnd();
        glColor3f(0.56f, 0.37f, 0.18f);
    }

    glColor3f(0.44f, 0.27f, 0.13f);
    for (int i = -1; i <= 1; i += 2)
    {
        glBegin(GL_QUADS);
        glVertex2f(x + i * 54 - 5, y);
        glVertex2f(x + i * 54 + 5, y);
        glVertex2f(x + i * 54 + 5, y + 50);
        glVertex2f(x + i * 54 - 5, y + 50);
        glEnd();
    }

    for (int i = -1; i <= 1; i += 2)
    {
        glBegin(GL_QUADS);
        glVertex2f(x + i * 54 - 5, y + 50);
        glVertex2f(x + i * 54 + 5, y + 50);
        glVertex2f(x + i * 54 + 5, y + 95);
        glVertex2f(x + i * 54 - 5, y + 95);
        glEnd();
    }

    glColor3f(0.46f, 0.28f, 0.14f);
    glBegin(GL_QUADS);
    glVertex2f(x - 60, y + 20);
    glVertex2f(x + 60, y + 20);
    glVertex2f(x + 60, y + 25);
    glVertex2f(x - 60, y + 25);
    glEnd();

    glColor3f(0.52f, 0.33f, 0.16f);
    for (int i = -1; i <= 1; i += 2)
    {
        glBegin(GL_QUADS);
        glVertex2f(x + i * 54 - 5, y);
        glVertex2f(x + i * 54 - 3, y);
        glVertex2f(x + i * 54 - 3, y + 50);
        glVertex2f(x + i * 54 - 5, y + 50);
        glEnd();
    }
}

void drawGround()
{
    glColor3f(0.42f, 0.65f, 0.32f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, 100);
    glVertex2f(0, 100);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawGround();
    drawBench(400, 100);

    glColor3f(0.2f, 0.2f, 0.2f);
    glRasterPos2f(20, windowHeight - 30);
    string instructions = "Bench";
    for (char c : instructions)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27 || key == 'q' || key == 'Q')
    {
        exit(0);
    }
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);

    windowWidth = w;
    windowHeight = h;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bench ");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

     glutMainLoop();
    return 0;
}
