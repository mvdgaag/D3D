#include "ShadowRenderer.h"


void ShadowRenderer::Render()
{

}


void ShadowRenderer::Init()
{
	assert(mInitialized == false);
	mInitialized = true;
}


void ShadowRenderer::CleanUp()
{
	mInitialized = false;
}
