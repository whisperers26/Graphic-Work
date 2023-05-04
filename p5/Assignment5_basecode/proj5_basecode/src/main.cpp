#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>

#include "Camera.h"

#include "Sphere.h"
#include "Box.h"
#include "Light.h"

#include "Text.h"

#include <fstream>
#include <iostream>
using namespace std;
using namespace glm;

int g_winWidth  = 640;
int g_winHeight = 480;

Camera g_cam;
Text g_text;
unsigned char g_keyStates[256];

GLfloat light0_pos[] = {0.0f, 5.0f, 5.0f, 0.0f};
GLfloat light0_Amb[] = {0.4f, 0.3f, 0.3f, 1.0f};
GLfloat light0_Diff[] = {0.8f, 0.8f, 0.7f, 1.0f};
GLfloat light0_Spec[] = {0.9f, 0.9f, 0.9f, 1.0f};

const char dataFile[128] = "geoData/geo.txt";

unsigned int g_box_num;
Box* g_boxes;

unsigned int g_sphere_num;
Sphere * g_spheres;

Light g_light;

void LoadConfigFile(const char* pFilePath)
{
    const unsigned int CAMERA= 0;
    const unsigned int LIGHT= 1;
    const unsigned int SPHERE= 2;
    const unsigned int BOX = 3;
    
    vec3 rot;
    fstream filestr;
    filestr.open (pFilePath, fstream::in);
    if(!filestr.is_open()){
        cout<<"'"<<pFilePath<<"'"<<", geo file does not exsit!"<<endl;
        filestr.close();
        return;
    }
    
    char objType[80];
    char attrType[80];
    unsigned int objState = -1;
    
    bool loop = true;
    while(loop){
        filestr>>objType;
        if(!strcmp(objType, "Camera:")) objState=CAMERA;
        else if(!strcmp(objType, "Light:")) objState = LIGHT;
        else if(!strcmp(objType, "Sphere:")) objState = SPHERE;
        else if(!strcmp(objType, "Box:")) objState = BOX;
        switch(objState){
            case CAMERA:
                float e_x, e_y, e_z;
                float l_x, l_y, l_z;
                float near_plane;
                
                for(int i=0; i<3; i++){
                    filestr>>attrType;
                    if(!strcmp(attrType, "eye:")){
                        filestr>>e_x;
                        filestr>>e_y;
                        filestr>>e_z;
                    }
                    else if(!strcmp(attrType, "lookat:")){
                        filestr>>l_x;
                        filestr>>l_y;
                        filestr>>l_z;
                    }
                    else if(!strcmp(attrType, "near_plane:")) filestr>>near_plane;
                }
                g_cam.set(e_x, e_y, e_z, l_x, l_y, l_z, g_winWidth, g_winHeight, 45.0f, near_plane, 1000.0f);
                break;
            case LIGHT:
                filestr>>attrType;
                if(!strcmp(attrType, "position:")){
                    filestr>>g_light.pos.x;
                    filestr>>g_light.pos.y;
                    filestr>>g_light.pos.z;
                }
                filestr>>attrType;
                if(!strcmp(attrType, "color:")){
                    filestr>>g_light.color.x;
                    filestr>>g_light.color.y;
                    filestr>>g_light.color.z;
                }
                filestr>>attrType;
                if(!strcmp(attrType, "intensity:")){
                    filestr>>g_light.intensity;
                }
                break;
            case SPHERE:
                filestr>>attrType;
                if(!strcmp(attrType, "num:"))
                    filestr>>g_sphere_num;
                if(g_sphere_num > 0) {
                    g_spheres = new Sphere[g_sphere_num];
                    
                    for(int i=0; i<g_sphere_num; i++){
                    filestr>>attrType;
                    if(!strcmp(attrType, "position:")){
                        filestr>>g_spheres[i].pos.x; filestr>>g_spheres[i].pos.y;  filestr>>g_spheres[i].pos.z;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "radius:")){
                        filestr>>g_spheres[i].radius;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "color:")){
                        filestr>>g_spheres[i].color.x; filestr>>g_spheres[i].color.y;  filestr>>g_spheres[i].color.z;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "ambient:"))	filestr>>g_spheres[i].ambient;
                    filestr>>attrType;
                    if(!strcmp(attrType, "diffuse:"))	filestr>>g_spheres[i].diffuse;
                    filestr>>attrType;
                    if(!strcmp(attrType, "phong:")) 	filestr>>g_spheres[i].phong;
                    
                }
                }
                break;
            case BOX:
                filestr>>attrType;
                if(!strcmp(attrType, "num:"))
                    filestr>>g_box_num;
                
                if(g_box_num > 0) {
                    g_boxes = new Box[g_box_num];
                    for(int i=0; i<g_box_num; i++){
                        filestr>>attrType;
                        if(!strcmp(attrType, "conner_position:")){
                            filestr>>g_boxes[i].minPos.x;
                            filestr>>g_boxes[i].minPos.y;
                            filestr>>g_boxes[i].minPos.z;
                            filestr>>g_boxes[i].maxPos.x;
                            filestr>>g_boxes[i].maxPos.y;
                            filestr>>g_boxes[i].maxPos.z;
                        }
                        
                        filestr>>attrType;
                        if(!strcmp(attrType, "color:")){
                            filestr>>g_boxes[i].color.x;
                            filestr>>g_boxes[i].color.y;
                            filestr>>g_boxes[i].color.z;
                        }
                        filestr>>attrType;
                        if(!strcmp(attrType, "rotate:")){
                            filestr>>rot.x;
                            filestr>>rot.y;
                            filestr>>rot.z;
							//Convert to radians
							rot.x *= 3.14159265f / 180.0f;
							rot.y *= 3.14159265f / 180.0f;
							rot.z *= 3.14159265f / 180.0f;

                            mat4 m (1.0f);
                            
                            
                            // rotation order is zyx
                            m = rotate(m, rot.z, vec3(0, 0, 1));
                            m = rotate(m, rot.y, vec3(0, 1, 0));
                            m = rotate(m, rot.x, vec3(1, 0, 0));
                            
                            //cout<<glm::to_string(m)<<endl;
                            
                            g_boxes[i].rotMat = m;
                            g_boxes[i].invRotMat = inverse(m);
                        }
                        filestr>>attrType;
                        if(!strcmp(attrType, "ambient:"))	filestr>>g_boxes[i].ambient;
                        filestr>>attrType;
                        if(!strcmp(attrType, "diffuse:"))	filestr>>g_boxes[i].diffuse;
                        filestr>>attrType;
                        if(!strcmp(attrType, "phong:"))	filestr>>g_boxes[i].phong;
                    }
                    loop = false;
                }
                break;
        }
    }
}

void initialization() 
{    
    //g_cam.set(3.0f, 4.0f, 3.0f, 0.0f, 0.0f, 0.0f, g_winWidth, g_winHeight);

    LoadConfigFile(dataFile);
    
	g_text.setColor(0.0f, 0.0f, 0.0f);
}

/****** GL callbacks ******/
void initialGL()
{    
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_Amb);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_Diff);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_Spec);


	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_FILL);

	
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
    // adding stuff to update at runtime ....
    
    g_cam.keyOperation(g_keyStates, g_winWidth, g_winHeight);
}

void display()
{
    //glFinish();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); // commenting out this line to make object always lit up in front of the cam. 

    // adding stuff to draw ...

    
    // drae sphere and box
    for (int i=0; i<g_sphere_num; i++)
        g_spheres[i].Draw();
    for (int i=0; i<g_box_num; i++)
        g_boxes[i].Draw();

    // displaying the camera
    g_cam.drawGrid();
    g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
    g_cam.drawCoordinate();

	// displaying the text
	if(g_cam.isFocusMode()) {
        string str = "Cam mode: Focus";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	} else if(g_cam.isFPMode()) {
        string str = "Cam mode: FP";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	}

	char s[128];
	g_text.draw(10, 50, s, g_winWidth, g_winHeight);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
	g_winWidth = w;
	g_winHeight = h;
	if (h == 0) {
		h = 1;
	}
	g_cam.setProj(g_winWidth, g_winHeight);
    g_cam.setModelView();
    glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
    g_cam.mouseClick(button, state, x, y, g_winWidth, g_winHeight);
	
}

void motion(int x, int y)
{
    g_cam.mouseMotion(x, y, g_winWidth, g_winHeight);
}

void keyup(unsigned char key, int x, int y)
{
    g_keyStates[key] = false;
}

void keyboard(unsigned char key, int x, int y)
{
    g_keyStates[key] = true;
	switch(key) { 
		case 27:
			exit(0);
			break;
        case 'c': // switch cam control mode
            g_cam.switchCamMode();
			glutPostRedisplay();
            break;
        case ' ':
            g_cam.PrintProperty();
            break;

	}
}


int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_winWidth, g_winHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Ray Casting");
	
	glewInit();
	initialGL();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	initialization();
	
    glutMainLoop();
    return EXIT_SUCCESS;
}