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
GLFrustum viewFrustum;
GLMatrixStack matrixStack;
float cameraAngleX = 0,cameraAngleY = 0,cameraAngleZ = 0,cameraBothZ = 0;

void ChangeSize(int w, int h) {
	glViewport(0,0,w,h);
}

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

	viewFrustum.SetPerspective(15.0f,(float)800.0/(float)600.0,0,1000);
}

float randf() {
	return ((float)(rand()%1000)) / 50000;
}

void pushSiatka() {
	matrixStack.PushMatrix();
	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,matrixStack.GetMatrix());

	glBegin(GL_LINES);
	glVertexAttrib4f(GLT_ATTRIBUTE_COLOR, 1.0, 1.0, 1.0, 1.0);
	for(int i = -10; i<=10; i++) {
		glVertex3f(i,-10,0);
		glVertex3f(i,10,0);
		glVertex3f(-10,i,0);
		glVertex3f(10,i,0);
	}
	glEnd();
}

void RenderPiramidka(float xPos, float yPos, float zPos) {
	//krêci wszystkim doko³a
	//matrixStack.Rotate(cameraBothZ, 0, 0, 1);

	matrixStack.Translate(xPos, yPos, zPos);

	//dla ³adnego startu
	matrixStack.Rotate(180, 1, 0, 0);
	
	if (xPos < 0)
		matrixStack.Rotate(-cameraAngleX, 1, 0, 0);
	else
		matrixStack.Rotate(cameraAngleX, 1, 0, 0);

	if (yPos < 0)
		matrixStack.Rotate(-cameraAngleY, 0, 1, 0);
	else
		matrixStack.Rotate(cameraAngleY, 0, 1, 0);

	matrixStack.Rotate(cameraAngleZ, 0, 0, 1);


	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,matrixStack.GetMatrix());

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

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.2, 0.2, 0.2);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);

	glEnd();

	matrixStack.PopMatrix();
}


void animate() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(shader);

	cameraAngleX += randf() * 50.0;
	cameraAngleY += randf() * 27.0;
	cameraAngleZ += randf() * 500.0;
	cameraBothZ += randf() * 10.0;

	//cout << cameraAngleX << "," << cameraAngleY << "," << cameraAngleZ << endl;
	
	matrixStack.LoadIdentity();
	matrixStack.PushMatrix();
	matrixStack.Scale(0.1,0.1,0.1);
	//matrixStack.MultMatrix(viewFrustum.GetProjectionMatrix());
		
	// dodac kiedys kamere
	
	//siatka
	pushSiatka();
	
	//piramidka 1
	RenderPiramidka(5,5,-2);

	//piramidka 2
	RenderPiramidka(-5,-5,-2);

	matrixStack.PopMatrix();

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
    glutDisplayFunc(animate);
	glutIdleFunc(animate);

	GLenum err = glewInit();

	SetupRC();

	glutMainLoop();

    return 0;
}
