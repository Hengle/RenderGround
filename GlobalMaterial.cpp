#include "GlobalMaterial.h"


GlobalMaterial::GlobalMaterial()
{
}

void GlobalMaterial::CreateDescriptorSet()
{
	VulkanResourceManager* RM = VulkanResourceManager::GetResourceManager();

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(GlobalBufferObject);

	std::vector<VkWriteDescriptorSet> descriptorWrites = {};
	descriptorWrites.resize(1);
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorSet = RenderingResourceLocater::get_descriptor_global()->allocDescriptorSet(descriptorWrites);

}

void GlobalMaterial::CreateDescriptorBuffer()
{
	VulkanResourceManager* RM = VulkanResourceManager::GetResourceManager();

	RM->createBuffer(sizeof(GlobalBufferObject),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		buffer,
		bufferMemory);
}

void GlobalMaterial::UpdateDescriptorSet()
{
	VulkanResourceManager* RM = VulkanResourceManager::GetResourceManager();

	void* data;
	RM->mapMemory(
		bufferMemory,
		sizeof(GlobalBufferObject),
		&data);

	memcpy(
		data,
		&(ubo),
		sizeof(GlobalBufferObject));

	RM->unMapMemory(
		bufferMemory);
}
