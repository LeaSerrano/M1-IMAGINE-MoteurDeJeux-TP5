#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/texture.hpp>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

//unsigned int textureFromFile(const char *path, const string &directory, bool gamma = false);

/*class Model {
    public : 

    int resolution = 8;
    std::vector<unsigned short> indices; //Triangles concaténés dans une liste
    std::vector<std::vector<unsigned short> > triangles;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<float> textureData;

    Model(std::string filename) {
        loadOFF(filename, indexed_vertices, indices, triangles )
    }

    /*Model(std::vector<unsigned short> indices, std::vector<glm::vec3> indexed_vertices, std::vector<float> textureData) {
        this->indices = indices;
        this->indexed_vertices = indexed_vertices;
        this->textureData = textureData;
    }

    void generatePlan(float length, int resolution) {

    float pas = length/(float)resolution;

    float positioni = 0, positionj;

    float nbSquare = pow(resolution, 2);

    float pasBasX = 0;
    float pasBasZ = 0;
    float pasHautX = pas;
    float pasHautZ = pas;

    int cpt = floor(nbSquare/resolution)-1;

    glm::vec3 basGauche, basDroit, hautGauche, hautDroit;

    for (int elt = 0; elt < nbSquare; elt++) {

        basGauche = glm::vec3(pasBasX, 0, pasBasZ);
        basDroit = glm::vec3(pasHautX, 0, pasBasZ);

        hautDroit = glm::vec3(pasHautX, 0, pasHautZ);
        hautGauche = glm::vec3(pasBasX, 0, pasHautZ);

        this->indexed_vertices.push_back(basGauche);
        this->indexed_vertices.push_back(basDroit);
        this->indexed_vertices.push_back(hautDroit);
        this->indexed_vertices.push_back(hautGauche);

        if (elt == cpt) {
            pasBasX -= pas*(resolution-1);
            pasHautX -= pas*(resolution-1);

            pasBasZ += pas;
            pasHautZ += pas;

            cpt += resolution;
        }
        else {
            pasBasX += pas;
            pasHautX += pas;
        }
    }

}

void generateTriangle(int resolution) {

    for (int i = 0; i < pow(resolution, 2)*4; i+=4) {

        this->indices.push_back(i);
        this->indices.push_back(i+1);
        this->indices.push_back(i+2);

        this->indices.push_back(i);
        this->indices.push_back(i+2);
        this->indices.push_back(i+3);

    }

}

void generateTextureCoords(float length, int resolution) {

    this->textureData.resize(resolution * resolution * 8);

    float pas = length/(float)resolution;

    float positioni = 0, positionj;

    float nbSquare = pow(resolution, 2);

    float pasBasX = 0;
    float pasBasZ = 0;
    float pasHautX = pas;
    float pasHautZ = pas;

    int cpt = floor(nbSquare/resolution)-1;

    int elt = 0;

    glm::vec3 basGauche, basDroit, hautGauche, hautDroit;

    for (int indexTexture = 0; indexTexture < resolution * resolution * 8; indexTexture+=8) {

        this->textureData[indexTexture] = pasBasX;
        this->textureData[indexTexture+1] = pasBasZ;
        this->textureData[indexTexture+2] = pasHautX;
        this->textureData[indexTexture+3] = pasBasZ;
        this->textureData[indexTexture+4] = pasHautX;
        this->textureData[indexTexture+5] = pasHautZ;
        this->textureData[indexTexture+6] = pasBasX;
        this->textureData[indexTexture+7] = pasHautZ;

        if (elt == cpt) {
            pasBasX -= pas*(resolution-1);
            pasHautX -= pas*(resolution-1);

            pasBasZ += pas;
            pasHautZ += pas;

            cpt += resolution;
        }
        else {
            pasBasX += pas;
            pasHautX += pas;
        }

        elt++;
    }

}

};*/