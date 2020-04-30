#include "VulkanRenderPass.h"

IVulkanRenderPass::IVulkanRenderPass()
{

}

VkRenderPass IVulkanRenderPass::GetInstance()
{
	return renderPass;
}

void IVulkanRenderPass::createRenderPass() {

	VulkanResourceManager* RM = VulkanResourceManager::GetResourceManager();
	VkDevice vkDevice = RM->GetDevice()->GetInstance();
	
	VkAttachmentDescription colorAttachment = {};
	//The format of the color attachment should match the format of the swap chain image
	colorAttachment.format = RM->GetSwapChain()->GetSwapChainImageFormat();
	//we're not doing anything with multisampling yet, so we'll stick to 1 sample.
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	//��������ֱ��ʾ��Ⱦ��ʼǰ����Ⱦ������
	/*
		VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
		VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
		VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them
	*/
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	/*
		VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memory and can be read later
		VK_ATTACHMENT_STORE_OP_DONT_CARE: Contents of the framebuffer will be undefined after the rendering operation
	*/
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	/*
	Textures and framebuffers in Vulkan are represented by VkImage objects with
	a certain pixel format,
	however the layout of the pixels in memory can change based on what
	you're trying to do with an image.
	*/
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//finalLayout��ʾ��ȾPass����֮��Image�Ĳ�������Զ�ת����
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = RM->findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	//������attachemtn����������������attachmentRef��
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//һ����ȾPass�ɶ��Subpass��϶��ɣ�Subpass��һϵ�е���Ⱦ������������ǰ����pass��framebuffer������
	//�����ǰ�pass���飬Vulkan�ܹ�����������Ⱦ˳�򽵵ʹ����������ܡ�
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	//ָ������Subpass�õ������е�attachment��Ref
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	
	/*
	pInputAttachments: Attachments that are read from a shader
	pResolveAttachments: Attachments used for multisampling color attachments
	pDepthStencilAttachment: Attachment for depth and stencil data
	pPreserveAttachments: Attachments that are not used by this subpass, but for which the data must be preserved
	*/

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	// ָ��ȫ��Ҫ�õ���attachment��
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	// ָ�����е�Subpass����ͬ��Subpass�����в�ͬ��Attachment��ϣ���SubpassInfo��ָ��Ref
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}

	//specify memory and execution dependencies between subpasses.
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
}

void IVulkanRenderPass::destroyRenderPass() {

	VulkanResourceManager* RM = VulkanResourceManager::GetResourceManager();
	VkDevice vkDevice = RM->GetDevice()->GetInstance();
	vkDestroyRenderPass(vkDevice, renderPass, nullptr);
}

