#pragma once
#include "GaagCommon.h"

REGISTERCLASS(ConstantBuffer);
REGISTERCLASS(PixelShader);
REGISTERCLASS(VertexShader);
REGISTERCLASS(DirectionalLight);
REGISTERCLASS(MeshObject);

class ShadowRenderer
{
public:
	ShadowRenderer() {}
	~ShadowRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(pDirectionalLight inLight, apMeshObject inShadowCasters);

private:
	ShadowRenderer(ShadowRenderer const&) = delete;
	void operator=(ShadowRenderer const&) = delete;
	bool mInitialized = false;

	struct ConstantDataEveryObject
	{
		float4x4 modelViewProjectionMatrix;
	};

	pConstantBuffer mConstantBufferEveryObject;
	pPixelShader mPixelShader;
	pVertexShader mVertexShader;
};

