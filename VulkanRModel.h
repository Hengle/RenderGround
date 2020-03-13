#pragma once
#include "Common.h"
#include "VulkanModel.h"
class VulkanRModel
{
public:
	VulkanRModel();
	void SetVertexBuffer(VkBuffer vertexBuffer)
	{
		this->vertexBuffer = vertexBuffer;
	}

	void SetIndexBuffer(VkBuffer indexBuffer)
	{
		this->indexBuffer = indexBuffer;
	}

	void SetIndexSize(size_t indexSize)
	{
		this->indexSize = indexSize;
	}

	VkBuffer GetIndexBuffer()
	{
		return indexBuffer;
	}

	VkBuffer GetVertexBuffer()
	{
		return vertexBuffer;
	}

	size_t GetIndexSize()
	{
		return indexSize;
	}

private:
	VkImage textureImage;
	VkImageView textureImageView;

	size_t indexSize;
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
};
