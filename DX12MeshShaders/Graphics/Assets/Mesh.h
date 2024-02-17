#pragma once

#include "../../Includes.h"
#include "AssetsLoader/IMeshLoader.h"
#include "../Memory/ResourceManager.h"

namespace Assets
{
	class Mesh final
	{
	public:
		Mesh(std::filesystem::path meshFileName, Memory::ResourceManager* resourceManager, bool optimizeMesh, bool generateMeshlets);
		~Mesh();

		void GenerateMeshlets();
		void ClearCPUSidedData();
		
		uint32_t GetMeshletNumber();

	private:
		Mesh() = delete;

		void CalculateBounds(const std::vector<uint8_t>& vertexBuffer, uint32_t vertexStride, float3& minPoint, float3& maxPoint);

		struct Meshlet
		{
			uint32_t vertexNumber;
			uint32_t vertexStartIndex;
			uint32_t triangleNumber;
			uint32_t triangleStartIndex;
		};

		Memory::ResourceId vertexBuffer;
		Memory::ResourceId uniqueIndexBuffer;
		Memory::ResourceId triangleBuffer;
		Memory::ResourceId meshletBuffer;
		
		std::vector<uint8_t> rawVertexBuffer;
		std::vector<uint32_t> rawIndexBuffer;

		VertexFormat vertexFormat;
		uint32_t meshletNumber;

		bool hasMeshlets;

		static constexpr int MAX_VERTICES_PER_MESHLET = 64;
		static constexpr int MAX_INDICES_PER_MESHLET = 84;
	};
}
