#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct D3D11_TEXTURE2D_DESC;

REGISTERCLASS(Texture);


typedef enum Format
{
	FORMAT_UNKNOWN = 0,
	FORMAT_R32G32B32A32_TYPELESS = 1,
	FORMAT_R32G32B32A32_FLOAT = 2,
	FORMAT_R32G32B32A32_UINT = 3,
	FORMAT_R32G32B32A32_SINT = 4,
	FORMAT_R32G32B32_TYPELESS = 5,
	FORMAT_R32G32B32_FLOAT = 6,
	FORMAT_R32G32B32_UINT = 7,
	FORMAT_R32G32B32_SINT = 8,
	FORMAT_R16G16B16A16_TYPELESS = 9,
	FORMAT_R16G16B16A16_FLOAT = 10,
	FORMAT_R16G16B16A16_UNORM = 11,
	FORMAT_R16G16B16A16_UINT = 12,
	FORMAT_R16G16B16A16_SNORM = 13,
	FORMAT_R16G16B16A16_SINT = 14,
	FORMAT_R32G32_TYPELESS = 15,
	FORMAT_R32G32_FLOAT = 16,
	FORMAT_R32G32_UINT = 17,
	FORMAT_R32G32_SINT = 18,
	FORMAT_R32G8X24_TYPELESS = 19,
	FORMAT_D32_FLOAT_S8X24_UINT = 20,
	FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	FORMAT_R10G10B10A2_TYPELESS = 23,
	FORMAT_R10G10B10A2_UNORM = 24,
	FORMAT_R10G10B10A2_UINT = 25,
	FORMAT_R11G11B10_FLOAT = 26,
	FORMAT_R8G8B8A8_TYPELESS = 27,
	FORMAT_R8G8B8A8_UNORM = 28,
	FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	FORMAT_R8G8B8A8_UINT = 30,
	FORMAT_R8G8B8A8_SNORM = 31,
	FORMAT_R8G8B8A8_SINT = 32,
	FORMAT_R16G16_TYPELESS = 33,
	FORMAT_R16G16_FLOAT = 34,
	FORMAT_R16G16_UNORM = 35,
	FORMAT_R16G16_UINT = 36,
	FORMAT_R16G16_SNORM = 37,
	FORMAT_R16G16_SINT = 38,
	FORMAT_R32_TYPELESS = 39,
	FORMAT_D32_FLOAT = 40,
	FORMAT_R32_FLOAT = 41,
	FORMAT_R32_UINT = 42,
	FORMAT_R32_SINT = 43,
	FORMAT_R24G8_TYPELESS = 44,
	FORMAT_D24_UNORM_S8_UINT = 45,
	FORMAT_R24_UNORM_X8_TYPELESS = 46,
	FORMAT_X24_TYPELESS_G8_UINT = 47,
	FORMAT_R8G8_TYPELESS = 48,
	FORMAT_R8G8_UNORM = 49,
	FORMAT_R8G8_UINT = 50,
	FORMAT_R8G8_SNORM = 51,
	FORMAT_R8G8_SINT = 52,
	FORMAT_R16_TYPELESS = 53,
	FORMAT_R16_FLOAT = 54,
	FORMAT_D16_UNORM = 55,
	FORMAT_R16_UNORM = 56,
	FORMAT_R16_UINT = 57,
	FORMAT_R16_SNORM = 58,
	FORMAT_R16_SINT = 59,
	FORMAT_R8_TYPELESS = 60,
	FORMAT_R8_UNORM = 61,
	FORMAT_R8_UINT = 62,
	FORMAT_R8_SNORM = 63,
	FORMAT_R8_SINT = 64,
	FORMAT_A8_UNORM = 65,
	FORMAT_R1_UNORM = 66,
	FORMAT_R9G9B9E5_SHAREDEXP = 67,
	FORMAT_R8G8_B8G8_UNORM = 68,
	FORMAT_G8R8_G8B8_UNORM = 69,
	FORMAT_BC1_TYPELESS = 70,
	FORMAT_BC1_UNORM = 71,
	FORMAT_BC1_UNORM_SRGB = 72,
	FORMAT_BC2_TYPELESS = 73,
	FORMAT_BC2_UNORM = 74,
	FORMAT_BC2_UNORM_SRGB = 75,
	FORMAT_BC3_TYPELESS = 76,
	FORMAT_BC3_UNORM = 77,
	FORMAT_BC3_UNORM_SRGB = 78,
	FORMAT_BC4_TYPELESS = 79,
	FORMAT_BC4_UNORM = 80,
	FORMAT_BC4_SNORM = 81,
	FORMAT_BC5_TYPELESS = 82,
	FORMAT_BC5_UNORM = 83,
	FORMAT_BC5_SNORM = 84,
	FORMAT_B5G6R5_UNORM = 85,
	FORMAT_B5G5R5A1_UNORM = 86,
	FORMAT_B8G8R8A8_UNORM = 87,
	FORMAT_B8G8R8X8_UNORM = 88,
	FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	FORMAT_B8G8R8A8_TYPELESS = 90,
	FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	FORMAT_B8G8R8X8_TYPELESS = 92,
	FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	FORMAT_BC6H_TYPELESS = 94,
	FORMAT_BC6H_UF16 = 95,
	FORMAT_BC6H_SF16 = 96,
	FORMAT_BC7_TYPELESS = 97,
	FORMAT_BC7_UNORM = 98,
	FORMAT_BC7_UNORM_SRGB = 99,
	FORMAT_AYUV = 100,
	FORMAT_Y410 = 101,
	FORMAT_Y416 = 102,
	FORMAT_NV12 = 103,
	FORMAT_P010 = 104,
	FORMAT_P016 = 105,
	FORMAT_420_OPAQUE = 106,
	FORMAT_YUY2 = 107,
	FORMAT_Y210 = 108,
	FORMAT_Y216 = 109,
	FORMAT_NV11 = 110,
	FORMAT_AI44 = 111,
	FORMAT_IA44 = 112,
	FORMAT_P8 = 113,
	FORMAT_A8P8 = 114,
	FORMAT_B4G4R4A4_UNORM = 115,
	FORMAT_P208 = 130,
	FORMAT_V208 = 131,
	FORMAT_V408 = 132,
	FORMAT_FORCE_UINT = 0xffffffff
};
inline Format operator|(Format a, Format b) { return static_cast<Format>(static_cast<int>(a) | static_cast<int>(b)); }


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
	BIND_VIDEO_ENCODER = 0x400L,

	// custom combined flags
	BIND_COMPUTE_TARGET = ((int)BIND_SHADER_RESOURCE | (int)BIND_RENDER_TARGET | (int)BIND_UNORDERED_ACCESS)
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
	virtual void			Init(ID3D11Texture2D* inTexture);
	virtual void			InitFromFile(std::string inFileName);

	int2					Texture::GetDimensions()	{ return int2(mWidth, mHeight); }
	int						Texture::GetMipLevels()		{ return mMipLevels; }
	Format					GetFormat()					{ return mFormat; }
	BindFlag				GetBindFlags()				{ return mBindFlags; }
	CPUAccessFlag			GetCPUAccessFlags()			{ return mCPUAccessFlags; }
	float4					GetPixel(int2 inPixelCoordinate);
	
	void					CleanUp() override;
	ResourceType			GetResourceType() const override { return ResourceType::TEXTURE; };

protected:
	ID3D11Texture2D*			mTexture = nullptr;
	ID3D11ShaderResourceView*	mShaderResourceView = nullptr;
	ID3D11ShaderResourceView**	mMipShaderResourceViews = nullptr;
	D3D11_TEXTURE2D_DESC*		mDesc = nullptr;
	unsigned int				mWidth;
	unsigned int				mHeight;
	unsigned int				mMipLevels;
	Format						mFormat;
	BindFlag					mBindFlags;
	CPUAccessFlag				mCPUAccessFlags;

private:
	Texture() : BaseResource() {};
	~Texture() { CleanUp(); }

	void SetMipShaderResourceViews();

	Texture(Texture const&) = delete;
	void operator=(Texture const&) = delete;
};
