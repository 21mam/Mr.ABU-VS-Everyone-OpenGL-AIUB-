#include <GLUT/glut.h>
#include <math.h>
#include <iostream>

// Character properties
float characterX = 0.0f;
float characterY = 0.0f;
float characterSize = 0.1f;
float characterSpeed = 0.02f;
float characterColor[3] = {0.0f, 0.8f, 1.0f}; // RGB color

// Camera properties
float cameraX = 0.0f;
float cameraY = 0.0f;

// Grid for visual reference
const int gridSize = 20;
bool showGrid = true;

// Initialize OpenGL
void init()
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Draw the character as a simple triangle/arrow
void drawCharacter()
{
    glPushMatrix();
    glTranslatef(characterX, characterY, 0.0f);

    // Set character color
    glColor3f(characterColor[0], characterColor[1], characterColor[2]);

    // Draw character as a triangle pointing up
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f, characterSize);
    glVertex2f(-characterSize, -characterSize);
    glVertex2f(characterSize, -characterSize);
    glEnd();

    // Draw a small circle at the center for reference
    glColor3f(1.0f, 1.0f, 0.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(0.0f, 0.0f);
    glEnd();

    glPopMatrix();
}

// Draw a grid for visual reference
void drawGrid()
{
    if (!showGrid)
        return;

    glColor4f(0.3f, 0.3f, 0.3f, 0.5f);
    glLineWidth(1.0f);

    // Draw vertical lines
    for (int i = -gridSize; i <= gridSize; i++)
    {
        glBegin(GL_LINES);
        glVertex2f(i, -gridSize);
        glVertex2f(i, gridSize);
        glEnd();
    }

    // Draw horizontal lines
    for (int i = -gridSize; i <= gridSize; i++)
    {
        glBegin(GL_LINES);
        glVertex2f(-gridSize, i);
        glVertex2f(gridSize, i);
        glEnd();
    }

    // Draw axes
    glColor4f(0.8f, 0.8f, 0.8f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-gridSize, 0.0f);
    glVertex2f(gridSize, 0.0f);
    glVertex2f(0.0f, -gridSize);
    glVertex2f(0.0f, gridSize);
    glEnd();
}

// Display callback function
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up camera/translation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-cameraX, -cameraY, 0.0f);

    // Draw grid and character
    drawGrid();
    drawCharacter();

    // Display coordinates
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Display position info
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, 580);
    std::string positionText = "Position: (" + std::to_string(characterX) + ", " + std::to_string(characterY) + ")";
    for (char c : positionText)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

    glRasterPos2f(10, 560);
    std::string cameraText = "Camera: (" + std::to_string(cameraX) + ", " + std::to_string(cameraY) + ")";
    for (char c : cameraText)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

    // Display controls
    glRasterPos2f(10, 530);
    std::string controls = "CONTROLS: Arrow/WASD = Move, Space = Toggle Grid, R = Reset";
    for (char c : controls)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

// Keyboard callback function
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 'W':
        characterY += characterSpeed;
        break;
    case 's':
    case 'S':
        characterY -= characterSpeed;
        break;
    case 'a':
    case 'A':
        characterX -= characterSpeed;
        break;
    case 'd':
    case 'D':
        characterX += characterSpeed;
        break;
    case ' ': // Space to toggle grid
        showGrid = !showGrid;
        break;
    case 'r':
    case 'R': // Reset position
        characterX = 0.0f;
        characterY = 0.0f;
        cameraX = 0.0f;
        cameraY = 0.0f;
        break;
    case 27: // ESC key
        exit(0);
        break;
    }

    // Update camera to follow character
    cameraX = characterX;
    cameraY = characterY;

    glutPostRedisplay();
}

// Special key callback for arrow keys
void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        characterY += characterSpeed;
        break;
    case GLUT_KEY_DOWN:
        characterY -= characterSpeed;
        break;
    case GLUT_KEY_LEFT:
        characterX -= characterSpeed;
        break;
    case GLUT_KEY_RIGHT:
        characterX += characterSpeed;
        break;
    }

    // Update camera to follow character
    cameraX = characterX;
    cameraY = characterY;

    glutPostRedisplay();
}

// Reshape callback function
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Calculate aspect ratio
    float aspect = (float)width / (float)height;

    // Set up orthographic projection
    if (width <= height)
    {
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
    else
    {
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    }

    glMatrixMode(GL_MODELVIEW);
}

// Timer function for animations (optional)
void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

// Main function
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Game Character - Move with Arrow/WASD Keys");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);

    std::cout << "Game Character Controls:\n";
    std::cout << "=======================\n";
    std::cout << "Arrow Keys or WASD: Move character\n";
    std::cout << "Space: Toggle grid display\n";
    std::cout << "R: Reset character position\n";
    std::cout << "ESC: Exit program\n";

    glutMainLoop();
    return 0;
}