#include <cuda_runtime.h>
#include <Windows.h>
#include <iostream>
#include "string"
#include "D3dTextureAlloc.h"

using namespace std;

int main(int argc, char** argv)
{
	bool firstD3D11ThenCuda = true;
	if (argc > 1)
	{
		int i = stoi(*(argv + 1));
		if (i > 0)
		{
			firstD3D11ThenCuda = false;
		}
	}

	const int TEXTURE_SIZE = 5 * 1024 * 1024;

	CoInitialize(NULL);
	size_t free, total;
	cudaMemGetInfo(&free, &total);
	std::cout << "GPU-Memory: Total " << total << " bytes , Free " << free << " bytes" << endl;

	size_t allocSize = free * 9 / 10;

	if (firstD3D11ThenCuda == true)
	{
		CDxTextureAllocator textureAllocator;
		textureAllocator.Initialize();
		int textureCnt = allocSize / TEXTURE_SIZE + 1;
		bool b = textureAllocator.AllocateAndKeep(TEXTURE_SIZE, textureCnt);
		if (b == true)
		{
			cout << "D3D11-allocation of " << TEXTURE_SIZE << "*" << textureCnt << "=" << TEXTURE_SIZE*(size_t)textureCnt << " bytes succeeded." << endl;
		}
		else
		{
			cout << "D3D11-allocation of " << TEXTURE_SIZE << "*" << textureCnt << "=" << TEXTURE_SIZE*(size_t)textureCnt << " bytes failed." << endl;
		}

		void* devPtr = nullptr;
		cudaError_t cudaStatus = cudaMalloc(&devPtr, allocSize);
		if (cudaStatus != cudaSuccess)
		{
			cout << "cudaMalloc (with " << allocSize << " bytes) failed : " << cudaGetErrorString(cudaStatus) << endl;
		}
		else
		{
			cout << "cudaMalloc (with " << allocSize << " bytes) succeeded." << endl;
		}

		if (devPtr!=nullptr)
		{
			cudaFree(devPtr);
		}
	}
	else
	{
		void* devPtr = nullptr;
		cudaError_t cudaStatus = cudaMalloc(&devPtr, allocSize);
		if (cudaStatus != cudaSuccess)
		{
			cout << "cudaMalloc (with " << allocSize << " bytes) failed : " << cudaGetErrorString(cudaStatus) << endl;
		}
		else
		{
			cout << "cudaMalloc (with " << allocSize << " bytes) succeeded." << endl;
		}

		CDxTextureAllocator textureAllocator;
		textureAllocator.Initialize();
		int textureCnt = allocSize / TEXTURE_SIZE + 1;
		bool b = textureAllocator.AllocateAndKeep(TEXTURE_SIZE, textureCnt);
		if (b == true)
		{
			cout << "D3D11-allocation of " << TEXTURE_SIZE << "*" << textureCnt << "=" << TEXTURE_SIZE*(size_t)textureCnt << " bytes succeeded." << endl;
		}
		else
		{
			cout << "D3D11-allocation of " << TEXTURE_SIZE << "*" << textureCnt << "=" << TEXTURE_SIZE*(size_t)textureCnt << " bytes failed." << endl;
		}

		if (devPtr != nullptr)
		{
			cudaFree(devPtr);
		}
	}

	return EXIT_SUCCESS;
}
