#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <iostream>
#include <vector>
using namespace std;

enum class ShapeType { Point, Line, Triangle, Quad, Polygon };

class Position {
public:
    float pos[2];
    Position(float x, float y)
    {
        pos[0] = x;
        pos[1] = y;
    }
};

class Shape {
public:
    float color[3];
    float pointSize;
    vector<Position> vertices;
    ShapeType shape;
    Shape() {
        pointSize = 10.0f;
    }
};


class Point : public Shape {
public:
    Point() :Shape()
    {
        for (int i = 0; i < *(&color + 1) - color; i++) color[i] = 0.0f;
        for (int i = 0; i < 1; i++) vertices.push_back(Position(0.0f, 0.0f));
        shape = ShapeType::Point;
    }
};

class Line : public Shape {
public:
    Line() :Shape()
    {
        for (int i = 0; i < *(&color + 1) - color; i++) color[i] = 0.0f;
        //for (int i = 0; i < 2; i++) vertices.push_back(Position(0.0f, 0.0f));
        shape = ShapeType::Line;
    }
};

class Triangle : public Shape {
public:
    Triangle() :Shape()
    {
        for (int i = 0; i < *(&color + 1) - color; i++) color[i] = 0.0f;
        for (int i = 0; i < 3; i++) vertices.push_back(Position(0.0f, 0.0f));
        shape = ShapeType::Triangle;
    }
};

class Quad : public Shape {
public:
    Quad() :Shape()
    {
        for (int i = 0; i < *(&color + 1) - color; i++) color[i] = 0.0f;
        for (int i = 0; i < 2; i++) vertices.push_back(Position(0.0f, 0.0f));
        shape = ShapeType::Quad;
    }
};

class Polygon : public Shape {
public:
    Polygon() :Shape()
    {
        for (int i = 0; i < *(&color + 1) - color; i++) color[i] = 0.0f;
        //for (int i = 0; i < 3; i++) vertices.push_back(0.0f);
        shape = ShapeType::Polygon;
    }
};

float canvasSize[] = { 10.0f, 10.0f };
int rasterSize[] = { 800, 600 };

// vertices storage
int numOfVertices = 0;
float vTri[2 * 3];
float vPoint[2 * 1];
vector<float> vLine;
float vRec[2 * 2];
vector<float> vPoly;
float color[3];

Position mousePosition = Position(0.0f, 0.0f);

// store drawn shapes
vector<Shape> shapes;
//check whether a shape is finished
bool isFinished = false;

// current shape and pointSize
ShapeType currentShape = ShapeType::Point;
float currentPointSize = 5.0f;

void clearDynamicVertices()
{
    vLine.clear();
    vPoly.clear();
}

void init(void)
{
    for (int i = 0; i < 6; i++)
        vTri[i] = 0.0f;
    mousePosition.pos[0] = mousePosition.pos[1] = 0.0f;
    color[0] = 1.0f;
    color[1] = color[2] = 0.0f;
    shapes.clear();
}

void drawCursor()
{
    glColor3f(color[0], color[1], color[2]);
    glPointSize(currentPointSize);
    glLineWidth(currentPointSize);
    glBegin(GL_POINTS);
    glVertex2fv(mousePosition.pos);
    glEnd();
    glPointSize(1.0f);
}

// draw stored shapes
void drawPreviousShapes() 
{
    for (int i = 0; i < shapes.size(); i++) {
        glColor3fv(shapes[i].color);
        glPointSize(shapes[i].pointSize);
        glLineWidth(shapes[i].pointSize);
        switch (shapes[i].shape)
        {
        case ShapeType::Point:
            glBegin(GL_POINTS);
            glVertex2f(shapes[i].vertices[0].pos[0], shapes[i].vertices[0].pos[1]);
            glEnd();
            break;
        case ShapeType::Line:
            glBegin(GL_LINE_STRIP);
            for (int j = 0; j < shapes[i].vertices.size(); j++) {
                glVertex2f(shapes[i].vertices[j].pos[0], shapes[i].vertices[j].pos[1]);
            }
            glEnd();
            break;
        case ShapeType::Triangle:
            glBegin(GL_TRIANGLES);
            for (int j = 0; j < shapes[i].vertices.size(); j++) {
                glVertex2fv(shapes[i].vertices[j].pos);
            }
            glEnd();
            break;
        case ShapeType::Quad:
            glBegin(GL_QUADS);
            glVertex2f(shapes[i].vertices[0].pos[0], shapes[i].vertices[0].pos[1]);
            glVertex2f(shapes[i].vertices[0].pos[0], shapes[i].vertices[1].pos[1]);
            glVertex2f(shapes[i].vertices[1].pos[0], shapes[i].vertices[1].pos[1]);
            glVertex2f(shapes[i].vertices[1].pos[0], shapes[i].vertices[0].pos[1]);
            glEnd();
            break;
        case ShapeType::Polygon:
            glBegin(GL_POLYGON);
            for (int j = 0; j < shapes[i].vertices.size(); j++) {
                glVertex2fv(shapes[i].vertices[j].pos);
            }
            glEnd();
            break;
        default:
            break;
        }
    }
}


void display(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3fv(color);
    glPointSize(currentPointSize);
    glLineWidth(currentPointSize);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // first draw previous shapes
    drawPreviousShapes();

    Shape temp;

    glColor3fv(color);
    glPointSize(currentPointSize);
    glLineWidth(currentPointSize);

    if (currentShape == ShapeType::Triangle) {
        if (numOfVertices > 0 && numOfVertices < 3) {

            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < numOfVertices; i++)
                glVertex2fv(vTri + i * 2);
            glVertex2fv(mousePosition.pos);
            glEnd();
        }
        else if (numOfVertices == 3) {
            isFinished = true;
            glBegin(GL_TRIANGLES);
            for (int i = 0; i < numOfVertices; i++)
                glVertex2fv(vTri + i * 2);
            glEnd();
            //store it to shapes
            temp.pointSize = currentPointSize;
            temp = Triangle();
            for (int i = 0; i < *(&color + 1) - color; i++) temp.color[i] = color[i];
            for (int i = 0; i < numOfVertices; i++) {
                for (int j = 0; j < 2; j++) {
                    temp.vertices[i].pos[j] = vTri[2*i+j];
                }
            }
            shapes.push_back(temp);
            numOfVertices = 0;
            isFinished = false;
        }
    }
    
    else if (currentShape == ShapeType::Point) {
        if (numOfVertices == 0 && isFinished) {
            glBegin(GL_POINTS);
            glVertex2f(vPoint[0], vPoint[1]);
            glEnd();
            //store point to shapes
            temp = Point();
            temp.pointSize = currentPointSize;
            for (int i = 0; i < *(&color + 1) - color; i++) temp.color[i] = color[i];
            for (int i = 0; i < numOfVertices + 1; i++) {
                for (int j = 0; j < 2; j++) {
                    temp.vertices[i].pos[j] = vPoint[2 * i + j];
                }
            }
            shapes.push_back(temp);
            isFinished = false;
        }
        else if (numOfVertices > 0) {
            glBegin(GL_POINTS);
            glVertex2f(vPoint[0], vPoint[1]);
            glEnd();
        }
    }

    else if (currentShape == ShapeType::Line) {
        if (numOfVertices == 0 && isFinished) {
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < vLine.size() / 2; i++)
                glVertex2f(vLine[2 * i], vLine[2 * i + 1]);
            glEnd();
            //storage line in shapes
            temp = Line();
            temp.pointSize = currentPointSize;
            for (int i = 0; i < *(&color + 1) - color; i++) temp.color[i] = color[i];
            for (int i = 0; i < vLine.size() / 2; i++) {
                temp.vertices.push_back(Position(vLine[2 * i], vLine[2 * i + 1]));
            }
            shapes.push_back(temp);
            vLine.clear();
            isFinished = false;
        }
        else {
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < numOfVertices; i++)
                glVertex2f(vLine[2*i], vLine[2 * i + 1]);
            glVertex2fv(mousePosition.pos);
            glEnd();
        }
    }

    else if (currentShape == ShapeType::Quad) {
        if (numOfVertices == 0 && isFinished) {
            glBegin(GL_QUADS);
            glVertex2f(vRec[0], vRec[1]);
            glVertex2f(vRec[0], mousePosition.pos[1]);
            glVertex2f(mousePosition.pos[0], mousePosition.pos[1]);
            glVertex2f(mousePosition.pos[0], vRec[1]);
            glEnd();
            isFinished = false;
            //store rec into shapes
            temp = Quad();
            temp.pointSize = currentPointSize;
            for (int i = 0; i < *(&color + 1) - color; i++) temp.color[i] = color[i];
            for (int i = 0; i < (*(&vRec + 1) - vRec)/2; i++) {
                for (int j = 0; j < 2; j++) {
                    temp.vertices[i].pos[j] = vRec[2 * i + j];
                }
            }
            shapes.push_back(temp);
        }
        else if(numOfVertices>0) {
            glBegin(GL_LINE_LOOP);
            glVertex2f(vRec[0], vRec[1]);
            glVertex2f(vRec[0], mousePosition.pos[1]);
            glVertex2f(mousePosition.pos[0], mousePosition.pos[1]);
            glVertex2f(mousePosition.pos[0], vRec[1]);
            glEnd();
            
            
        }
    }

    else if (currentShape == ShapeType::Polygon) {
        if (numOfVertices == 0 && isFinished) {
            glBegin(GL_TRIANGLE_FAN);
            for (int i = 0; i < vPoly.size() / 2; i++)
                glVertex2f(vPoly[2 * i], vPoly[2 * i + 1]);
            glEnd();
            // store polygon into shapes
            temp = Polygon::Polygon();
            temp.pointSize = currentPointSize;
            for (int i = 0; i < *(&color + 1) - color; i++) temp.color[i] = color[i];
            for (int i = 0; i < vPoly.size() / 2; i++) {
                temp.vertices.push_back(Position(vPoly[2 * i], vPoly[2 * i + 1]));
            }
            shapes.push_back(temp);

            vPoly.clear();
            isFinished = false;
        }
        else if (numOfVertices == 1) {
            glBegin(GL_LINES);
            glVertex2f(vPoly[0], vPoly[1]);
            glVertex2fv(mousePosition.pos);
            glEnd();
        }
        else {
            glBegin(GL_TRIANGLE_FAN);
            for (int i = 0; i < numOfVertices; i++)
                glVertex2f(vPoly[2 * i], vPoly[2 * i + 1]);
            glVertex2fv(mousePosition.pos);
            glEnd();
        }
    }
    drawCursor();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    rasterSize[0] = w;
    rasterSize[1] = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, canvasSize[0], 0.0, canvasSize[1]);
    glViewport(0, 0, rasterSize[0], rasterSize[1]);

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    switch (currentShape) {
    case ShapeType::Triangle:
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (numOfVertices >= 3)
                numOfVertices = 0;

            mousePosition.pos[0] = (float)x / rasterSize[0] * canvasSize[0];
            mousePosition.pos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];
            vTri[numOfVertices * 2 + 0] = mousePosition.pos[0];
            vTri[numOfVertices * 2 + 1] = mousePosition.pos[1];

            numOfVertices++;
            glutPostRedisplay();
        }
        break;
    case ShapeType::Point:
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            numOfVertices = 0;
            mousePosition.pos[0] = (float)x / rasterSize[0] * canvasSize[0];
            mousePosition.pos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];
            vPoint[0] = mousePosition.pos[0];
            vPoint[1] = mousePosition.pos[1];
            isFinished = true;
            glutPostRedisplay();
        }
        break;
    case ShapeType::Line:
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            mousePosition.pos[0] = (float)x / rasterSize[0] * canvasSize[0];
            mousePosition.pos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];
            vLine.push_back(mousePosition.pos[0]);
            vLine.push_back(mousePosition.pos[1]);

            numOfVertices++;
            glutPostRedisplay();
        }
        break;
    case ShapeType::Quad:
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            mousePosition.pos[0] = (float)x / rasterSize[0] * canvasSize[0];
            mousePosition.pos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];
            vRec[numOfVertices * 2 + 0] = mousePosition.pos[0];
            vRec[numOfVertices * 2 + 1] = mousePosition.pos[1];

            numOfVertices++;
            if (numOfVertices >= 2) {
                numOfVertices = 0;
                isFinished = true;
            }
                
            glutPostRedisplay();
        }
        break;
    case ShapeType::Polygon:
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            mousePosition.pos[0] = (float)x / rasterSize[0] * canvasSize[0];
            mousePosition.pos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];
            vPoly.push_back(mousePosition.pos[0]);
            vPoly.push_back(mousePosition.pos[1]);

            numOfVertices++;
            glutPostRedisplay();
        }
        break;
    default:
        break;
    }
}

void motion(int x, int y)
{
    // mouse events are handled by OS, eventually. When using mouse in the raster window, it assumes top-left is the origin.
    // Note: the raster window created by GLUT assumes bottom-left is the origin.
    mousePosition.pos[0] = (float)x / rasterSize[0] * canvasSize[0];
    mousePosition.pos[1] = (float)(rasterSize[1] - y) / rasterSize[1] * canvasSize[1];

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        exit(0);
        break;
    case '\r':
        if (currentShape == ShapeType::Line || currentShape == ShapeType::Polygon) {
            numOfVertices = 0;
            isFinished = true;
            glutPostRedisplay();
        }
        break;
    }
}

void menu(int value)
{
    switch (value) {
    case 0: // clear
        numOfVertices = 0;
        shapes.clear();
        glutPostRedisplay();
        break;
    case 1: //exit
        exit(0);
    case 2: // red
        color[0] = 1.0f;
        color[1] = 0.0f;
        color[2] = 0.0f;
        glutPostRedisplay();
        break;
    case 3: // green
        color[0] = 0.0f;
        color[1] = 1.0f;
        color[2] = 0.0f;
        glutPostRedisplay();
        break;
    case 4: // blue
        color[0] = 0.0f;
        color[1] = 0.0f;
        color[2] = 1.0f;
        glutPostRedisplay();
        break;
    case 5:
        if (currentShape != ShapeType::Point) {
            currentShape = ShapeType::Point;
            numOfVertices = 0;
            clearDynamicVertices();
            glutPostRedisplay();
        }
        break;
    case 6:
        if (currentShape != ShapeType::Line) {
            currentShape = ShapeType::Line;
            numOfVertices = 0;
            clearDynamicVertices();
            glutPostRedisplay();
        }
        break;
    case 7:
        if (currentShape != ShapeType::Triangle) {
            currentShape = ShapeType::Triangle;
            numOfVertices = 0;
            clearDynamicVertices();
            glutPostRedisplay();
        }
        break;
    case 8:
        if (currentShape != ShapeType::Quad) {
            currentShape = ShapeType::Quad;
            numOfVertices = 0;
            clearDynamicVertices();
            glutPostRedisplay();
        }
        break;
    case 9:
        if (currentShape != ShapeType::Polygon) {
            currentShape = ShapeType::Polygon;
            numOfVertices = 0;
            clearDynamicVertices();
            glutPostRedisplay();
        }
        break;
    case 10:
        currentPointSize = 1.0f;
        glutPostRedisplay();
        break;
    case 11:
        currentPointSize = 5.0f;
        glutPostRedisplay();
        break;
    case 12:
        currentPointSize = 10.0f;
        glutPostRedisplay();
        break;
    default:
        break;
    }
}
void createMenu()
{
    int colorMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Red", 2);
    glutAddMenuEntry("Green", 3);
    glutAddMenuEntry("Blue", 4);

    int shapeMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Point", 5);
    glutAddMenuEntry("Line", 6);
    glutAddMenuEntry("Triangle", 7);
    glutAddMenuEntry("Quad", 8);
    glutAddMenuEntry("Polygon", 9);

    int sizeMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Small", 10);
    glutAddMenuEntry("Medium", 11);
    glutAddMenuEntry("Large", 12);

    glutCreateMenu(menu);
    glutAddMenuEntry("Clear", 0);
    glutAddSubMenu("Colors", colorMenu);
    glutAddSubMenu("Shapes", shapeMenu);
    glutAddSubMenu("Size", sizeMenu);
    glutAddMenuEntry("Exit", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char* argv[])
{
    init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(rasterSize[0], rasterSize[1]);
    glutCreateWindow("2d drawing");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(motion);
    createMenu();
    glutMainLoop();
    return 0;


}