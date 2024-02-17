#pragma once

#include "..\..\..\Includes.h"
#include "IMeshLoader.h"

namespace Assets
{
	class OBJLoader final : public IMeshLoader
	{
	public:
		OBJLoader();
		~OBJLoader();

		void Load(std::filesystem::path fileName, bool optimizeMesh, std::vector<uint8_t>& vertexBufferData,
			std::vector<uint32_t>& indexBufferData, VertexFormat& vertexFormat) override final;

	private:
		std::string GetToken(const std::string& objLine);
		float2 GetVector2(const std::string& objLine);
		float3 GetVector3(const std::string& objLine);

		VertexFormat GetFaceFormat(size_t texCoordCount, size_t normalCount);
		void GetTriangulatedFace(const std::string& objLine, VertexFormat vertexFormat, std::vector<size_t>& positionFace,
			std::vector<size_t>& normalFace, std::vector<size_t>& texCoordFace);

		void SimpleTriangulation(std::vector<size_t>& face);
		bool CheckChunkEquality(uint8_t* chunk0, uint8_t* chunk1, size_t chunkSize);
	};
}
