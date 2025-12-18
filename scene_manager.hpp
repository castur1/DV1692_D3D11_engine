#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include <unordered_map>
#include <string>
#include <memory>
#include "mesh_data.hpp"
#include "material.hpp"

class Scene;
class Renderer;

class SceneManager {
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;

    // TODO: Asset manager
    std::vector<std::unique_ptr<MeshData>> meshes;
    std::vector<std::unique_ptr<Material>> materials;

    Scene *currentScene;
    std::string currentSceneName;
    std::string targetSceneName;

    Scene *AddBlankScene(const std::string &name);
    void CreateScenes(Renderer *renderer);
    void ChangeScene(const std::string &name);

public:
    SceneManager();
    ~SceneManager();

    void Initialize(Renderer *renderer);

    void RequestSceneChange(const std::string &name);

    void Update(float deltaTime);
    void Render(Renderer *renderer);
};

#endif