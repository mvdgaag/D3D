#pragma once
#include <d3d11_1.h>


// predeclarations
class ComputeShader;


class IndirectLightingRenderer
{
public:
	IndirectLightingRenderer() {}
	~IndirectLightingRenderer() { CleanUp(); }
	
	void Init();
	void CleanUp();
	void Render(ID3D11DeviceContext* inDeviceContext);

private:
	ComputeShader* mShader = nullptr;
};