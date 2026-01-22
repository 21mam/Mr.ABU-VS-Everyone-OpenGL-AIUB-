#define GL_SILENCE_DEPRECATION 

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;

#define PI acos(-1)
GLdouble x = 0;

void circle(GLfloat x, GLfloat y, GLfloat radius);
void drawCircle(GLdouble x, GLdouble y, GLfloat r, GLint steps);

void display() {
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 1.0f);             
    glVertex2f(-6.0, 0.1);
    glVertex2f(0.3, -1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, -0.5);
    glVertex2f(0.6, 0.1);
    glEnd();

    
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 0.0f);             
    glVertex2f(-.6, .1);
    glVertex2f(.6, .1);
    glVertex2f(0.0, -.1);
    glEnd();

    
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-.4, .05);
    glVertex2f(-.1, -0.04);
    glVertex2f(-.32, -.03);
    glEnd();

    
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(.4, .05);
    glVertex2f(.1, -0.04);
    glVertex2f(.32, -.03);
    glEnd();

    
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-0.13, 0);
    glVertex2f(0.13, 0);
    glVertex2f(0.0, -.16);
    glEnd();

   
    glColor3f(1.0f, 1.0f, 1.0f);             
    drawCircle(0.0, +0.17, .15, 100);

    
    glColor3f(0.0f, 0.0f, 0.0f);             
    drawCircle(0.0, 0.03, .03, 100);    

    
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(-.04, .08);
    glVertex2f(-.1, 0.0);
    glVertex2f(0.0, 0.0);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(.04, .08);
    glVertex2f(.1, 0.0);
    glVertex2f(0.0, 0.0);
    glEnd();

    glFlush();
    x += 0.001;
    if (x > 1.00) {
        x = -1.00;
    }
    glutSwapBuffers();
}

void drawCircle(GLdouble x, GLdouble y, GLfloat r, GLint steps) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    GLfloat angle;
    for (GLint i = 0; i <= steps; ++i) {
        angle = i * 2.0f * PI / steps;
        glVertex2f(cos(angle) * r + x, sin(angle) * r + y);
    }
    glEnd();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Batman Logo");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}
