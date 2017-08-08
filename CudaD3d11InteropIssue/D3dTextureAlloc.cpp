#include "D3dTextureAlloc.h"
#include <cmath>
#include <algorithm>
#include <memory>

using namespace std;

struct free_deleter
{
	void operator()(void* x) { free(x); }
};

CDxTextureAllocator::CDxTextureAllocator()
	: pDevice(nullptr)
{
}

CDxTextureAllocator::~CDxTextureAllocator()
{
	this->FreeHeldTextures();

	if (this->pDevice != nullptr)
	{
		this->pDevice->Release();
	}
}

void CDxTextureAllocator::Initialize()
{
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&this->pDevice,
		nullptr,
		nullptr);
}

bool CDxTextureAllocator::AllocateAndKeep(int size, int count)
{
	// we allocate a RGBX32-surface, calculate approx. width and height for the given size (in bytes)
	int pixelNumber = size / 4;
	int width = (std::max)((int)(0.5 + sqrt(pixelNumber)), 1);
	int height = width;

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, sizeof(desc), 0);
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = /*D3D11_USAGE_DEFAULT;*/ D3D11_USAGE_IMMUTABLE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	unique_ptr<int, free_deleter>  buffer((int*)malloc(width * 4 * height));

	D3D11_SUBRESOURCE_DATA subResData;
	memset(&subResData, sizeof(subResData), 0);
	subResData.pSysMem = buffer.get();
	subResData.SysMemPitch = width * 4;

	ID3D11Texture2D* pD3dTexture = nullptr;

	for (int i = 0; i < count; ++i)
	{
		HRESULT hr = this->pDevice->CreateTexture2D(&desc, &subResData, &pD3dTexture);
		if (FAILED(hr))
		{
			return false;
		}

		this->textures.push_back(pD3dTexture);
	}

	return true;
}

void CDxTextureAllocator::FreeHeldTextures()
{
	for (auto texture : this->textures)
	{
		texture->Release();
	}

	this->textures.clear();
}