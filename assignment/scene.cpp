#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <cmath>
#include <iostream>

using namespace std;

float fanRotation = 0.0f;
bool isFanOn = true;

int windowWidth = 1200;
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

void drawSun()
{
    glColor3f(1.0f, 0.95f, 0.3f);
    drawCircle(1000, 600, 50, 30);
}

void drawGround()
{
    glBegin(GL_QUADS);
    glColor3f(0.58f, 0.78f, 0.88f);
    glVertex2f(0, 250);
    glVertex2f(windowWidth, 250);
    glColor3f(0.68f, 0.85f, 0.90f);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.42f, 0.65f, 0.32f);
    glVertex2f(0, 80);
    glVertex2f(windowWidth, 80);
    glColor3f(0.48f, 0.70f, 0.38f);
    glVertex2f(windowWidth, 250);
    glVertex2f(0, 250);
    glEnd();

    glColor3f(0.72f, 0.68f, 0.62f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, 80);
    glVertex2f(0, 80);
    glEnd();

    glColor3f(0.62f, 0.58f, 0.52f);
    glLineWidth(1.5);
    for (int i = 0; i < 8; i++)
    {
        int lineY = 10 + i * 9;
        glBegin(GL_LINES);
        glVertex2f(0, lineY);
        glVertex2f(windowWidth, lineY);
        glEnd();
    }

    for (int i = 0; i < 6; i++)
    {
        int lineX = 200 + i * 180;
        glBegin(GL_LINES);
        glVertex2f(lineX, 0);
        glVertex2f(lineX, 80);
        glEnd();
    }
}

void drawTree(int x, int y)
{
    glColor3f(0.55f, 0.35f, 0.20f);
    glBegin(GL_QUADS);
    glVertex2f(x - 12, y);
    glVertex2f(x + 12, y);
    glVertex2f(x + 12, y + 80);
    glVertex2f(x - 12, y + 80);
    glEnd();

    glColor3f(0.25f, 0.55f, 0.25f);
    drawCircle(x, y + 100, 50, 20);
    drawCircle(x - 30, y + 85, 35, 16);
    drawCircle(x + 30, y + 85, 35, 16);
}

void drawBuilding(int x, int y)
{
    glColor3f(0.90f, 0.87f, 0.82f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 230, y);
    glVertex2f(x + 230, y + 330);
    glVertex2f(x, y + 330);
    glEnd();

    glColor3f(0.80f, 0.77f, 0.72f);
    glBegin(GL_QUADS);
    glVertex2f(x + 210, y);
    glVertex2f(x + 230, y);
    glVertex2f(x + 230, y + 330);
    glVertex2f(x + 210, y + 330);
    glEnd();

    glColor3f(0.60f, 0.58f, 0.54f);
    glLineWidth(2.5);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + 230, y);
    glVertex2f(x + 230, y + 330);
    glVertex2f(x, y + 330);
    glEnd();

    glLineWidth(1.5);
    for (int i = 1; i < 4; i++)
    {
        int lineY = y + i * 82;
        glBegin(GL_LINES);
        glVertex2f(x, lineY);
        glVertex2f(x + 230, lineY);
        glEnd();
    }

    glColor3f(0.58f, 0.35f, 0.25f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 15, y + 330);
    glVertex2f(x + 245, y + 330);
    glVertex2f(x + 115, y + 395);
    glEnd();

    glColor3f(0.48f, 0.28f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 115, y + 395);
    glVertex2f(x + 245, y + 330);
    glVertex2f(x + 115, y + 330);
    glEnd();

    glColor3f(0.40f, 0.24f, 0.16f);
    glLineWidth(2.5);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 15, y + 330);
    glVertex2f(x + 245, y + 330);
    glVertex2f(x + 115, y + 395);
    glEnd();

    for (int floor = 1; floor < 4; floor++)
    {
        for (int col = 0; col < 4; col++)
        {
            int wx = x + 22 + col * 54;
            int wy = y + 15 + floor * 82;

            glColor3f(0.50f, 0.68f, 0.85f);
            glBegin(GL_QUADS);
            glVertex2f(wx, wy);
            glVertex2f(wx + 40, wy);
            glVertex2f(wx + 40, wy + 58);
            glVertex2f(wx, wy + 58);
            glEnd();

            glColor3f(0.70f, 0.85f, 0.95f);
            glBegin(GL_QUADS);
            glVertex2f(wx + 2, wy + 40);
            glVertex2f(wx + 15, wy + 40);
            glVertex2f(wx + 15, wy + 56);
            glVertex2f(wx + 2, wy + 56);
            glEnd();

            glColor3f(0.28f, 0.26f, 0.23f);
            glLineWidth(3.0);
            glBegin(GL_LINE_LOOP);
            glVertex2f(wx, wy);
            glVertex2f(wx + 40, wy);
            glVertex2f(wx + 40, wy + 58);
            glVertex2f(wx, wy + 58);
            glEnd();

            glLineWidth(2.0);
            glBegin(GL_LINES);
            glVertex2f(wx, wy + 29);
            glVertex2f(wx + 40, wy + 29);
            glVertex2f(wx + 20, wy);
            glVertex2f(wx + 20, wy + 58);
            glEnd();
        }
    }

    glColor3f(0.45f, 0.26f, 0.16f);
    glBegin(GL_QUADS);
    glVertex2f(x + 95, y);
    glVertex2f(x + 135, y);
    glVertex2f(x + 135, y + 80);
    glVertex2f(x + 95, y + 80);
    glEnd();

    glColor3f(0.65f, 0.62f, 0.58f);
    glLineWidth(6.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 95, y);
    glVertex2f(x + 135, y);
    glVertex2f(x + 135, y + 80);
    glVertex2f(x + 95, y + 80);
    glEnd();

    glColor3f(0.35f, 0.20f, 0.12f);
    glLineWidth(2.5);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 95, y);
    glVertex2f(x + 135, y);
    glVertex2f(x + 135, y + 80);
    glVertex2f(x + 95, y + 80);
    glEnd();

    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 100, y + 58);
    glVertex2f(x + 130, y + 58);
    glVertex2f(x + 130, y + 98);
    glVertex2f(x + 100, y + 98);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 100, y + 7);
    glVertex2f(x + 130, y + 7);
    glVertex2f(x + 130, y + 50);
    glVertex2f(x + 100, y + 50);
    glEnd();

    glColor3f(0.72f, 0.62f, 0.32f);
    drawCircle(x + 124, y + 55, 4, 16);
    glBegin(GL_QUADS);
    glVertex2f(x + 124, y + 53);
    glVertex2f(x + 132, y + 53);
    glVertex2f(x + 132, y + 57);
    glVertex2f(x + 124, y + 57);
    glEnd();

    glColor3f(0.70f, 0.68f, 0.64f);
    glBegin(GL_QUADS);
    glVertex2f(x + 85, y);
    glVertex2f(x + 145, y);
    glVertex2f(x + 145, y + 5);
    glVertex2f(x + 85, y + 5);
    glEnd();
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

void drawLamppost(int x, int y)
{
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(x - 5, y);
    glVertex2f(x + 5, y);
    glVertex2f(x + 5, y + 180);
    glVertex2f(x - 5, y + 180);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x - 5, y + 180);
    glVertex2f(x + 40, y + 180);
    glVertex2f(x + 40, y + 185);
    glVertex2f(x - 5, y + 185);
    glEnd();

    glColor3f(0.95f, 0.90f, 0.70f);
    drawCircle(x + 35, y + 170, 12, 16);

    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(x - 15, y);
    glVertex2f(x + 15, y);
    glVertex2f(x + 10, y + 15);
    glVertex2f(x - 10, y + 15);
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

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawSun();
    drawGround();

    drawTree(150, 250);
    drawTree(380, 250);

    drawBuilding(850, 80);

    drawLamppost(250, 80);
    drawBench(450, 80);
    drawBench(750, 80);
    drawFan(600, 80);

    glColor3f(0.2f, 0.2f, 0.2f);
    glRasterPos2f(20, windowHeight - 30);
    string status = "Fan: " + string(isFanOn ? "ON" : "OFF") + " (SPACE )";
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
    glutCreateWindow("Park Scene");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    cout << "Park Scene Controls:" << endl;
    cout << "SPACE - ON/OFF fan" << endl;

    glutMainLoop();
    return 0;
}
