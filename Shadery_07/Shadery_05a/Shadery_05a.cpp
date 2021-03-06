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
GLint MVMatrixLocation, PMatrixLocation, NormalMatrixLocation, ambientLightLocation, textureLocation; 
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

GLuint vertex_buffer, faces_buffer, texture_buffer;

vector<float> ico_vertices;
vector<GLuint> ico_faces;
int n_faces, n_vertices;

void readVertices() {
   FILE *fvertices=fopen("cube_vertices.dat","r");
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
	FILE *ffaces=fopen("cube_faces.dat","r");
   if(ffaces==NULL) {
   fprintf(stderr,"cannot open faces file for reading\n");
   exit(-1);
   }
	n_faces = 0;
	char line[120];
   while(fgets(line,120,ffaces)!=NULL) {
	   GLuint  i,j,k,l;
	   
	   sscanf(line,"%u %u %u %u",&i,&j,&k,&l);
	   n_faces++;
	   ico_faces.push_back(i-1);
	   ico_faces.push_back(j-1);
	   ico_faces.push_back(k-1);
	   ico_faces.push_back(l-1);
   }
}

GLuint textureID[3];

void SetupTexture(int n, const char *szFileName) {
	glBindTexture(GL_TEXTURE_2D, textureID[n]);

	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);

	cout << "Read texture: " << szFileName << " (" << nWidth << "," << nHeight << ")" << endl;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);

	free(pBits);
};

GLuint textureCoords[] = {
      // Front face
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0,

      // Back face
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0,
      0.0, 0.0,

      // Top face
      0.0, 1.0,
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,

      // Bottom face
      1.0, 1.0,
      0.0, 1.0,
      0.0, 0.0,
      1.0, 0.0,

      // Right face
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0,
      0.0, 0.0,

      // Left face
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0,
};

void SetupRC() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	shader = gltLoadShaderPairWithAttributes("pass_thru_shader.vp", "pass_thru_shader.fp", 
				4, 
				GLT_ATTRIBUTE_VERTEX, "vVertex", 
				GLT_ATTRIBUTE_COLOR, "vColor",
	            GLT_ATTRIBUTE_TEXTURE0, "texCoord0",
				GLT_ATTRIBUTE_NORMAL, "vNormal");

	glGenTextures(3,textureID);

	SetupTexture(0, "1.tga");

	fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_NORMAL : %d \n",
            GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_NORMAL);
	
	MVMatrixLocation = glGetUniformLocation(shader, "MVMatrix");
	if(MVMatrixLocation == -1) {
		fprintf(stdout, "Hej ho, hej ho, uniformy nie dzia�aj� MVMatrix\n");
	}

	PMatrixLocation = glGetUniformLocation(shader, "PMatrix");
	if(PMatrixLocation == -1) {
		fprintf(stdout, "Hej ho, hej ho, uniformy nie dzia�aj� PMatrix\n");
	}

	NormalMatrixLocation = glGetUniformLocation(shader, "normalMatrix");
	if(NormalMatrixLocation == -1) {
		fprintf(stdout, "Hej ho, hej ho, uniformy nie dzia�aj� normalMatrix\n");
	}

	textureLocation = glGetUniformLocation(shader, "texture0");
	
	ambientLightLocation = glGetUniformLocation(shader, "ambientLight");

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,n_faces*sizeof(GLuint)*4,&ico_faces[0],GL_STATIC_DRAW);

	glGenBuffers(1,&texture_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,texture_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,n_faces*sizeof(GLuint)*4,textureCoords,GL_STATIC_DRAW);
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
void RenderSzescian(float xPos, float yPos, float zPos, float scale) {
	matrixStack.PushMatrix();

	matrixStack.Translate(xPos, yPos, zPos);
	matrixStack.Scale(scale, scale, scale);

	M3DMatrix44f MVMatrix;
	m3dCopyMatrix44(MVMatrix, matrixStack.GetMatrix());
	
	glUniformMatrix4fv(MVMatrixLocation,1,GL_FALSE,MVMatrix);

	M3DMatrix44f normalMatrix;
	m3dInvertMatrix44(normalMatrix,MVMatrix);

	float tmp;
	// transponse
	for(int n = 0; n < 3; n++) {
		for(int m = n + 1; m <= 3; m++) {
			tmp = normalMatrix[4*n + m];
			normalMatrix[4*n + m] = normalMatrix[4*m + n];
			normalMatrix[4*m + n] = tmp;
		}
	}

	glUniformMatrix3fv(NormalMatrixLocation,1,GL_FALSE,normalMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID[0]);
	glUniform1i(textureLocation, 0);

	glDrawElements(GL_QUADS,4*n_faces,GL_UNSIGNED_INT,0);

	matrixStack.PopMatrix();
}

int frameNo = 0;

void animate() {
	frameNo++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(shader);

	//cameraAngleX += randf() * 5.0;
	//cameraAngleY += randf() * 2.7;
	//cameraAngleZ += randf() * 50.0;
	cameraBothZ += randf() * 10.0;
	
	matrixStack.LoadIdentity();
	matrixStack.PushMatrix();
	matrixStack.Scale(0.1,0.1,0.1);
	//Perspektywa + kr�cenie kamery
	matrixStack.Rotate(40, 1, 0, 0);
	matrixStack.Rotate(cameraBothZ, 0, 0, 1);
		
	glUniformMatrix4fv(PMatrixLocation,1,GL_FALSE,matrixStack.GetMatrix());
	
	float ambient[] = {0.5,0.5,0.0}, diffuse[] = {0.5,0.2,1}, specular[] = {0.2,1,0.2};
	material.setMaterial(ambient, diffuse, specular, 1);

	float color[] = {1,0.5,0}, position[] = {0,0,-1};
	light.setLight(position, color, 180, 1, 1, 2);

	float ambientLight[] = {1,1,1};
	glUniform3fv(ambientLightLocation, 1, ambientLight);

	float small_color[] = {1,1,1}, small_position[] = {5 + 2*cos(frameNo/180.0*PI/2.0), 5 + 2*sin(frameNo/180.0*PI/2.0), -1};
	M3DVector4f small_position_transformed;
	m3dTransformVector4(small_position_transformed, small_position, matrixStack.GetMatrix());
	small_light.setLight(small_position_transformed, small_color, 180, 1, 1, 2);

	matrixStack.PopMatrix();

	//siatka
	pushSiatka();
	float ambient2[] = {0.2,0.2,0.2};
	material.setMaterial(ambient2, diffuse, specular, 1);

	RenderSzescian(5,5,-2,0.8);

	RenderSzescian(-5,-5,-2,0.5);
	
	RenderSzescian(small_position[0], small_position[1], small_position[2], 0.2);

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
