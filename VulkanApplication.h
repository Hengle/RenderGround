#pragma once
#include "Common.h"
#include "Utility.h"


class VulkanApplication
{

private :
	VkInstance instance;
	GLFWwindow* window;
	QueueFamilyIndices indices;
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;
	int windowWidth;
	int windowHeight;

	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation",
	 "VK_LAYER_RENDERDOC_Capture"
	};

public:	


	VkPhysicalDevice GetVkPhysicalDevice() {
		return physicalDevice;
	}
	VkInstance GetVkInstance() {
		return instance;
	}
	VkSurfaceKHR GetVkSurfaceKHR() {
		return surface;
	}
	GLFWwindow* GetWindow() {
		return window;
	}

	int GetWindowHeight()
	{
		return windowHeight;
	}
	int GetWindowWidth()
	{
		return windowWidth;
	}
	bool IsResized()
	{
		return framebufferResized;
	}

	void ResetResizedFlag()
	{
		framebufferResized = false;
	}

	bool framebufferResized = false;
	VulkanApplication(int,int);
	void createWindow();
	void updateWindowSize();
	void destroyWindow();
	void checkExtension();
	void createInstance();
	void destroyInstance();
	bool checkValidationLayerSupport();
	void createSurface();
	void destroySurface();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	int rateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	
};