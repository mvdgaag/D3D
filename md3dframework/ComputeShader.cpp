#include "ComputeShader.h"
#include "RenderContext.h"
#include <d3dcompiler.h>
#include <d3d11_1.h>


void ComputeShader::InitFromFile(std::string inFileName)
{
	CleanUp();

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
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
	D3DCall(theRenderContext.GetDevice()->CreateComputeShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &mHandle));
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