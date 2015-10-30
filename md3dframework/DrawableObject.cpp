#include "DrawableObject.h"
#include "RenderContext.h"
#include "Gaag.h"
#include "Camera.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "TAARenderer.h"
#include <assert.h>


void DrawableObject::Init(pMesh inMesh, pMaterial inMaterial)
{
	mMesh = inMesh;
	mMaterial = inMaterial;
	mTransform = DirectX::XMMatrixIdentity();
}


void DrawableObject::CleanUp()
{
	mMesh = nullptr;
	mMaterial = nullptr;
}