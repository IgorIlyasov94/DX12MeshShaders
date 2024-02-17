#include "Mesh.h"
#include "AssetsLoader/OBJLoader.h"

Assets::Mesh::Mesh(std::filesystem::path meshFileName, Memory::ResourceManager* resourceManager, bool optimizeMesh, bool generateMeshlets)
	: vertexFormat(VertexFormat::UNDEFINED), hasMeshlets(false)
{
	std::unique_ptr<IMeshLoader> meshLoader = nullptr;

	if (meshFileName.extension() == ".obj" || meshFileName.extension() == ".OBJ")
		meshLoader = std::make_unique<OBJLoader>();
	else
		throw std::runtime_error("Assets::Mesh: Unsupported mesh format!");

	if (meshLoader != nullptr)
		meshLoader->Load(meshFileName, optimizeMesh, rawVertexBuffer, rawIndexBuffer, vertexFormat);

	auto stride = VertexStride(vertexFormat);

	float3 minPoint{};
	float3 maxPoint{};

	CalculateBounds(rawVertexBuffer, stride, minPoint, maxPoint);

	Memory::ResourceDesc desc{};
	desc.data = rawVertexBuffer.data();
	desc.dataSize = rawVertexBuffer.size();
	desc.dataStride = stride;
	desc.numElements = (stride > 0u) ? rawVertexBuffer.size() / stride : 0u;

	vertexBuffer = resourceManager->CreateResource(Memory::ResourceType::BUFFER, desc);

	desc.data = rawIndexBuffer.data();
	desc.dataSize = rawIndexBuffer.size() * sizeof(uint32_t);
	desc.bufferFormat = DXGI_FORMAT_R32_UINT;
	desc.numElements = rawIndexBuffer.size();

	uniqueIndexBuffer = resourceManager->CreateResource(Memory::ResourceType::BUFFER, desc);

	if (generateMeshlets)
		GenerateMeshlets();
}

Assets::Mesh::~Mesh()
{

}

void Assets::Mesh::GenerateMeshlets()
{
	if (hasMeshlets || rawVertexBuffer.empty() || rawIndexBuffer.empty())
		return;



	hasMeshlets = true;
}

void Assets::Mesh::ClearCPUSidedData()
{
	rawVertexBuffer.clear();
	rawIndexBuffer.clear();
}

uint32_t Assets::Mesh::GetMeshletNumber()
{
	return meshletNumber;
}

void Assets::Mesh::CalculateBounds(const std::vector<uint8_t>& vertexBuffer, uint32_t vertexStride, float3& minPoint, float3& maxPoint)
{
	minPoint = float3(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
	maxPoint = float3(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

	for (uint32_t vertexOffset = 0u; vertexOffset < vertexBuffer.size(); vertexOffset += vertexStride)
	{
		float3 position = *reinterpret_cast<const float3*>(vertexBuffer.data() + vertexOffset);

		if (position.x < minPoint.x)
			minPoint.x = position.x;
		if (position.y < minPoint.y)
			minPoint.y = position.y;
		if (position.z < minPoint.z)
			minPoint.z = position.z;

		if (position.x > maxPoint.x)
			maxPoint.x = position.x;
		if (position.x > maxPoint.x)
			maxPoint.x = position.x;
		if (position.x > maxPoint.x)
			maxPoint.x = position.x;
	}
}
