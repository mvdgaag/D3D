#include "PixelShader.h"
#include "RenderContext.h"
#include <string>
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include "FileUtil.h"
#include <assert.h>


void PixelShader::InitFromFile(std::string inFileName)
{
	assert(FileUtil::FileExists(inFileName.c_str()));

	CleanUp();

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	std::wstring filename = std::wstring(inFileName.begin(), inFileName.end());

#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pPSBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	D3DCall(D3DCompileFromFile(filename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", dwShaderFlags, 0, &pPSBlob, &pErrorBlob));
	if (pErrorBlob)
	{
		OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
		pErrorBlob->Release();
	}

	ID3D11ShaderReflection* pPixelShaderReflection = NULL;
	D3DCall(D3DReflect(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pPixelShaderReflection));

	D3D11_SHADER_DESC shader_desc;
	pPixelShaderReflection->GetDesc(&shader_desc);

	for (int i = 0; i < shader_desc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC resource_desc;
		pPixelShaderReflection->GetResourceBindingDesc(i, &resource_desc);

		// FETCH RESOURCES
		switch (resource_desc.Type)
		{
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
			mTextures[resource_desc.Name] = resource_desc.BindPoint;
			break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
			mSamplers[resource_desc.Name] = resource_desc.BindPoint;
			break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
			mConstantBuffers[resource_desc.Name] = resource_desc.BindPoint;
			break;
		default:
			break;
		}
	}

	D3DCall(theRenderContext.GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &mHandle));
	pPSBlob->Release();
}


void PixelShader::CleanUp()
{
	if (mHandle != NULL)
	{
		mHandle->Release();
		mHandle = NULL;
	}
}