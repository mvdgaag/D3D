#pragma once


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

