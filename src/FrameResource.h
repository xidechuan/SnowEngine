#pragma once

#include <Luna/UploadBuffer.h>

#include "RenderData.h"

struct FrameResource
{
	FrameResource( ID3D12Device* device, UINT passCount, UINT objectCount );

	// We cannot reset the allocator until the GPU is done processing the commands.
	// So each frame needs their own allocator.
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmd_list_alloc;

	// We cannot update a cbuffer until the GPU is done processing the commands
	// that reference it.  So each frame needs their own cbuffers.
	std::unique_ptr<UploadBuffer<PassConstants>> pass_cb = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> object_cb = nullptr;

	// Fence value to mark commands up to this fence point.  This lets us
	// check if these frame resources are still in use by the GPU.
	UINT64 fence = 0;
};