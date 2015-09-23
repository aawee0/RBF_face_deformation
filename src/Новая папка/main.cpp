//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

//#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// My definition
#include "define.h"
#include "G308_Geometry.h"
#include "string.h"
#include <math.h>
#include <GL/glew.h>
#pragma comment(lib,"glew32.lib")
#include <GL/glut.h>

#define PI 3.14159265

// using namespace std;

// Global Variables
GLuint g_mainWnd;
GLuint g_nWinWidth  = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;
G308_Geometry* g_pTable = NULL;
G308_Geometry* g_pSphere = NULL;
G308_Geometry* g_pBunny = NULL;

G308_Point camPos;
G308_Point camLookAt;
G308_Point camUp;

// Picking Stuff
#define RENDER					1
#define SELECT					2
int mode = RENDER;

float param;
bool lmb_pressed;
int clickX,clickY;

void G308_Display() ;
void G308_Reshape(int w, int h) ;
void G308_SetCamera();
void G308_SetLight();
void G308_ChangeFrame(int button, int state, int x, int y);
void G308_keyboardListener(unsigned char, int, int);
void MouseMotion(int x, int y);
void renderBitmapString(float x, float y, void *font, char *string);

int main(int argc, char** argv)
{
	camPos.x = 0.5f;
	camPos.y = 0.0f;
	camPos.z = 1.5f; //25

	camLookAt.x = 0.0f;
	camLookAt.y = 0.0f;
	camLookAt.z = 0.0f;
	
/*
	camPos.x = -0.040429;
	camPos.y = 0.071349;
	camPos.z = 0.071386 + 1.5; //25

	camLookAt.x = -0.040429;
	camLookAt.y = 0.071349;
	camLookAt.z = 0.071386;
*/
	camUp.x = 0.0f;
	camUp.y = 1.0f;
	camUp.z = 0.0f;

	lmb_pressed = false;

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition( 800, 100 );
    glutInitWindowSize(g_nWinWidth, g_nWinHeight);
    g_mainWnd = glutCreateWindow("COMP308 Assignment1");
	glewInit();

    char dir[] = "assets/wood.jpg";
    //init(dir);

    glutDisplayFunc(G308_Display);
    glutReshapeFunc(G308_Reshape);
    glutKeyboardFunc(G308_keyboardListener);
    glutMouseFunc(G308_ChangeFrame);
	glutMotionFunc (MouseMotion);

    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);

	G308_Point pvt = {0.0,-1.0,0.0};
	g_pBunny = new G308_Geometry(2);
	g_pBunny->ReadOBJ("assets/head with eyes low.obj"); // 1) read OBJ function
	//g_pBunny->ReadOBJ("assets/eyes.obj"); // 1) read OBJ function
	//pvt.x = 0.0; pvt.y = -1.0; pvt.z = 0.0; // 0,-1,0 because obj coordinates are messed up
	g_pBunny->setPivot(pvt);

	param = 0.0;
	G308_SetLight();
	G308_SetCamera();
	glutMainLoop();

	if(g_pTable != NULL) delete g_pTable;

    return 0;
}

// Display function
void G308_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    G308_SetLight();
	
	glDisable(GL_TEXTURE);
    glDisable(GL_TEXTURE_2D);
	renderBitmapString(0, 0.8, GLUT_BITMAP_TIMES_ROMAN_24, "hello");

	glColor3f(0, 1, 0); 
	glRasterPos2f(0.5, 0.5); 
	
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'S');
	glEnable(GL_TEXTURE);
    glEnable(GL_TEXTURE_2D);
	
	if (mode == SELECT) g_pBunny->startPicking();
	
	g_pBunny->RenderGeometry();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	
	if (mode == SELECT) { 
		g_pBunny->stopPicking();
		mode = RENDER;
	} else glutSwapBuffers();
}

// Reshape function
void G308_Reshape(int w, int h)
{
    if (h == 0) h = 1;

	g_nWinWidth = w;
	g_nWinHeight = h;

    glViewport(0, 0, g_nWinWidth, g_nWinHeight);
}

// Set Light
void G308_SetLight()
{
		glShadeModel (GL_SMOOTH);

		// AMBIENT
		GLfloat amb = 0.9;
		GLfloat	amb0[] = {amb, amb, amb, 1.0};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb0);

		GLfloat	lit_amb[] = {0.0, 0.0, 0.0, 1.0}; // 1.0*param, 1.0*param, 1.0*param
		GLfloat	lit_diff[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat	lit_spec[] = {1.0, 1.0, 1.0, 1.0};

		GLfloat spot_dir[] = { 0.0, -1.0, 0.0, 1.0 };
		//GLfloat spot_dir[] = { 1.8, -0.8, -1.0, 1.0 };
		GLfloat spot_pos[] = { 0.0, 5.0, 1.0, 1.0 };
		//GLfloat spot_pos[] = { -7.19, 5.4, 3.8, 1.0 };

		// spot light
	    glLightfv(GL_LIGHT1, GL_AMBIENT,lit_amb);
	    glLightfv(GL_LIGHT1, GL_DIFFUSE,lit_diff);
	    glLightfv(GL_LIGHT1, GL_SPECULAR,lit_spec);
	    glLightfv(GL_LIGHT1,GL_POSITION,spot_pos);

	    //glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.01);
	    //glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01);

	    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.005);

	    //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

	    //glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	    // Definig spotlight attributes

	    glLightf(GL_LIGHT1,GL_SPOT_CUTOFF, 45.0);
	    glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,20.0);
	    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,spot_dir);

	    // directional
	    GLfloat dir = 0.5;
	    GLfloat	dir_diff[] = {dir, dir, dir, 1.0};
	    GLfloat	dir_spec[] = {dir, dir, dir, 1.0};
	    //GLfloat pnt_pos[] = { -2.0, 2.0, -2.0, 1.0 };
	    GLfloat dir_pos[] = { 0.5, 0.5, 0.5, 0.0 };

	    glLightfv(GL_LIGHT2, GL_AMBIENT,lit_amb);
   	    glLightfv(GL_LIGHT2, GL_DIFFUSE,dir_diff);
   	    glLightfv(GL_LIGHT2, GL_SPECULAR,dir_spec);
   	    glLightfv(GL_LIGHT2, GL_POSITION, dir_pos);

   	    // point
   	    GLfloat pnt_pos[] = { -2.0, 4.0, 2.0, 1.0 };
   	    glLightfv(GL_LIGHT3, GL_AMBIENT,lit_amb);
 	    glLightfv(GL_LIGHT3, GL_DIFFUSE,dir_diff);
   	 	glLightfv(GL_LIGHT3, GL_SPECULAR,dir_spec);
   	 	glLightfv(GL_LIGHT3, GL_POSITION, pnt_pos);


	    glEnable(GL_COLOR_MATERIAL);
	    glEnable(GL_DEPTH_TEST);





	    //glEnable(GL_LIGHT1);
	    //glEnable(GL_LIGHT2);
		//glEnable(GL_LIGHT2);

}

void G308_ChangeFrame(int button, int state, int x, int y){

	// keep an indicator of the LMB
	if (button == GLUT_LEFT_BUTTON) lmb_pressed=(state == GLUT_DOWN);

	printf(" { %d } ", button);
	if ((button == 3) || (button == 4)) // It's a wheel event
	{
	  // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
	  if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
	  GLdouble norm = sqrt( (camPos.x-camLookAt.x)*(camPos.x-camLookAt.x) + (camPos.y-camLookAt.y)*(camPos.y-camLookAt.y) + (camPos.z-camLookAt.z)*(camPos.z-camLookAt.z) );

	  if (norm>1.0 && button==3) {
		  camPos.x -= camPos.x/norm;
		  camPos.y -= camPos.y/norm;
		  camPos.z -= camPos.z/norm;
	  }
	  else {
		  camPos.x += camPos.x/norm;
		  camPos.y += camPos.y/norm;
		  camPos.z += camPos.z/norm;
	  }

	  G308_SetCamera(); 
	  //glutPostRedisplay();
	  //printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);
	} else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mode = SELECT;
		clickX=x;
		clickY=y;
		//printf(" ( %d, %d ) ", x,y);
		g_pBunny->cursorX=x;
		g_pBunny->cursorY=y;
	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		g_pBunny->interpolateDisp();
	} else if (state == GLUT_DOWN){
		g_pBunny->toggleMode();
	}
	glutPostRedisplay();
}
// Set Camera Position
void G308_SetCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos.x, camPos.y, camPos.z, camLookAt.x, camLookAt.y, camLookAt.z, camUp.x, camUp.y, camUp.z);
	glutPostRedisplay();
}


void G308_keyboardListener(unsigned char key, int x, int y) {
	//Code to respond to key events
	GLdouble angle=(5.0*PI/180.0);
	GLdouble norm = sqrt( (camPos.x-camLookAt.x)*(camPos.x-camLookAt.x) + (camPos.y-camLookAt.y)*(camPos.y-camLookAt.y) + (camPos.z-camLookAt.z)*(camPos.z-camLookAt.z) );
	//angle+=0.0872664626;
	switch (key)
	{
	  case 27:             // ESCAPE key
		  exit (0);
		  break;
	  case 'r':
		  //glRotatef( 5.0, 0, 1.0f, 0 );
		  //glutPostRedisplay();
		  // rotate camera
		  if((- camPos.x * sin(angle) + camPos.z * cos(angle))>0) {
			camPos.x = camPos.x * cos(angle) + camPos.z * sin(angle);
			camPos.z = - camPos.x * sin(angle) + camPos.z * cos(angle);

			G308_SetCamera();
		  }
		  break;
	  case 'e':
		  //glRotatef( -5.0, 0, 1.0f, 0 );
		  //glutPostRedisplay();
		  if((camPos.x * sin(angle) + camPos.z * cos(angle))>0) {
			camPos.x = camPos.x * cos(angle) - camPos.z * sin(angle);
			camPos.z = camPos.x * sin(angle) + camPos.z * cos(angle);
			G308_SetCamera();
		  }
		  break;
	  case 'c':
		  if (g_pBunny->showCtrl) g_pBunny->showCtrl=false;
		  else g_pBunny->showCtrl=true;
		  glutPostRedisplay();
		  break;
	  case 'n':
		  g_pBunny->CalcNormals();
		  glutPostRedisplay();
		  break;
	  case '+':
		  if (norm>2.0) {
			camPos.x -= camPos.x/norm;
			camPos.y -= camPos.y/norm;
			camPos.z -= camPos.z/norm;
			G308_SetCamera(); 
		  }
		  break;
	  case '-':
		  camPos.x += camPos.x/norm;
		  camPos.y += camPos.y/norm;
		  camPos.z += camPos.z/norm;
		  G308_SetCamera(); 
		  break;
	  }

}

void MouseMotion(int x, int y)
{
	if (lmb_pressed) {
		if (camPos.x<0) g_pBunny->dislaceVertex((x-clickX),(y-clickY), -(x-clickX));
		else g_pBunny->dislaceVertex((x-clickX),(y-clickY),(x-clickX));
		printf(" ( %d, %d ) ", (x-clickX),(y-clickY));
		clickX=x;
		clickY=y;
		//g_pBunny->motionX=x;
		//g_pBunny->motionY=y;
		//g_pBunny->interpolateDisp();
	}
	glutPostRedisplay();
}

void renderBitmapString(float x, float y, void *font, char *string) 
{  
  char *c;
  glColor3f(1, 1, 1); 
  glRasterPos2f(x,y);
  for (c=string; *c != '\0'; c++) 
  {
    glutBitmapCharacter(font, *c);
  }
}