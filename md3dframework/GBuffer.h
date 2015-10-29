#pragma once
#include "GAAGCommon.h"
#include "RenderTarget.h"

PREDEFINE(GBuffer, pGBuffer);
PREDEFINE(Texture, pTexture);
PREDEFINE(DepthStencilTarget, pDepthStencilTarget);


class GBuffer
{
public:
	enum GBufferType
	{
		LINEAR_DEPTH,
		NORMAL,
		DIFFUSE,
		MATERIAL,
		MOTION_VECTORS,
		NUM_RENDER_TARGETS
	};

	GBuffer();
	~GBuffer() { CleanUp(); }

	void Init(int inWidth, int inHeight);
	void CleanUp();
	bool IsInitialized()								{ return mInitialized; }
	int GetWidth()										{ return mWidth; }
	int GetHeight()										{ return mHeight; }
	pRenderTarget GetRenderTarget(GBufferType inType)	{ return mRenderTargets[inType]; }
	pRenderTarget* GetRenderTargets()					{ return mRenderTargets; }
	pDepthStencilTarget GetDepthStencilTarget()			{ return mDepthStencilTarget; }
	pTexture GetTexture(GBufferType inType)				{ return mRenderTargets[inType]->GetTexture(); }

protected:
	pRenderTarget			mRenderTargets[NUM_RENDER_TARGETS];
	pDepthStencilTarget		mDepthStencilTarget = nullptr;

	int mWidth;
	int mHeight;
	bool mInitialized = false;

private:
	GBuffer(GBuffer const&) = delete;
	void operator=(GBuffer const&) = delete;
};

