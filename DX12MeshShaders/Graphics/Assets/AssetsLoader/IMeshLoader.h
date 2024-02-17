#pragma once

#include "../../../Includes.h"

namespace Assets
{
	enum class VertexFormat : uint32_t
	{
		UNDEFINED = 0U,
		POSITION = 1U,
		TEXCOORD = 2U,
		NORMAL = 4U,
		TANGENT_BINORMAL = 8U
	};

	inline VertexFormat operator|(VertexFormat leftValue, VertexFormat rightValue)
	{
		using VertexFormatType = std::underlying_type_t<VertexFormat>;

		return static_cast<VertexFormat>(static_cast<VertexFormatType>(leftValue) | static_cast<VertexFormatType>(rightValue));
	}

	inline VertexFormat& operator|=(VertexFormat& leftValue, VertexFormat rightValue)
	{
		leftValue = leftValue | rightValue;

		return leftValue;
	}

	inline VertexFormat operator&(VertexFormat leftValue, VertexFormat rightValue)
	{
		using VertexFormatType = std::underlying_type_t<VertexFormat>;

		return static_cast<VertexFormat>(static_cast<VertexFormatType>(leftValue) & static_cast<VertexFormatType>(rightValue));
	}

	inline VertexFormat& operator&=(VertexFormat& leftValue, VertexFormat rightValue)
	{
		leftValue = leftValue & rightValue;

		return leftValue;
	}

	inline VertexFormat operator~(VertexFormat leftValue)
	{
		using VertexFormatType = std::underlying_type_t<VertexFormat>;

		return static_cast<VertexFormat>(~static_cast<VertexFormatType>(leftValue));
	}

	inline VertexFormat& operator~(VertexFormat& leftValue)
	{
		using VertexFormatType = std::underlying_type_t<VertexFormat>;

		leftValue = static_cast<VertexFormat>(~static_cast<VertexFormatType>(leftValue));

		return leftValue;
	}

	inline size_t VertexStride(VertexFormat format) noexcept
	{
		size_t vertexStride = 0;

		if ((format & VertexFormat::POSITION) == VertexFormat::POSITION)
			vertexStride += 12;

		if ((format & VertexFormat::NORMAL) == VertexFormat::NORMAL)
			vertexStride += 12;

		if ((format & VertexFormat::TANGENT_BINORMAL) == VertexFormat::TANGENT_BINORMAL)
			vertexStride += 24;

		if ((format & VertexFormat::TEXCOORD) == VertexFormat::TEXCOORD)
			vertexStride += 8;

		return vertexStride;
	}

	class IMeshLoader
	{
	public:
		virtual ~IMeshLoader() = 0 {};

		virtual void Load(std::filesystem::path fileName, bool optimizeMesh, std::vector<uint8_t>& vertexBufferData,
			std::vector<uint32_t>& indexBufferData, VertexFormat& vertexFormat) = 0;
	};
}
