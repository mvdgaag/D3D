#include "BrushLibrary.h"
#include "ComputeShader.h"

void BrushLibrary::Init()
{
	CleanUp();

	pComputeShader basic_brush_shader = theResourceFactory.LoadComputeShader("Shaders/BrushBasicShader.hlsl");
	pBrush basic_brush = MAKE_NEW(Brush);
	basic_brush->Init(basic_brush_shader, 10.0, 0.5, float4(1.0, 0.0, 0.0, 0.0), 0.5, false);
	AddBrush("Basic", basic_brush);

	pComputeShader noise_brush_shader = theResourceFactory.LoadComputeShader("Shaders/BrushNoiseShader.hlsl");
	pBrush noise_brush = MAKE_NEW(Brush);
	noise_brush->Init(noise_brush_shader);
	AddBrush("Noise", noise_brush);

	pComputeShader smooth_brush_shader = theResourceFactory.LoadComputeShader("Shaders/BrushSmoothShader.hlsl");
	pBrush smooth_brush = MAKE_NEW(Brush);
	smooth_brush->Init(smooth_brush_shader, 10.0, 0.5, float4(1.0), 0.1, true);
	AddBrush("Smooth", smooth_brush);
}


void BrushLibrary::CleanUp()
{
	mBrushes.clear();
}


pBrush BrushLibrary::GetBrush(std::string inName)
{
	if (mBrushes.find(inName) == mBrushes.end())
		return nullptr;
	else
		return mBrushes[inName];
}


void BrushLibrary::AddBrush(std::string inName, pBrush inBrush)
{
	assert(mBrushes.find(inName) == mBrushes.end());
	mBrushes[inName] = inBrush;
}


void BrushLibrary::RemoveBrush(std::string inName)
{
	mBrushes.erase(inName);
}
