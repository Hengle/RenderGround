#include "VulkanResourceManager.h"

VulkanResourceManager* VulkanResourceManager::vulkanResourceManager = NULL;

VulkanResourceManager::VulkanResourceManager(VulkanDevice* vulkanDevice, VulkanApplication * vulkanInstance)
{
	this->vulkanDevice = vulkanDevice;
	this->vulkanInstance = vulkanInstance;
}

VkFormat VulkanResourceManager::findDepthFormat() {
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

VkFramebuffer VulkanResourceManager::createFramebuffer(VkFramebufferCreateInfo* framebufferInfo)
{
	VkFramebuffer vkFramebuffer;
	if (vkCreateFramebuffer(vulkanDevice->GetDevice(), framebufferInfo, nullptr, &vkFramebuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer!");
	}
	return vkFramebuffer;
}

QueueFamilyIndices VulkanResourceManager::findQueueFamilies() {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vulkanInstance->GetVkPhysicalDevice(), &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vulkanInstance->GetVkPhysicalDevice(), &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		// 可以提交图形渲染数据
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		// 可以提交绘制到Surface命令
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(vulkanInstance->GetVkPhysicalDevice(), i, vulkanInstance->GetVkSurfaceKHR(), &presentSupport);
		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

void VulkanResourceManager::mapMemory(VkDeviceMemory memory, VkDeviceSize size, void** data)
{
	vkMapMemory(vulkanDevice->GetDevice(), memory, 0, size, 0, data);
}

void VulkanResourceManager::unMapMemory(VkDeviceMemory memory)
{
	vkUnmapMemory(vulkanDevice->GetDevice(), memory);
}

void VulkanResourceManager::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0; // Optional
	if (vkCreateCommandPool(vulkanDevice->GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

VkFormat VulkanResourceManager::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(vulkanInstance->GetVkPhysicalDevice(), format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}

void VulkanResourceManager::createBuffer(VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer,
	VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(vulkanDevice->GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vulkanDevice->GetDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = Utility::findMemoryType(vulkanInstance->GetVkPhysicalDevice(),memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(vulkanDevice->GetDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(vulkanDevice->GetDevice(), buffer, bufferMemory, 0);
}

void VulkanResourceManager::createImage(uint32_t width, uint32_t height,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage& image,
	VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(vulkanDevice->GetDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(vulkanDevice->GetDevice(), image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = Utility::findMemoryType(vulkanInstance->GetVkPhysicalDevice(), memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(vulkanDevice->GetDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(vulkanDevice->GetDevice(), image, imageMemory, 0);
}

void VulkanResourceManager::destroyImage(VkImage image)
{
	vkDestroyImage(vulkanDevice->GetDevice(), image, nullptr);
}

void VulkanResourceManager::destroyBuffer(VkBuffer buffer)
{
	vkDestroyBuffer(vulkanDevice->GetDevice(), buffer, nullptr);
}

void VulkanResourceManager::freeMemory(VkDeviceMemory memory)
{
	vkFreeMemory(vulkanDevice->GetDevice(), memory, nullptr);
}

VkImageView VulkanResourceManager::createImageView(VkImage image,
	VkFormat format,
	VkImageAspectFlags flags)
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	
	VkImageView imageView;
	if (vkCreateImageView(vulkanDevice->GetDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Image View");

	}
	return imageView;
}