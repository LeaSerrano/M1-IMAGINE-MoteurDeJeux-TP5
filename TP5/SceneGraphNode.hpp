#include "Mesh.hpp"
#include "Transform.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <list>
#include <memory>

class SceneGraphNode : public Mesh {

    public : 
    Transform transform;

    SceneGraphNode(std::string filename, bool isATexturedMesh) : Mesh(filename, isATexturedMesh) {}

    SceneGraphNode(std::vector<glm::vec3> indexed_vertices, std::vector<float> textureData, std::vector<unsigned short> indices, bool isSolarSystemScene, bool isATexturedMesh) : Mesh(indexed_vertices, textureData, indices, isSolarSystemScene, isATexturedMesh) {}

    SceneGraphNode(std::vector<glm::vec3> indexed_vertices, std::vector<float> textureData, std::vector<unsigned short> indices, bool isSolarSystemScene, bool isATexturedMesh, GLfloat* color) : Mesh(indexed_vertices, textureData, indices, isSolarSystemScene, isATexturedMesh, color) {}

    SceneGraphNode(std::vector<glm::vec3> indexed_vertices, std::vector<float> textureData, std::vector<unsigned short> indices, bool isSolarSystemScene, bool isATexturedMesh, int planet) : Mesh(indexed_vertices, textureData, indices, isSolarSystemScene, isATexturedMesh, planet) {}


    SceneGraphNode() : Mesh() {}

    std::vector<SceneGraphNode*> children;

    void addChild(SceneGraphNode* child) {
        children.push_back(child);
    }

    void update() {
        transform.modelMatrix = this->transform.changeModelMatrix();
    }

    void drawScene(GLuint programID) {
        this->sendDatas();
        this->drawMesh(programID);
    }

    void updateScene(std::vector<glm::vec3> indexed_vertices, std::vector<float> textureData, std::vector<unsigned short> indices) {
        this->updateMeshData(indexed_vertices, textureData, indices);
    }

    void deleteBuffer() {
        this->deleteBuffer();
    }
};