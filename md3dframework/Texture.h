#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct D3D11_TEXTURE2D_DESC;

PREDEFINE(Texture, pTexture);


typedef enum BindFlag
{
	BIND_VERTEX_BUFFER = 0x1L,
	BIND_INDEX_BUFFER = 0x2L,
	BIND_CONSTANT_BUFFER = 0x4L,
	BIND_SHADER_RESOURCE = 0x8L,
	BIND_STREAM_OUTPUT = 0x10L,
	BIND_RENDER_TARGET = 0x20L,
	BIND_DEPTH_STENCIL = 0x40L,
	BIND_UNORDERED_ACCESS = 0x80L,
	BIND_DECODER = 0x200L,
	BIND_VIDEO_ENCODER = 0x400L
} BindFlag;
inline BindFlag operator|(BindFlag a, BindFlag b) { return static_cast<BindFlag>(static_cast<int>(a) | static_cast<int>(b)); }


typedef enum CPUAccessFlag 
{
	CPU_ACCESS_DEFAULT = 0,
	CPU_ACCESS_WRITE = 0x10000L,
	CPU_ACCESS_READ = 0x20000L
} CPUAccessFlag;
inline CPUAccessFlag operator|(CPUAccessFlag a, CPUAccessFlag b) { return static_cast<CPUAccessFlag>(static_cast<int>(a) | static_cast<int>(b)); }


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
	int2 GetResolution();
	int GetMipLevels();		

	virtual void Init(
		int inWidth, 
		int inHeight, 
		int inMipLevels, 
		unsigned int inFormat, 
		unsigned int inUsage = 0, 
		BindFlag inBindFlag = BindFlag::BIND_SHADER_RESOURCE, 
		CPUAccessFlag inCPUAccessFlags = CPU_ACCESS_DEFAULT);
	virtual void Init(ID3D11Texture2D* inTexture);
	virtual void InitFromFile(std::string inFileName);

	float4 GetPixel(int2 inPixelCoordinate);
	
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
