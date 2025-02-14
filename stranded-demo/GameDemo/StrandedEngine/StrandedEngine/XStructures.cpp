/*
   Game Name:  Stranded
      Author:  Allen Sherrod
     Chapter:  Chapter 16
*/


#include "XStructures.h"


HRESULT CD3DAllocate::CreateFrame(LPCSTR name, LPD3DXFRAME *outFrame)
{
   stD3DFrameEx *frame = new stD3DFrameEx;
   ZeroMemory(frame, sizeof(stD3DFrameEx));

   *outFrame = NULL;
   frame->Name = NULL;

   if(name)
      {	
         int len = strlen(name) + 1;

         frame->Name = new char[len];
         memcpy(frame->Name, name, len * sizeof(char));
      }

   frame->pMeshContainer = NULL;
   frame->pFrameSibling = NULL;
   frame->pFrameFirstChild = NULL;
   D3DXMatrixIdentity(&frame->TransformationMatrix);
   D3DXMatrixIdentity(&frame->finalMatrix);

   *outFrame = frame;

   return S_OK;
}


HRESULT CD3DAllocate::CreateMeshContainer(THIS_ LPCSTR name,
   CONST D3DXMESHDATA *mesh, CONST D3DXMATERIAL *mats,
   CONST D3DXEFFECTINSTANCE *effects, unsigned long numMats,
   CONST unsigned long *indices, LPD3DXSKININFO skin,
   LPD3DXMESHCONTAINER* outContainer)
{    
   stD3DContainerEx *meshCon = new stD3DContainerEx;
   ZeroMemory(meshCon, sizeof(stD3DContainerEx));

   *outContainer = NULL;
   meshCon->Name = NULL;

   if(name)
      {
         int len = strlen(name) + 1;

         meshCon->Name = new char[len];
         memcpy(meshCon->Name, name, len * sizeof(char));
      }

   if(mesh->Type != D3DXMESHTYPE_MESH)
      {
         // D3DXMESHTYPE_PMESH are progressive meshes.
         // D3DXMESHTYPE_PATCHMESH are patch meshes.
         // In this code we only handle normal meshes.
         DestroyMeshContainer(meshCon);
         return E_FAIL;
      }

   meshCon->MeshData.Type = D3DXMESHTYPE_MESH;

   // Copy indices.
   unsigned long numFaces = mesh->pMesh->GetNumFaces();
   meshCon->pAdjacency = new unsigned long[numFaces * 3];
   memcpy(meshCon->pAdjacency, indices, sizeof(unsigned long) * numFaces * 3);

   // Get D3D Device.
   LPDIRECT3DDEVICE9 d3dDevice = NULL;
   mesh->pMesh->GetDevice(&d3dDevice);

   // Get mesh's declaration.
   D3DVERTEXELEMENT9 elements[MAX_FVF_DECL_SIZE];
   if(FAILED(mesh->pMesh->GetDeclaration(elements)))
      return E_FAIL;

   // Clone mesh to get copy of mesh.
   mesh->pMesh->CloneMesh(D3DXMESH_MANAGED, elements, d3dDevice,
                          &meshCon->MeshData.pMesh);

   // Allocate materials and textures.
   meshCon->NumMaterials = max(numMats, 1);
   meshCon->mat = new D3DMATERIAL9[meshCon->NumMaterials];
   meshCon->textures  = new LPDIRECT3DTEXTURE9[meshCon->NumMaterials];
   ZeroMemory(meshCon->mat, sizeof(D3DMATERIAL9) * meshCon->NumMaterials);
   ZeroMemory(meshCon->textures, sizeof(LPDIRECT3DTEXTURE9) * meshCon->NumMaterials);

   if(numMats > 0)
      {
         // If any materials get them and any textures.
         for(unsigned long i = 0; i < numMats; ++i)
            {
               meshCon->textures[i] = NULL;	
               meshCon->mat[i] = mats[i].MatD3D;

               if(mats[i].pTextureFilename)
                  {
                     if(FAILED(D3DXCreateTextureFromFile(d3dDevice, mats[i].pTextureFilename,
                        &meshCon->textures[i]))) meshCon->textures[i] = NULL;
                  }
            }
      }
	else    
      {
         // Set to default material and null texture.
         ZeroMemory(&meshCon->mat[0], sizeof(D3DMATERIAL9));

         meshCon->mat[0].Diffuse.r = 0.5f;
         meshCon->mat[0].Diffuse.g = 0.5f;
         meshCon->mat[0].Diffuse.b = 0.5f;
         meshCon->mat[0].Specular = meshCon->mat[0].Diffuse;
         meshCon->textures[0] = NULL;
      }


   // Nullify.
   meshCon->pSkinInfo = NULL;
   meshCon->boneMatrices = NULL;

	if(skin)
	   {
	      // If skin mesh allocate and get bones.
         meshCon->pSkinInfo = skin;
         skin->AddRef();

         unsigned int numBones = skin->GetNumBones();
         meshCon->boneMatrices = new D3DXMATRIX*[numBones];
	   }

   // Release reference count.
   if(d3dDevice) d3dDevice->Release(); d3dDevice = NULL;

   *outContainer = meshCon;    

   return S_OK;
}


HRESULT CD3DAllocate::DestroyFrame(LPD3DXFRAME inFrame) 
{
   stD3DFrameEx *frame = (stD3DFrameEx*)inFrame;

   if(frame->Name) delete[] frame->Name;
   frame->Name = NULL;

   if(frame) delete frame;
   frame = NULL;

   return S_OK;
}


HRESULT CD3DAllocate::DestroyMeshContainer(LPD3DXMESHCONTAINER container)
{
   stD3DContainerEx* meshCon = (stD3DContainerEx*)container;

   if(meshCon->Name) delete[] meshCon->Name;
   meshCon->Name = NULL;

   if(meshCon->mat) delete[] meshCon->mat;
   meshCon->mat = NULL;

   if(meshCon->textures)
      {
         for(unsigned int i = 0; i < meshCon->NumMaterials; i++)
            {
               if(meshCon->textures[i])
                  meshCon->textures[i]->Release();

               meshCon->textures[i] = NULL;
            }
      }

   if(meshCon->textures) delete[] meshCon->textures;
   meshCon->textures = NULL;

   if(meshCon->pAdjacency) delete[] meshCon->pAdjacency;
   meshCon->pAdjacency = NULL;

   if(meshCon->boneMatrices) delete[] meshCon->boneMatrices;
   meshCon->boneMatrices = NULL;

   if(meshCon->originalMesh) meshCon->originalMesh->Release();
   meshCon->originalMesh = NULL;

   if(meshCon->MeshData.pMesh) meshCon->MeshData.pMesh->Release();
   meshCon->MeshData.pMesh = NULL;

   if(meshCon) delete meshCon;
   meshCon = NULL;

   return S_OK;
}