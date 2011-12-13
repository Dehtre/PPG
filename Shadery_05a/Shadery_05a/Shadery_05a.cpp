// Shadery_05a.cpp : Defines the entry point for the console application.
//

// Szadery.cpp : main project file.

#include "stdafx.h"
#include <iostream>

#include <stdio.h>
#include <vector>

#include <GLTools.h> 
#define FREEGLUT_STATIC
#include <GL/glut.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>

#include <time.h>

#define PI 3.1415926535

using namespace std;

GLuint shader;
GLint MVMatrixLocation, MVPMatrixLocation, NormalMatrixLocation, ambientLightLocation; 
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
LightUniform small_light;
MaterialUniform material;

void ChangeSize(int w, int h) {
	glViewport(0,0,w,h);
}

GLuint vertex_buffer, faces_buffer;

vector<float> ico_vertices;
vector<GLuint> ico_faces;
int n_faces, n_vertices;

void readVertices() {
   FILE *fvertices=fopen("geode_vertices.dat","r");
   if(fvertices==NULL) {
   fprintf(stderr,"cannot open vertices file for reading\n");
   exit(-1);
   }
   char line[120];
   n_vertices = 0;
   while(fgets(line,120,fvertices)!=NULL) {
	   float x,y,z;
	   double norm;
	   sscanf(line,"%f %f %f",&x,&y,&z);
	  
	   norm=x*x+y*y+z*z;
	   norm=sqrt(norm);
	   n_vertices++;
	   ico_vertices.push_back(x);
	   ico_vertices.push_back(y);
	   ico_vertices.push_back(z);
	   ico_vertices.push_back(1.0f);
	   ico_vertices.push_back(x/norm);
	   ico_vertices.push_back(y/norm);
	   ico_vertices.push_back(z/norm);
   }
}


void readFaces() {
	FILE *ffaces=fopen("geode_faces.dat","r");
   if(ffaces==NULL) {
   fprintf(stderr,"cannot open faces file for reading\n");
   exit(-1);
   }
	n_faces = 0;
	char line[120];
   while(fgets(line,120,ffaces)!=NULL) {
	   GLuint  i,j,k;
	   
	   if(3!=sscanf(line,"%u %u %u",&i,&j,&k)){
		   fprintf(stderr,"error reading faces\n"); 
		   exit(-1);
	   }
	   //fprintf(stderr,"%u %u %u\n",i-1,j-1,k-1);
	   n_faces++;
	   ico_faces.push_back(i-1);
	   ico_faces.push_back(j-1);
	   ico_faces.push_back(k-1);
   
   }
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

	ambientLightLocation = glGetUniformLocation(shader, "ambientLight");

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

	small_light.positionLocation = glGetUniformLocation(shader, "light2.position");
	small_light.colorLocation = glGetUniformLocation(shader, "light2.color");
	small_light.angleLocation = glGetUniformLocation(shader, "light2.angle");
	small_light.attenuation0Location = glGetUniformLocation(shader, "light2.attenuation0");
	small_light.attenuation1Location = glGetUniformLocation(shader, "light2.attenuation1");
	small_light.attenuation2Location = glGetUniformLocation(shader, "light2.attenuation2");

	material.ambientColorLocation = glGetUniformLocation(shader, "material.ambientColor");
	material.diffuseColorLocation = glGetUniformLocation(shader, "material.diffuseColor");
	material.specularColorLocation = glGetUniformLocation(shader, "material.specularColor");
	material.specularExponentLocation = glGetUniformLocation(shader, "material.specularExponent");

	viewFrustum.SetPerspective(15.0f,(float)800.0/(float)600.0,0,1000);

	glGenBuffers(1,&vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	readVertices();
	glBufferData(GL_ARRAY_BUFFER,n_vertices*sizeof(float)*7,&ico_vertices[0],GL_STATIC_DRAW);
	glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX,4,GL_FLOAT,GL_FALSE,sizeof(float)*7,(const GLvoid *)0);
	glVertexAttribPointer(GLT_ATTRIBUTE_NORMAL,3,GL_FLOAT,GL_FALSE,sizeof(float)*7,(const GLvoid *)(4*sizeof(float)) );

	glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
	glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);

	glGenBuffers(1,&faces_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,faces_buffer);

	readFaces();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,n_faces*sizeof(GLuint)*3,&ico_faces[0],GL_STATIC_DRAW);
	
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
void RenderDwudziestoscian(float xPos, float yPos, float zPos, float scale) {
	matrixStack.PushMatrix();

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

	matrixStack.Scale(scale, scale, scale);

	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,matrixStack.GetMatrix());

	glDrawElements(GL_TRIANGLES,3*n_faces,GL_UNSIGNED_INT,0);

	matrixStack.PopMatrix();
}

int frameNo = 0;

void animate() {
	frameNo++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(shader);

	float ambient[] = {0.5,0.5,0.0}, diffuse[] = {0.5,0.2,1}, specular[] = {0.2,1,0.2};
	material.setMaterial(ambient, diffuse, specular, 1);

	float color[] = {1,0.5,0}, position[] = {0,0,-1};
	light.setLight(position, color, 180, 1, 1, 2);

	float ambientLight[] = {1,1,1};
	glUniform3fv(ambientLightLocation, 1, ambientLight);

	float small_color[] = {1,1,1}, small_position[] = {-5 + 2*cos(frameNo/180.0*PI/2.0),-5 + 2*sin(frameNo/180.0*PI/2.0),-2};
	small_light.setLight(small_position, small_color, 180, 1, 1, 2);

	cameraAngleX += randf() * 5.0;
	cameraAngleY += randf() * 2.7;
	cameraAngleZ += randf() * 50.0;
	cameraBothZ += randf() * 1.0;
	
	matrixStack.LoadIdentity();
	matrixStack.PushMatrix();
	matrixStack.Scale(0.1,0.1,0.1);
	//Perspektywa + krêcenie kamery
	matrixStack.Rotate(40, 1, 0, 0);
	matrixStack.Rotate(cameraBothZ, 0, 0, 1);
		
	// dodac kiedys kamere
	
	//siatka
	pushSiatka();
	float ambient2[] = {0.2,0.2,0.2};
	material.setMaterial(ambient2, diffuse, specular, 1);

	RenderDwudziestoscian(5,5,-2,0.8);

	RenderDwudziestoscian(-5,-5,-2,0.5);
	
	RenderDwudziestoscian(small_position[0], small_position[1], small_position[2], 0.2);


	matrixStack.PopMatrix();

	glutSwapBuffers();
}

int _tmain(int argc, char* argv[])
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
