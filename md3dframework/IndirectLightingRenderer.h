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
	void Render();

private:
	ComputeShader* mShader = nullptr;
};