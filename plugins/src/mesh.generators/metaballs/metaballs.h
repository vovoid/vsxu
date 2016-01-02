/**
 Public domain
*/

#ifndef METABALLS_H
#define METABALLS_H

#define MAX_BALLS    32
#define MAX_VERTICES 3000
#define MAX_INDICES  3000

struct SBall
{
	float p[3];
	float v[3];
	float a[3];
	float t;
	float m;
};

//#define FVF_VERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)
/*struct SVertex
{
	float v[3];
	float n[3];
	float t[2];
};*/

class CMetaballs
{
public:
	CMetaballs();
	~CMetaballs();

	void  Compute();
	void  Update(float fDeltaTime);
	void  Render();

	void  SetGridSize(int nSize);

  vsx_ma_vector< vsx_vector3<> >* vertices;
  vsx_ma_vector< vsx_vector3<> >* vertex_normals;
  vsx_ma_vector<vsx_tex_coord2f>* vertex_tex_coords;
  vsx_ma_vector<vsx_face3>* faces;

protected:
	float ComputeEnergy(float x, float y, float z);
  void  ComputeNormal(vsx_vector3<>* vv, vsx_vector3<>* vn, vsx_tex_coord2f* vt);
	float ComputeGridPointEnergy(int x, int y, int z);
	int   ComputeGridVoxel(int x, int y, int z);

	bool  IsGridPointComputed(int x, int y, int z);
	bool  IsGridVoxelComputed(int x, int y, int z);
	bool  IsGridVoxelInList(int x, int y, int z);
	void  SetGridPointComputed(int x, int y, int z);
	void  SetGridVoxelComputed(int x, int y, int z);
	void  SetGridVoxelInList(int x, int y, int z);

	float ConvertGridPointToWorldCoordinate(int x);
	int   ConvertWorldCoordinateToGridPoint(float x);
	void  AddNeighborsToList(int nCase, int x, int y, int z);
	void  AddNeighbor(int x, int y, int z);

	float  m_fLevel;

int    m_nNumBalls;
	SBall  m_Balls[MAX_BALLS];

	int    m_nNumOpenVoxels;
	int    m_nMaxOpenVoxels;
	int   *m_pOpenVoxels;

	int    m_nGridSize;
	float  m_fVoxelSize;

	float *m_pfGridEnergy;
	char  *m_pnGridPointStatus;
	char  *m_pnGridVoxelStatus;

	int             m_nNumVertices;
	int             m_nNumIndices;
	int             m_nFacePart;
  vsx_nw_vector_nd< vsx_vector3<> > m_pVertices;
};

#endif
