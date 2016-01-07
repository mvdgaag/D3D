#include "MeshObject.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "Camera.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "TAARenderer.h"
#include <assert.h>


void MeshObject::Init(pMesh inMesh, pMaterial inMaterial)
{
	mMesh = inMesh;
	mAABB = mMesh->GetAABB();
	mMaterial = inMaterial;
	mTransform = float4x4();
}


void MeshObject::CleanUp()
{
	mMesh = nullptr;
	mMaterial = nullptr;
}