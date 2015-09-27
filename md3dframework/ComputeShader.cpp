#include "ComputeShader.h"
#include <d3dcompiler.h>
#include "main.h"
#include "FrameWork.h"


void ComputeShader::InitFromFile(std::string inFileName)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pPSBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	D3DCall(D3DCompileFromFile(filename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS", "cs_5_0", dwShaderFlags, 0, &pPSBlob, &pErrorBlob));
	if (pErrorBlob)
	{
		OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
		pErrorBlob->Release();
	}
	D3DCall(theFrameWork.GetDevice()->CreateComputeShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &mHandle));
	pPSBlob->Release();
}


void ComputeShader::CleanUp()
{
	if (mHandle != NULL)
	{
		mHandle->Release();
		mHandle = NULL;
	}
}