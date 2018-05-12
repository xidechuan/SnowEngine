#pragma once

#include "RenderUtils.h"

#include <DirectXCollision.h>

#include "DescriptorHeap.h"

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct StaticMesh
{
	struct SceneMaterial
	{
		int base_color_tex_idx;
		int normal_tex_idx;
		int specular_tex_idx;
	};
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<std::string> textures;
	std::vector<std::pair<std::string, SceneMaterial>> materials;
	struct Submesh
	{
		std::string name;
		size_t nindices;
		size_t index_offset;
		int material_idx;
		DirectX::XMFLOAT4X4 transform;
	};
	std::vector<Submesh> submeshes;
};

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	// Give it a name so we can look it up by name.
	std::string Name;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	// Data about the buffers.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// We can free this memory after we finish upload to the GPU.
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct MaterialConstants
{
	DirectX::XMFLOAT4X4 mat_transform;
	DirectX::XMFLOAT3 diffuse_fresnel;
};

struct StaticMaterial
{
	MaterialConstants mat_constants;

	D3D12_GPU_DESCRIPTOR_HANDLE base_color_desc;
	D3D12_GPU_DESCRIPTOR_HANDLE normal_map_desc;
	D3D12_GPU_DESCRIPTOR_HANDLE specular_desc;

	Microsoft::WRL::ComPtr<ID3D12Resource> cb_gpu = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> cb_uploader = nullptr;

	void DisposeUploaders()
	{
		cb_uploader = nullptr;
	}
	void LoadToGPU( ID3D12Device& device, ID3D12GraphicsCommandList& cmd_list );
};

struct StaticTexture
{
	Microsoft::WRL::ComPtr<ID3D12Resource> texture_gpu = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> texture_uploader = nullptr;

	std::unique_ptr<Descriptor> main_srv = nullptr;
};

struct RenderItem
{
	// geom
	MeshGeometry* geom = nullptr;
	uint32_t index_count = 0;
	uint32_t index_offset = 0;
	uint32_t vertex_offset = 0;
	
	StaticMaterial* material = nullptr;

	// uniforms
	int cb_idx = -1;
	DirectX::XMFLOAT4X4 world_mat = Identity4x4;

	int n_frames_dirty = 0;
};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 model = Identity4x4;
	DirectX::XMFLOAT4X4 model_inv_transpose = Identity4x4;
};

struct LightConstants
{
	DirectX::XMFLOAT3 strength;
	float falloff_start; // point and spotlight
	DirectX::XMFLOAT3 origin; // point and spotlight
	float falloff_end; // point and spotlight
	DirectX::XMFLOAT3 dir; // spotlight and parallel, direction TO the light source
	float spot_power; // spotlight only
};

struct Light
{
	enum class Type
	{
		Ambient,
		Parallel,
		Point,
		Spotlight
	} type;

	LightConstants data;
};

constexpr int MAX_LIGHTS = 16;

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = Identity4x4;
	DirectX::XMFLOAT4X4 InvView = Identity4x4;
	DirectX::XMFLOAT4X4 Proj = Identity4x4;
	DirectX::XMFLOAT4X4 InvProj = Identity4x4;
	DirectX::XMFLOAT4X4 ViewProj = Identity4x4;
	DirectX::XMFLOAT4X4 InvViewProj = Identity4x4;
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	LightConstants lights[MAX_LIGHTS];
	int n_parallel_lights = 0;
	int n_point_lights = 0;
	int n_spotlight_lights = 0;
};