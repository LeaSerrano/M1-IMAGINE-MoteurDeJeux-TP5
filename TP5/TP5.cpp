// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/texture.hpp>

#include <string.h>

#include "SceneGraphNode.hpp"

void processInput(GLFWwindow *window);
/*void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xpos, double ypos);*/

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 camera_position   = glm::vec3(2.0f, 1.0f, 10.0f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400;
float lastY = 300;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float timeSpeed = 1.0;

//rotation
float angle = 0.;
float zoom = 1.;

bool display_cameraOrbitale = false;

bool PkeyIsPressed = false;
bool SemicolonkeyIsPressed = false;
bool CkeyIsPressed = false;
bool UpKeyIsPressed = false;
bool DownKeyIsPressed = false;
bool LeftKeyIsPressed = false;
bool RightKeyIsPressed = false;

float timeCst = 0;


std::vector<glm::vec3> indexed_vertices;
std::vector<std::vector<unsigned short> > triangles;
std::vector<float> textureData;
int resolution = 8;
std::vector<unsigned short> indices;

std::vector<glm::vec3> object_indexed_vertices;
std::vector<float> object_textureData;
std::vector<unsigned short> object_indices;

/*bool solarSytem = true;

SceneGraphNode* rootNodeMountainPlan = new SceneGraphNode(indexed_vertices, textureData, indices, false, true);
SceneGraphNode* moutainPlan = new SceneGraphNode(indexed_vertices, textureData, indices, false, true);*/

SceneGraphNode* rootNodeMovement = new SceneGraphNode();
GLfloat c1[] = {0.5, 0, 1};
SceneGraphNode* sphere = new SceneGraphNode(object_indexed_vertices, object_textureData, object_indices, false, false, c1);
GLfloat c2[] = {1, 0, 0};
SceneGraphNode* terrain = new SceneGraphNode(indexed_vertices, textureData, indices, false, false, c2);

/*******************************************************************************/
bool loadOBJ( const char *path ) {
    object_indexed_vertices.clear();
    object_indices.clear();

    printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<float> temp_uvs;
    std::vector<glm::vec3> temp_normals;


    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        getchar();
        return false;
    }

    while( 1 ){

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);

        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv.x);
            temp_uvs.push_back(uv.y);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                fclose(file);
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }

    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){
       object_indices.push_back(vertexIndices[i]);
    }

    for( unsigned int i=0; i<temp_vertices.size(); i++ ){
        object_indexed_vertices.push_back(temp_vertices[i]);
    }

    for( unsigned int i=0; i<temp_vertices.size()*2; i++ ){
        object_textureData.push_back(temp_uvs[i]);
    }

    fclose(file);
    return true;
}

std::vector<float> generateTextureCoords(float length, int resolution) {

    std::vector<float> texture;

    texture.resize(resolution * resolution * 8);

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

        texture[indexTexture] = pasBasX;
        texture[indexTexture+1] = pasBasZ;
        texture[indexTexture+2] = pasHautX;
        texture[indexTexture+3] = pasBasZ;
        texture[indexTexture+4] = pasHautX;
        texture[indexTexture+5] = pasHautZ;
        texture[indexTexture+6] = pasBasX;
        texture[indexTexture+7] = pasHautZ;

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

    return texture;

}


void createPlane(std::vector<vec3>& vertices, std::vector<unsigned short>& indices)
{
    const int size = 16;
    const float step = 6.f / (size - 1);

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            vec3 vertex(step * j, /*(float)(rand()) / ((float)(RAND_MAX/(0.5)))*/0, step * i);
            vertices.push_back(vertex);
        }
    }

    for (int i = 0; i < size - 1; ++i)
    {
        for (int j = 0; j < size - 1; ++j)
        {
            int index = i * size + j;
            indices.push_back(index);
            indices.push_back(index + size);
            indices.push_back(index + 1);

            indices.push_back(index + 1);
            indices.push_back(index + size);
            indices.push_back(index + size + 1);
        }
    }
}


vec3 calculateBarycentricCoords(vec3 point, vec3 v0, vec3 v1, vec3 v2)
{
    vec3 u = v1 - v0;
    vec3 v = v2 - v0;
    vec3 w = point - v0;

    float vCrossW = dot(v, w);
    float uCrossW = dot(u, w);
    float denominator = dot(u, u) * dot(v, v) - dot(u, v) * dot(v, u);

    float b2 = (dot(v, v) * uCrossW - dot(u, v) * vCrossW) / denominator;
    float b3 = (dot(u, u) * vCrossW - dot(u, v) * uCrossW) / denominator;
    float b1 = 1.0f - b2 - b3;

    return vec3(b1, b2, b3);
}

bool isPointInTriangle(vec3 point, vec3 v0, vec3 v1, vec3 v2)
{
    vec3 edge0 = v1 - v0;
    vec3 edge1 = v2 - v1;
    vec3 edge2 = v0 - v2;

    vec3 C0 = point - v0;
    vec3 C1 = point - v1;
    vec3 C2 = point - v2;

    vec3 cross0 = cross(edge0, C0);
    vec3 cross1 = cross(edge1, C1);
    vec3 cross2 = cross(edge2, C2);

    if (dot(cross0, cross1) > 0 && dot(cross0, cross2) > 0)
    {
        return true;
    }

    return false;
}

float getHeightAtPoint(vec3 point, std::vector<vec3> vertices, std::vector<unsigned short> indices)
{
    for (int i = 0; i < indices.size(); i += 3)
    {
        vec3 v0 = vertices[indices[i]];
        vec3 v1 = vertices[indices[i+1]];
        vec3 v2 = vertices[indices[i+2]];

        if (isPointInTriangle(point, v0, v1, v2))
        {
            vec3 barycentricCoords = calculateBarycentricCoords(point, v0, v1, v2);

            float height = barycentricCoords.x * v0.y + barycentricCoords.y * v1.y + barycentricCoords.z * v2.y;

            return height;
        }
    }

    return 0;
}

float placeObjectOnTerrain(vec3 objectPosition, std::vector<vec3> terrainVertices, std::vector<unsigned short> terrainIndices)
{
    float heightOffset = 0.1;
    float objectRadius = 0.4;

    objectPosition[1] -= objectRadius;
    float terrainHeight = getHeightAtPoint(objectPosition, terrainVertices, terrainIndices);


    float height = terrainHeight + heightOffset + objectRadius;

    return height;
}



int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "TP4 - GLFW", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    /*glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, scroll_callback);*/

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);


    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);


    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );

    /*****************TODO***********************/
    // Get a handle for our "Model View Projection" matrices uniforms

    /****************************************/
    //Chargement du fichier de maillage
    /*std::string filename("cube.off");
    loadOFF(filename, indexed_vertices, indices, triangles );*/

    //TP4
    createPlane(indexed_vertices, indices);
    textureData = generateTextureCoords(1, resolution);

    terrain->updateMeshData(indexed_vertices, textureData, indices);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    loadOBJ("sphere.obj");
    sphere->updateMeshData(object_indexed_vertices, object_textureData, object_indices);
    sphere->transform.scale /= 2;
    sphere->transform.position.x += 1;
    sphere->transform.position.z += 3;
    sphere->transform.position.y = placeObjectOnTerrain(sphere->transform.position, indexed_vertices, indices);

    rootNodeMovement->addChild(sphere);
    rootNodeMovement->addChild(terrain);

    //std::cout << rootNodeMovement->children.size() << " "  << sphere->children.size() << " " << terrain->children.size() << std::endl;

    //Solar system
    /*loadOBJ("sphere.obj");

    SceneGraphNode* rootNodeSolarSystem, *sun, *earth, *moon;

    if (solarSytem) {
        rootNodeSolarSystem = new SceneGraphNode(indexed_vertices, textureData, indices, solarSytem, true);

        float sunScale = 0.75;
        sun = new SceneGraphNode(indexed_vertices, textureData, indices, solarSytem, true, 0);
        sun->transform.position.x = 1;
        sun->transform.scale = {sunScale, sunScale, sunScale};

        float earthScale = 0.5;
        earth = new SceneGraphNode(indexed_vertices, textureData, indices, solarSytem, true, 1);
        earth->transform.position.x = 3.5;
        earth->transform.scale = {earthScale, earthScale, earthScale};

        float moonScale = 0.2;
        moon = new SceneGraphNode(indexed_vertices, textureData, indices, solarSytem, true, 2);
        moon->transform.position.x = 5;
        moon->transform.scale = {moonScale, moonScale, moonScale};

        rootNodeSolarSystem->addChild(sun);
        sun->addChild(earth);
        earth->addChild(moon);

    }
    
    //Mountain plan
    else {
        indexed_vertices = generatePlan(1, resolution);
        indices = generateTriangle(resolution);
        textureData = generateTextureCoords(1, resolution);

        rootNodeMountainPlan->updateMeshData(indexed_vertices, textureData, indices);
        moutainPlan->updateMeshData(indexed_vertices, textureData, indices);
        rootNodeMountainPlan->addChild(moutainPlan);
    }*/


    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");


    // For speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    bool isSphereLoaded = true;
    bool isCubeLoaded = false;

    do{

        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);


        /*****************TODO***********************/
        // Model matrix : an identity matrix (model will be at the origin) then change

        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms

        /****************************************/
        glm::mat4 modelMatrix, viewMatrix, projectionMatrix;

        viewMatrix = glm::lookAt(camera_position, camera_position + camera_target, camera_up);
        projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1 , GL_FALSE, &viewMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1 , GL_FALSE, &projectionMatrix[0][0]);

        float distance = sqrt((camera_position[0] - sphere->transform.position.x) * (camera_position[0] - sphere->transform.position.x) +
                      (camera_position[1] - sphere->transform.position.y) * (camera_position[1] - sphere->transform.position.y) +
                      (camera_position[2] - sphere->transform.position.z) * (camera_position[2] - sphere->transform.position.z));

        if (distance < 5.0f && !isSphereLoaded) {
          loadOBJ("sphere.obj");
            sphere->updateMeshData(object_indexed_vertices, object_textureData, object_indices);
            isSphereLoaded = true;
            isCubeLoaded = false;

        } else if(distance >= 5.0f && !isCubeLoaded) {
          loadOBJ("cube.obj");
            sphere->updateMeshData(object_indexed_vertices, object_textureData, object_indices);
            isSphereLoaded = false;
            isCubeLoaded = true;
        }


        glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &sphere->transform.changeModelMatrix()[0][0]);
        sphere->drawScene(programID);

        glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &terrain->transform.changeModelMatrix()[0][0]);
        terrain->drawScene(programID);

        /*glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &rootNodeMovement->children[1]->transform.changeModelMatrix()[0][0]);
        rootNodeMovement->drawScene(programID);*/

        /*glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &sphere->children[0]->transform.changeModelMatrix()[0][0]);
        sphere->drawScene(programID);*/

        /*glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &rootNodeMovement->children[1]->transform.changeModelMatrix()[0][0]);
        rootNodeMovement->drawScene(programID);*/
        
        //Solar system
        /*if (solarSytem) {
            glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &earth->children[0]->transform.changeModelMatrix()[0][0]);
            earth->drawScene(programID);

            glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &sun->children[0]->transform.changeModelMatrix()[0][0]);
            sun->drawScene(programID);

            glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &rootNodeSolarSystem->children[0]->transform.changeModelMatrix()[0][0]);
            rootNodeSolarSystem->drawScene(programID);

            float earthRotationSpeed = 50.0f;
            glm::vec3 earthPositionRelativeToSun = earth->transform.position - sun->transform.position;
            glm::mat4 earthRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(earthRotationSpeed*deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            earthPositionRelativeToSun = glm::vec3(earthRotationMatrix * glm::vec4(earthPositionRelativeToSun, 1.0f));

            earth->transform.position = sun->transform.position + earthPositionRelativeToSun;
            earth->transform.rotation.x += 23*deltaTime;


            float moonRotationSpeed = 200.0f;
            glm::vec3 moonPositionRelativeToEarth = moon->transform.position - earth->transform.position;
            glm::mat4 moonRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(moonRotationSpeed*deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
            moonPositionRelativeToEarth = glm::vec3(moonRotationMatrix * glm::vec4(moonPositionRelativeToEarth, 1.0f));

            moon->transform.position = earth->transform.position + moonPositionRelativeToEarth;
            moon->transform.position.x += 0.07;
            moon->transform.rotation.x += 6*deltaTime;


            earth->update();
            sun->transform.rotation.y += 20*deltaTime;
            sun->update();
            rootNodeSolarSystem->update();
        }

        //Moutain plan
        else {
            if (!display_cameraOrbitale) {

                modelMatrix = glm::mat4(1.0f);
            }
            else if (display_cameraOrbitale) {

                timeCst += deltaTime*timeSpeed;

                modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

                modelMatrix = glm::rotate(modelMatrix, timeCst, glm::vec3(0, 1, 0));
            }

            glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, false, &modelMatrix[0][0]);

            rootNodeMountainPlan->drawScene(programID);
        }*/



        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    glDeleteProgram(programID);
    /*rootNodeSolarSystem->deleteBuffer();
    rootNodeMountainPlan->deleteBuffer();*/
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    //Camera zoom in and out
    float cameraSpeed = 2.5 * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) { //touche W en azerty
        camera_position += cameraSpeed * camera_target;

        std::cout << camera_position[2] << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { 
        camera_position -= cameraSpeed * camera_target;
    }
    //TODO add translations

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { //touche A
        camera_position -= glm::normalize(glm::cross(camera_position, camera_up)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_position += glm::normalize(glm::cross(camera_position, camera_up)) * cameraSpeed;
    }


    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !PkeyIsPressed) {
        resolution ++;

        /*indexed_vertices = generatePlan(1, resolution);
        indices = generateTriangle(resolution);
        textureData = generateTextureCoords(1, resolution);*/

        /*rootNodeMountainPlan->updateScene(indexed_vertices, textureData, indices);
        moutainPlan->updateScene(indexed_vertices, textureData, indices);*/

        PkeyIsPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
    {
        PkeyIsPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS && !SemicolonkeyIsPressed) { //M
        resolution --;

        /*indexed_vertices = generatePlan(1, resolution);
        indices = generateTriangle(resolution);
        textureData = generateTextureCoords(1, resolution);*/

        /*rootNodeMountainPlan->updateScene(indexed_vertices, textureData, indices);
        moutainPlan->updateScene(indexed_vertices, textureData, indices);*/

        SemicolonkeyIsPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_RELEASE)
    {
        SemicolonkeyIsPressed = false;
    }


    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !CkeyIsPressed) {
        display_cameraOrbitale = !display_cameraOrbitale;

        CkeyIsPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        CkeyIsPressed = false;
    }


    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !UpKeyIsPressed) {
        timeSpeed += 0.1;

        UpKeyIsPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
    {
        UpKeyIsPressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !DownKeyIsPressed) {
        timeSpeed -= 0.1;

        DownKeyIsPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
    {
        DownKeyIsPressed = false;
    }



    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS /*&& !LeftKeyIsPressed*/) {
        sphere->transform.position.x -= 0.05;
        sphere->update();

        sphere->transform.position.y = placeObjectOnTerrain(sphere->transform.position, indexed_vertices, indices);
        LeftKeyIsPressed = true;

        sphere->update();
    }
    /*else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
    {
        LeftKeyIsPressed = false;
    }*/

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS /*&& !RightKeyIsPressed*/) {
        sphere->transform.position.x += 0.05;
        sphere->update();

        sphere->transform.position.y = placeObjectOnTerrain(sphere->transform.position, indexed_vertices, indices);
        RightKeyIsPressed = true;

        sphere->update();
    }
    /*else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
    {
        RightKeyIsPressed = false;
    }*/

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        float distance = 4 * deltaTime;
        sphere->transform.position.x += distance;
        sphere->transform.position.y += distance;
        sphere->update();
    }


    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        terrain->transform.position.x -= 0.05;
        terrain->update();
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        terrain->transform.position.x += 0.05;
        terrain->update();
    }

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}