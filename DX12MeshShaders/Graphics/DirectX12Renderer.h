#pragma once

#include "../Includes.h"
#include "Memory/ResourceManager.h"

namespace Graphics
{
	class DirectX12Renderer final
	{
	public:
		DirectX12Renderer(HWND& windowHandler, uint32_t initialWidth, uint32_t initialHeight, bool fullscreen);
		~DirectX12Renderer();

		void GpuRelease();

		void OnResize(uint32_t newWidth, uint32_t newHeight);
		void OnSetFocus();
		void OnLostFocus();

		void FrameStart();
		void FrameRender();

		ID3D12GraphicsCommandList6* GetCommandList();

	private:
		DirectX12Renderer() = delete;
		DirectX12Renderer(const DirectX12Renderer&) = delete;
		DirectX12Renderer(DirectX12Renderer&&) = delete;
		DirectX12Renderer& operator=(const DirectX12Renderer&) = delete;
		DirectX12Renderer& operator=(DirectX12Renderer&&) = delete;

		void Initialize(HWND& windowHandler, uint32_t initialWidth, uint32_t initialHeight, bool fullscreen);
		void CreateSwapChainBuffers(uint32_t initialWidth, uint32_t initialHeight);

		void PrepareNextFrame();
		void WaitForGpu();
		void SwitchFullscreenMode(bool toggleFullscreen);

		static const uint32_t SWAP_CHAIN_BUFFER_COUNT = 2;

		ComPtr<ID3D12Device9> device;
		ComPtr<ID3D12GraphicsCommandList6> commandList;
		ComPtr<ID3D12CommandQueue> commandQueue;
		ComPtr<ID3D12CommandAllocator> commandAllocator[SWAP_CHAIN_BUFFER_COUNT];
		ComPtr<ID3D12Fence1> fence;

		ComPtr<IDXGIFactory7> dxgiFactory;
		ComPtr<IDXGISwapChain4> swapChain;

		std::shared_ptr<Memory::ResourceManager> resourceManager;
		Memory::ResourceId swapChainBuffers[SWAP_CHAIN_BUFFER_COUNT];
		D3D12_CPU_DESCRIPTOR_HANDLE swapChainCPUDescriptors[SWAP_CHAIN_BUFFER_COUNT];

		HANDLE fenceEvent;

		D3D12_VIEWPORT sceneViewport;
		D3D12_RECT sceneScissorRect;

		uint32_t bufferIndex;
		uint64_t fenceValues[SWAP_CHAIN_BUFFER_COUNT];
	};
}
