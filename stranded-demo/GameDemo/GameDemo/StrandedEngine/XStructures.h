/*
   Game Name:  Stranded
      Author:  Allen Sherrod
     Chapter:  Chapter 16
*/


#ifndef _X_ALLOCATE_
#define _X_ALLOCATE_

#include<d3d9.h>
#include<d3dx9.h>


struct stD3DFrameEx: public D3DXFRAME
{
   D3DXMATRIX finalMatrix;
};


struct stD3DContainerEx: public D3DXMESHCONTAINER
{
   LPDIRECT3DTEXTURE9 *textures;
   D3DMATERIAL9 *mat;
   LPD3DXMESH originalMesh;
   D3DXMATRIX **boneMatrices;
};


class CD3DAllocate : public ID3DXAllocateHierarchy
{
   public:	
      STDMETHOD(CreateFrame)(THIS_ LPCSTR name, LPD3DXFRAME *outFrame);

      STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR name, CONST D3DXMESHDATA *mesh, 
         CONST D3DXMATERIAL *mats, CONST D3DXEFFECTINSTANCE *effects,
         DWORD numMats, CONST DWORD *indices, LPD3DXSKININFO skin, 
         LPD3DXMESHCONTAINER *outContainer);

      STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME inFrame);
      STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER container);
};

#endif