/*
Andrew Chalmers
*/


#include "ObjVboLoader.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>

#include <GL/glew.h>
#define GLEW_STATIC


ObjVboLoader::ObjVboLoader(void) {
	vertices = NULL;
	normals = NULL;
	uvCoords = NULL;
	indicesVertices = NULL;
	indicesNormals = NULL;
	indicesUvs = NULL;

	vertexSet = NULL;
	indices = NULL;

	mode = G308_SHADE_POLYGON;

	m_nNumPoint = m_nNumUV = m_nNumPolygon = 0;
	m_glGeomListPoly = m_glGeomListWire = 0;
}

ObjVboLoader::~ObjVboLoader(void) {
	if (vertices != NULL)
		delete[] vertices;
	if (normals != NULL)
		delete[] normals;
	if (uvCoords != NULL)
		delete[] uvCoords;
	if (indicesVertices != NULL)
		delete[] indicesVertices;
	if (indicesNormals != NULL)
		delete[] indicesNormals;
	if (indicesUvs != NULL)
		delete[] indicesUvs;

	if (vertexSet != NULL)
		delete[] vertexSet;
	if (indices != NULL)
		delete[] indices;

	glDeleteBuffers(1, &vertexVBOID);
	glDeleteBuffers(1, &indexVBOID);
}

void ObjVboLoader::ReadOBJ(const char *filename) {
	printf("Reading Object\n");

	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	int numVert, numNorm, numUV, numFace, numVertSet;
	float x, y, z;
	float u, v;

	numVert = numNorm = numUV = numFace = numVertSet = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error reading %s file\n", filename);
	else
		printf("Reading %s file\n", filename);

	// Check number of vertex, normal, uvCoord, and Face
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
				numUV++;
			else if (vmode == 'n') // normal
				numNorm++;
			else if (vmode == ' ') // vertex
				numVert++;
			break;
		case 'f': /* faces */
			numFace++;
			break;
		}
	}

	m_nNumPoint = numVert;
	m_nNumUV = numUV;
	m_nNumPolygon = numFace;
	m_nNumNormal = numNorm;

	printf("Number of Point %d, UV %d, Normal %d, Face %d\n", numVert, numUV, numNorm, numFace);
	//-------------------------------------------------------------
	//	Allocate memory for array
	//-------------------------------------------------------------	
	if (vertices != NULL)
		delete[] vertices;
	vertices = new GLfloat[m_nNumPoint*3];
	
	if (normals != NULL)
		delete[] normals;
	normals = new GLfloat[m_nNumNormal*3];

	if (uvCoords != NULL)
		delete[] uvCoords;
	uvCoords = new GLfloat[m_nNumUV*2];

	if (indicesVertices != NULL)
		delete[] indicesVertices;
	indicesVertices = new GLuint[m_nNumPolygon*3];

	if (indicesNormals != NULL)
		delete[] indicesNormals;
	indicesNormals = new GLuint[m_nNumPolygon*3];

	if (indicesUvs != NULL)
		delete[] indicesUvs;
	indicesUvs = new GLuint[m_nNumPolygon*3];
	
	if (vertexSet != NULL)
		delete[] vertexSet;
	vertexSet = new Vert3D[m_nNumPoint];

	if (indices != NULL)
		delete[] indices;
	indices = new GLuint[m_nNumPolygon*3];

	
	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = numVertSet = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
			{
				sscanf(str, "vt %f %f", &u, &v);

				uvCoords[numUV] =  u;
				uvCoords[numUV+1] =  v;
				numUV+=2;
			} else if (vmode == 'n') // normal
			{
				sscanf(str, "vn %f %f %f", &x, &y, &z);

				normals[numNorm] = x;
				normals[numNorm+1] = y;
				normals[numNorm+2] = z;
				numNorm+=3;
			} else if (vmode == ' ') // vertex
			{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				if(numVert == -1){
					vertices[numVert] = x-3;
					vertices[numVert+1] = y;
					vertices[numVert+2] = z+3;
				}else{
					vertices[numVert] = x;
					vertices[numVert+1] = y;
					vertices[numVert+2] = z;
				}
				numVert+=3;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */
			if (numNorm > 0 && numUV > 0) {
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			} else if(numNorm > 0 && numUV ==0){
				sscanf(str, "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
			} else if(numUV > 0 && numNorm==0){
				sscanf(str, "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
			} else if(numUV==0 && numNorm==0){
				sscanf(str, "f %d %d %d", &v1, &v2, &v3);
			}

			// Vertex indicies for triangle:
			if (numVert != 0) {
				indicesVertices[numFace] = (v1 - 1);
				indicesVertices[numFace+1] = (v2 - 1);
				indicesVertices[numFace+2] = (v3 - 1);
			}

			// Normal indicies for triangle
			if (numNorm != 0) {
				indicesNormals[numFace] = n1 - 1;
				indicesNormals[numFace+1] = n2 - 1;
				indicesNormals[numFace+2] = n3 - 1;
			}

			// UV indicies for triangle
			if (numUV != 0) {
				indicesUvs[numFace] = t1 - 1;
				indicesUvs[numFace+1] = t2 - 1;  
				indicesUvs[numFace+2] = t3 - 1;
			}
			numFace+=3;
			break;
		default:
			break;
		}
	}
	fclose(fp);

	// Wavefront .obj has attribute based indexing
	// Need to use a single index to represent polygons pos, norm and uv
	// Go through, create Vert3D structure holding data, and index the Vert3D object
	int w = 3;
	for(int i = 0; i < m_nNumPolygon*3;){
		Vert3D  v;
		v.vertex.x = vertices[indicesVertices[i]*w];
		v.vertex.y = vertices[indicesVertices[i]*w+1];
		v.vertex.z = vertices[indicesVertices[i]*w+2];

		v.normal.x = normals[indicesNormals[i]*w];
		v.normal.y = normals[indicesNormals[i]*w+1];
		v.normal.z = normals[indicesNormals[i]*w+2];

		v.uv_coord.u = uvCoords[indicesUvs[i]*2];
		v.uv_coord.v = uvCoords[indicesUvs[i]*2+1];

		vertexSet[indicesVertices[i]] = v;
		indices[i] =  indicesVertices[i];

		i++;
	}
	printf("Reading OBJ file: Success\n");
}

void ObjVboLoader::CreateGLPolyGeometry() {
	printf("\nCreating Geometry\n\n");
	vertexVBOID = 0;
	glGenBuffers(1, &vertexVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert3D)*m_nNumPoint, &vertexSet[0].vertex.x, GL_STATIC_DRAW);

	indexVBOID = 0;
	glGenBuffers(1, &indexVBOID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_nNumPolygon*3, indices, GL_STATIC_DRAW);
}

void ObjVboLoader::toggleMode() {
	if (mode == G308_SHADE_POLYGON) {
		mode = G308_SHADE_WIREFRAME;
	} else {
		mode = G308_SHADE_POLYGON;
	}
	std::cout << "Mode:"<< mode << std::endl;
}

//http://www.opengl.org/wiki/VBO_-_just_examples
void ObjVboLoader::RenderGeometry() {
	if (mode == G308_SHADE_POLYGON){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else if (mode == G308_SHADE_WIREFRAME) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		printf("Warning: Wrong Shading Mode. \n");
	}

	// Bind the vertex set ID and enable attributes
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBOID);

	/*
	// Old GL version
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vert3D), BUFFER_OFFSET(0));  

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(Vert3D), BUFFER_OFFSET(12)); 

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vert3D), BUFFER_OFFSET(24)); 
	*/

	glEnableVertexAttribArray(0);   
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert3D), BUFFER_OFFSET(0)); 
	glEnableVertexAttribArray(1);  
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert3D), BUFFER_OFFSET(12));  
	glEnableVertexAttribArray(2);   
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vert3D), BUFFER_OFFSET(24)); 

	// Bind the indices of vertices and draw triangle strip
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
	glDrawElements(GL_TRIANGLES, m_nNumPolygon*3, GL_UNSIGNED_INT, BUFFER_OFFSET(0)); //sizeof(GLuint)*m_nNumPolygon*3
		
	// Disable attributes
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ObjVboLoader::SetShaderIndex(int i) {
	shaderIndex = i;
}

int ObjVboLoader::GetShaderIndex() {
	return shaderIndex;
}
