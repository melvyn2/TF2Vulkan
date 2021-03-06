#include "vk_mem_alloc.hpp"

using namespace TF2Vulkan;
using namespace vma;

UniqueAllocator vma::createAllocatorUnique(const AllocatorCreateInfo& createInfo)
{
	VmaAllocator retVal;
	if (auto result = vk::Result(vmaCreateAllocator(&createInfo, &retVal));
		result != vk::Result::eSuccess)
	{
		Warning(TF2VULKAN_PREFIX "%s\n", vk::to_string(result).c_str());
		assert(false);
		return nullptr;
	}

	return UniqueAllocator(retVal);
}

UniqueAllocation::UniqueAllocation(VmaAllocator allocator, VmaAllocation allocation) :
	m_Allocation(allocation, allocator)
{
}

AllocationInfo UniqueAllocation::getAllocationInfo() const
{
	AllocationInfo retVal;
	vmaGetAllocationInfo(GetAllocator(), GetAllocation(), &retVal);
	return retVal;
}

bool UniqueAllocation::IsMapped() const
{
	return !!data();
}

void UniqueAllocation::Write(const void* srcData, size_t srcSize, size_t dstOffset)
{
	auto allocInfo = getAllocationInfo();

	if ((srcSize + dstOffset) > allocInfo.size)
		NOT_IMPLEMENTED_FUNC(); // How should we handle this?

	assert(allocInfo.pMappedData);
	auto err = memcpy_s((std::byte*)allocInfo.pMappedData + dstOffset,
		allocInfo.size - dstOffset, srcData, srcSize);

	assert(err == errno_t{});
}

void UniqueAllocation::Read(void* dstData, size_t srcSize) const
{
	NOT_IMPLEMENTED_FUNC();
}

std::byte* UniqueAllocation::data()
{
	return const_cast<std::byte*>(std::as_const(*this).data());
}

const std::byte* UniqueAllocation::data() const
{
	if (m_Allocation)
		return reinterpret_cast<const std::byte*>(getAllocationInfo().pMappedData);

	return nullptr;
}

UniqueAllocation::operator bool() const
{
	return !!GetAllocation();
}

VmaAllocation UniqueAllocation::GetAllocation() const
{
	return m_Allocation.get();
}

VmaAllocator UniqueAllocation::GetAllocator() const
{
	return m_Allocation.get_deleter().m_Allocator;
}

UniqueAllocator::UniqueAllocator(VmaAllocator allocator) :
	m_Allocator(allocator)
{
}

AllocatedBuffer UniqueAllocator::createBufferUnique(const vk::BufferCreateInfo& bufCreateInfo,
	const AllocationCreateInfo& allocCreateInfo)
{
	auto cBufCreateInfo = (VkBufferCreateInfo)bufCreateInfo;

	VkBuffer outBuf;
	VmaAllocation outAllocation;
	if (auto result = vk::Result(vmaCreateBuffer(m_Allocator.get(), &cBufCreateInfo,
		&allocCreateInfo, &outBuf, &outAllocation, nullptr));
		result != vk::Result::eSuccess)
	{
		throw VulkanException(result, EXCEPTION_DATA());
	}

	return { outBuf, UniqueAllocation(m_Allocator.get(), outAllocation), Util::SafeConvert<size_t>(bufCreateInfo.size) };
}

AllocatedImage UniqueAllocator::createImageUnique(const vk::ImageCreateInfo& imgCreateInfo,
	const AllocationCreateInfo& allocCreateInfo)
{
	auto cImgCreateInfo = (VkImageCreateInfo)imgCreateInfo;

	[[maybe_unused]] auto flags = (vk::ImageCreateFlagBits)(VkImageCreateFlags)imgCreateInfo.flags;
	[[maybe_unused]] auto usage = (vk::ImageUsageFlagBits)(VkImageUsageFlags)imgCreateInfo.usage;

	VkImage outImg;
	VmaAllocation outAllocation;
	if (auto result = vk::Result(vmaCreateImage(m_Allocator.get(), &cImgCreateInfo,
		&allocCreateInfo, &outImg, &outAllocation, nullptr));
		result != vk::Result::eSuccess)
	{
		throw VulkanException(result, EXCEPTION_DATA());
	}

	return { outImg, UniqueAllocation(m_Allocator.get(), outAllocation) };
}

AllocatedBuffer::AllocatedBuffer(VkBuffer buf, UniqueAllocation&& allocation, size_t realSize) :
	m_Buffer(vk::Buffer(buf), std::move(allocation)), m_RealSize(realSize)
{
}

const vk::Buffer& AllocatedBuffer::GetBuffer() const
{
	return m_Buffer.get();
}

UniqueAllocation& AllocatedBuffer::GetAllocation()
{
	return const_cast<UniqueAllocation&>(std::as_const(*this).GetAllocation());
}

const UniqueAllocation& AllocatedBuffer::GetAllocation() const
{
	return m_Buffer.get_deleter().m_Allocation;
}

size_t AllocatedBuffer::size() const
{
	return m_RealSize;
}

AllocatedBuffer::operator bool() const
{
	return !!GetBuffer();
}

AllocatedImage::AllocatedImage(VkImage img, UniqueAllocation&& allocation) :
	m_Image(vk::Image(img), std::move(allocation))
{
}

const vk::Image& AllocatedImage::GetImage() const
{
	return m_Image.get();
}

UniqueAllocation& AllocatedImage::GetAllocation()
{
	return const_cast<UniqueAllocation&>(std::as_const(*this).GetAllocation());
}

const UniqueAllocation& AllocatedImage::GetAllocation() const
{
	return m_Image.get_deleter().m_Allocation;
}

AllocatedImage::operator bool() const
{
	return !!GetImage();
}

void detail::Deleter::operator()(VmaAllocator allocator) const
{
	if (allocator)
		vmaDestroyAllocator(allocator);
}

void detail::AllocatedObjectDeleter::operator()(vk::Image& image) noexcept
{
	if (image)
	{
		vmaDestroyImage(m_Allocation.GetAllocator(), (VkImage)image, m_Allocation.GetAllocation());
		image = nullptr;
		m_Allocation.m_Allocation.release(); // The vmaDestroyImage call deletes this
	}
}

detail::AllocatedObjectDeleter::AllocatedObjectDeleter(UniqueAllocation&& allocation) :
	m_Allocation(std::move(allocation))
{
}

void detail::AllocatedObjectDeleter::operator()(vk::Buffer& buffer) noexcept
{
	if (buffer)
	{
		vmaDestroyBuffer(m_Allocation.GetAllocator(), (VkBuffer)buffer, m_Allocation.GetAllocation());
		buffer = nullptr;
		m_Allocation.m_Allocation.release(); // The vmaDestroyImage call deletes this
	}
}

void detail::AllocationDeleter::operator()(VmaAllocation allocation) const noexcept
{
	if (allocation)
		vmaFreeMemory(m_Allocator, allocation);
}
