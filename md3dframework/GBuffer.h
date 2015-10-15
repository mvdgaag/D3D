#pragma once
#include <vector>


// predefinitions
class Texture;
class RenderTarget;
class DepthStencilTarget;


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
	RenderTarget* GetRenderTarget(GBufferType inType)	{ return mRenderTargets[inType]; }
	RenderTarget** GetRenderTargets()					{ return mRenderTargets; }
	DepthStencilTarget* GetDepthStencilTarget()			{ return mDepthStencilTarget; }
	Texture* GetTexture(GBufferType inType);

private:
	RenderTarget**			mRenderTargets = nullptr;
	DepthStencilTarget*		mDepthStencilTarget = nullptr;

	int mWidth;
	int mHeight;
	bool mInitialized = false;
};

