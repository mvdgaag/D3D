#pragma once


// predeclarations
class ComputeShader;
class RenderTarget;
class Texture;


class LightComposeRenderer
{
public:
	LightComposeRenderer() { mInitialized = false; }
	~LightComposeRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(Texture* inDirectDiffuse, Texture* inDirectSpecular, Texture* inIndirect, Texture* inReflections, RenderTarget* inTarget);

private:
	ComputeShader* mShader = nullptr;
	bool mInitialized = false;
};