#include "OBJLoader.h"

Assets::OBJLoader::OBJLoader()
{

}

Assets::OBJLoader::~OBJLoader()
{

}

void Assets::OBJLoader::Load(std::filesystem::path fileName, bool optimizeMesh, std::vector<uint8_t>& vertexBufferData,
	std::vector<uint32_t>& indexBufferData, VertexFormat& vertexFormat)
{
	std::ifstream objFile(fileName, std::ios::in);

	std::vector<float3> positions;
	std::vector<float3> normals;
	std::vector<float2> texCoords;
	std::vector<size_t> positionFaces;
	std::vector<size_t> normalFaces;
	std::vector<size_t> texCoordFaces;
	std::vector<size_t> verticesPerFaces;

	std::string objLine;

	vertexFormat = VertexFormat::POSITION;

	while (std::getline(objFile, objLine))
	{
		std::string token = GetToken(objLine);

		if (token == "v")
			positions.push_back(GetVector3(objLine));
		else if (token == "vn")
			normals.push_back(GetVector3(objLine));
		else if (token == "vt")
			texCoords.push_back(GetVector2(objLine));
		else if (token == "f")
		{
			vertexFormat = GetFaceFormat(texCoords.size(), normals.size());

			std::vector<size_t> positionFace;
			std::vector<size_t> normalFace;
			std::vector<size_t> texCoordFace;

			GetTriangulatedFace(objLine, vertexFormat, positionFace, normalFace, texCoordFace);

			positionFaces.insert(positionFaces.end(), positionFace.begin(), positionFace.end());
			normalFaces.insert(normalFaces.end(), normalFace.begin(), normalFace.end());
			texCoordFaces.insert(texCoordFaces.end(), texCoordFace.begin(), texCoordFace.end());
		}
	}

	size_t stride = VertexStride(vertexFormat);

	vertexBufferData.resize(stride * positionFaces.size());
	indexBufferData.resize(positionFaces.size());

	size_t actualVertexIndex = 0;

	for (size_t vertexIndex = 0; vertexIndex < positionFaces.size(); vertexIndex++)
	{
		auto vertexChunk = &vertexBufferData[actualVertexIndex * stride];
		auto startAddress = reinterpret_cast<uint8_t*>(&positions[positionFaces[vertexIndex]]);
		auto endAddress = startAddress + sizeof(float3);
		
		std::copy(startAddress, endAddress, vertexChunk);
		size_t vertexChunkOffset = sizeof(float3);

		if ((vertexFormat & VertexFormat::NORMAL) == VertexFormat::NORMAL)
		{
			startAddress = reinterpret_cast<uint8_t*>(&normals[normalFaces[vertexIndex]]);
			endAddress = startAddress + sizeof(float3);

			std::copy(startAddress, endAddress, vertexChunk + vertexChunkOffset);
			vertexChunkOffset += sizeof(float3);
		}

		if ((vertexFormat & VertexFormat::TEXCOORD) == VertexFormat::TEXCOORD)
		{
			startAddress = reinterpret_cast<uint8_t*>(&texCoords[texCoordFaces[vertexIndex]]);
			endAddress = startAddress + sizeof(float2);

			std::copy(startAddress, endAddress, vertexChunk + vertexChunkOffset);
		}

		if (vertexIndex > 0 && optimizeMesh)
		{
			bool skip = false;

			for (size_t vertexPrevIndex = 0; vertexPrevIndex < actualVertexIndex; vertexPrevIndex++)
			{
				auto prevVertexChunk = &vertexBufferData[vertexPrevIndex * stride];

				if (CheckChunkEquality(vertexChunk, prevVertexChunk, stride))
				{
					indexBufferData[vertexIndex] = vertexPrevIndex;

					skip = true;

					break;
				}
			}

			if (skip)
				continue;
		}

		indexBufferData[vertexIndex] = vertexIndex;
		actualVertexIndex++;
	}

	vertexBufferData.resize(actualVertexIndex);
	vertexBufferData.shrink_to_fit();
	indexBufferData.shrink_to_fit();
}

std::string Assets::OBJLoader::GetToken(const std::string& objLine)
{
	std::stringstream objLineStream(objLine);
	std::string token;

	objLineStream >> token;

	return token;
}

float2 Assets::OBJLoader::GetVector2(const std::string& objLine)
{
	std::stringstream objLineStream(objLine);
	std::string token;
	float2 result{};

	objLineStream >> token >> result.x >> result.y;

	return result;
}

float3 Assets::OBJLoader::GetVector3(const std::string& objLine)
{
	std::stringstream objLineStream(objLine);
	std::string token;
	float3 result{};

	objLineStream >> token >> result.x >> result.y >> result.z;

	return result;
}

Assets::VertexFormat Assets::OBJLoader::GetFaceFormat(size_t texCoordCount, size_t normalCount)
{
	VertexFormat faceFormat = VertexFormat::POSITION;

	if (normalCount > 0)
		faceFormat |= VertexFormat::NORMAL;

	if (texCoordCount > 0)
		faceFormat |= VertexFormat::TEXCOORD;

	return faceFormat;
}

void Assets::OBJLoader::GetTriangulatedFace(const std::string& objLine, VertexFormat vertexFormat, std::vector<size_t>& positionFace,
	std::vector<size_t>& normalFace, std::vector<size_t>& texCoordFace)
{
	size_t verticesPerFace = 0;

	std::stringstream objLineStream(objLine);

	std::string token;

	objLineStream >> token;

	while (!objLineStream.eof())
	{
		size_t attributeIndex;

		objLineStream >> attributeIndex;

		if (objLineStream.bad() || objLineStream.fail())
			break;

		positionFace.push_back(--attributeIndex);

		verticesPerFace++;

		if ((vertexFormat & ~VertexFormat::POSITION) == VertexFormat::UNDEFINED)
			continue;

		objLineStream.ignore((vertexFormat == (VertexFormat::POSITION | VertexFormat::NORMAL)) ? 2 : 1);
		objLineStream >> attributeIndex;

		if (vertexFormat == (VertexFormat::POSITION | VertexFormat::NORMAL))
			normalFace.push_back(--attributeIndex);
		else
			texCoordFace.push_back(--attributeIndex);

		if (vertexFormat == (VertexFormat::POSITION | VertexFormat::NORMAL | VertexFormat::TEXCOORD))
		{
			objLineStream.ignore(1);

			objLineStream >> attributeIndex;
			normalFace.push_back(--attributeIndex);
		}
	}

	if (verticesPerFace <= 2)
	{
		positionFace.clear();
		normalFace.clear();
		texCoordFace.clear();
	}
	else if (verticesPerFace > 3)
	{
		SimpleTriangulation(positionFace);

		if ((vertexFormat & VertexFormat::NORMAL) == VertexFormat::NORMAL)
			SimpleTriangulation(normalFace);

		if ((vertexFormat & VertexFormat::TEXCOORD) == VertexFormat::TEXCOORD)
			SimpleTriangulation(texCoordFace);
	}
}

void Assets::OBJLoader::SimpleTriangulation(std::vector<size_t>& face)
{
	size_t trianglesNumber = face.size() - 2;

	std::vector<size_t> triangulatedFace(trianglesNumber * 3);

	for (size_t triangleIndex = 0; triangleIndex < trianglesNumber; triangleIndex++)
	{
		triangulatedFace[triangleIndex * 3] = face[0];
		triangulatedFace[triangleIndex * 3 + 1] = face[triangleIndex + 1];
		triangulatedFace[triangleIndex * 3 + 2] = face[triangleIndex + 2];
	}

	face = triangulatedFace;
}

bool Assets::OBJLoader::CheckChunkEquality(uint8_t* chunk0, uint8_t* chunk1, size_t chunkSize)
{
	for (size_t byteIndex = 0; byteIndex < chunkSize; byteIndex++)
		if (chunk0[byteIndex] != chunk1[byteIndex])
			return false;

	return true;
}
