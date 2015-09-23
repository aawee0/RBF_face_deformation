
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "define.h"
#include "string.h"
#include <GL/glew.h>
#pragma comment(lib,"glew32.lib")
#include <GL/glut.h>
#include "RBFInterpolator.h"	
#include "GeomStructs.h"

class G308_Geometry
{
public:
	G308_Geometry(int obj_num);
	~G308_Geometry(void);

	// VBO arrays
	//Vert3D* vertexSet;
	GLfloat* vertexSet;
	GLuint* indices;

	GLuint vertexVBOID;
	GLuint normalsVBOID;
	GLuint uvVBOID;
	GLuint indexVBOID;

	// Array for Geometry
	G308_Point* m_pVertexArray;		// Vertex Array
	G308_Point* m_pSelectedVertex;	
	G308_Normal* m_pNormalArray;	// Normal Array
	G308_Normal* m_pFaceNormalArray;
	G308_Triangle* m_pTriangles;	// Trianglem_glGeomListMap Array
	G308_UVcoord* m_pUVArray;	    // Texture Coordinate Array
	G308_Point* m_pDisplaced;	// displaced vertices

	vector<float> controlPointPosX;										// X-coordinates of control points
	vector<float> controlPointPosY;										// Y-coordinates of control points
	vector<float> controlPointPosZ;										// Z-coordinates of control points
	vector<float> controlPointDisplacementX;							// Displacement of control points in the X-direction
	vector<float> controlPointDisplacementY;							// Displacement of control points in the Y-direction
	vector<float> controlPointDisplacementZ;							// Displacement of control points in the Z-direction
	RBFInterpolator rbfX, rbfY, rbfZ;	

	GLfloat *distances; // geodesic distance from a point

	GLuint *cp_numbers; 
	int cp_num;
	

	G308_Point pivot; // translation of an obj
	G308_Point sel; // selected vertex
	int sel_vrt; // selected face number
	int sel_cnt; // closest vertex of the face

	int sel_axis; // -2 is X, -3 is Y, -4 is Z

	// Data for Geoemetry
	int m_nNumPoint;
	int m_nNumUV;
	int m_nNumPolygon;
	int m_nNumPolygon_Eyes, m_nNumPolygon_Head;
	int m_nNumNormal;

	int mode; // Which mode to display

	// Data for Rendering
	int m_glGeomListPoly;	// Display List for Polygon
	int m_glGeomListWire;	// Display List for Wireframe
	int m_glGeomListEyes;

	int object;
	GLuint *texture;
	bool textured;

	bool showCtrl;

	int cursorX,cursorY;


public:
	void ReadOBJ(const char* filename);
	void CalcDistances();
	bool isIn(unsigned int vrt, unsigned int nbrs[], int length);
	GLfloat eclDist(unsigned int vrt1, unsigned int vrt2);
	void CalcNormals();

	void CreateEyes();
	void RenderGeometry();     // mode : G308_SHADE_POLYGON, G308_SHADE_WIREFRAME
	void toggleMode(); //Switch between showing filled polygons and wireframes
	void setPivot(G308_Point pvt);
	
	void isOnClick(GLfloat winX, GLfloat winY);
	void startPicking();
	void processHits2 (GLint hits, GLuint buffer[], int sw);
	void stopPicking();
	void dislaceVertex(GLfloat disX, GLfloat disY, GLfloat disZ);

	void toggleGeo();
	//void interpolateDisp();
};



