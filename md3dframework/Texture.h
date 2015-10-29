#pragma once
#include "BaseResource.h"
#include "GAAGCommon.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct D3D11_TEXTURE2D_DESC;

PREDEFINE(Texture, pTexture);

class Texture : public BaseResource
{
	friend class RenderContext;
	friend PREDEFINE(RenderTarget, pRenderTarget);
	friend PREDEFINE(DepthStencilTarget, pDepthStencilTarget);

public:
	Texture() : BaseResource() {};
	~Texture() { CleanUp(); }

	int GetWidth();			
	int GetHeight();		
	int GetMipLevels();		

	virtual void Init(int inWidth, int inHeight, int inMipLevels, unsigned int inFormat, unsigned int inBindFlags = 8); // 8 = D3D11_BIND_SHADER_RESOURCE
	virtual void Init(ID3D11Texture2D* inTexture);
	virtual void InitFromFile(std::string inFileName);
	
	virtual void CleanUp() override;
	virtual ResourceType GetResourceType() const override { return ResourceType::TEXTURE; };

protected:
	ID3D11Texture2D*			mTexture = nullptr;
	ID3D11ShaderResourceView*	mShaderResourceView = nullptr;
	D3D11_TEXTURE2D_DESC*		mDesc = nullptr;

private:
	Texture(Texture const&) = delete;
	void operator=(Texture const&) = delete;
};
