#define GL_SILENCE_DEPRECATION 

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>  

void display() {
    glClear(GL_COLOR_BUFFER_BIT); 
    glColor3f(1.0f, 0.0f, 0.0f); 
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, 0.0f);  
        glVertex2f(1.0f, 0.0f);   
        glVertex2f(1.0f, 1.0f);   
        glVertex2f(-1.0f, 1.0f);  
    glEnd();

    
    glColor3f(1.0f, 1.0f, 1.0f); 
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); 
        glVertex2f(1.0f, -1.0f);  
        glVertex2f(1.0f, 0.0f);   
        glVertex2f(-1.0f, 0.0f);  
    glEnd();

    glFlush(); 
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);                        
    glutInitWindowSize(800, 600);            
    glutCreateWindow("Flag of Monaco");           
    glutDisplayFunc(display);      
    glutMainLoop(); 
    return 0;
}
