//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
//
// Edited by Daniel Atkins
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

#pragma once

// Default Window 
#define G308_WIN_WIDTH	1024
#define G308_WIN_HEIGHT	768

// Projection parameters
#define G308_FOVY		20.0
#define G308_ZNEAR_3D	1
#define G308_ZFAR_3D	1000.0
#define G308_ZNEAR_2D	-50.0
#define G308_ZFAR_2D	50.0

// Shading mode : 0 Polygon, 1 Wireframe
#define G308_SHADE_POLYGON 0		
#define G308_SHADE_WIREFRAME 1

// Define number of vertex 
#define G308_NUM_VERTEX_PER_FACE 3 // Triangle = 3, Quad = 4 

// Define Basic Structures
struct G308_Point {
	float x;
	float y;
	float z;
};

struct vec4{
	float x;
	float y;
	float z;
	float w;
};

struct G308_RGBA {
	float r;
	float g;
	float b;
	float a;
};

typedef G308_Point G308_Normal;

struct G308_UVcoord {

	float u;
	float v;
};

/*
 * Normals and textures need to be defined by face, not by vertex.
 * Reminder: these are just indicies into the normal and texture arrays.
 * n1 and t1 are the normals and texture co-ordinates for vertex 1 of this face.
 * Same goes for (v2,n2,t2) etc.
 */
struct G308_Triangle {

	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	unsigned int n1;
	unsigned int n2;
	unsigned int n3;
	unsigned int t1;
	unsigned int t2;
	unsigned int t3;
};

struct G308_Quad {

	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	unsigned int v4;
	unsigned int n1;
	unsigned int n2;
	unsigned int n3;
	unsigned int n4;
	unsigned int t1;
	unsigned int t2;
	unsigned int t3;
	unsigned int t4;
};


struct lightParameters
{
   vec4 ambient;              // Aclarri
   vec4 diffuse;              // Dcli
   vec4 specular;             // Scli
   vec4 position;             // Ppli
   vec4 halfVector;           // Derived: Hi
   vec4 spotDirection;        // Sdli
   float spotExponent;        // Srli
   float spotCutoff;          // Crli
                              // (range: [0.0,90.0], 180.0)
   float spotCosCutoff;       // Derived: cos(Crli)
                              // (range: [1.0,0.0],-1.0)
   float constantAttenuation; // K0
   float linearAttenuation;   // K1
   float quadraticAttenuation;// K2
};


struct materialParameters
{
   vec4 emission;    // Ecm
   vec4 ambient;     // Acm
   vec4 diffuse;     // Dcm
   vec4 specular;    // Scm
   float shininess;  // Srm
};
