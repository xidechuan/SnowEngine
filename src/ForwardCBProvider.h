#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "utils/span.h"

#include "RenderData.h"
#include "SceneItems.h"

// Fills PassConstants circular buffer on GPU
class ForwardCBProvider
{
public:
	ForwardCBProvider( ID3D12Device& device, int n_bufferized_frames );
	~ForwardCBProvider() noexcept;

	void Update( const Camera::Data& camera, const span<const SceneLight>& scene_lights );

	D3D12_GPU_VIRTUAL_ADDRESS GetCBPointer() const noexcept;

	float m_interpolator = 0.0f;

private:
	void FillCameraData( const Camera::Data& camera, PassConstants& gpu_data ) const noexcept;

	// pass transposed matrix here, it's more convenient for the caller 
	void FillLightData( const span<const SceneLight>& lights,
						const DirectX::XMMATRIX& inv_view_matrix_transposed,
						const DirectX::XMMATRIX& view_matrix,
						PassConstants& gpu_data ) const;

	void FillCSMData( const Camera::Data& camera, PassConstants& gpu_data ) const noexcept;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_gpu_res = nullptr;
	span<uint8_t> m_mapped_data;
	int m_cur_res_idx = 0;
	const int m_nbuffers;

	static constexpr UINT BufferGPUSize = Utils::CalcConstantBufferByteSize( sizeof( PassConstants ) );
	static constexpr size_t MaxLights = sizeof( PassConstants::lights ) / sizeof( LightConstants );
	static constexpr size_t MaxParallelLights = sizeof( PassConstants::parallel_lights ) / sizeof( ParallelLightConstants );
};