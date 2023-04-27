#include "Scene.h"


Scene::Scene(const SceneID& id) : id { id } {

}


SceneID Scene::getID() const {
    return id;
}