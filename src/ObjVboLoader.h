/*
Andrew Chalmers
*/

#pragma once

#include "GeomStructs.h"

#include <GL/glew.h>
#define GLEW_STATIC 

#define G308_SHADE_POLYGON 0		
#define G308_SHADE_WIREFRAME 1

// VBO Vert3D buffer offset for values
//#define BUFFER_OFFSET(i) ((void*)(i))
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class ObjVboLoader{
private:
	Vert3D* vertexSet;
	GLuint* indices;

	// Array for Geometry
	GLfloat* vertices;
	GLfloat* normals;
	GLfloat* uvCoords;

	GLuint* indicesVertices;
	GLuint* indicesNormals;
	GLuint* indicesUvs;

	GLuint vertexVBOID;
	GLuint normalsVBOID;
	GLuint uvVBOID;
	GLuint indexVBOID;

	// Data for Geoemetry
	int m_nNumPoint;
	int m_nNumUV;
	int m_nNumNormal;
	int m_nNumPolygon;

	int mode; // Which mode to display
	int shaderIndex; // Which shader the geo is rendered with

	// Data for Rendering
	int m_glGeomListPoly;	// Display List for Polygon
	int m_glGeomListWire;	// Display List for Wireframe

	void createPolyObject(char*);
	void createWireFrameObject(char*);

public:
	ObjVboLoader(void);
	~ObjVboLoader(void);

	void ReadOBJ(const char* filename);

	void CreateGLPolyGeometry(); 
	void RenderGeometry();
	void toggleMode();
	void SetShaderIndex(int i);	
	int GetShaderIndex();
};
