// This OpenGL project demonstrates a simple interactive camera implementation and a few vertex and fragment shaders. 
// Two modes of camera controls (press c to switch between two modes): 
// 1. Focus Mode: Holding ALT and Left Mouse Button (LMB) and moving the mouse will rotate the camera about the LookAt Position
//                Holding ALT and MMB and moving the mouse will pan the camera.
//                Holding ALT and RMB and moving the mouse will zoom the camera.
// 2. First-Person Mode: Pressing W, A, S, or D will move the camera
//                       Holding LMB and moving the mouse will roate the camera.
// Basic shader - demonstrate the basic use of vertex and fragment shader
// Displacement shader - a special fireball visual effects with Perlin noise function
// Toon shading shader - catoonish rendering effects
// Per-vertex shading v.s. per-fragment shading = visual comparison between two types of shading 

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Camera.h"
#include "Text.h"
#include "Mesh.h"

#include <iostream>
using namespace std;
using namespace glm;

int g_winWidth  = 1024;
int g_winHeight = 512;

Camera g_cam;
Text g_text;

unsigned char g_keyStates[256];

char v_shader_file[] =
".\\shaders\\fragImp.vert";
//".\\shaders\\displacement.vert"; // vertex displacement shader with perlin noise
//".\\shaders\\perVert_lambert.vert"; // basic lambert lighting  
// ".\\shaders\\perFrag_lambert.vert"; // basic lambert lighting with per-fragment implementation
// ".\\shaders\\toon_shading.vert"; // basic toon shading with per-fragment implementation

char f_shader_file[] =
".\\shaders\\fragImp.frag";
// ".\\shaders\\displacement.frag"; // vertex displacement shader with perlin noise
// ".\\shaders\\perVert_lambert.frag"; // basic lambert shading 
// ".\\shaders\\perFrag_lambert.frag"; // basic lambert shading with per-fragment implementation
// ".\\shaders\\toon_shading.frag"; // basic toon shading with per-fragment implementation

char v_shader_file2[] = ".\\shaders\\vertImp.vert";
char f_shader_file2[] = ".\\shaders\\vertImp.frag";

const char meshFile[128] = 
//"Mesh/sphere.obj";
//"Mesh/bunny2K.obj";
//"Mesh/teapot.obj";
"Mesh/teapot.obj";

Mesh g_mesh;
Mesh g_mesh2;


// position inf
vec3 g_lightPos = vec3(0, 0, 0);
vec3 tea1_translate = vec3(0.0f, 2.0f, 0.0f);
vec3 tea2_translate = vec3(3.0f, 2.0f, 0.0f);
vec3 tea1_scale = vec3(0.5f, 0.5f, 0.5f);
vec3 tea2_scale = vec3(0.5f, 0.5f, 0.5f);
vec3 light1_pos = vec3(3.0f, 3.0f, 3.0f);
vec3 light2_pos = vec3(1.0f, 0.0f, -2.0f);
uint selectedLight = 1;


float g_time = 0.0f;

// draw point lights
void drawLights() 
{
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	if (selectedLight == 1) {
		// draw light points
		
		glPushMatrix();
		glLoadMatrixf(value_ptr(g_cam.viewMat));
		glColor3f(1.0f, 0.0f, 0.0f);
		glTranslatef(light1_pos.x, light1_pos.y, light1_pos.z);
		glutSolidSphere(0.3f, 10, 10);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glLoadMatrixf(value_ptr(g_cam.viewMat));
		glColor3f(1.0f, 0.0f, 0.0f);
		glTranslatef(light2_pos.x, light2_pos.y, light2_pos.z);
		glutWireSphere(0.3f, 10, 10);
		glEnd();
		glPopMatrix();
	}
	else {
		// draw light points
		glPushMatrix();
		glLoadMatrixf(value_ptr(g_cam.viewMat));
		glColor3f(1.0f, 0.0f, 0.0f);
		glTranslatef(light1_pos.x, light1_pos.y, light1_pos.z);
		glutWireSphere(0.3f, 10, 10);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glLoadMatrixf(value_ptr(g_cam.viewMat));
		glColor3f(1.0f, 0.0f, 0.0f);
		glTranslatef(light2_pos.x, light2_pos.y, light2_pos.z);
		glutSolidSphere(0.3f, 10, 10);
		glEnd();
		glPopMatrix();
	}
}


void initialization() 
{    
    g_cam.set(3.0f, 4.0f, 14.0f, 0.0f, 1.0f, -0.5f, g_winWidth, g_winHeight);
	g_text.setColor(0.0f, 0.0f, 0.0f);

	g_mesh.create(meshFile, v_shader_file, f_shader_file);
	g_mesh2.create(meshFile, v_shader_file2, f_shader_file2);
	// add any stuff you want to initialize ...
}

/****** GL callbacks ******/
void initialGL()
{    
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
    // add any stuff to update at runtime ....

    g_cam.keyOperation(g_keyStates, g_winWidth, g_winHeight);

	glutPostRedisplay();
}

void display()
{	 
	glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// add any stuff you'd like to draw	

	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	g_cam.drawGrid();
    g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
    g_cam.drawCoordinate();

	
	// draw light points
	drawLights();

	// draw mesh
	g_time = (float)glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	g_mesh.draw(g_cam.viewMat, g_cam.projMat, light1_pos, light2_pos, g_time, tea1_translate, tea1_scale, g_cam.eye.xyz());
	g_mesh2.draw(g_cam.viewMat, g_cam.projMat, light1_pos, light2_pos, g_time, tea2_translate, tea2_scale, g_cam.eye.xyz());

    glutSwapBuffers();
}

void reshape(int w, int h)
{
	g_winWidth = w;
	g_winHeight = h;
	if (h == 0) {
		h = 1;
	}
	g_cam.setProjectionMatrix(g_winWidth, g_winHeight);
    g_cam.setViewMatrix();
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
   //     case 'c': // switch cam control mode
   //         g_cam.switchCamMode();
			//glutPostRedisplay();
   //         break;
        case ' ':
            g_cam.PrintProperty();
            break;
		case '1':
			selectedLight = 1;
			break;
		case '2':
			selectedLight = 2;
			break;
		case 's':
			if (selectedLight == 1) light1_pos.z += 0.1;
			else light2_pos.z += 0.1;
			break;
		case 'w':
			if (selectedLight == 1) light1_pos.z -= 0.1;
			else light2_pos.z -= 0.1;
			break;
		case 'd':
			if (selectedLight == 1) light1_pos.x += 0.1;
			else light2_pos.x += 0.1;
			break;
		case 'a':
			if (selectedLight == 1) light1_pos.x -= 0.1;
			else light2_pos.x -= 0.1;
			break;
		case 'u':
			if (selectedLight == 1) light1_pos.y += 0.1;
			else light2_pos.y += 0.1;
			break;
		case 'j':
			if (selectedLight == 1) light1_pos.y -= 0.1;
			else light2_pos.y -= 0.1;
			break;

		/*case '+':
			g_mesh.normal_offset += 0.01;
			break;
		case'-':
			g_mesh.normal_offset -= 0.01;*/
	}
}

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_winWidth, g_winHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("VertFrag Shader Example");
	
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