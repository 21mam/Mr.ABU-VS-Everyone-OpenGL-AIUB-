// happy_bday.cpp
#define GL_SILENCE_DEPRECATION 

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <cstdlib>
#include <cmath>   // for cosf, sinf

// Window size
const int WIDTH  = 800;
const int HEIGHT = 600;

// helper: draw a string using a bitmap font
void drawBitmapString(void *font, const char *str) {
    for (const char *c = str; *c != '\0'; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

// compute pixel width of a bitmap string (for centering)
int bitmapStringWidth(void *font, const char *str) {
    int w = 0;
    for (const char *c = str; *c != '\0'; ++c) {
        w += glutBitmapWidth(font, *c);
    }
    return w;
}

// draw a filled circle (used for balloons)
void filledCircle(float cx, float cy, float r, int num_segments = 40) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= num_segments; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
            float x = r * cosf(theta);
            float y = r * sinf(theta);
            glVertex2f(cx + x, cy + y);
        }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw confetti (small random-ish colored squares)
    for (int i = 0; i < 60; ++i) {
        float x = -0.95f + (i % 10) * 0.2f + ((i * 7) % 10) * 0.01f;
        float y =  0.6f - (i / 10) * 0.18f + ((i * 13) % 7) * 0.01f;
        float s =  0.01f + ((i * 3) % 5) * 0.003f;
        glColor3f(((i*37)%100)/100.0f, ((i*59)%100)/100.0f, ((i*23)%100)/100.0f);
        glBegin(GL_QUADS);
            glVertex2f(x-s, y-s);
            glVertex2f(x+s, y-s);
            glVertex2f(x+s, y+s);
            glVertex2f(x-s, y+s);
        glEnd();
    }

    // Draw three balloons
    // Left balloon: red
    glColor3f(0.86f, 0.1f, 0.15f);
    filledCircle(-0.55f, -0.05f, 0.12f);
    glLineWidth(2.0f);
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
        glVertex2f(-0.55f, -0.17f);
        glVertex2f(-0.55f, -0.4f);
    glEnd();

    // Middle balloon: blue
    glColor3f(0.12f, 0.45f, 0.82f);
    filledCircle(0.0f, 0.05f, 0.14f);
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
        glVertex2f(0.0f, -0.09f);
        glVertex2f(0.0f, -0.42f);
    glEnd();

    // Right balloon: green
    glColor3f(0.12f, 0.74f, 0.34f);
    filledCircle(0.55f, -0.05f, 0.12f);
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
        glVertex2f(0.55f, -0.17f);
        glVertex2f(0.55f, -0.4f);
    glEnd();

    // Draw a banner rectangle behind text
    glColor3f(1.0f, 0.95f, 0.6f); // pale yellow
    glBegin(GL_QUADS);
        glVertex2f(-0.75f,  0.15f);
        glVertex2f( 0.75f,  0.15f);
        glVertex2f( 0.75f, -0.05f);
        glVertex2f(-0.75f, -0.05f);
    glEnd();

    // Draw the birthday text centered
    const char message[] = "Happy birthday zimam";
    void *font = GLUT_BITMAP_HELVETICA_18;
    int textWidth = bitmapStringWidth(font, message);

    // convert pixel width to normalized device coordinates [-1,1]
    float pxToND       = 2.0f / WIDTH;
    float textWidthNDC = textWidth * pxToND;
    float xNDC         = -textWidthNDC / 2.0f; // center horizontally
    float yNDC         = 0.02f;                // vertical position on banner

    // Set text color
    glColor3f(0.25f, 0.05f, 0.45f);

    // Set raster position directly in NDC (since projection is [-1,1])
    glRasterPos2f(xNDC, yNDC);
    drawBitmapString(font, message);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // simple orthographic projection matching [-1,1] both axes
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Happy Birthday Zimam!");

    // white background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
