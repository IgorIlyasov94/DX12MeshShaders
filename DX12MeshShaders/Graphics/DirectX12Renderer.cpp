#include "DirectX12Renderer.h"
#include "DirectX12Helper.h"

Graphics::DirectX12Renderer::DirectX12Renderer(HWND& windowHandler, uint32_t initialWidth, uint32_t initialHeight, bool fullscreen)
	: fenceEvent(nullptr), bufferIndex(0), fenceValues{}
{
	initialWidth = std::max(initialWidth, 1u);
	initialHeight = std::max(initialHeight, 1u);

	sceneViewport.TopLeftX = 0.0f;
	sceneViewport.TopLeftY = 0.0f;
	sceneViewport.Width = static_cast<float>(initialWidth);
	sceneViewport.Height = static_cast<float>(initialHeight);
	sceneViewport.MinDepth = D3D12_MIN_DEPTH;
	sceneViewport.MaxDepth = D3D12_MAX_DEPTH;

	sceneScissorRect.left = 0;
	sceneScissorRect.top = 0;
	sceneScissorRect.right = initialWidth;
	sceneScissorRect.bottom = initialHeight;

	Initialize(windowHandler, initialWidth, initialHeight, fullscreen);
}

Graphics::DirectX12Renderer::~DirectX12Renderer()
{

}

void Graphics::DirectX12Renderer::GpuRelease()
{
	WaitForGpu();

	swapChain->SetFullscreenState(false, nullptr);

	CloseHandle(fenceEvent);
}

void Graphics::DirectX12Renderer::OnResize(uint32_t newWidth, uint32_t newHeight)
{

}

void Graphics::DirectX12Renderer::OnSetFocus()
{
	WaitForGpu();

	swapChain->SetFullscreenState(true, nullptr);

	for (auto& fenceValue : fenceValues)
		fenceValue = fenceValues[bufferIndex];

	resourceManager.ResetSwapChainBuffers(swapChain.Get());

	bufferIndex = swapChain->GetCurrentBackBufferIndex();
}

ID3D12GraphicsCommandList6* Graphics::DirectX12Renderer::GetCommandList()
{
	return commandList.Get();
}

void Graphics::DirectX12Renderer::Initialize(HWND& windowHandler, uint32_t initialWidth, uint32_t initialHeight, bool fullscreen)
{
	DirectX12Helper::CreateFactory(&dxgiFactory);

	ComPtr<IDXGIAdapter4> adapter;
	DirectX12Helper::GetHardwareAdapter(dxgiFactory.Get(), &adapter);

	DirectX12Helper::CreateDevice(adapter.Get(), &device);
	DirectX12Helper::CreateCommandQueue(device.Get(), &commandQueue);

	ComPtr<IDXGISwapChain1> swapChain1;
	DirectX12Helper::CreateSwapChain(dxgiFactory.Get(), commandQueue.Get(), windowHandler, SWAP_CHAIN_BUFFER_COUNT,
		initialWidth, initialHeight, &swapChain1);

	ThrowIfFailed(swapChain1.As(&swapChain), "DirectX12Renderer::Initialize: Swap Chain conversion error!");

	if (fullscreen)
		SwitchFullscreenMode(true);

	bufferIndex = swapChain->GetCurrentBackBufferIndex();

	for (auto swapChainBufferId = 0; swapChainBufferId < SWAP_CHAIN_BUFFER_COUNT; swapChainBufferId++)
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator[swapChainBufferId])),
			"DirectX12Renderer::Initialize: Command Allocator creating error!");

	ThrowIfFailed(device->CreateFence(fenceValues[bufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)),
		"DirectX12Renderer::Initialize: Fence creating error!");
	fenceValues[bufferIndex]++;

	fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (fenceEvent == nullptr)
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()), "DirectX12Renderer::Initialize: Fence Event creating error!");

	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[bufferIndex].Get(), nullptr, IID_PPV_ARGS(&commandList)),
		"DirectX12Renderer::Initialize: Command List creating error!");

	resourceManager = std::make_shared<Memory::ResourceManager>(device.Get());
	CreateSwapChainBuffers(initialWidth, initialHeight);

	ThrowIfFailed(commandList->Close(), "RendererDirectX12::Initialize: Command List closing error!");

	ID3D12CommandList* commandLists[] = { commandList.Get() };

	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	WaitForGpu();
}

void Graphics::DirectX12Renderer::CreateSwapChainBuffers(uint32_t initialWidth, uint32_t initialHeight)
{
	for (uint32_t bufferId = 0; bufferId < SWAP_CHAIN_BUFFER_COUNT; bufferId++)
	{
		DescriptorAllocation renderTargetDescriptorAllocation{};

		descriptorAllocator.Allocate(device, false, buffersCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, renderTargetDescriptorAllocation);
		swapChainDescriptorBases.push_back(renderTargetDescriptorAllocation.descriptorBase);

		Memory::ResourceDesc desc{};
		desc.textureInfo = std::make_shared<Graphics::TextureInfo>(Graphics::TextureInfo{});
		desc.textureInfo->width = initialWidth;
		desc.textureInfo->height = initialHeight;
		desc.textureInfo->depth = 1;
		desc.textureInfo->mipLevels = 1;
		desc.textureInfo->dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		swapChainBuffers[bufferId] = resourceManager->CreateResource(Memory::ResourceType::RENDER_TARGET, desc);
		swapChainCPUDescriptors[bufferId] = resourceManager->GetRenderTargetDescriptorBase(swapChainBuffers[bufferId]);

		swapChainBuffers.push_back(nullptr);
		swapChain->GetBuffer(bufferId, IID_PPV_ARGS(&swapChainBuffers.back()));

		device->CreateRenderTargetView(swapChainBuffers.back().Get(), nullptr, swapChainDescriptorBases.back());

		swapChainStates.push_back(D3D12_RESOURCE_STATE_PRESENT);
	}
}
