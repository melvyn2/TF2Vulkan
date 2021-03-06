#pragma once

#include "TF2Vulkan/DataFormat.h"

#include <cstdint>

enum ImageFormat;

namespace TF2Vulkan
{
	enum class FormatUsage
	{
		ImmutableTexture,
		RenderTarget,
		DepthStencil,
		ReadBackTexture,

		COUNT,
	};
}

namespace TF2Vulkan{ namespace FormatInfo
{
	vk::Format ConvertImageFormat(ImageFormat format);
	ImageFormat ConvertImageFormat(vk::Format format);

	ImageFormat RemoveRuntimeFlags(ImageFormat format);

	bool IsCompressed(vk::Format format);
	bool HasHardwareSupport(vk::Format format, FormatUsage usage, bool filtering);
	bool HasHardwareSupport(ImageFormat format, FormatUsage usage, bool filtering);
	ImageFormat PromoteToHardware(ImageFormat format, FormatUsage usage, bool filtering);
	vk::Format PromoteToHardware(vk::Format format, FormatUsage usage, bool filtering);
	vk::Extent2D GetBlockSize(vk::Format format);
	vk::Extent3D GetMipResolution(uint_fast8_t mipIndex, uint32_t width, uint32_t height, uint32_t depth = 1);

	// See https://developer.valvesoftware.com/wiki/Valve_Texture_Format#VTF_layout
	size_t GetImageSize(vk::Format format,
		uint32_t width, uint32_t height, uint32_t depth,
		uint_fast8_t faceCount, uint32_t frameCount,
		uint_fast8_t mipCount = uint_fast8_t(-1), uint_fast8_t baseMip = 0);
	size_t GetMipLevelSize(vk::Format format,
		uint32_t width, uint32_t height, uint32_t depth,
		uint_fast8_t faceCount, uint32_t frameCount);
	size_t GetFrameSize(vk::Format format,
		uint32_t width, uint32_t height, uint32_t depth,
		uint_fast8_t faceCount);
	size_t GetFaceSize(vk::Format format,
		uint32_t width, uint32_t height, uint32_t depth);
	size_t GetSliceSize(vk::Format format,
		uint32_t width, uint32_t height);
	size_t GetStride(vk::Format format, uint32_t width);
	size_t GetPixelSize(vk::Format format);

	struct SubrectOffset
	{
		size_t m_Offset;
		size_t m_Stride;
	};
	SubrectOffset GetSubrectOffset(ImageFormat format,
		uint32_t offsetX, uint32_t offsetY, uint32_t offsetZ,
		uint32_t baseWidth, uint32_t baseHeight, uint32_t baseDepth,
		CubeMapFaceIndex_t face, uint_fast8_t faceCount,
		uint32_t frame, uint32_t frameCount,
		uint_fast8_t mipLevel);
	SubrectOffset GetSubrectOffset(vk::Format format,
		uint32_t offsetX, uint32_t offsetY, uint32_t offsetZ,
		uint32_t baseWidth, uint32_t baseHeight, uint32_t baseDepth,
		CubeMapFaceIndex_t face, uint_fast8_t faceCount,
		uint32_t frame, uint32_t frameCount,
		uint_fast8_t mipLevel);

	vk::ImageAspectFlags GetAspects(const vk::Format& format);

	vk::Format ConvertDataFormat(DataFormat fmt, uint_fast8_t components, uint_fast8_t componentSize);
	[[nodiscard]] bool ConvertDataFormat(vk::Format inFmt, DataFormat& outFmt, uint_fast8_t& numComponents, uint_fast8_t& byteSize);

	inline vk::Extent2D GetBlockSize(ImageFormat format)
	{
		return GetBlockSize(ConvertImageFormat(format));
	}
	inline bool IsCompressed(ImageFormat format)
	{
		return IsCompressed(ConvertImageFormat(format));
	}
	inline size_t GetStride(ImageFormat format, uint32_t width)
	{
		return GetStride(ConvertImageFormat(format), width);
	}
	inline size_t GetPixelSize(ImageFormat format)
	{
		return GetPixelSize(ConvertImageFormat(format));
	}
	inline size_t GetSliceSize(ImageFormat format, uint32_t width, uint32_t height)
	{
		return GetSliceSize(ConvertImageFormat(format), width, height);
	}
} }
