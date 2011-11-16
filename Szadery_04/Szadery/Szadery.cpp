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
GLint MVMatrixLocation, MVPMatrixLocation, NormalMatrixLocation; 
GLFrustum viewFrustum;
GLMatrixStack matrixStack;
float cameraAngleX = 0,cameraAngleY = 0,cameraAngleZ = 0,cameraBothZ = 0;

struct LightUniform {
	GLint positionLocation;
	GLint colorLocation;
	GLint angleLocation;
	GLint attenuation0Location;
	GLint attenuation1Location;
	GLint attenuation2Location;

	void setLight(M3DVector3f position, M3DVector3f color, GLfloat angle, GLfloat attenuation0, GLfloat attenuation1, GLfloat attenuation2) {
		glUniform3fv(positionLocation, 1, position);
		glUniform3fv(colorLocation, 1, color);
		glUniform1f(angleLocation, angle);
		glUniform1f(attenuation0Location, attenuation0);
		glUniform1f(attenuation1Location, attenuation1);
		glUniform1f(attenuation2Location, attenuation2);
	}
};

struct MaterialUniform {
    GLint  ambientColorLocation;
    GLint  diffuseColorLocation;
    GLint  specularColorLocation;
    GLint specularExponentLocation;	

	void setMaterial(M3DVector3f ambientColor,M3DVector3f diffuseColor, M3DVector3f specularColor, GLfloat specularExponent) {
		glUniform3fv(ambientColorLocation, 1, ambientColor);
		glUniform3fv(diffuseColorLocation, 1, diffuseColor);
		glUniform3fv(specularColorLocation, 1, specularColor);
		glUniform1f(specularExponentLocation, specularExponent);
	}
};

LightUniform light;
MaterialUniform material;

void ChangeSize(int w, int h) {
	glViewport(0,0,w,h);
}

void SetupRC() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp", 
		2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
	fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_NORMAL : %d \n",
            GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_NORMAL);
	
	MVMatrixLocation = glGetUniformLocation(shader, "MVMatrix");
	if(MVMatrixLocation == -1) {
		fprintf(stdout, "Hej ho, hej ho, uniformy nie dzia³aj¹ MVMatrix\n");
	}

	MVPMatrixLocation = glGetUniformLocation(shader, "MVPMatrix");
	if(MVPMatrixLocation == -1) {
		fprintf(stdout, "Hej ho, hej ho, uniformy nie dzia³aj¹ MVPMatrix\n");
	}

	NormalMatrixLocation = glGetUniformLocation(shader, "normalMatrix");
	if(NormalMatrixLocation == -1) {
		fprintf(stdout, "Hej ho, hej ho, uniformy nie dzia³aj¹ NormalMatrix\n");
	}

	light.positionLocation = glGetUniformLocation(shader, "light1.position");
	light.colorLocation = glGetUniformLocation(shader, "light1.color");
	light.angleLocation = glGetUniformLocation(shader, "light1.angle");
	light.attenuation0Location = glGetUniformLocation(shader, "light1.attenuation0");
	light.attenuation1Location = glGetUniformLocation(shader, "light1.attenuation1");
	light.attenuation2Location = glGetUniformLocation(shader, "light1.attenuation2");

	float color[] = {1,0,0}, position[] = {0,0,10};
	light.setLight(position, color, 180, 1, 1, 2);

	material.ambientColorLocation = glGetUniformLocation(shader, "material.ambientColor");
	material.diffuseColorLocation = glGetUniformLocation(shader, "material.diffuseColor");
	material.specularColorLocation = glGetUniformLocation(shader, "material.specularColor");
	material.specularExponentLocation = glGetUniformLocation(shader, "material.specularExponent");

	float ambient[] = {1,0,0}, diffuse[] = {0,0,1}, specular[] = {0,1,0};
	material.setMaterial(ambient, diffuse, specular, 1);

	viewFrustum.SetPerspective(15.0f,(float)800.0/(float)600.0,0,1000);
}

float randf(float scale = 0.05f) {
	return scale * ((float)(rand()%100000)) / 100000;
}

void TriangleFace(M3DVector3f a, M3DVector3f b, M3DVector3f c) {
   M3DVector3f normal, bMa, cMa;
   m3dSubtractVectors3(bMa, b, a);
   m3dSubtractVectors3(cMa, c, a);
   m3dCrossProduct3(normal, bMa, cMa);
   m3dNormalizeVector3(normal);
   glVertexAttrib3fv(GLT_ATTRIBUTE_NORMAL, normal);
   glVertex3fv(a);
   glVertex3fv(b);
   glVertex3fv(c);
}


void pushSiatka() {
	matrixStack.PushMatrix();
	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,matrixStack.GetMatrix());

	glBegin(GL_TRIANGLES);
	glVertexAttrib4f(GLT_ATTRIBUTE_COLOR, 0.2, 0.2, 0.2, 1.0);
	for(int i = -10; i<=10; i++) {
		float v1[] = {i,-10,0};
		float v2[] = {i,10,0};
		float v3[] = {-10,i,0};
		float v4[] = {10,i,0};
		TriangleFace(v1,v2,v3);
		TriangleFace(v2,v3,v4);
	}
	glEnd();
}

float ico_vertices[3 * 12] = {
      0., 0., -0.9510565162951536,
      0., 0., 0.9510565162951536,
      -0.85065080835204, 0., -0.42532540417601994,
      0.85065080835204, 0., 0.42532540417601994,
      0.6881909602355868, -0.5, -0.42532540417601994,
      0.6881909602355868, 0.5, -0.42532540417601994,
      -0.6881909602355868, -0.5, 0.42532540417601994,
      -0.6881909602355868, 0.5, 0.42532540417601994,
      -0.2628655560595668, -0.8090169943749475, -0.42532540417601994,
      -0.2628655560595668, 0.8090169943749475, -0.42532540417601994,
      0.2628655560595668, -0.8090169943749475, 0.42532540417601994,
      0.2628655560595668, 0.8090169943749475, 0.42532540417601994
      };
int ico_faces[3*20]={
      1 ,			 11 ,			 7 ,
      1 ,			 7 ,			 6 ,
      1 ,			 6 ,			 10 ,
      1 ,			 10 ,			 3 ,
      1 ,			 3 ,			 11 ,
      4 ,			 8 ,			 0 ,
      5 ,			 4 ,			 0 ,
      9 ,			 5 ,			 0 ,
      2 ,			 9 ,			 0 ,
      8 ,			 2 ,			 0 ,
      11 ,			 9 ,			 7 ,
      7 ,			 2 ,			 6 ,
      6 ,			 8 ,			 10 ,
      10 ,			 4 ,			 3 ,
      3 ,			 5 ,			 11 ,
      4 ,			 10 ,			 8 ,
      5 ,			 3 ,			 4 ,
      9 ,			 11 ,			 5 ,
      2 ,			 7 ,			 9 ,
      8 ,			 6 ,			 2 };

void drawTriangles(int n_faces, float *vertices, int *faces) {
      for (int i = 0; i < n_faces; i++) {
      glBegin(GL_TRIANGLES);
      TriangleFace(vertices + 3 * faces[3 * i], vertices + 3 * faces[3 * i + 1], vertices + 3 * faces[3 * i + 2]);
      glEnd();
      }
      }
 
void drawSmoothTriangles(int n_faces, float *vertices, int *faces) {
      M3DVector3f normal;
      for (int i = 0; i < n_faces; i++) {
      glBegin(GL_TRIANGLES);
      for(int j=0;j<3;++j) {
      m3dCopyVector3(normal,vertices+3*faces[i*3+j]);
      m3dNormalizeVector3(normal);
      glVertexAttrib3fv(GLT_ATTRIBUTE_NORMAL, normal);
      glVertex3fv(vertices+3*faces[i*3+j]);
      
      }
      glEnd();
      }
      }

void RenderDwudziestoscian(float xPos, float yPos, float zPos) {
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

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 0.0, 0.0);

	drawTriangles(20, ico_vertices, ico_faces);

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
	//Perspektywa + krêcenie kamery
	matrixStack.Rotate(40, 1, 0, 0);
	matrixStack.Rotate(cameraBothZ, 0, 0, 1);
	//Nie dzia³a!
	//matrixStack.MultMatrix(viewFrustum.GetProjectionMatrix());
		
	// dodac kiedys kamere
	
	//siatka
	pushSiatka();
	
	//piramidka 1
	RenderDwudziestoscian(5,5,-2);

	//piramidka 2
	RenderDwudziestoscian(-5,-5,-2);

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
