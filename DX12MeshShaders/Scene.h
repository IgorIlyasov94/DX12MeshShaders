#pragma once

#include "Includes.h"
#include "Graphics/Memory/ResourceManager.h"

namespace Common
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnFrameRender(ID3D12GraphicsCommandList6* commandList);

	private:

		Memory::ResourceId meshlet;
	};
}
