/** License: Public domain */

#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

class CMarchingCubes
{
public:
	static void  BuildTables();

	static int  m_CubeEdges[12][2];
	static int  m_CubeTriangles[256][16];
	static int  m_CubeNeighbors[256];
	static float m_CubeVertices[8][3];
};

#endif
