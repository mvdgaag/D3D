#pragma once
#include <d3d11_1.h>


// predeclarations
class ComputeShader;
class RenderTarget;
class Texture;


class ReflectionRenderer
{
public:
	ReflectionRenderer() { mInitialized = false; }
	~ReflectionRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(Texture* inSource, RenderTarget* inTarget);

private:
	ComputeShader* mShader = nullptr;
	bool mInitialized = false;
};

