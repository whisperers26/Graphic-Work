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
#include "ParticleSystem.h"

#include <iostream>
using namespace std;
using namespace glm;

int g_winWidth = 1024;
int g_winHeight = 512;

Camera g_cam;
Text g_text;

unsigned char g_keyStates[256];

vec3 spherePos = vec3(0.0, 5.0, -8.0);
vec3 sphereColor = vec3(0.0, 1.0, 0.0);
vec3 simulateCamEye = vec3(0.0f, 5.0f, 20.0f);
float radius = 2.0f;


unsigned int curTime = 0; //the milliseconds since the start
unsigned int preTime = 0;
unsigned int parWidth = 64;
unsigned int parHeight = 32;
ParticleSystem parSys;

char v_shader_file[] = ".\\shaders\\v_shader.vert";
char f_shader_file[] = ".\\shaders\\f_shader.frag";
char c_shader_file[] = ".\\shaders\\c_shader.comp";

void initialization()
{
	parSys.create(parWidth, parHeight, vec3(-10.0f, 0.0f, -5.0f), vec3(10.0f, 10.0f, -5.0f),
		c_shader_file, v_shader_file, f_shader_file);

	g_cam.set(15.0f, 40.0f, 15.0f, 0.0f, 0.0f, 0.0f, g_winWidth, g_winHeight, 45.0f, 0.01f, 10000.0f);
	g_text.setColor(0.0f, 0.0f, 0.0f);

	// add any stuff you want to initialize ...
}

/****** GL callbacks ******/
void initialGL()
{
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
	// add any stuff to update at runtime ....
	curTime = glutGet(GLUT_ELAPSED_TIME);
	parSys.update(simulateCamEye, spherePos, radius);

	g_cam.keyOperation(g_keyStates, g_winWidth, g_winHeight);

	glutPostRedisplay();

	preTime = curTime;
}

void drawSimulateCamera() {
	// draw simulate camera
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(value_ptr(g_cam.viewMat));
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(simulateCamEye.x, simulateCamEye.y, simulateCamEye.z);
	glutSolidSphere(0.3f, 10, 10);
	glEnd();
	glPopMatrix();

	glBegin(GL_LINES);
	glVertex3f(0.0f, 5.0f, 20.0f);
	glVertex3f(-10.0f, 0.0f, -5.0f);
	glVertex3f(0.0f, 5.0f, 20.0f);
	glVertex3f(10.0f, 10.0f, -5.0f);
	glVertex3f(0.0f, 5.0f, 20.0f);
	glVertex3f(10.0f, 0.0f, -5.0f);
	glVertex3f(0.0f, 5.0f, 20.0f);
	glVertex3f(-10.0f, 10.0f, -5.0f);
	glEnd();
}

void drawSphere() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(value_ptr(g_cam.viewMat));
	glColor3f(sphereColor.r, sphereColor.g, sphereColor.b);
	glTranslatef(spherePos.x, spherePos.y, spherePos.z);
	glutWireSphere(radius, 20, 20);
	glEnd();
	glPopMatrix();
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	parSys.draw(1.0f, g_cam.viewMat, g_cam.projMat);

	g_cam.drawGrid();
	g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
	g_cam.drawCoordinate();

	// display the text
	if (g_cam.isFocusMode()) {
		string str = "Cam mode: Focus";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
		str = "Resolution: " + std::to_string(parWidth) + " * " + std::to_string(parHeight);
		g_text.draw(10, 60, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);

	}
	else if (g_cam.isFPMode()) {
		string str = "Cam mode: FP";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	}

	drawSimulateCamera();
	drawSphere();



	glPopMatrix();
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
	switch (key) {
	case 27:
		exit(0);
		break;
	//case 'c': // switch cam control mode
	//	g_cam.switchCamMode();
	//	glutPostRedisplay();
	//	break;
	case ' ':
		g_cam.PrintProperty();
		break;
	case '+':
		if (parHeight < 2049) {
			parWidth *= 2;
			parHeight *= 2;
			parSys.create(parWidth, parHeight, vec3(-10.0f, 0.0f, -5.0f), vec3(10.0f, 10.0f, -5.0f),
				c_shader_file, v_shader_file, f_shader_file);
			display();
		}
		break;
	case '-':
		if (parHeight > 15) {
			parWidth /= 2;
			parHeight /= 2;
			parSys.create(parWidth, parHeight, vec3(-10.0f, 0.0f, -5.0f), vec3(10.0f, 10.0f, -5.0f),
				c_shader_file, v_shader_file, f_shader_file);
			display();
		}
		break;
	case 'a':
		spherePos.x -= 0.1f;
		display();
		break;
	case 'd':
		spherePos.x += 0.1f;
		display();
		break;
	case 's':
		spherePos.z -= 0.1f;
		display();
		break;
	case 'w':
		spherePos.z += 0.1f;
		display();
		break;
	case 'j':
		spherePos.y -= 0.1f;
		display();
		break;
	case 'u':
		spherePos.y += 0.1f;
		display();
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(g_winWidth, g_winHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Compute Shader Example: A Simple particle System");

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