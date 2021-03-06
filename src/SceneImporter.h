#pragma once

#include <DirectXMath.h>
#include "RenderData.h"

struct ImportedScene
{
    struct SceneMaterial
    {
        int base_color_tex_idx;
        int normal_tex_idx;
        int specular_tex_idx;
        MaterialID material_id;
    };
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<std::pair<std::string, TextureID>> textures;
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
    StaticMeshID mesh_id = StaticMeshID::nullid;
};

// todo: obj format
bool LoadFbxFromFile( const std::string& filename, ImportedScene& mesh );