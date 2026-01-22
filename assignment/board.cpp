#define GL_SILENCE_DEPRECATION 

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <cstdlib>
#include <cmath>
using namespace std;

void poli(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4)
{
    glBegin(GL_POLYGON);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glVertex2f(x3, y3);
        glVertex2f(x4, y4);
    glEnd();
}

void myInit ()
{
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);  
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 600.0, 0.0, 600.0, -10.0, 10.0);
}

void display ()
{
    glClear(GL_COLOR_BUFFER_BIT);

    int cellSize = 150;   

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            
            if ( (row + col) % 2 == 0 )
                glColor3f(1.0f, 1.0f, 1.0f);  
            else
                glColor3f(0.0f, 0.0f, 0.0f);  

            int x = col * cellSize;
            int y = row * cellSize;

            poli(x, y,
                 x + cellSize, y,
                 x + cellSize, y + cellSize,
                 x, y + cellSize);
        }
    }

    glFlush();
}

int main (int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(600, 600);
    glutInitWindowPosition(600, 200);
    glutCreateWindow("Chess Board");

    myInit();
    glutDisplayFunc(display);
    glutMainLoop();
}
