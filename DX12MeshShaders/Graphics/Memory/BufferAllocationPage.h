#pragma once

#include "..\..\Includes.h"
#include "..\..\CommonUtility.h"
#include "..\DirectX12Helper.h"

namespace Memory
{
	struct BufferAllocation
	{
	public:
		BufferAllocation() = default;
		~BufferAllocation() {};
		BufferAllocation(BufferAllocation&& source) noexcept;

		uint8_t* cpuAddress;
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
		uint64_t gpuPageOffset;
		size_t nonAlignedSizeInBytes;
		ID3D12Resource* bufferResource;
	};

	struct BufferAllocationPage
	{
	public:
		BufferAllocationPage(ID3D12Device* device, D3D12_HEAP_TYPE _heapType,
			D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE, uint64_t _pageSize = 2 * Graphics::_MB);
		~BufferAllocationPage();

		void Allocate(uint64_t _size, uint64_t alignment, BufferAllocation& allocation);
		bool HasSpace(uint64_t _size, uint64_t alignment) const noexcept;

	private:
		uint64_t pageSize;
		uint64_t offset;

		uint8_t* currentCPUAddress;
		D3D12_GPU_VIRTUAL_ADDRESS currentGPUAddress;

		D3D12_HEAP_TYPE heapType;

		ComPtr<ID3D12Resource> pageResource;
	};
}
