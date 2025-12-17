#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include <unordered_map>
#include <string>
#include <memory>

class Scene;
class Renderer;

class SceneManager {
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;

    Scene *currentScene;
    std::string currentSceneName;
    std::string targetSceneName;

    Scene *AddBlankScene(const std::string &name);
    void CreateScenes();
    void ChangeScene(const std::string &name);

public:
    SceneManager();
    ~SceneManager();

    void RequestSceneChange(const std::string &name);

    void Update(float deltaTime);
    void Render(Renderer *renderer);
};

#endif