/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <animator.h>

#include <singleton.h>

class ShaderFactory;
class Asset;
class Mesh;
class Material;
class ShaderProgram;
class Texture;
class Cubemap;

// AssetManager owns all the Scene's assets and caches them for reference via names.
// Assets not created through the AssetManager will not be kept track of.
// Asset names are unique for each type of Asset.
class AssetManager : public Singleton<AssetManager> {
public:
    AssetManager(ShaderFactory &shader_factory);
    ~AssetManager();

    // Load from Disk. If failed, has no effect and writes a log message.
    // If the name already exists, replaces the pre-existing asset.
    void LoadTexture(const std::string& name, const std::string& path);
    void LoadCubemap(const std::string& name, const std::string& path);
    void LoadMesh(const std::string& name, const std::string& path, bool internal=false);

    // Creates an asset in-memory. If the name already exists, returns a nullptr.
    // Internal indicates this asset is not to be serialized (as it is created in the code).
    Mesh* CreateMesh(const std::string& name, MeshType meshtype = MeshType::Triangles, bool internal = true, bool hidden = true); // Returns an empty Mesh
    Material* CreateMaterial(const std::string& name, bool internal = true); // Returns a material using the default shading program
    ShaderProgram* CreateShaderProgram(const std::string& name, bool internal = true); // Returns a shading program with default vertex and fragment shaders

    // Unload from Memory
    void UnloadTexture(const std::string& name);
    void UnloadCubemap(const std::string& name);
    void UnloadMaterial(const std::string& name);
    void UnloadMesh(const std::string& name);
    void UnloadShaderProgram(const std::string& name);

    // Cache accessors
    Texture* GetTexture(const std::string& name);
    Cubemap* GetCubemap(const std::string& name);
    Material* GetMaterial(const std::string& name);
    Mesh* GetMesh(const std::string& name);
    ShaderProgram* GetShaderProgram(const std::string& name);

    Texture* GetOrCreateSolidTexture(glm::vec3 color);

    void Refresh();  // Reloads all assets from disk

    // Signals
    Signal1<Asset&> AssetCreated;
    Signal1<uint64_t> AssetDeleted;

    // Used for serialization and UI
    std::vector<Texture*> GetTextures() const;
    std::vector<Cubemap*> GetCubemaps() const;
    std::vector<Material*> GetMaterials() const;
    std::vector<Mesh*> GetMeshes() const;
    std::vector<ShaderProgram*> GetShaderPrograms() const;

    ShaderProgram& GetDefaultShaderProgram() const { return *default_shader_; }
    Material& GetDefaultMaterial() const { return *default_material_; }
    Texture& GetDefaultTexture() const { return *default_texture_; }
    Cubemap& GetDefaultCubemap() const { return *default_cubemap_; }

protected:
    const std::string depth_frag_src =
        "#version 400\n"
        "out vec4 frag_color;"
        "in vec4 interpolated_position;"
        "void main() {"
            "float depth = length(interpolated_position.xyz);"
            "frag_color = vec4(depth/10.f, depth/10.f, depth/10.f, 1.f);"
        "}";


    ShaderFactory* shader_factory_;

    // Default assets that serve as fallbacks in case the user asset is not valid
    // TODO: Default mesh?
    std::unique_ptr<Texture> default_texture_;
    std::unique_ptr<Cubemap> default_cubemap_;
    std::unique_ptr<ShaderProgram> default_shader_;
    std::unique_ptr<Material> default_material_;

    // Resource Cache
    // TODO: Potential memory issue here. Material depends on reference to ShaderProgram, but ShaderProgram can be unloaded separately from Material, which invalidates Material's pointer.
    // Same issue with like Material uses Texture etc. Unless we guarantee that they won't be unloaded until the very end, or some other way to deal with this.
    std::map<std::string, std::unique_ptr<Texture>> textures_;
    std::map<std::string, std::unique_ptr<Cubemap>> cubemaps_;
    std::map<std::string, std::unique_ptr<Material>> materials_;
    std::map<std::string, std::unique_ptr<Mesh>> meshes_;
    std::map<std::string, std::unique_ptr<ShaderProgram>> shader_programs_;

    std::map<unsigned int, std::unique_ptr<Texture>> solid_textures_;
};

#endif // ASSETMANAGER_H