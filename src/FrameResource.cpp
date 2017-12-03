#include "stdafx.h"

#include "FrameResource.h"

FrameResource::FrameResource( ID3D12Device* device, UINT passCount, UINT objectCount )
{
	ThrowIfFailed( device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( cmd_list_alloc.GetAddressOf() ) ) );
	pass_cb = std::make_unique<UploadBuffer<PassConstants>>( device, passCount, true );
	object_cb = std::make_unique<UploadBuffer<ObjectConstants>>( device, objectCount, true );
}