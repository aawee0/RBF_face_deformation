
#define _CRT_SECURE_NO_WARNINGS

#include "G308_Geometry.h"
#include <math.h>

#define EYES 16134

//using namespace std;
int numVert, numNorm, numUV, numFace;

// Picking Stuff
#define BUFSIZE 1024
GLuint selectBuf[BUFSIZE];
GLint hits;

GLuint VertexArrayID;
// This will identify our vertex buffer
GLuint vertexbuffer;
GLuint indexbuffer;



G308_Geometry::G308_Geometry(int obj_num) {
	m_pVertexArray = NULL;
	m_pSelectedVertex = NULL;
	m_pNormalArray = NULL;
	m_pFaceNormalArray = NULL;
	m_pUVArray = NULL;
	m_pTriangles = NULL;
	m_pDisplaced = NULL;
	distances = NULL;

	// numbers of control points
	cp_num = 29;
	cp_numbers = new GLuint[cp_num];
	cp_numbers[0]=17784; cp_numbers[1]=17918; cp_numbers[2]=17643; cp_numbers[3]=17946;	cp_numbers[4]=16999; cp_numbers[5]=16921; cp_numbers[6]=17668; cp_numbers[7]=17766;
	cp_numbers[8]=17930; cp_numbers[9]=17915; cp_numbers[10]=17944; cp_numbers[11]=16589; cp_numbers[12]=16728; cp_numbers[13]=17184; cp_numbers[14]=16900; cp_numbers[15]=16791;
	cp_numbers[16]=17433; cp_numbers[17]=16908; cp_numbers[18]=17691; cp_numbers[19]=18152; cp_numbers[20]=17309; cp_numbers[21]=17063; cp_numbers[22]=17142; cp_numbers[23]=17375;
	cp_numbers[24]=17771; cp_numbers[25]=17982; cp_numbers[26]=17972; cp_numbers[27]=18180; cp_numbers[28]=17695;



	pivot.x = 0.0;
	pivot.y = 0.0;
	pivot.z = 0.0;

	sel.x = 0.0;
	sel.y = 0.0;
	sel.z = 0.0;
	sel_vrt=-1;
	sel_cnt=0;
	sel_axis=-1;

	cursorX = 0;
	cursorY = 0;

	vertexSet = NULL;
	indices = NULL;


	mode = G308_SHADE_POLYGON;

	m_nNumPoint = m_nNumUV = m_nNumPolygon = 0;
	m_nNumPolygon_Eyes = m_nNumPolygon_Head = 0;
	m_glGeomListPoly = m_glGeomListWire = m_glGeomListEyes = 0;

	object = obj_num;

	showCtrl = true;

	texture = new GLuint;
	textured = false;

}

G308_Geometry::~G308_Geometry(void) {
	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	if (m_pSelectedVertex != NULL)
		delete[] m_pSelectedVertex;
	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	if (m_pFaceNormalArray != NULL)
		delete[] m_pFaceNormalArray;
	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	if (m_pDisplaced != NULL)
		delete[] m_pDisplaced;

	// VBO
	if (vertexSet != NULL)
		delete[] vertexSet;
	if (indices != NULL)
		delete[] indices;

}

//-------------------------------------------------------
// This function read obj file having
// triangle faces consist of vertex v, texture coordinate vt, and normal vn
// e.g. f v1/vt1/vn1 v2/vt1/vn2 v3/vt3/vn3
//
// You can revise the following function for reading other variations of obj file
// 1) f v1//vn1 v2//vn2 v3//vn3 ; no texture coordinates such as bunny.obj
// 2) f v1 v2 v3 ; no normal and texture coordinates such as dragon.obj
// The case 2) needs additional functions to build vertex normals
//--------------------------------------------------------

void G308_Geometry::ReadOBJ(const char *filename) {
	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, v4, n1, n2, n3, t1, t2, t3, t4;

	float x, y, z;
	float u, v;

	numVert = numNorm = numUV = numFace = 0;

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
	m_nNumNormal = numVert;

	printf("Number of Point %d, UV %d, Normal %d, Face %d\n", numVert, numUV,
			numNorm, numFace);
	//-------------------------------------------------------------
	//	Allocate memory for array
	//-------------------------------------------------------------

	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	m_pVertexArray = new G308_Point[m_nNumPoint];

	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	m_pNormalArray = new G308_Normal[m_nNumNormal];

	if (m_pFaceNormalArray != NULL)
		delete[] m_pFaceNormalArray;
	m_pFaceNormalArray = new G308_Normal[2*m_nNumPolygon];

	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	m_pUVArray = new G308_UVcoord[m_nNumUV];

	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	m_pTriangles = new G308_Triangle[2*m_nNumPolygon];

	if (m_pDisplaced != NULL)
		delete[] m_pDisplaced;
	m_pDisplaced = new G308_Point[m_nNumPoint];

	// VBO
	if (vertexSet != NULL)
		delete[] vertexSet;
	vertexSet = new GLfloat[m_nNumPoint*6];

	if (indices != NULL)
		delete[] indices;
	indices = new GLuint[m_nNumPolygon*6];


	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
			{
				sscanf(str, "vt %f %f", &u, &v);
				m_pUVArray[numUV].u = u;
				m_pUVArray[numUV].v = v;
				numUV++;
				//numUV=0;
			} else if (vmode == 'n') // normal
			{
				sscanf(str, "vn %f %f %f", &x, &y, &z);
				m_pNormalArray[numNorm].x = x;
				m_pNormalArray[numNorm].y = y;
				m_pNormalArray[numNorm].z = z;
				numNorm++;
			} else if (vmode == ' ') // vertex
			{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				//if (numVert<100) printf("v %f %f %f", x, y, z);
				m_pVertexArray[numVert].x = x;
				m_pVertexArray[numVert].y = y;
				m_pVertexArray[numVert].z = z;
				// displacements - zero
				m_pDisplaced[numVert].x = x;
				m_pDisplaced[numVert].y = y;
				m_pDisplaced[numVert].z = z;
				numVert++;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */
			{

			int match = sscanf(str, "f %d/%d %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3, &v4, &t4);
			if (numFace==0) printf (str);

			// Vertex indicies for triangle:
			if (numVert != 0) {
				m_pTriangles[numFace].v1 = v1 - 1;
				m_pTriangles[numFace].v2 = v3 - 1;
				m_pTriangles[numFace].v3 = v4 - 1;
			}
			// Normal indicies for triangle
			m_pTriangles[numFace].n1 = v1 - 1;
			m_pTriangles[numFace].n2 = v3 - 1;
			m_pTriangles[numFace].n3 = v4 - 1;
			// UV indicies for triangle
			if (numUV != 0) {
				m_pTriangles[numFace].t1 = t1 - 1;
				m_pTriangles[numFace].t2 = t3 - 1;
				m_pTriangles[numFace].t3 = t4 - 1;
			}
			numFace++;

			// SECOND TRIANGLE OF
			// Vertex indicies for triangle:
			if (numVert != 0) {
				m_pTriangles[numFace].v1 = v1 - 1;
				m_pTriangles[numFace].v2 = v2 - 1;
				m_pTriangles[numFace].v3 = v3 - 1;
			}
			// Normal indicies for triangle
			m_pTriangles[numFace].n1 = v1 - 1;
			m_pTriangles[numFace].n2 = v2 - 1;
			m_pTriangles[numFace].n3 = v3 - 1;
			// UV indicies for triangle
			if (numUV != 0) {
				m_pTriangles[numFace].t1 = t1 - 1;
				m_pTriangles[numFace].t2 = t2 - 1;
				m_pTriangles[numFace].t3 = t3 - 1;
			}
			numFace++;

		}
		break;
		default:
			break;
		}
	}

	fclose(fp);
	printf("Reading OBJ file is DONE.\n");

	// NORMALS
	CalcNormals();
	//CalcDistances();
	CreateEyes();
	int i = 17784;

	/*
	printf (" %d %f \n", i, distances[i-EYES]);
	i = 17930;
	printf (" %d %f \n", i, distances[i-EYES]);
	i = 16921;
	printf (" %d %f \n", i, distances[i-EYES]);
	i = 17668;
	printf (" %d %f \n", i, distances[i-EYES]);
	i = 17771;
	printf (" %d %f \n", i, distances[i-EYES]);
	i = 17309;
	printf (" %d %f \n", i, distances[i-EYES]);
	i = 17142;
	printf (" %d %f \n", i, distances[i-EYES]);
	*/

	// initialize displacement data for RBF interpolation

	for (int i = 0; i<cp_num; i++)
	{
		controlPointPosX.push_back(m_pVertexArray[cp_numbers[i]].x);
		controlPointPosY.push_back(m_pVertexArray[cp_numbers[i]].y);
		controlPointPosZ.push_back(m_pVertexArray[cp_numbers[i]].z);

		controlPointDisplacementX.push_back(0.0f);
		controlPointDisplacementY.push_back(0.0f);
		controlPointDisplacementZ.push_back(0.0f);
	}

	// initialize interpolation functions
	rbfX = RBFInterpolator(controlPointPosX, controlPointPosY, controlPointPosZ, controlPointDisplacementX );
	rbfY = RBFInterpolator(controlPointPosX, controlPointPosY, controlPointPosZ, controlPointDisplacementY );
	rbfZ = RBFInterpolator(controlPointPosX, controlPointPosY, controlPointPosZ, controlPointDisplacementZ );

	// VBO
	for(int i = 0; i < m_nNumPoint-EYES; i++){
		/*
		if (i<=16134) {
			vertexSet[6*i] = 0;
			vertexSet[6*i+1] = 0;
			vertexSet[6*i+2] = 0;

			vertexSet[6*i+3] = 0;
			vertexSet[6*i+4] = 0;
			vertexSet[6*i+5] = 0;
		} else { */
			vertexSet[6*i] = m_pVertexArray[i+EYES].x;
			vertexSet[6*i+1] = m_pVertexArray[i+EYES].y;
			vertexSet[6*i+2] = m_pVertexArray[i+EYES].z;

			vertexSet[6*i+3] = m_pNormalArray[i+EYES].x;
			vertexSet[6*i+4] = m_pNormalArray[i+EYES].y;
			vertexSet[6*i+5] = m_pNormalArray[i+EYES].z;
		//}

		//if (i==17784 || i==17643 || i==17918) printf( " %f, %f, %f, ", vertexSet[3*i], vertexSet[3*i+1],vertexSet[3*i+2]);
		//vertexSet[i] = v;


	}
	/* for(int i = 0; i < 6*m_nNumPoint; i++){
		printf( " %f ", vertexSet[i]);
	} */
	printf( " ( %d %d ) ", numFace, m_nNumPoint);
	for(int i = 0; i < m_nNumPolygon*2; i++){
		if (m_pTriangles[i].v1>=EYES && m_pTriangles[i].v2>=EYES && m_pTriangles[i].v1>=EYES) {
			indices[3*m_nNumPolygon_Head] =  m_pTriangles[i].v1-EYES;
			indices[3*m_nNumPolygon_Head+1] =  m_pTriangles[i].v2-EYES;
			indices[3*m_nNumPolygon_Head+2] =  m_pTriangles[i].v3-EYES;
			m_nNumPolygon_Head++;
		}
	}
	m_nNumPolygon_Eyes=m_nNumPolygon*2 - m_nNumPolygon_Head;


	//printf( " %f, %f, %f, %f, %f, %f, %f, %f, %f  ", vertexSet[0], vertexSet[1],vertexSet[2], vertexSet[6], vertexSet[7],vertexSet[8], vertexSet[12], vertexSet[13],vertexSet[14]);
	printf( " %d %d %d ", indices[0], indices[1], indices[2]);


	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);

	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	// Give our vertices to OpenGL.
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(m_nNumPoint-EYES)*6 , vertexSet, GL_STATIC_DRAW); // sizeof(vertexSet)


	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_index_buffer_data), g_index_buffer_data, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_nNumPolygon*6, indices, GL_STATIC_DRAW); //  sizeof(indices)
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*m_nNumPolygon_Head*3, indices, GL_STATIC_DRAW); //  sizeof(indices)

	printf(" !!! %d !!! ", m_nNumPoint-EYES);

}

void G308_Geometry::CalcDistances() {
	string file_name = string("distances.txt");
	FILE *fp = fopen(file_name.c_str(), "w");

	if (distances != NULL) delete[] distances;
	distances = new GLfloat[m_nNumPoint-EYES];
	bool *checked = new bool[m_nNumPoint-EYES]; // the "-EYES" coordinates

	//for (int the_point=EYES; the_point<m_nNumPoint; the_point++) {

	for (int j=0; j<cp_num; j++) {

		//unsigned int the_point = 17784;
		unsigned int the_point = cp_numbers[j];
		printf (" (( %d %d )) \n", the_point ,cp_numbers[j]);
		for (int i=0; i<m_nNumPoint-EYES; i++) {
			checked[i]=false;
			distances[i]=1000;
			// comment out the following
			double dX=m_pVertexArray[the_point].x-m_pVertexArray[i+EYES].x;
			double dY=m_pVertexArray[the_point].y-m_pVertexArray[i+EYES].y;
			double dZ=m_pVertexArray[the_point].z-m_pVertexArray[i+EYES].z;
			distances[i]=sqrt(dX*dX + dY*dY + dZ*dZ);

		}// loop over all face points, set max distance
		distances[the_point-EYES]=0; // initial point
/*
		unsigned int cur_point = the_point; // start from the initial point

		// vertices left and vertex processing queue
		int vrtLeft=m_nNumPoint-EYES;
		unsigned int* queue = new unsigned int[2100];
		queue[0]=the_point;
		int queue_num = 0;
		int queue_size = 1;

		while (vrtLeft>0) {
			cur_point=queue[queue_num];

			//if (checked[ cur_point-EYES]) printf(" %#@%#%^@#%@#%#%#$%@#$%#$%#$%#$%#$%  \n");

			unsigned int* neighbors = new unsigned int[25];
			int nb_count = 0;
			for (int i=0; i<m_nNumPolygon*2; i++) {
				// are vertices of the face already checked or already neighbors
				bool v1_able = !isIn(m_pTriangles[i].v1, neighbors, nb_count) && !checked[m_pTriangles[i].v1-EYES];
				bool v2_able = !isIn(m_pTriangles[i].v2, neighbors, nb_count) && !checked[m_pTriangles[i].v2-EYES];
				bool v3_able = !isIn(m_pTriangles[i].v3, neighbors, nb_count) && !checked[m_pTriangles[i].v3-EYES];
				//printf(" %d %d \n ", m_pTriangles[i].v1, cur_point);
				// if not - add them to the neighbors
				if (m_pTriangles[i].v1==cur_point) {
					if (v2_able) neighbors[nb_count++]=m_pTriangles[i].v2;
					if (v3_able) neighbors[nb_count++]=m_pTriangles[i].v3;
				} else if (m_pTriangles[i].v2==cur_point) {
					if (v1_able) neighbors[nb_count++]=m_pTriangles[i].v1;
					if (v3_able) neighbors[nb_count++]=m_pTriangles[i].v3;
				} else if (m_pTriangles[i].v3==cur_point) {
					if (v1_able) neighbors[nb_count++]=m_pTriangles[i].v1;
					if (v2_able) neighbors[nb_count++]=m_pTriangles[i].v2;
				}

				if (nb_count>=25) {
					//for (int n=0; n<nb_count; n++) printf(" %d ", neighbors[n]);
					printf ("Too many neighbors for %d \n", cur_point);
				}
			}

			for (int i=0; i<nb_count; i++) {
				GLfloat dist = distances[cur_point-EYES] + eclDist(cur_point, neighbors[i]);
				if (dist < distances[neighbors[i]-EYES]) distances[neighbors[i]-EYES]=dist;

				bool inQue = false;
				for (int j=0; j<queue_size; j++) {
					if (queue[j]==neighbors[i]) inQue=true;
				}
				if (!inQue) {
					queue[queue_size]=neighbors[i];
					queue_size++;
				}
			}


			checked[cur_point-EYES]=true;

			queue_num++;
			delete[] neighbors;

			vrtLeft=0;

			if (queue_size>2000000) printf ("Queue overflow %d \n", vrtLeft);
			else if (queue_num>queue_size)  printf ("Shariki za roliki!!! %d \n");
			else {
				for (int i=0; i<m_nNumPoint-EYES; i++) if (checked[i]==false) vrtLeft++; //smthLeft=true;
				printf(" %d %d %d \n", vrtLeft, cur_point, nb_count);
			}

		}
		//printf(" !!!!! %d \n", queue_size);
		delete[] queue;
*/
		for (int i=0; i<m_nNumPoint-EYES; i++) fprintf(fp, "%lf\n", distances[i]);
		printf(" {{ %d }} \n", the_point);
	}

	fclose(fp);
}

bool G308_Geometry::isIn(unsigned int vrt, unsigned int nbrs[], int length) {
	for (int i=0; i<length; i++) if ((nbrs[i]==vrt) && (vrt >= EYES)) return true;
	// if no array entries at all, return false:
	return false;
}

GLfloat G308_Geometry::eclDist(unsigned int vrt1, unsigned int vrt2) {
	GLfloat dX = m_pVertexArray[vrt1].x - m_pVertexArray[vrt2].x;
	GLfloat dY = m_pVertexArray[vrt1].y - m_pVertexArray[vrt2].y;
	GLfloat dZ = m_pVertexArray[vrt1].z - m_pVertexArray[vrt2].z;
	return ( sqrt (dX*dX + dY*dY + dZ*dZ) );
}

void G308_Geometry::CalcNormals() {

	for (int i=0; i< numFace; i++) {
		int v1=m_pTriangles[i].v1;
		int v2=m_pTriangles[i].v2;
		int v3=m_pTriangles[i].v3;

		G308_Point uf = {(m_pVertexArray[v2].x-m_pVertexArray[v1].x), (m_pVertexArray[v2].y-m_pVertexArray[v1].y), (m_pVertexArray[v2].z-m_pVertexArray[v1].z)};
		G308_Point vf = {(m_pVertexArray[v3].x-m_pVertexArray[v1].x), (m_pVertexArray[v3].y-m_pVertexArray[v1].y), (m_pVertexArray[v3].z-m_pVertexArray[v1].z)};
		uf.x*=10;uf.y*=10;uf.z*=10;
		vf.x*=10;vf.y*=10;vf.z*=10;
		double xf = uf.y*vf.z - uf.z*vf.y;
		double yf = uf.z*vf.x - uf.x*vf.z;
		double zf = uf.x*vf.y - uf.y*vf.x;
		double nf = sqrt(xf*xf + yf*yf + zf*zf);
		m_pFaceNormalArray[i].x = xf/nf;
		m_pFaceNormalArray[i].y = yf/nf;
		m_pFaceNormalArray[i].z = zf/nf;
		//if (i < 20) printf("(%d %d %f %f %f) \n", i, m_pTriangles[i].v1, m_pFaceNormalArray[i].x, m_pFaceNormalArray[i].y, m_pFaceNormalArray[i].z);
		//if (i < 20) printf("(%d %d %f %f %f) \n", i, m_pTriangles[i].v1, m_pFaceNormalArray[i].x, m_pFaceNormalArray[i].y, m_pFaceNormalArray[i].z);
	}

	for (int i=0; i<numVert; i++) {
		float adj = 0.0;
		G308_Point normSum = {0.0,0.0,0.0};
		for (int j=0; j<numFace; j++) {
			if (m_pTriangles[j].v1 == i || m_pTriangles[j].v2 == i || m_pTriangles[j].v3 == i) {
				normSum.x+=m_pFaceNormalArray[j].x;
				normSum.y+=m_pFaceNormalArray[j].y;
				normSum.z+=m_pFaceNormalArray[j].z;

				adj+=1.0;
			}
			if (i==1 && (m_pTriangles[j].v1 == 1 || m_pTriangles[j].v2 == 1 || m_pTriangles[j].v3 == 1)) {
					//printf(" (%d %d %d %d) ", j, m_pTriangles[j].v1, m_pTriangles[j].v2, m_pTriangles[j].v3);

				}
			//printf(" %d ", j);
		}
		if (adj!=0){
			normSum.x/=adj;
			normSum.y/=adj;
			normSum.z/=adj;
			//printf(" %f %f %f \n", normSum.x, normSum.y, normSum.z);
		}
		m_pNormalArray[i].x=normSum.x;
		m_pNormalArray[i].y=normSum.y;
		m_pNormalArray[i].z=normSum.z;

		// update vertex buffer
		vertexSet[6*i+3] = m_pNormalArray[i].x;
		vertexSet[6*i+4] = m_pNormalArray[i].y;
		vertexSet[6*i+5] = m_pNormalArray[i].z;

		//if (numNorm < 20) printf("(%d %f %f %f) \n", numNorm, m_pNormalArray[numNorm].x, m_pNormalArray[numNorm].y, m_pNormalArray[numNorm].z);
		//numNorm++;

	}

}

void G308_Geometry::CreateEyes() {
	if (m_glGeomListEyes != 0){
		glDeleteLists(m_glGeomListEyes, 1);
	}
	// Assign a display list; return 0 if err
	m_glGeomListEyes = glGenLists(1);
	glNewList(m_glGeomListEyes, GL_COMPILE);


	float colorAmbt[4] = { 0.13125, 0.13125, 0.13125, 1.0 };
    float colorDiff[4] = { 0.4775, 0.4775, 0.4775, 1.0 };
	float colorSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
    //float colorAmbt[4] = {0.25f, 0.148f, 0.06475f, 1.0f  };
	//float colorDiff[4] = {0.4f, 0.2368f, 0.1036f, 1.0f };
    //float colorSpec[4] = {0.774597f, 0.458561f, 0.200621f, 1.0f };
    float colorEmis[4] = { 0.0, 0.0, 0.0, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, colorAmbt);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colorDiff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, colorSpec);
    glMaterialfv(GL_FRONT, GL_EMISSION, colorEmis);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.2*20);
    glColor4fv(colorDiff);

	glPushMatrix();
	glTranslatef(pivot.x,pivot.y,pivot.z);
	//glTranslatef(100,100,100);

	for (int i=0; i<numFace; i++) {
		int vrt1=m_pTriangles[i].v1, vrt2=m_pTriangles[i].v2, vrt3=m_pTriangles[i].v3;
		if (vrt1 < EYES && vrt2 < EYES && vrt3 < EYES) {
			glPushMatrix();
			glBegin(GL_TRIANGLES);
			glNormal3f(m_pNormalArray[vrt1].x, m_pNormalArray[vrt1].y, m_pNormalArray[vrt1].z);
			glVertex3f(m_pDisplaced[vrt1].x, m_pDisplaced[vrt1].y, m_pDisplaced[vrt1].z);

			glNormal3f(m_pNormalArray[vrt2].x, m_pNormalArray[vrt2].y, m_pNormalArray[vrt2].z);
			glVertex3f(m_pDisplaced[vrt2].x, m_pDisplaced[vrt2].y, m_pDisplaced[vrt2].z);

			glNormal3f(m_pNormalArray[vrt3].x, m_pNormalArray[vrt3].y, m_pNormalArray[vrt3].z);
			glVertex3f(m_pDisplaced[vrt3].x, m_pDisplaced[vrt3].y, m_pDisplaced[vrt3].z);
			glEnd();
			glPopMatrix();
		}
	}

	glPopMatrix();


	glEndList();


}

void G308_Geometry::toggleMode() {
	if (mode == G308_SHADE_POLYGON) {
		mode = G308_SHADE_WIREFRAME;
		printf("Reading OBJ file is DONE.\n");
	} else {
		mode = G308_SHADE_POLYGON;
	}
}


void G308_Geometry::RenderGeometry() {

	glCallList(m_glGeomListEyes);

	/*
	glColor3f(1.0f,0.0f,0.0f);
	glTexCoordPointer(3, GL_FLOAT, sizeof(GLfloat)*8, (float*)(sizeof(GLfloat)*5));
	glNormalPointer(GL_FLOAT, sizeof(GLfloat)*8, (float*)(sizeof(GLfloat)*3));
	glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*8, NULL);

	// Bind the indices of vertices and draw triangle strip
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
	glDrawElements(GL_TRIANGLES, m_nNumPolygon*3, GL_UNSIGNED_INT, NULL); //sizeof(GLuint)*m_nNumPolygon*3
	*/

	// 1rst attribute buffer : vertices
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*6, 0);
	glNormalPointer(GL_FLOAT, sizeof(GLfloat)*6, (float*)(sizeof(GLfloat)*3));

	/*glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);*/

			  //float colorAmbt[4] = { 0.53125, 0.53125, 0.53125, 1.0 };
			  float colorAmbt[4] = { 0.1, 0.1, 0.1, 1.0 };
			  //float colorDiff[4] = { 0.3775, 0.3775, 0.3775, 1.0 };
			  //float colorSpec[4] = { 0.5, 0.5, 0.5, 1.0 };
			  float colorDiff[4] = { 0.24, 0.16, 0.09, 1.0 };
			  float colorSpec[4] = { 0.1, 0.1, 0.1, 1.0 };
			  float colorEmis[4] = { 0.0, 0.0, 0.0, 1.0 };

			  glMaterialfv(GL_FRONT, GL_AMBIENT, colorAmbt);
			  glMaterialfv(GL_FRONT, GL_DIFFUSE, colorDiff);
			  glMaterialfv(GL_FRONT, GL_SPECULAR, colorSpec);
			  glMaterialfv(GL_FRONT, GL_EMISSION, colorEmis);
			  glMaterialf(GL_FRONT, GL_SHININESS, 0.2*20);
			  glColor4fv(colorDiff);

	glPushMatrix();
	glTranslatef(pivot.x,pivot.y,pivot.z);
	//glDrawElements(GL_TRIANGLES, m_nNumPolygon*6, GL_UNSIGNED_INT, NULL); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDrawElements(GL_TRIANGLES, m_nNumPolygon_Head*3, GL_UNSIGNED_INT, NULL); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	// draw control points
	glColor4f(0.7,0,0,1);
	glMaterialfv(GL_FRONT, GL_AMBIENT, colorAmbt);
	if (showCtrl) {
		glPushMatrix();
		glTranslatef(pivot.x,pivot.y,pivot.z);
		for (int i=0; i<cp_num; i++) {
			glPushMatrix();
			glPushName(cp_numbers[i]);
			glTranslatef(m_pDisplaced[cp_numbers[i]].x+controlPointDisplacementX[i], m_pDisplaced[cp_numbers[i]].y+controlPointDisplacementY[i], m_pDisplaced[cp_numbers[i]].z+controlPointDisplacementZ[i]);
			glutSolidSphere(0.003, 100, 100);
			//if (cp_numbers[i]==17784) glutSolidSphere(0.01, 100, 100);
			//glutSolidSphere(distances[cp_numbers[i]-EYES]*0.003, 100, 100);
			glPopName();
			glPopMatrix();
		}
		glPopMatrix();
	}

	/*
	glPushMatrix();
	glTranslatef(pivot.x,pivot.y,pivot.z);
	for (int i=EYES; i<numVert; i++) {
			glPushMatrix();
			glTranslatef(m_pVertexArray[i].x,m_pVertexArray[i].y,m_pVertexArray[i].z);
			glutSolidSphere(distances[i-EYES]*0.003, 100, 100);
			glPopMatrix();
	}
	glPopMatrix();
	*/

	/*
	glPushMatrix();
	glColor3f(1.0,0.0,0.0);
	glTranslatef(0.052729, 0.099127, 0.107657);
	glutSolidSphere(0.001, 100, 100);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0,0.0,0.0);
	glTranslatef(0.000515, 0.093231, 0.115924  );
	glutSolidSphere(0.001, 100, 100);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0,0.0,0.0);
	glTranslatef(0.026212, 0.098129, 0.115766);
	glutSolidSphere(0.001, 100, 100);
	glPopMatrix();*/






			//	printf(" %d ", sel_vrt);
		if (sel_vrt!=-1) {
/*
			glPushMatrix();
			glColor3f(1.0,0.0,0.0);
			if (sel_cnt==1) glTranslatef(m_pVertexArray[m_pTriangles[sel_vrt].v1].x+pivot.x, m_pVertexArray[m_pTriangles[sel_vrt].v1].y+pivot.y, m_pVertexArray[m_pTriangles[sel_vrt].v1].z+pivot.z);
			else if (sel_cnt==2) glTranslatef(m_pVertexArray[m_pTriangles[sel_vrt].v2].x+pivot.x, m_pVertexArray[m_pTriangles[sel_vrt].v2].y+pivot.y, m_pVertexArray[m_pTriangles[sel_vrt].v2].z+pivot.z);
			else if (sel_cnt==3) glTranslatef(m_pVertexArray[m_pTriangles[sel_vrt].v3].x+pivot.x, m_pVertexArray[m_pTriangles[sel_vrt].v3].y+pivot.y, m_pVertexArray[m_pTriangles[sel_vrt].v3].z+pivot.z);
			glutSolidSphere(0.002, 100, 100);
			glPopMatrix();
*/

			//printf (" %d ( %f %f %f ) ", sel_vrt, m_pVertexArray[m_pTriangles[sel_vrt].v1].x, m_pVertexArray[m_pTriangles[sel_vrt].v1].y, m_pVertexArray[m_pTriangles[sel_vrt].v1].z);

			int vrt1=m_pTriangles[sel_vrt].v1, vrt2=m_pTriangles[sel_vrt].v2, vrt3=m_pTriangles[sel_vrt].v3;

			glPushMatrix();
				double rad = 0.005;
				double hgt = 0.01;
				double scl = 0.005; // how far is dragger from the surface

				int cp;
				for (int i=0; i<cp_num; i++) if (cp_numbers[i]==sel_vrt) cp=i;
				glTranslatef(m_pDisplaced[sel_vrt].x+scl*m_pNormalArray[sel_vrt].x+controlPointDisplacementX[cp]+pivot.x, m_pDisplaced[sel_vrt].y+scl*m_pNormalArray[sel_vrt].y+controlPointDisplacementY[cp]+pivot.y, m_pDisplaced[sel_vrt].z+scl*m_pNormalArray[sel_vrt].z+controlPointDisplacementZ[cp]+pivot.z);
				// NEW
				/*
				if (sel_cnt==1) glTranslatef(m_pDisplaced[vrt1].x+scl*m_pNormalArray[vrt1].x+pivot.x, m_pDisplaced[vrt1].y+scl*m_pNormalArray[vrt1].y+pivot.y, m_pDisplaced[vrt1].z+scl*m_pNormalArray[vrt1].z+pivot.z);
				else if (sel_cnt==2) glTranslatef(m_pDisplaced[vrt2].x+scl*m_pNormalArray[vrt2].x+pivot.x, m_pDisplaced[vrt2].y+scl*m_pNormalArray[vrt2].y+pivot.y, m_pDisplaced[vrt2].z+scl*m_pNormalArray[vrt2].z+pivot.z);
				else if (sel_cnt==3) glTranslatef(m_pDisplaced[vrt3].x+scl*m_pNormalArray[vrt3].x+pivot.x, m_pDisplaced[vrt3].y+scl*m_pNormalArray[vrt3].y+pivot.y, m_pDisplaced[vrt3].z+scl*m_pNormalArray[vrt3].z+pivot.z);
				*/

				GLUquadric* q = gluNewQuadric();
				gluQuadricNormals(q, GLU_SMOOTH);

				glPushMatrix(); // Z-axis
				glPushName(-4);
				glColor3f(0,0,0.5);
				gluCylinder(q,rad/2,rad/2,hgt,100,100);
				glTranslatef(0,0,hgt);
				glutSolidCone(rad, rad, 100, 100);
				glPopName();
				glPopMatrix();

				glPushMatrix(); // Y-axis
				glPushName(-3);
				glColor3f(0,0.5,0);
				glRotatef(-90,1,0,0);
				gluCylinder(q,rad/2,rad/2,hgt,100,100);
				glTranslatef(0,0,hgt);
				glutSolidCone(rad, rad, 100, 100);
				glPopName();
				glPopMatrix();

				glPushMatrix(); // X-axis
				glPushName(-2);
				glColor3f(0.5,0,0);
				glRotatef(90,0,1,0);
				gluCylinder(q,rad/2,rad/2,hgt,100,100);
				glTranslatef(0,0,hgt);
				glutSolidCone(rad, rad, 100, 100);
				glPopName();
				glPopMatrix();

				gluDeleteQuadric(q);
			glPopMatrix();

		}

}


void G308_Geometry::setPivot(G308_Point pvt) {

	pivot.x=pvt.x;
	pivot.y=pvt.y;
	pivot.z=pvt.z;
}



void G308_Geometry::isOnClick(GLfloat winX, GLfloat winY){
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winZ = 0;

	//printf(" ( %f %f ) ", winX, winY);
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)winX;
	winY = (float)viewport[3] - (float)winY;
	winZ = selectBuf[1]/4294967295.0;

	printf(" (( %f )) ", winZ);

	GLdouble worldX, worldY, worldZ;

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
	//printf(" ( %f %f %f ) ", worldX, worldY, worldZ);

	//if (sel_vrt>)
	GLfloat distance, min_distance;
	// number of the closest vertex
	int cnt=1;
	// distance between the pressed point and the first vertex of selected triangle
	min_distance = (m_pVertexArray[m_pTriangles[sel_vrt].v1].x-worldX)*(m_pVertexArray[m_pTriangles[sel_vrt].v1].x-worldX) +
		(m_pVertexArray[m_pTriangles[sel_vrt].v1].y-worldY)*(m_pVertexArray[m_pTriangles[sel_vrt].v1].y-worldY) +
		(m_pVertexArray[m_pTriangles[sel_vrt].v1].z-worldZ)*(m_pVertexArray[m_pTriangles[sel_vrt].v1].z-worldZ);
	// distance to the second vertex
	distance = (m_pVertexArray[m_pTriangles[sel_vrt].v2].x-worldX)*(m_pVertexArray[m_pTriangles[sel_vrt].v2].x-worldX) +
		(m_pVertexArray[m_pTriangles[sel_vrt].v2].y-worldY)*(m_pVertexArray[m_pTriangles[sel_vrt].v2].y-worldY) +
		(m_pVertexArray[m_pTriangles[sel_vrt].v2].z-worldZ)*(m_pVertexArray[m_pTriangles[sel_vrt].v2].z-worldZ);
	if (distance>min_distance) {
		min_distance = distance;
		cnt=2;
	}
	// third vertex of a triangle
	distance = (m_pVertexArray[m_pTriangles[sel_vrt].v3].x-worldX)*(m_pVertexArray[m_pTriangles[sel_vrt].v3].x-worldX) +
		(m_pVertexArray[m_pTriangles[sel_vrt].v3].y-worldY)*(m_pVertexArray[m_pTriangles[sel_vrt].v3].y-worldY) +
		(m_pVertexArray[m_pTriangles[sel_vrt].v3].z-worldZ)*(m_pVertexArray[m_pTriangles[sel_vrt].v3].z-worldZ);
	if (distance>min_distance) cnt=3;
	sel_cnt = cnt;

	int num_vrt;
	if (sel_cnt==1) num_vrt=m_pTriangles[sel_vrt].v1;
	else if (sel_cnt==2) num_vrt=m_pTriangles[sel_vrt].v2;
	else if (sel_cnt==3) num_vrt=m_pTriangles[sel_vrt].v3;
	printf(" {{ %d }} ", num_vrt);

}



void G308_Geometry::startPicking() {

	GLint viewport[4];
	float ratio;

	glSelectBuffer(BUFSIZE,selectBuf);
	glGetIntegerv(GL_VIEWPORT,viewport);
	glRenderMode(GL_SELECT);
	glInitNames();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//printf(" %d %d \n", cursorX, viewport[3]-cursorY);
	//printf(" %d %d %d %d \n", viewport[0], viewport[1], viewport[2], viewport[3]);
	gluPickMatrix(cursorX,viewport[3]-cursorY,1.0f,1.0f,viewport);
	ratio = (viewport[2]+0.0) / viewport[3];
	//printf(" %f ", ratio);
	gluPerspective(20,ratio,0.1,1000);
	glMatrixMode(GL_MODELVIEW);
}


void G308_Geometry::processHits2 (GLint hits, GLuint buffer[], int sw) {
	GLint i, j, numberOfNames;
	GLuint names, *ptr, minZ,*ptrNames;


	ptr = (GLuint *) buffer;

	printf("\n");
	for (int i = 0; i<10; i++) {
		printf(" %u \n", buffer[i]);
	}

	minZ = 0xffffffff;
	for (i = 0; i < hits; i++) {
		names = *ptr;
		ptr++;
		if (*ptr < minZ) {
			numberOfNames = names;
			minZ = *ptr;
			ptrNames = ptr+2;
		}
		ptr += names+2;
	}

	if (numberOfNames > 0) {
		printf ("You picked snowman  ");
		ptr = ptrNames;
		//printf (" { %d , %d } ", (ptr[0] + 1), ptr[0] > -1);
		if ( ptr[0] == -2 || ptr[0] == -3 || ptr[0] == -4 ) sel_axis=ptr[0]; // axis selected
		else {
			sel_vrt=ptr[0];
			isOnClick(cursorX, cursorY);
		}
		//sel_cnt
		for (j = 0; j < numberOfNames; j++,ptr++) {
			printf ("%d ", *ptr);
		}
	}
	else {
		printf("You didn't click a snowman!");
	}

}

void G308_Geometry::stopPicking() {

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	hits = glRenderMode(GL_RENDER);
	//printf(" Back to render, %d hits ", hits);
	if (hits != 0){
		processHits2(hits,selectBuf,0);
	}
	else sel_vrt=-1; // clear clicks
}

void G308_Geometry::dislaceVertex(GLfloat disX, GLfloat disY, GLfloat disZ) {

	int cp=-1;
	for (int i=0; i<cp_num; i++) if (cp_numbers[i]==sel_vrt) cp=i;
	if (cp!=-1) {
		int startpt = 16134; // 16134 - last eye vertex
		if (sel_axis==-2) {
			controlPointDisplacementX[cp]+= disX*0.0005;
			rbfX.UpdateFunctionValues(controlPointDisplacementX);
			for (unsigned int i = startpt; i < numVert; i++) vertexSet[6*(i-EYES)] = m_pVertexArray[i].x + rbfX.interpolate(m_pVertexArray[i].x, m_pVertexArray[i].y, m_pVertexArray[i].z, i);
			//vertexSet[6*sel_vrt] += disX*0.0005;
		}
		else if (sel_axis==-3) {
			controlPointDisplacementY[cp]-= disY*0.0005;
			rbfY.UpdateFunctionValues(controlPointDisplacementY);
			for (unsigned int i = startpt; i < numVert; i++) vertexSet[6*(i-EYES)+1] = m_pVertexArray[i].y + rbfY.interpolate(m_pVertexArray[i].x, m_pVertexArray[i].y, m_pVertexArray[i].z, i);
			//vertexSet[6*sel_vrt+1] -= disY*0.0005;
		}
		else if (sel_axis==-4) {
			controlPointDisplacementZ[cp]-= disZ*0.0005;
			rbfZ.UpdateFunctionValues(controlPointDisplacementZ);
			for (unsigned int i = startpt; i < numVert; i++) vertexSet[6*(i-EYES)+2] = m_pVertexArray[i].z + rbfZ.interpolate(m_pVertexArray[i].x, m_pVertexArray[i].y, m_pVertexArray[i].z, i);
			//vertexSet[6*sel_vrt+2] -= disX*0.0005;
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(m_nNumPoint-EYES)*6 , vertexSet, GL_STATIC_DRAW);
	}

}

/*
void G308_Geometry::interpolateDisp() {




	for (unsigned int i = 0; i < numVert; i++) // 16134
	{
		vertexSet[6*i] = m_pVertexArray[i].x + rbfX.interpolate(m_pVertexArray[i].x, m_pVertexArray[i].y, m_pVertexArray[i].z, i);
		vertexSet[6*i+1] = m_pVertexArray[i].y + rbfY.interpolate(m_pVertexArray[i].x, m_pVertexArray[i].y, m_pVertexArray[i].z, i);
		vertexSet[6*i+2] = m_pVertexArray[i].z + rbfZ.interpolate(m_pVertexArray[i].x, m_pVertexArray[i].y, m_pVertexArray[i].z, i);
	}

	//CalcNormals();

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(m_nNumPoint-EYES)*6 , vertexSet, GL_STATIC_DRAW);

}
*/

void G308_Geometry::toggleGeo() {
	rbfX.toggleGeodesic();
	rbfY.toggleGeodesic();
	rbfZ.toggleGeodesic();
};



