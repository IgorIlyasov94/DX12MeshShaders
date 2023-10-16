#pragma once

#include "IResourceFactory.h"

namespace Memory
{
	class SamplerFactory : public IResourceFactory
	{
	public:
		SamplerFactory(ID3D12Device8* device, std::weak_ptr<DescriptorAllocator> descriptorAllocator);

		ResourceData CreateResource(ComPtr<ID3D12GraphicsCommandList> commandList, ResourceType type,
			const ResourceDesc& desc) override final;

	private:
		ID3D12Device8* _device;
		std::weak_ptr<DescriptorAllocator> _descriptorAllocator;
	};
}
