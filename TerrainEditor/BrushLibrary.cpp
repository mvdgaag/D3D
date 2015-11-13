#include "BrushLibrary.h"
#include "ComputeShader.h"

void BrushLibrary::Init()
{
	CleanUp();

	pComputeShader basic_brush_shader = MAKE_NEW(ComputeShader);
	basic_brush_shader->InitFromFile("Shaders/BrushBasicShader.hlsl");
	pBrush basic_brush = MAKE_NEW(Brush);
	basic_brush->Init(basic_brush_shader);
	AddBrush("Basic", basic_brush);

	pComputeShader noise_brush_shader = MAKE_NEW(ComputeShader);
	noise_brush_shader->InitFromFile("Shaders/BrushNoiseShader.hlsl");
	pBrush noise_brush = MAKE_NEW(Brush);
	noise_brush->Init(noise_brush_shader);
	AddBrush("Noise", noise_brush);

	pComputeShader smooth_brush_shader = MAKE_NEW(ComputeShader);
	smooth_brush_shader->InitFromFile("Shaders/BrushSmoothShader.hlsl");
	pBrush smooth_brush = MAKE_NEW(Brush);
	smooth_brush->Init(smooth_brush_shader);
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
