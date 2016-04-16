/*
#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"

struct ID3D11Texture3D;
struct ID3D11ShaderResourceView;
struct D3D11_TEXTURE3D_DESC;

REGISTERCLASS(TextureCube);


class TextureCube : public BaseResource
{
	friend class ResourceFactory;
	friend class RenderContext;

	friend REGISTERCLASS(RenderTarget);
	friend REGISTERCLASS(DepthStencilTarget);

public:
	virtual void Init(
		int inWidth,
		int inHeight,
		int inMipLevels,
		Format inFormat,
		unsigned int inUsage = 0,
		BindFlag inBindFlag = BindFlag::BIND_SHADER_RESOURCE,
		CPUAccessFlag inCPUAccessFlags = CPU_ACCESS_DEFAULT);
	virtual void			Init(ID3D11Texture3D* inTexture);
	virtual void			InitFromFile(std::string inFileName);

	int2					GetDimensions()				{ return int2(mWidth, mHeight); }
	int						GetMipLevels()				{ return mMipLevels; }
	Format					GetFormat()					{ return mFormat; }
	BindFlag				GetBindFlags()				{ return mBindFlags; }
	CPUAccessFlag			GetCPUAccessFlags()			{ return mCPUAccessFlags; }

	void					CleanUp() override;
	ResourceType			GetResourceType() const override { return ResourceType::TEXTURE; };

protected:
	ID3D11Texture3D*			mTexture = nullptr;
	ID3D11ShaderResourceView*	mShaderResourceView = nullptr;
	ID3D11ShaderResourceView**	mMipShaderResourceViews = nullptr;
	D3D11_TEXTURE3D_DESC*		mDesc = nullptr;
	unsigned int				mWidth;
	unsigned int				mHeight;
	unsigned int				mMipLevels;
	Format						mFormat;
	BindFlag					mBindFlags;
	CPUAccessFlag				mCPUAccessFlags;

private:
	TextureCube() : BaseResource() {};
	~TextureCube() { CleanUp(); }

	void SetMipShaderResourceViews();

	TextureCube(TextureCube const&) = delete;
	void operator=(TextureCube const&) = delete;
};
*/
