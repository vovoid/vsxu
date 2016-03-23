/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/



#include "vsx_param.h"
#include <module/vsx_module.h>
#include "metaballs.h"
#include "marching_cubes.h"

#ifdef VSXU_MAC_XCODE
#include <syslog.h>
#include <unistd.h>
#endif

//=============================================================================
CMetaballs::CMetaballs()
{
	m_fLevel    = 100.0f;
	m_nNumBalls = 12;

	m_nGridSize = 0;

	m_nMaxOpenVoxels = 8;
	m_pOpenVoxels    = new int[m_nMaxOpenVoxels*3];
   
	m_nNumOpenVoxels    = 0;
	m_pfGridEnergy      = 0;
	m_pnGridPointStatus = 0;
	m_pnGridVoxelStatus = 0;

	m_nNumVertices = 0;
	m_nNumIndices  = 0;
	m_nFacePart = 0;
//	m_pVertices    = 0;
//	m_pIndices     = 0;

//	srand(timeGetTime());

	for( int i = 0; i < m_nNumBalls; i++ )
	{
		m_Balls[i].p[0] = 0;
		m_Balls[i].p[1] = 0;
		m_Balls[i].p[2] = 0;
		m_Balls[i].v[0] = 0;
		m_Balls[i].v[1] = 0;
		m_Balls[i].v[2] = 0;
		m_Balls[i].a[0] = ((float)rand()/RAND_MAX*2-1)/2;
		m_Balls[i].a[1] = ((float)rand()/RAND_MAX*2-1)/2;
		m_Balls[i].a[2] = ((float)rand()/RAND_MAX*2-1)/2;
		m_Balls[i].t = float(rand())/RAND_MAX;
		m_Balls[i].m = 1;
	}
}

CMetaballs::~CMetaballs() {
	delete[] m_pfGridEnergy;
	delete[] m_pnGridPointStatus;
	delete[] m_pnGridVoxelStatus;
  delete[] m_pOpenVoxels;
}
//=============================================================================
void CMetaballs::Update(float dt)
{
	for( int i = 0; i < m_nNumBalls; i++ )
	{
		m_Balls[i].p[0] += dt*m_Balls[i].v[0];
		m_Balls[i].p[1] += dt*m_Balls[i].v[1];
		m_Balls[i].p[2] += dt*m_Balls[i].v[2];

		m_Balls[i].t -= dt;
		if( m_Balls[i].t < 0 )
		{
			// When is the next time to act?
			m_Balls[i].t = float(rand())/RAND_MAX;

			// Use a new attraction point
			m_Balls[i].a[0] = (float(rand())/RAND_MAX*2-1)/2;
			m_Balls[i].a[1] = (float(rand())/RAND_MAX*2-1)/2;
			m_Balls[i].a[2] = (float(rand())/RAND_MAX*2-1)/2;
		}

		// Accelerate towards the attraction point
		float x = m_Balls[i].a[0] - m_Balls[i].p[0];
		float y = m_Balls[i].a[1] - m_Balls[i].p[1];
		float z = m_Balls[i].a[2] - m_Balls[i].p[2];
		float fDist = 1/sqrtf(x*x + y*y + z*z);

		x *= fDist;
		y *= fDist;
		z *= fDist;
		
		m_Balls[i].v[0] += 0.1f*x*dt;
		m_Balls[i].v[1] += 0.1f*y*dt;
		m_Balls[i].v[2] += 0.1f*z*dt;

		fDist = m_Balls[i].v[0]*m_Balls[i].v[0] + 
		        m_Balls[i].v[1]*m_Balls[i].v[1] + 
		        m_Balls[i].v[2]*m_Balls[i].v[2];
		if( fDist > 0.040f )
		{
			fDist = 1/sqrtf(fDist);
			m_Balls[i].v[0] = 0.20f*m_Balls[i].v[0]*fDist;
			m_Balls[i].v[1] = 0.20f*m_Balls[i].v[1]*fDist;
			m_Balls[i].v[2] = 0.20f*m_Balls[i].v[2]*fDist;
		}

		if( m_Balls[i].p[0] < -1+m_fVoxelSize ) 
		{
			m_Balls[i].p[0] = -1+m_fVoxelSize;
			m_Balls[i].v[0] = 0;
		}
		if( m_Balls[i].p[0] >  1-m_fVoxelSize ) 
		{
			m_Balls[i].p[0] =  1-m_fVoxelSize;
			m_Balls[i].v[0] = 0;
		}
		if( m_Balls[i].p[1] < -1+m_fVoxelSize ) 
		{
			m_Balls[i].p[1] = -1+m_fVoxelSize;
			m_Balls[i].v[1] = 0;
		}
		if( m_Balls[i].p[1] >  1-m_fVoxelSize ) 
		{
			m_Balls[i].p[1] =  1-m_fVoxelSize;
			m_Balls[i].v[1] = 0;
		}
		if( m_Balls[i].p[2] < -1+m_fVoxelSize ) 
		{
			m_Balls[i].p[2] = -1+m_fVoxelSize;
			m_Balls[i].v[2] = 0;
		}
		if( m_Balls[i].p[2] >  1-m_fVoxelSize ) 
		{
			m_Balls[i].p[2] =  1-m_fVoxelSize;
			m_Balls[i].v[2] = 0;
		}
	}
}

//=============================================================================
void CMetaballs::Render()
{
	int nCase = 0,x,y,z;
	bool bComputed;
	

	// Lock the vertex and index buffer
	m_nNumIndices = 0;
	m_nFacePart = 0;
	m_nNumVertices = 0;
	m_pVertices.reset_used(0);
	vertices->reset_used(0);
  vertex_normals->reset_used(0);
  vertex_tex_coords->reset_used(0);
  faces->reset_used(0);
		// Clear status grids
	memset(m_pnGridPointStatus, 0, sizeof(char)*(m_nGridSize+1)*(m_nGridSize+1)*(m_nGridSize+1));
	memset(m_pnGridVoxelStatus, 0, sizeof(char)*m_nGridSize*m_nGridSize*m_nGridSize);
	
	for( int i = 0; i < m_nNumBalls; i++ )
	{
//    printf("ball: %d\n",i);
		x = ConvertWorldCoordinateToGridPoint(m_Balls[i].p[0]);
		y = ConvertWorldCoordinateToGridPoint(m_Balls[i].p[1]);
		z = ConvertWorldCoordinateToGridPoint(m_Balls[i].p[2]);
//    printf("ball_a: %d\n",i);
		
		// Work our way out from the center of the ball until the surface is
		// reached. If the voxel at the surface is already computed then this
		// ball share surface with a previous ball.
		bComputed = false;
		while(1)
		{
			if( IsGridVoxelComputed(x,y,z) )
			{
				bComputed = true;
				break;
			}

			nCase = ComputeGridVoxel(x,y,z);
			

			if( nCase < 255 )
				break;

			z--;
		}

		if( bComputed )
			continue;

//    printf("ball_c: %d\n",i);

		// Compute all voxels on the surface by computing neighbouring voxels
		// if the surface goes into them.
		AddNeighborsToList(nCase,x,y,z);
//    printf("ball_d: %d\n",i);

		while( m_nNumOpenVoxels )
		{
//      printf("ball_e: %d %d\n",i,m_nNumOpenVoxels);
      
			m_nNumOpenVoxels--;
			x = m_pOpenVoxels[m_nNumOpenVoxels*3];
			y = m_pOpenVoxels[m_nNumOpenVoxels*3 + 1];
			z = m_pOpenVoxels[m_nNumOpenVoxels*3 + 2];

			nCase = ComputeGridVoxel(x,y,z);

			AddNeighborsToList(nCase,x,y,z);
		}
	}

	// Unlock buffers
//	pGfx->UnlockVertexBuffer();
//	pGfx->UnlockIndexBuffer();

	// Render the last triangles
//	IDirect3DDevice8 *pDev = pGfx->GetD3DDevice();
//	pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_nNumVertices, 
//		                                           0, m_nNumIndices/3);
}

//=============================================================================
void CMetaballs::AddNeighborsToList(int nCase, int x, int y, int z)
{
	if( CMarchingCubes::m_CubeNeighbors[nCase] & (1<<0) )
		AddNeighbor(x+1, y, z);

	if( CMarchingCubes::m_CubeNeighbors[nCase] & (1<<1) )
		AddNeighbor(x-1, y, z);

	if( CMarchingCubes::m_CubeNeighbors[nCase] & (1<<2) )
		AddNeighbor(x, y+1, z);

	if( CMarchingCubes::m_CubeNeighbors[nCase] & (1<<3) )
		AddNeighbor(x, y-1, z);

	if( CMarchingCubes::m_CubeNeighbors[nCase] & (1<<4) )
		AddNeighbor(x, y, z+1);

	if( CMarchingCubes::m_CubeNeighbors[nCase] & (1<<5) )
		AddNeighbor(x, y, z-1);
}

//=============================================================================
void CMetaballs::AddNeighbor(int x, int y, int z)
{
	if( IsGridVoxelComputed(x,y,z) || IsGridVoxelInList(x,y,z) )
		return;

	// Make sure the array is large enough
	if( m_nMaxOpenVoxels == m_nNumOpenVoxels )
	{
		m_nMaxOpenVoxels *= 2;
		//printf("metaballs allocating more memory\n");
		int *pTmp = new int[m_nMaxOpenVoxels*3];
		memcpy(pTmp, m_pOpenVoxels, m_nNumOpenVoxels*3*sizeof(int));
		delete[] m_pOpenVoxels;
		m_pOpenVoxels = pTmp;
	}

	m_pOpenVoxels[m_nNumOpenVoxels*3] = x;
	m_pOpenVoxels[m_nNumOpenVoxels*3+1] = y;
	m_pOpenVoxels[m_nNumOpenVoxels*3+2] = z;

	SetGridVoxelInList(x,y,z);

	m_nNumOpenVoxels++;
}

//=============================================================================
float CMetaballs::ComputeEnergy(float x, float y, float z)
{
	float fEnergy = 0;
	float fSqDist;

	for( int i = 0; i < m_nNumBalls; i++ )
	{
		// The formula for the energy is 
		// 
		//   e += mass/distance^2 

		fSqDist = (m_Balls[i].p[0] - x)*(m_Balls[i].p[0] - x) +
		          (m_Balls[i].p[1] - y)*(m_Balls[i].p[1] - y) +
		          (m_Balls[i].p[2] - z)*(m_Balls[i].p[2] - z);

		if( fSqDist < 0.0001f ) fSqDist = 0.0001f;

		fEnergy += m_Balls[i].m / fSqDist;
	}

	return fEnergy;
}

float xx, yy, zz;
float fSqDist;
float fsqr;
//=============================================================================
void inline CMetaballs::ComputeNormal(vsx_vector3<>* vv, vsx_vector3<>* vn, vsx_tex_coord2f* vt)
{
	

	for( int i = 0; i < m_nNumBalls; i ++ )
	{
		// To compute the normal we derive the energy formula and get
		//
		//   n += 2 * mass * vector / distance^4

		xx = vv->x - m_Balls[i].p[0];
		yy = vv->y - m_Balls[i].p[1];
		zz = vv->z - m_Balls[i].p[2];

		fSqDist = xx*xx + yy*yy + zz*zz;
		fSqDist *= fSqDist;
    fsqr = 2.0f * m_Balls[i].m / (fSqDist);
		vn->x +=  xx * fsqr;
		vn->y +=  yy * fsqr;
		vn->z +=  zz * fsqr;
	}
	
	vn->normalize();

//	D3DXVec3Normalize((D3DXVECTOR3*)pVertex->n, (D3DXVECTOR3*)pVertex->n);

	// Compute the sphere-map texture coordinate
	// Note: The normal used here should be transformed to camera space first
	// for correct result. In this application no transformation is needed 
	// since the camera is fixed. 
	vt->s = vn->x*0.5f + 0.5f;
	vt->t = -vn->y*0.5f + 0.5f;
}

//=============================================================================
float CMetaballs::ComputeGridPointEnergy(int x, int y, int z)
{
	if( IsGridPointComputed(x,y,z) )
		return m_pfGridEnergy[x +
		                      y*(m_nGridSize+1) +
		                      z*(m_nGridSize+1)*(m_nGridSize+1)];

	// The energy on the edges are always zero to make sure the isosurface is
	// always closed.
	if( x == 0 || y == 0 || z == 0 ||
	    x == m_nGridSize || y == m_nGridSize || z == m_nGridSize )
	{
		m_pfGridEnergy[x +
		               y*(m_nGridSize+1) +
		               z*(m_nGridSize+1)*(m_nGridSize+1)] = 0;
		SetGridPointComputed(x,y,z);
		return 0;
	}

	float fx = ConvertGridPointToWorldCoordinate(x);
	float fy = ConvertGridPointToWorldCoordinate(y);
	float fz = ConvertGridPointToWorldCoordinate(z);

	m_pfGridEnergy[x +
	               y*(m_nGridSize+1) +
	               z*(m_nGridSize+1)*(m_nGridSize+1)] = ComputeEnergy(fx,fy,fz);

	SetGridPointComputed(x,y,z);

	return m_pfGridEnergy[x +
  	                      y*(m_nGridSize+1) +
	                      z*(m_nGridSize+1)*(m_nGridSize+1)];
}

//=============================================================================
int CMetaballs::ComputeGridVoxel(int x, int y, int z)
{
	float b[8];
	b[0] = ComputeGridPointEnergy(x  , y  , z  );
	b[1] = ComputeGridPointEnergy(x+1, y  , z  );
	b[2] = ComputeGridPointEnergy(x+1, y  , z+1);
	b[3] = ComputeGridPointEnergy(x  , y  , z+1);
	b[4] = ComputeGridPointEnergy(x  , y+1, z  );
	b[5] = ComputeGridPointEnergy(x+1, y+1, z  );
	b[6] = ComputeGridPointEnergy(x+1, y+1, z+1);
	b[7] = ComputeGridPointEnergy(x  , y+1, z+1);

	float fx,fy,fz;
	
/*	float fx = ConvertGridPointToWorldCoordinate(x) + m_fVoxelSize/2;
	float fy = ConvertGridPointToWorldCoordinate(y) + m_fVoxelSize/2;
	float fz = ConvertGridPointToWorldCoordinate(z) + m_fVoxelSize/2;*/
	

	int c = 0;
	c |= b[0] > m_fLevel ? (1<<0) : 0;
	c |= b[1] > m_fLevel ? (1<<1) : 0;
	c |= b[2] > m_fLevel ? (1<<2) : 0;
	c |= b[3] > m_fLevel ? (1<<3) : 0;
	c |= b[4] > m_fLevel ? (1<<4) : 0;
	c |= b[5] > m_fLevel ? (1<<5) : 0;
	c |= b[6] > m_fLevel ? (1<<6) : 0;
	c |= b[7] > m_fLevel ? (1<<7) : 0;
//	syslog(LOG_ERR,"ComputeGridVoxel c = %d %d",c,__LINE__);


	// Compute vertices from marching pyramid case
	fx = ConvertGridPointToWorldCoordinate(x);
	fy = ConvertGridPointToWorldCoordinate(y);
	fz = ConvertGridPointToWorldCoordinate(z);
//	syslog(LOG_ERR,"ComputeGridVoxel fx=%f   fy=%f   fz=%f %d",fx,fy,fz,__LINE__);

	int i = 0;
	unsigned short EdgeIndices[12];
	memset(EdgeIndices, 0xFF, 12*sizeof(unsigned short));

	while(1)
	{
//    printf("inner\n");
	//	usleep(1000);
		int nEdge =	CMarchingCubes::m_CubeTriangles[c][i];
	//	syslog(LOG_ERR,"ComputeGridVoxel(%d) c = %d   i = %d   nEdge=%d",__LINE__,c,i,nEdge);

		if( nEdge == -1 )
			break;
		if( EdgeIndices[nEdge] == 0xFFFF )
		{
			EdgeIndices[nEdge] = m_nNumVertices;
			

			// Optimization: It's possible that the non-interior edges
			// have been computed already in neighbouring voxels

			// Compute the vertex by interpolating between the two points
			int nIndex0 = CMarchingCubes::m_CubeEdges[nEdge][0];
			int nIndex1 = CMarchingCubes::m_CubeEdges[nEdge][1];

			float t = (m_fLevel - b[nIndex0])/(b[nIndex1] - b[nIndex0]);


			(*vertices)[m_nNumVertices].x =
				CMarchingCubes::m_CubeVertices[nIndex0][0]*(1-t) +
			    CMarchingCubes::m_CubeVertices[nIndex1][0]*t;
			(*vertices)[m_nNumVertices].y =
				CMarchingCubes::m_CubeVertices[nIndex0][1]*(1-t) +
				CMarchingCubes::m_CubeVertices[nIndex1][1]*t;
			(*vertices)[m_nNumVertices].z =
				CMarchingCubes::m_CubeVertices[nIndex0][2]*(1-t) +
				CMarchingCubes::m_CubeVertices[nIndex1][2]*t;
			(*vertices)[m_nNumVertices].x = fx +
				(*vertices)[m_nNumVertices].x*m_fVoxelSize;
			(*vertices)[m_nNumVertices].y = fy +
				(*vertices)[m_nNumVertices].y*m_fVoxelSize;
			(*vertices)[m_nNumVertices].z = fz +
				(*vertices)[m_nNumVertices].z*m_fVoxelSize;

			// Compute the normal at the vertex
//			ComputeNormal(&m_pVertices[m_nNumVertices]);
			ComputeNormal(&(*vertices)[m_nNumVertices],&(*vertex_normals)[m_nNumVertices],&(*vertex_tex_coords)[m_nNumVertices]);

			m_nNumVertices++;
//			return 0;
		}

		// Add the edge's vertex index to the index list
		
    if (m_nFacePart == 0) {
      (*faces)[m_nNumIndices].a = EdgeIndices[nEdge];
    } else
    if (m_nFacePart == 1) {
      (*faces)[m_nNumIndices].b = EdgeIndices[nEdge];
    } else
    if (m_nFacePart == 2) {
      (*faces)[m_nNumIndices].c = EdgeIndices[nEdge];
    }

    ++m_nFacePart;
    if (m_nFacePart == 3) {
      m_nFacePart = 0;
      ++m_nNumIndices;
    }
		++i;
    //if (i == 2000) break;
	}

	SetGridVoxelComputed(x,y,z);


	return c;
}

//=============================================================================
float CMetaballs::ConvertGridPointToWorldCoordinate(int x)
{
	return float(x)*m_fVoxelSize - 1.0f;
}

//=============================================================================
int CMetaballs::ConvertWorldCoordinateToGridPoint(float x)
{
	return int((x + 1.0f)/m_fVoxelSize + 0.5f);
}

//=============================================================================
void CMetaballs::SetGridSize(int nSize)
{
	m_fVoxelSize = 2/float(nSize);
	m_nGridSize  = nSize;

	m_pfGridEnergy      = new float[(nSize+1)*(nSize+1)*(nSize+1)];
	m_pnGridPointStatus = new char[(nSize+1)*(nSize+1)*(nSize+1)];
	m_pnGridVoxelStatus = new char[nSize*nSize*nSize];
}

//=============================================================================
inline bool CMetaballs::IsGridPointComputed(int x, int y, int z)
{
	if( m_pnGridPointStatus[x +
	                        y*(m_nGridSize+1) +
	                        z*(m_nGridSize+1)*(m_nGridSize+1)] == 1 )
		return true;
	else
		return false;
}

//=============================================================================
inline bool CMetaballs::IsGridVoxelComputed(int x, int y, int z)
{
	if( m_pnGridVoxelStatus[x +
	                        y*m_nGridSize +
	                        z*m_nGridSize*m_nGridSize] == 1 )
		return true;
	else
		return false;
}

//=============================================================================
inline bool CMetaballs::IsGridVoxelInList(int x, int y, int z)
{
	if( m_pnGridVoxelStatus[x +
	                        y*m_nGridSize +
	                        z*m_nGridSize*m_nGridSize] == 2 )
		return true;
	else
		return false;
}

//=============================================================================
inline void CMetaballs::SetGridPointComputed(int x, int y, int z)
{
	m_pnGridPointStatus[x +
	                    y*(m_nGridSize+1) +
	                    z*(m_nGridSize+1)*(m_nGridSize+1)] = 1;
}

//=============================================================================
inline void CMetaballs::SetGridVoxelComputed(int x, int y, int z)
{
	m_pnGridVoxelStatus[x +
	                    y*m_nGridSize +
	                    z*m_nGridSize*m_nGridSize] = 1;
}

//=============================================================================
inline void CMetaballs::SetGridVoxelInList(int x, int y, int z)
{
	m_pnGridVoxelStatus[x +
	                    y*m_nGridSize +
	                    z*m_nGridSize*m_nGridSize] = 2;
}
