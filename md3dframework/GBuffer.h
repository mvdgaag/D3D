#pragma once
#include <d3d11_1.h>
#include <vector>


// predefinitions
class Texture;
class RenderTarget;


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
	bool IsInitialized() { return mInitialized; }
	RenderTarget* GetRenderTarget(GBufferType inType)	{ return mRenderTargets[inType]; }
	std::vector<ID3D11RenderTargetView*> GetRenderTargetViewArray();
	ID3D11DepthStencilView* GetDepthStencilView() { return mDepthStencilView; }

private:
	RenderTarget**			mRenderTargets = nullptr;
	ID3D11Texture2D*		mDepthStencil = nullptr;
	ID3D11DepthStencilView*	mDepthStencilView = nullptr;

	int mWidth;
	int mHeight;
	bool mInitialized = false;
};

