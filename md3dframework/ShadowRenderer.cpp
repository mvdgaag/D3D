#include "ShadowRenderer.h"
#include "GaagFramework.h"
#include "Camera.h"
#include "DrawableObject.h"
#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"

void ShadowRenderer::Init()
{
	assert(mInitialized == false);
	
	mPixelShader = theResourceFactory.LoadPixelShader("../md3dFramework/Shaders/ShadowPixelShader.hlsl");
	mVertexShader = theResourceFactory.LoadVertexShader("../md3dFramework/Shaders/ShadowVertexShader.hlsl");
	mConstantBufferEveryObject = theResourceFactory.MakeConstantBuffer(sizeof(ShadowRenderer::ConstantDataEveryObject));

	mInitialized = true;
}


void ShadowRenderer::CleanUp()
{
	if (mInitialized == false)
		return;

	theResourceFactory.DestroyItem(mPixelShader);
	theResourceFactory.DestroyItem(mVertexShader);
	theResourceFactory.DestroyItem(mConstantBufferEveryObject);

	mPixelShader = nullptr;
	mVertexShader = nullptr;
	mConstantBufferEveryObject = nullptr;

	mInitialized = false;
}


void ShadowRenderer::Render(pDirectionalLight inLight, apDrawableObject inShadowCasters)
{
	assert(mInitialized);
	assert(inLight != nullptr);

	Camera& camera = *(Gaag.GetCamera());
	Frustum frustum = camera.ExtractFrustum();

	// make view-projection matrix
	float3 cam_pos = camera.GetPosition();
	float3 cam_dir = camera.GetForward();
	float3 target = float3(0.0);// cam_pos + 50.0f * cam_dir;
	float3 light_pos = target - 480.0f * inLight->GetDirection();
	float4x4 view = lookAt(light_pos, target, float3(0.0, 1.0, 0.0));
	float4x4 proj = ortho(-100.0f, 100.0f, -100.0f, 100.0f, -1000.0f, 1000.0f);
	float4x4& view_proj = inLight->GetShadowMatrix(); // set the light's shadowmatrix and render targets
	view_proj = proj * view;
	
	pRenderTarget shadow_map = inLight->GetShadowMap();
	pDepthStencilTarget depth_stencil_target = inLight->GetDepthStencilTarget();
	assert(shadow_map != nullptr);
	assert(depth_stencil_target != nullptr);

	// setup context
	theRenderContext.SetRasterizerState(FILL_SOLID, CULL_BACK, true, 0, 0.0f, 0.0f, true, false, false, false);
	theRenderContext.SetViewport(inLight->GetShadowMap()->GetDimensions(), 0.0f, 1.0f, int2(0, 0)); // depth for -1 to 1 for ortho
	theRenderContext.ClearRenderTarget(shadow_map, float4(0, 0, 0, 0));
	theRenderContext.ClearDepthStencil(depth_stencil_target, 1.0, 0);
	theRenderContext.SetRenderTargets(1, &shadow_map, depth_stencil_target);

	// draw all objects
	for each (pDrawableObject obj in inShadowCasters)
	{
		theRenderContext.SetMarker("Drawing Shadow Object");

		ConstantDataEveryObject constantData;
		constantData.modelViewProjectionMatrix = transpose(view_proj * obj->GetTransform());
		theRenderContext.UpdateSubResource(*mConstantBufferEveryObject, &constantData);
		theRenderContext.VSSetConstantBuffer(mConstantBufferEveryObject, 0);

		// set shaders
		// might be reset by custom shader, so need to be set for every object
		// TODO: make more efficient?
		theRenderContext.PSSetShader(mPixelShader);
		theRenderContext.VSSetShader(mVertexShader);

		obj->PrepareToDrawShadow();
		theRenderContext.DrawShadowMesh(*(obj->GetMesh()));
		obj->FinalizeAfterDrawShadow();

		// reset state
		theRenderContext.VSSetConstantBuffer(NULL, 0);
	}
	theRenderContext.Flush();

	// reset state
	pRenderTarget null_target = NULL;
	theRenderContext.SetRenderTargets(1, &null_target, NULL);
	theRenderContext.PSSetShader(NULL);
	theRenderContext.VSSetShader(NULL);
}