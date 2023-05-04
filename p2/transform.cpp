#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <iostream>
#include <math.h>
#include "SimpleTreeNode.hpp"

using namespace std;

// raster window size
int win_width = 800, win_height = 800;

// tree to store body parts
// each node represents a part of body containing translate/rotate/color inf
struct  bodyTransform
{
    float centerTranslation[3];
    float width, length;
    float color[3];
    float rotation[4];
    int index;

    bool operator== (const bodyTransform& other) {
        return index == other.index;
    }
};

SimpleTreeNode<bodyTransform> bodyTree;
bodyTransform skeletons[16];

// canvas w,h
float canvas_width = 20.0f; float canvas_height = 20.0f;

// record current selected skeleton
int currentSkeleton = 0;

GLint depth;

// initialize 
void init(void) {

    // initialize the data of the robot body
    
    // chest
    skeletons[0] = {
        {0.0f, 3.0f, 0.0f},
        2.0f, 4.0f, 
        {1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        0
    };
    // arms
    skeletons[1] = {
        {-3.0f, 0.5f, 0.0f},
        1.0f, 2.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        1
    };
    skeletons[2] = {
        {-2.0f, 0.0f, 0.0f},
        1.0f, 2.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        2
    };
    skeletons[3] = {
        {3.0f, 0.5f, 0.0f},
        1.0f, 2.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        3
    };
    skeletons[4] = {
        {2.0f, 0.0f, 0.0f},
        1.0f, 2.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        4
    };
    // hands
    skeletons[5] = {
        {-2.0f, 0.0f, 0.0f},
        2.0f, 2.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        5
    };
    skeletons[6] = {
        {2.0f, 0.0f, 0.0f},
        2.0f, 2.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        6
    };
    // neck
    skeletons[7] = {
        {0.0f, 1.5f, 0.0f},
        1.0f, 1.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        7
    };
    // head
    skeletons[8] = {
        {0.0f, 2.0f, 0.0f},
        3.0f, 3.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        8
    };
    // abdomen
    skeletons[9] = {
        {0.0f, -2.5f, 0.0f},
        3.0f, 3.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        9
    };
    // left leg and foot
    skeletons[10] = {
        {-1.0f, -3.0f, 0.0f},
        3.0f, 1.5f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        10
    };
    skeletons[11] = {
        {0.0f, -3.0f, 0.0f},
        3.0f, 1.5f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        11
    };
    skeletons[12] = {
        {-0.5f, -2.25f, 0.0f},
        1.5f, 2.5f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        12
    };
    // right leg and foot
    skeletons[13] = {
        {1.0f, -3.0f, 0.0f},
        3.0f, 1.5f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        13
    };
    skeletons[14] = {
        {0.0f, -3.0f, 0.0f},
        3.0f, 1.5f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        14
    };
    skeletons[15] = {
        {0.5f, -2.25f, 0.0f},
        1.5f, 2.5f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        15
    };

    // add skeletons to the body tree
    SimpleTreeNode<bodyTransform>* temp[10];
    temp[0] = bodyTree.addChild(skeletons[0]);
    //left arm and hand
    temp[0] = (* temp[0]).addChild(skeletons[1]);
    temp[0] = (*temp[0]).addChild(skeletons[2]);
    temp[0] = (*temp[0]).addChild(skeletons[5]);
    //right arm and hand
    temp[0] = &(bodyTree.getChildren()[0]);
    temp[0] = (*temp[0]).addChild(skeletons[3]);
    temp[0] = (*temp[0]).addChild(skeletons[4]);
    temp[0] = (*temp[0]).addChild(skeletons[6]);
    //neck and head
    temp[0] = &(bodyTree.getChildren()[0]);
    temp[0] = (*temp[0]).addChild(skeletons[7]);
    temp[0] = (*temp[0]).addChild(skeletons[8]);
    // abdomen
    temp[0] = &(bodyTree.getChildren()[0]);
    temp[0] = (*temp[0]).addChild(skeletons[9]);
    temp[1] = temp[0];
    // left leg and feet
    temp[1] = (*temp[1]).addChild(skeletons[10]);
    temp[1] = (*temp[1]).addChild(skeletons[11]);
    temp[1] = (*temp[1]).addChild(skeletons[12]);
    //right leg and feet
    temp[1] = temp[0];
    temp[1] = (*temp[1]).addChild(skeletons[13]);
    temp[1] = (*temp[1]).addChild(skeletons[14]);
    temp[1] = (*temp[1]).addChild(skeletons[15]);
}

// draw a rectangle
void drawRectangle(bodyTransform& rec) {
    glColor3fv(rec.color);
    glLineWidth(3.0f);
    glTranslatef(rec.centerTranslation[0], rec.centerTranslation[1], rec.centerTranslation[2]);
    glRotatef(rec.rotation[0], rec.rotation[1], rec.rotation[2], rec.rotation[3]);
    glBegin(GL_LINE_LOOP);
    glVertex2f(rec.length / 2.0f, rec.width / 2.0f);
    glVertex2f(-rec.length / 2.0f, rec.width / 2.0f);
    glVertex2f(-rec.length / 2.0f, -rec.width / 2.0f);
    glVertex2f(rec.length / 2.0f, -rec.width / 2.0f);
    glEnd();
}

// draw all the children bodies of a node
void drawBody(SimpleTreeNode<bodyTransform>& currentBody) {
    vector<SimpleTreeNode<bodyTransform>> childrenBodies = currentBody.getChildren();
    //drawRectangle(currentBody.get());
    if (childrenBodies.size() > 0) {
        for(SimpleTreeNode<bodyTransform> childBody : childrenBodies)
        {
            if (childrenBodies.size() > 1) glPushMatrix();
            drawRectangle(childBody.get());
            drawBody(childBody);
        }
    }
    else {
        glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &depth);
        glPopMatrix();
    }
}

// refresh display
void display(void) {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // loop the body tree until children is empty
    drawBody(bodyTree);
    
    glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &depth);
    //clear stack
    while (depth > 1) {
        glPopMatrix();
        glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &depth);
    } 


    glutSwapBuffers();
}

// reshape window
void reshape(int w, int h) {
    win_width = w;
    win_height = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-canvas_width / 2.0f, canvas_width / 2.0f, -canvas_height / 2.0f, canvas_height / 2.0f);
    glViewport(0, 0, (GLsizei)win_width, (GLsizei)win_height);

    glutPostRedisplay();
}

// keyboard press
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) // 'esc' key
        exit(0);

    bodyTransform* temp = NULL;
    bodyTree.searchSubtreeByIndex(skeletons[currentSkeleton], &temp);
    unsigned char asciiOffset = 49; // see an ascii table
    if (key == 'a') {
        temp->rotation[0] += 10.0f;
    }
    if (key == 'd') {
        temp->rotation[0] -= 10.0f;
    }
    glutPostRedisplay();
}

// use left and right arrow keys to switch selected skeleton
void switchSkeleton(bool bLeft) {
    bodyTransform* temp = NULL;
    for (int i = 0; i < *(&skeletons[currentSkeleton].color + 1) - skeletons[currentSkeleton].color; i++) {
        bodyTree.searchSubtreeByIndex(skeletons[currentSkeleton], &temp);
        temp->color[0] = 0.0f;
    }
    if (bLeft) {
        currentSkeleton--;
        if (currentSkeleton < 0) currentSkeleton = (* (&skeletons + 1) - skeletons) - 1;
    }
    else {
        currentSkeleton++;
        if (currentSkeleton >= *(&skeletons + 1) - skeletons) currentSkeleton = 0;
    }
    bodyTree.searchSubtreeByIndex(skeletons[currentSkeleton], &temp);
    temp->color[0] = 1.0f;
}

// arrow keys
void specialInput(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        switchSkeleton(true);
        break;
    case GLUT_KEY_RIGHT:
        switchSkeleton(false);
        break;
    }
    glutPostRedisplay();
}

// rotation control
void motion(int x, int y) {

}

int main(int argc, char* argv[]) {
    init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("2D transformation Tree");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialInput);
    glutMotionFunc(motion);
    glutMainLoop();

    
    return 0;
}