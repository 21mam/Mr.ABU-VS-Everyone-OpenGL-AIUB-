#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <cmath>
#include <iostream>

using namespace std;

float fanRotation = 0.0f;
bool isFanOn = true;

int windowWidth = 800;
int windowHeight = 600;

void init()
{
    glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
}

void drawCircle(float cx, float cy, float r, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void drawFan(int x, int y)
{
    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(x - 15, y);
    glVertex2f(x + 15, y);
    glVertex2f(x + 10, y + 15);
    glVertex2f(x - 10, y + 15);
    glEnd();

    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(x - 7, y + 15);
    glVertex2f(x + 7, y + 15);
    glVertex2f(x + 7, y + 180);
    glVertex2f(x - 7, y + 180);
    glEnd();

    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(x - 7, y + 15);
    glVertex2f(x - 5, y + 15);
    glVertex2f(x - 5, y + 180);
    glVertex2f(x - 7, y + 180);
    glEnd();

    glColor3f(0.22f, 0.22f, 0.22f);
    glBegin(GL_QUADS);
    glVertex2f(x - 12, y + 180);
    glVertex2f(x + 12, y + 180);
    glVertex2f(x + 12, y + 200);
    glVertex2f(x - 12, y + 200);
    glEnd();

    glColor3f(0.18f, 0.18f, 0.18f);
    drawCircle(x, y + 200, 15, 20);
    glColor3f(0.28f, 0.28f, 0.28f);
    drawCircle(x, y + 200, 12, 16);

    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glTranslatef(x, y + 200, 0);
        glRotatef(i * 120 + fanRotation, 0, 0, 1);

        glColor3f(0.25f, 0.25f, 0.30f);
        glBegin(GL_POLYGON);
        glVertex2f(5, 0);
        glVertex2f(45, 8);
        glVertex2f(48, 6);
        glVertex2f(48, -6);
        glVertex2f(45, -8);
        glVertex2f(5, 0);
        glEnd();

        glColor3f(0.35f, 0.35f, 0.40f);
        glBegin(GL_QUADS);
        glVertex2f(5, 1);
        glVertex2f(45, 6);
        glVertex2f(45, 4);
        glVertex2f(5, -1);
        glEnd();

        glPopMatrix();
    }

    glColor3f(0.85f, 0.28f, 0.28f);
    drawCircle(x, y + 200, 8, 16);
    glColor3f(0.95f, 0.38f, 0.38f);
    drawCircle(x - 2, y + 202, 3, 10);
    glColor3f(0.15f, 0.15f, 0.15f);
    drawCircle(x, y + 200, 2, 8);
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
    drawFan(400, 100);

    glColor3f(0.2f, 0.2f, 0.2f);
    glRasterPos2f(20, windowHeight - 30);
    string status = "Fan: " + string(isFanOn ? "ON" : "OFF") + " (PRES SPACE )";
    for (char c : status)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glFlush();
}

void update(int value)
{
    if (isFanOn)
    {
        fanRotation += 6.0f;
        if (fanRotation >= 360.0f)
        {
            fanRotation -= 360.0f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(30, update, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == ' ')
    {
        isFanOn = !isFanOn;
        cout << "Fan " << (isFanOn ? "ON" : "OFF") << endl;
    }
    else if (key == 27 || key == 'q' || key == 'Q')
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
    glutCreateWindow("Fan Demo");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    cout << "Fan Controls:" << endl;
    cout << "SPACE - for ON/OFF" << endl;

    glutMainLoop();
    return 0;
}