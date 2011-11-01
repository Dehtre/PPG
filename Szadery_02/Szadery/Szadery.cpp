// Szadery.cpp : main project file.

#include "stdafx.h"
#include <iostream>

#include <GLTools.h> 
#define FREEGLUT_STATIC
#include <GL/glut.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>

#include <time.h>

#define PI 3.1415926535

using namespace std;

GLuint shader;
GLint MVMatrixLocation; 
GLMatrixStack camera;

void ChangeSize(int w, int h) {
	glViewport(0,0,w,h);
}

GLFrustum viewFrustum;

void SetupRC() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp", 
		2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");
	fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n",
            GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);
	
	MVMatrixLocation = glGetUniformLocation(shader, "MVMatrix");
	if(MVMatrixLocation == -1) {
		fprintf(stdout, "Hej ho, hej ho, uniformy nie dzia³aj¹ \n");
	}


	viewFrustum.SetPerspective(15.0f,(float)800.0/(float)600.0,0,10000);
	camera.LoadMatrix(viewFrustum.GetProjectionMatrix());
}

float cameraAngleX = 0,cameraAngleY = 0,cameraAngleZ = 0;

float randf() {
	return ((float)(rand()%1000)) / 50000;
}

void animate() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	cameraAngleX += randf() * 1.0;
	cameraAngleY += randf() * 1.5;
	cameraAngleZ += randf() * 3.0;

	cout << cameraAngleX << "," << cameraAngleY << "," << cameraAngleZ << endl;
	
	M3DMatrix44f matrix, S, R, tmpR1, tmpR2, tmpR3;
	m3dScaleMatrix44(S, 0.2f, 0.2f, 0.5f);
	
	m3dRotationMatrix44(tmpR1, cameraAngleX, 1.0f, 0.0f, 0.0f);
	m3dRotationMatrix44(tmpR2, cameraAngleY, 0.0f, 1.0f, 0.0f);
	m3dRotationMatrix44(tmpR3, cameraAngleZ, 0.0f, 0.0f, 1.0f);
	m3dMatrixMultiply44(R, tmpR1, tmpR2);
	m3dMatrixMultiply44(R, R, tmpR3);

	m3dMatrixMultiply44(matrix, S, R);
	// m3dMatrixMultiply44(matrix, matrix, camera.GetMatrix());
	

	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,matrix);
	glutPostRedisplay();
}

void RenderScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(shader);

    glBegin(GL_TRIANGLES);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 1.0f, 0.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 1.0, 0.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 0.0, 0.0);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);


	glEnd();

	glutSwapBuffers();
}


int main(int argc, char* argv[])
{
    cout << "Hello le World" << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800,600);
    glutCreateWindow("Triangle");

    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
	glutIdleFunc(animate);

	GLenum err = glewInit();

	SetupRC();

	glutMainLoop();

    return 0;
}
