#include "stdafx.h"
#include "VertexFormat.h"

#include <materialsystem/imesh.h>

using namespace TF2Vulkan;

static constexpr VertexFormat::ElementType s_ElementTypesUncompressed[VERTEX_ELEMENT_NUMELEMENTS] =
{
	{ VERTEX_ELEMENT_POSITION, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_NORMAL, DataFormat::UInt, 3, sizeof(float) },
	{ VERTEX_ELEMENT_COLOR, DataFormat::UNorm, 4, sizeof(uint8_t) },
	{ VERTEX_ELEMENT_SPECULAR, DataFormat::UNorm, 4, sizeof(uint8_t) },
	{ VERTEX_ELEMENT_TANGENT_S, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TANGENT_T, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_WRINKLE, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_BONEINDEX, DataFormat::UInt, 4, sizeof(uint8_t) },
	{ VERTEX_ELEMENT_BONEWEIGHTS1, DataFormat::SInt, 1, sizeof(float) },
	{ VERTEX_ELEMENT_BONEWEIGHTS2, DataFormat::SInt, 2, sizeof(float) },
	{ VERTEX_ELEMENT_BONEWEIGHTS3, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_BONEWEIGHTS4, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_USERDATA1, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_USERDATA2, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_USERDATA3, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_USERDATA4, DataFormat::SFloat, 4, sizeof(float) },

	{ VERTEX_ELEMENT_TEXCOORD1D_0, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_1, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_2, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_3, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_4, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_5, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_6, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_7, DataFormat::SFloat, 1, sizeof(float) },

	{ VERTEX_ELEMENT_TEXCOORD2D_0, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_1, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_2, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_3, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_4, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_5, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_6, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_7, DataFormat::SFloat, 2, sizeof(float) },

	{ VERTEX_ELEMENT_TEXCOORD3D_0, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_1, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_2, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_3, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_4, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_5, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_6, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_7, DataFormat::SFloat, 3, sizeof(float) },

	{ VERTEX_ELEMENT_TEXCOORD4D_0, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_1, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_2, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_3, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_4, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_5, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_6, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_7, DataFormat::SFloat, 4, sizeof(float) },
};

static constexpr VertexFormat::ElementType s_ElementTypesCompressed[VERTEX_ELEMENT_NUMELEMENTS] =
{
	{ VERTEX_ELEMENT_POSITION, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_NORMAL, DataFormat::UInt, 4, sizeof(uint8_t) },
	{ VERTEX_ELEMENT_COLOR, DataFormat::UNorm, 4, sizeof(uint8_t) },
	{ VERTEX_ELEMENT_SPECULAR, DataFormat::UNorm, 4, sizeof(uint8_t) },
	{ VERTEX_ELEMENT_TANGENT_S, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TANGENT_T, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_WRINKLE, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_BONEINDEX, DataFormat::UInt, 4, sizeof(uint8_t) },
	{ VERTEX_ELEMENT_BONEWEIGHTS1, DataFormat::SInt, 2, sizeof(int16_t) },
	{ VERTEX_ELEMENT_BONEWEIGHTS2, DataFormat::SInt, 2, sizeof(int16_t) },
	{ VERTEX_ELEMENT_BONEWEIGHTS3, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_BONEWEIGHTS4, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_USERDATA1, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_USERDATA2, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_USERDATA3, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_USERDATA4, DataFormat::SFloat, 4, 0 },

	{ VERTEX_ELEMENT_TEXCOORD1D_0, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_1, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_2, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_3, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_4, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_5, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_6, DataFormat::SFloat, 1, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD1D_7, DataFormat::SFloat, 1, sizeof(float) },

	{ VERTEX_ELEMENT_TEXCOORD2D_0, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_1, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_2, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_3, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_4, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_5, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_6, DataFormat::SFloat, 2, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD2D_7, DataFormat::SFloat, 2, sizeof(float) },

	{ VERTEX_ELEMENT_TEXCOORD3D_0, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_1, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_2, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_3, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_4, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_5, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_6, DataFormat::SFloat, 3, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD3D_7, DataFormat::SFloat, 3, sizeof(float) },

	{ VERTEX_ELEMENT_TEXCOORD4D_0, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_1, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_2, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_3, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_4, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_5, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_6, DataFormat::SFloat, 4, sizeof(float) },
	{ VERTEX_ELEMENT_TEXCOORD4D_7, DataFormat::SFloat, 4, sizeof(float) },
};

static constexpr auto& GetElemTypes(VertexCompressionType_t compression)
{
	if (compression == VERTEX_COMPRESSION_NONE)
		return s_ElementTypesUncompressed;
	else if (compression == VERTEX_COMPRESSION_ON)
		return s_ElementTypesCompressed;
}

#ifdef _DEBUG
namespace
{
	struct VertexElementValidator final
	{
		template<size_t size> static void Validate(const VertexFormat::ElementType(&arr)[size],
			VertexCompressionType_t compression)
		{
			static_assert(size == VERTEX_ELEMENT_NUMELEMENTS);
			for (int i = 0; i < VERTEX_ELEMENT_NUMELEMENTS; i++)
			{
				VertexElement_t elemType = VertexElement_t(i);
				auto& elem = arr[elemType];
				assert(elem.m_Element == elemType);

				const auto actualSize = elem.GetTotalSize();
				const auto expectedSize = GetVertexElementSize(elem.m_Element, compression);
				assert(actualSize == expectedSize);
			}
		}

		VertexElementValidator()
		{
			Validate(s_ElementTypesUncompressed, VERTEX_COMPRESSION_NONE);
			Validate(s_ElementTypesCompressed, VERTEX_COMPRESSION_ON);
		}

	} static const s_VertexElementValidator;
}
#endif

size_t VertexFormat::GetVertexSize() const
{
	using VFF = VertexFormatFlags;
	size_t size = 0;

	NOT_IMPLEMENTED_FUNC();

	return size;
}

uint_fast8_t VertexFormat::GetTexCoordSize(uint_fast8_t index) const
{
	return TexCoordSize(index, m_BaseFmt);
}

VertexCompressionType_t VertexFormat::GetCompressionType() const
{
	if (m_Flags & VertexFormatFlags::Meta_Compressed)
		return VERTEX_COMPRESSION_ON;

	return VERTEX_COMPRESSION_NONE;
}

void VertexFormat::SetCompressionEnabled(bool enabled)
{
	if (enabled)
		m_Flags = m_Flags | VertexFormatFlags::Meta_Compressed;
	else
		m_Flags = m_Flags & ~VertexFormatFlags::Meta_Compressed;
}

uint_fast8_t VertexFormat::GetVertexElements(Element* elements, uint_fast8_t maxElements, size_t* totalSize) const
{
	assert(maxElements >= VERTEX_ELEMENT_NUMELEMENTS);

	using VFF = VertexFormatFlags;
	size_t totalSizeTemp = 0;
	uint_fast8_t elemCount = 0;

	const auto compression = GetCompressionType();

	const auto StoreElement = [&](VertexElement_t elemType)
	{
		assert(elemType < VERTEX_ELEMENT_NUMELEMENTS);
		if (elemCount >= maxElements)
			return;

#ifdef _DEBUG
		if (elemCount > 0)
		{
			auto prev = elements[elemCount - 1].m_Type->m_Element;
			if (prev >= VERTEX_ELEMENT_TEXCOORD1D_0)
			{
				auto prevIdx = (prev - VERTEX_ELEMENT_TEXCOORD1D_0) % VERTEX_ELEMENT_NUMELEMENTS;
				auto thisIdx = (elemType - VERTEX_ELEMENT_TEXCOORD1D_0) % VERTEX_ELEMENT_NUMELEMENTS;
				assert(prevIdx < thisIdx);
			}
			else
			{
				assert(prev < elemType);
			}
		}
#endif

		Element& e = elements[elemCount++];
		e.m_Type = &GetElemTypes(compression)[elemType];
		e.m_Offset = totalSizeTemp;
		totalSizeTemp += e.m_Type->GetTotalSize();
	};

	if (m_Flags & VFF::Position)
		StoreElement(VERTEX_ELEMENT_POSITION);
	if (m_Flags & VFF::Normal)
		StoreElement(VERTEX_ELEMENT_NORMAL);
	if (m_Flags & VFF::Color)
		StoreElement(VERTEX_ELEMENT_COLOR);
	if (m_Flags & VFF::Specular)
		StoreElement(VERTEX_ELEMENT_SPECULAR);
	if (m_Flags & VFF::TangentS)
		StoreElement(VERTEX_ELEMENT_TANGENT_S);
	if (m_Flags & VFF::TangentT)
		StoreElement(VERTEX_ELEMENT_TANGENT_T);
	if (m_Flags & VFF::Wrinkle)
		StoreElement(VERTEX_ELEMENT_WRINKLE);
	if (m_Flags & VFF::BoneIndex)
		StoreElement(VERTEX_ELEMENT_BONEINDEX);

	if (m_BoneWeightCount >= 1)
		StoreElement(VERTEX_ELEMENT_BONEWEIGHTS1);
	if (m_BoneWeightCount >= 2)
		StoreElement(VERTEX_ELEMENT_BONEWEIGHTS2);
	if (m_BoneWeightCount >= 3)
		StoreElement(VERTEX_ELEMENT_BONEWEIGHTS3);
	if (m_BoneWeightCount >= 4)
		StoreElement(VERTEX_ELEMENT_BONEWEIGHTS4);

	switch (m_UserDataSize)
	{
	default:
		assert(!"Unknown userdata size");
	case 0:
		break;
	case 1:
		StoreElement(VERTEX_ELEMENT_USERDATA1);
		break;
	case 2:
		StoreElement(VERTEX_ELEMENT_USERDATA2);
		break;
	case 3:
		StoreElement(VERTEX_ELEMENT_USERDATA3);
		break;
	case 4:
		StoreElement(VERTEX_ELEMENT_USERDATA4);
		break;
	}

	for (int i = 0; i < VERTEX_MAX_TEXTURE_COORDINATES; i++)
	{
		auto texCoordSize = GetTexCoordSize(i);
		if (texCoordSize > 0)
			StoreElement(VertexElement_t(VERTEX_ELEMENT_TEXCOORD1D_0 + ((texCoordSize - 1) * VERTEX_MAX_TEXTURE_COORDINATES) + i));
	}

	if (totalSize)
		*totalSize = totalSizeTemp;

	return elemCount;
}

uint_fast8_t VertexFormat::ElementType::GetTotalSize() const
{
	return m_Components * m_ComponentSize;
}

vk::Format VertexFormat::ElementType::GetVKFormat() const
{
	return TF2Vulkan::ConvertDataFormat(m_Format, m_Components, m_ComponentSize);
}
