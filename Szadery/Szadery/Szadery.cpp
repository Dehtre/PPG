// Szadery.cpp : main project file.

#include "stdafx.h"
#include <iostream>

#include <GLTools.h> 
#define FREEGLUT_STATIC
#include <GL/glut.h>

#include <time.h>
#include <array.h>

using namespace std;

GLuint shader;

void ChangeSize(int w, int h) {
	glViewport(0,0,w,h);
}

void SetupRC() {
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp", 
		2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");
	fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n",
            GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);
}

float cameraAngle = 0;

void animate() {
	cameraAngle += 0.5;
	cout << cameraAngle << endl;

	glutPostRedisplay();
}

std::array<GLfloat, 16> projection;

void RenderScene() {

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -5.0);
	glRotatef(cameraAngle, 0.0, 1.0, 0.0);
	glGetFloatv(GL_PROJECTION_MATRIX, projection.data());
	glPopMatrix();

	glUniformMatrix4fv(glGetUniformLocation(shader, "gl_ModelViewProjectionMatrix"), 1, GL_FALSE, projection.data());


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(shader);

    glBegin(GL_TRIANGLES);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 1.0, 0.0);
    glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
    glVertex3f(1.0f, 1.0f, 1.0f);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 1.0, 0.0);
    glVertex3f(1.0f, 1.0f, 1.0f);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
    glVertex3f(1.0f, -1.0f, 1.0f);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 1.0, 0.0);
    glVertex3f(1.0f, -1.0f, 1.0f);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 1.0f);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 1.0, 0.0);
    glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
    glVertex3f(-1.0f, 1.0f, 1.0f);

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
