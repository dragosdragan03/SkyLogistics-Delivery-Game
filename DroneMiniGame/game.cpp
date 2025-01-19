#include "game.h"

#include "lab_m1/DroneMiniGame/game.h"
#include "lab_m1/DroneMiniGame/objects.h"
#include "Transform3d.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Game::Game()
{
    // variables for drone
    lengthParalelipiped = 0.05f;
    widthParalelipiped = 1.0f;
    heightParalelipiped = 0.05f;
    velocityPropeller = 0.005f;
    dronePosition = glm::vec3(5.0f);
    fixed = false;
    speed = false;
    additionalHeightCrate = 0;

    // variables for silo
    heightSilo = 8.0f;
    widthSilo = 3.5f;

    // variables for helipad and crate
    replaceHelipad = true;
    tookCrate = false;
    dropCrate = false;
    score = 0;
    maxScore = 5;
}


Game::~Game()
{
}

void Game::Init()
{
    polygonMode = GL_FILL;
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DroneMiniGame", "textureShaders");
    camera = new implemented::DroneCamera();
    miniMapCamera = new implemented::DroneCamera();

    glm::vec3 cameraPos = glm::vec3(8.0f, 7, 0.0f);

    camera->Set(cameraPos, dronePosition, glm::vec3(0, 1, 0));
    camera->distanceToTarget = glm::length(cameraPos - dronePosition);

    glm::vec3 miniMapCameraPos = dronePosition + glm::vec3(0.0f, 10.0f, 0.0f);

    miniMapCamera->Set(miniMapCameraPos, dronePosition, glm::vec3(0, 1, 0));
    miniMapCamera->distanceToTarget = glm::length(miniMapCameraPos - dronePosition);

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "crate.jpg").c_str(), GL_REPEAT);
        mapTextures["crate"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "silo2.jpg").c_str(), GL_REPEAT);
        mapTextures["silo"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "helipad.jpg").c_str(), GL_REPEAT);
        mapTextures["helipad"] = texture;
    }

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader *shader = new Shader("TextureShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DroneMiniGame", "textureShaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DroneMiniGame", "textureShaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader *shader = new Shader("CustomShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DroneMiniGame", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "DroneMiniGame", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    Mesh* paralelipiped1 = objects::CreateParalelipiped(glm::vec3(52.0f/255.0f, 52.0f/255.0f, 52.0f/255.0f), "paralelipipedDrone", lengthParalelipiped, widthParalelipiped, heightParalelipiped);
    AddMeshToList(paralelipiped1);

    Mesh* extensions = objects::CreateParalelipiped(glm::vec3(175.0f/255.0f, 54.0f/255.0f, 60.0f/255.0f), "extensions", lengthParalelipiped, widthParalelipiped/15, heightParalelipiped);
    AddMeshToList(extensions);

    Mesh* propeller = objects::CreateParalelipiped(glm::vec3(52.0f/255.0f, 52.0f/255.0f, 52.0f/255.0f), "propeller", lengthParalelipiped, widthParalelipiped/3, heightParalelipiped);
    AddMeshToList(propeller);

    Mesh* tree = objects::CreateTree(glm::vec3(0, 100/255.0f, 0), glm::vec3(130.0f/255.0f, 90.0f/255.0f, 44.0f/255.0f), "tree", 5.0f, 1.5f);
    AddMeshToList(tree);

    Mesh* field = objects::CreateField("field", glm::vec3(1, 1, 1), 150, 150);
    AddMeshToList(field);

    Mesh* silo = objects::CreateSilo("silo", glm::vec3(100.0f/255.0f, 100.0f/255.0f, 100.0f/255.0f), widthSilo, heightSilo);
    AddMeshToList(silo);

    Mesh* helipad = objects::CreateHelipad("helipad", glm::vec3(0,0,0), 2.5f, 1.5f);
    AddMeshToList(helipad);

    Mesh* checkpoint = objects::CreateParalelipiped(glm::vec3(1, 0, 0), "checkpoint", 50.0f, 0.2f, 0.2f);
    AddMeshToList(checkpoint);

    Mesh* pickup = objects::CreateParalelipiped(glm::vec3(0, 1, 0), "pickup", 50.0f, 0.2f, 0.2f);
    AddMeshToList(pickup);

    Mesh* arrow = objects::CreateArrow("arrow", glm::vec3(0, 0, 1), 0.12f, 0.5f);
    AddMeshToList(arrow);

    Mesh* border = objects::CreateBorder("border", glm::vec4(1, 1, 1, 0.5f), 0.8f, 0.15f);
    AddMeshToList(border);

    Mesh* score = objects::CreateBorder("score", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f), 0.8f, 0.15f);
    AddMeshToList(score);

    Mesh* minimapDrone = objects::CreateArrow("minimapDrone", glm::vec3(1, 0, 0), 0.5f, 2.0f);  // Red arrow, adjust size as needed
    AddMeshToList(minimapDrone);

    generateObjects();

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    glm::ivec2 resolution = window->GetResolution(true);
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
}

bool checkOverlap(const glm::vec3& newPos, const std::vector<glm::vec3>& existingTrees, const std::vector<glm::vec3>& existingSilos) {
    float minDist = 13.0f;

    for (const auto& pos : existingTrees) { // verify if already exist a tree
        if (glm::distance(glm::vec2(newPos.x, newPos.z), glm::vec2(pos.x, pos.z)) < minDist) {
            return true; // Overlap found
        }
    }

    for (const auto& pos : existingSilos) { // verify if alredy exsit a silo
        if (glm::distance(glm::vec2(newPos.x, newPos.z), glm::vec2(pos.x, pos.z)) < minDist) {
            return true; // Overlap found
        }
    }

    return false; // No overlap
}

void Game::generateObjects() {
    int numberOfTrees = 300;
    float border = 140;
    float x_min = -border, x_max = border;  // Field dimensions
    float z_min = -border, z_max = border;

    for (int i = 0; i < numberOfTrees; i++) {
        glm::vec3 newPos;
        newPos.x = x_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(x_max - x_min)));
        newPos.y = 0;
        newPos.z = z_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(z_max - z_min)));
        if (!checkOverlap(newPos, positionTree, positionSilo)) {
            positionTree.push_back(newPos);
        }
    }
    int numberOfSilos = 100;
    for (int i = 0; i < numberOfSilos; i++) {
        glm::vec3 newPos;
        newPos.x = x_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(x_max - x_min)));
        newPos.y = 0;
        newPos.z = z_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(z_max - z_min)));
        if (!checkOverlap(newPos, positionTree, positionSilo)) {
            positionSilo.push_back(newPos);
        }
    }
    cout << positionSilo.size() << " " << positionTree.size() << endl;
}

void Game::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.68, 0.85, 0.92, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution(true);
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Game::placePropellers(float deltaTimeSeconds, glm::mat4 droneMatrix) {
    // PROPELLERS
    glm::mat4 modelMatrix;
    velocityPropeller += deltaTimeSeconds * 10.0f;
    float disntace = widthParalelipiped/2.0f - widthParalelipiped / 30;

    modelMatrix = droneMatrix;
    modelMatrix *= Transform3D::Translate(0, 2 * heightParalelipiped, disntace);
    modelMatrix *= Transform3D::RotateOY(velocityPropeller);
    RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);

    modelMatrix = droneMatrix;
    modelMatrix *= Transform3D::Translate(0, 2 * heightParalelipiped, -disntace);
    modelMatrix *= Transform3D::RotateOY(velocityPropeller);
    RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);

    modelMatrix = droneMatrix;
    modelMatrix *= Transform3D::RotateOY(glm::radians(90.0f));
    modelMatrix *= Transform3D::Translate(0, 2 * heightParalelipiped, -disntace);
    modelMatrix *= Transform3D::RotateOY(velocityPropeller);
    RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);

    modelMatrix = droneMatrix;
    modelMatrix *= Transform3D::RotateOY(glm::radians(90.0f));
    modelMatrix *= Transform3D::Translate(0, 2 * heightParalelipiped, disntace);
    modelMatrix *= Transform3D::RotateOY(velocityPropeller);
    RenderMesh(meshes["propeller"], shaders["VertexColor"], modelMatrix);
}

void Game::placeExtensions(float deltaTimeSeconds, glm::mat4 droneMatrix) {
    glm::mat4 modelMatrix = glm::mat4(1);
    float disntace = widthParalelipiped/2.0f - widthParalelipiped / 30;

    modelMatrix = droneMatrix;
    modelMatrix *= Transform3D::Translate(0, heightParalelipiped, disntace);
    RenderMesh(meshes["extensions"], shaders["VertexColor"], modelMatrix);

    modelMatrix = droneMatrix;
    modelMatrix *= Transform3D::Translate(0, heightParalelipiped, -disntace);
    RenderMesh(meshes["extensions"], shaders["VertexColor"], modelMatrix);

    modelMatrix = droneMatrix;
    modelMatrix *= Transform3D::RotateOY(glm::radians(90.0f));
    modelMatrix *= Transform3D::Translate(0, heightParalelipiped, disntace);
    RenderMesh(meshes["extensions"], shaders["VertexColor"], modelMatrix);

    modelMatrix = droneMatrix;
    modelMatrix *= Transform3D::RotateOY(glm::radians(90.0f));
    modelMatrix *= Transform3D::Translate(0, heightParalelipiped, -disntace);
    RenderMesh(meshes["extensions"], shaders["VertexColor"], modelMatrix);
}

void Game::collision(glm::vec3 objectPos, float collisionThreshold) {

    glm::vec2 pushDirection = glm::normalize(
                glm::vec2(dronePosition.x, dronePosition.z) -
                glm::vec2(objectPos.x, objectPos.z)
            );

    dronePosition.x = objectPos.x + pushDirection.x * (collisionThreshold + 0.05f);
    dronePosition.z = objectPos.z + pushDirection.y * (collisionThreshold + 0.05f);

    // Update camera position while maintaining orientation
    glm::vec3 cameraForward = camera->forward;
    glm::vec3 cameraUp = camera->up;
    camera->position = dronePosition - (cameraForward * camera->distanceToTarget);
    camera->forward = cameraForward;
    camera->up = cameraUp;
}

bool Game::isDroneInsideSilo() {
    float droneRadius = widthParalelipiped / 2;

    for(const auto& siloPos : positionSilo) {
        float siloRadius;

        if(dronePosition.y < heightSilo * 2 / 3) {
            siloRadius = widthSilo;
        } else if(dronePosition.y  - additionalHeightCrate < heightSilo) {
            siloRadius = widthSilo * (1.0f - (dronePosition.y - additionalHeightCrate - heightSilo * 2 / 3) / (heightSilo * 1 / 3));
        } else {
            continue;
        }

        float horizontalDistance = glm::distance(
            glm::vec2(dronePosition.x, dronePosition.z),
            glm::vec2(siloPos.x, siloPos.z)
        );

        // Combined collision radius
        float collisionThreshold = siloRadius + droneRadius; // the 0.15 is for the propeller

        if (horizontalDistance < collisionThreshold) { // this means the drone hits the tree
            collision(siloPos, collisionThreshold);
            return true;
        }
    }
    return false;
}

bool Game::isDroneInsideTree() {
    const float height = 5.0f; // height of the tree
    const float baseWidth = 1.5f;
    const float eqDistance = height / 4.0f;

    // Use the longest parallelpiped dimension as the drone's radius
    float droneRadius = widthParalelipiped / 2;

    for (const auto& treePos : positionTree) {

        float treeRadius;

        // this is the trunk of the tree
        if (dronePosition.y <= eqDistance) {
            treeRadius = baseWidth * 0.5f;
        }
        else if (dronePosition.y - additionalHeightCrate <= 2.0f * eqDistance) {
            // First cone (widest)
            treeRadius = baseWidth * 1.5f * (1.0f - (dronePosition.y - additionalHeightCrate - eqDistance) / (1.5f * eqDistance));
        }
        else if (dronePosition.y - additionalHeightCrate <= 3.0f * eqDistance) {
            // Second cone
            treeRadius = baseWidth * 1.2f * (1.0f - (dronePosition.y - additionalHeightCrate - 2.5f * eqDistance) / eqDistance);
        }
        else if (dronePosition.y - additionalHeightCrate <= 5.0f * eqDistance) {
            // Third cone (top)
            treeRadius = baseWidth * (1.0f - (dronePosition.y - additionalHeightCrate - 3.5f * eqDistance) / (1.5f * eqDistance));
        }
        else {
            continue; // Above the tree
        }

        // Calculate horizontal distance from drone to tree center
        float horizontalDistance = glm::distance(
            glm::vec2(dronePosition.x, dronePosition.z),
            glm::vec2(treePos.x, treePos.z)
        );

        // Combined collision radius
        float collisionThreshold = treeRadius + droneRadius; // the 0.15 is for the propeller

        if (horizontalDistance < collisionThreshold) { // this means the drone hits the tree
            collision(treePos, collisionThreshold);
            return true;
        }
    }
    return false;
}

bool Game::isDroneInsideCrate() {

    bool insideY = dronePosition.y > (cratePosition.y - 0.5f) && dronePosition.y < (cratePosition.y + 0.5f); // the dimensions for the crate+helipad
    bool insideX = dronePosition.x > (cratePosition.x - 0.5f) && dronePosition.x < (cratePosition.x + 0.5f); // 0.5 is the dimension for the box
    bool insideZ = dronePosition.z > (cratePosition.z - 0.5f) && dronePosition.z < (cratePosition.z + 0.5f);

    return insideX && insideY && insideZ;
}

bool Game::isDroneInsideDropArea() {
    float distanceToPoint = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(helipadDropPosition.x, helipadDropPosition.z));
    return (distanceToPoint < 2.5f) && (dronePosition.y < 15.0f); // this is the radius of the helipad
}

float random(glm::vec2 st) {
    float dotProduct = glm::dot(st, glm::vec2(12.9998f, 78.233f));
    return glm::fract(sin(dotProduct) * 43758.5453123f);
}

float noise(glm::vec2 st) {
    glm::vec2 i = glm::floor(st);
    glm::vec2 f = glm::fract(st);

    // Four corners of a tile, exactly as in shader
    float a = random(i);
    float b = random(i + glm::vec2(1.0f, 0.0f));
    float c = random(i + glm::vec2(0.0f, 1.0f));
    float d = random(i + glm::vec2(1.0f, 1.0f));

    // Cubic Hermite curve: f*f*(3.0-2.0*f)
    glm::vec2 u = f * f * (3.0f - 2.0f * f);

    // Mix 4 corners exactly as in shader
    return glm::mix(a, b, u.x) +
           (c - a) * u.y * (1.0f - u.x) +
           (d - b) * u.x * u.y;
}

float getTerrainHeight(float x, float z) {
    // Match shader scaling: vec2 st = vec2(worldPos.x, worldPos.z) * 0.1;
    glm::vec2 st = glm::vec2(x, z) * 0.1f;

    float noiseValue = noise(st * 1.5f) * 1.3f;

    return noiseValue;  // This is the additional height above base terrain
}

void Game::generateDrone(float deltaTimeSeconds) {

    {
        glm::mat4 modelMatrix;
        glm::mat4 droneMatrix = glm::mat4(1);

        glm::vec3 dir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z)); // vector paralel cu XOZ
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 axa = glm::normalize(glm::cross(up, dir)); // pentru a vedea pe ce axa rotesc
        float terrainHeight = getTerrainHeight(dronePosition.x, dronePosition.z) + 0.5f;
        if (tookCrate) {
            terrainHeight += 1.0f;
        }

        if (dronePosition.y > terrainHeight + 0.2f && fixed) {
            if(!isDroneInsideTree() && !isDroneInsideSilo()) {
                float newHeight = dronePosition.y - glm::vec3(0,1,0).y * deltaTimeSeconds * 1.5f;

                if (newHeight > terrainHeight + 0.2f) {
                    camera->TranslateUpward(1.5f * -deltaTimeSeconds);
                    dronePosition.y = newHeight;
                } else {
                    float dif = terrainHeight - dronePosition.y;
                    dronePosition.y = terrainHeight;
                    camera->position.y += dif;
                }
            }
        } else if(dronePosition.y < terrainHeight) {
            float dif = terrainHeight - dronePosition.y;
            dronePosition.y = terrainHeight;
            camera->position.y += dif;
        }

        droneMatrix *= Transform3D::Translate(dronePosition.x, dronePosition.y, dronePosition.z);
        //here i should rotate the drone

        droneMatrix = glm::rotate(droneMatrix, inclineForwardBckward ,axa);
        droneMatrix = glm::rotate(droneMatrix, inclineRightLeft, camera->forward);

        modelMatrix = droneMatrix;
        RenderMesh(meshes["paralelipipedDrone"], shaders["VertexColor"], modelMatrix);

        modelMatrix = droneMatrix;
        modelMatrix *= Transform3D::RotateOY(glm::radians(90.0f));
        RenderMesh(meshes["paralelipipedDrone"], shaders["VertexColor"], modelMatrix);

        if(tookCrate && !dropCrate) { // this means is attached to the drone
            modelMatrix = droneMatrix;
            modelMatrix *= Transform3D::Translate(0, -0.5f - 0.05f / 2, 0); // 0.5f is the dimension of the crate and 0.05/2 is the height of the drone
            RenderSimpleMesh(meshes["box"], shaders["TextureShader"], modelMatrix, mapTextures["crate"]);
            RenderArrow(meshes["arrow"], shaders["VertexColor"], helipadDropPosition);
        }

        if(dropCrate && cratePosition.y > 2.0f) { // until it heats the ground
            modelMatrix = glm::mat4(1);
            modelMatrix *= Transform3D::Translate(cratePosition.x, cratePosition.y, cratePosition.z);
            RenderSimpleMesh(meshes["box"], shaders["TextureShader"], modelMatrix, mapTextures["crate"]);
            cratePosition.y -= 1.5f * deltaTimeSeconds;
        } else if(dropCrate && cratePosition.y < 2.0f) { // this means was delivered so should i render agan the drop zone
            replaceHelipad = true;
            tookCrate = false;
            dropCrate = false;
            score++;
        }

        // for extentions
        placeExtensions(deltaTimeSeconds, droneMatrix);
        placePropellers(deltaTimeSeconds, droneMatrix);
    }
}

void Game::placeHelipadAndCrate() {
    float border = 140;
    float x_min = -border, x_max = border;  // Field dimensions
    float z_min = -border, z_max = border;

    glm::vec3 newPos;

    do {
        newPos.x = x_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(x_max - x_min)));
        newPos.y = 0;
        newPos.z = z_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(z_max - z_min)));
    } while (checkOverlap(newPos, positionTree, positionSilo));
    helipadPosition = newPos;
    cratePosition = helipadPosition;
    cratePosition.y = 2.0f; // is placed in center of the box

    do {
        newPos.x = x_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(x_max - x_min)));
        newPos.y = 0;
        newPos.z = z_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(z_max - z_min)));
    } while (checkOverlap(newPos, positionTree, positionSilo));
    helipadDropPosition = newPos;
    // based on this position i have to calculate the position of the crate
}

void Game::generateGame() {
    RenderMeshCustom(meshes["field"], shaders["CustomShader"], glm::mat4(1));

    if(replaceHelipad) {
        placeHelipadAndCrate();
        replaceHelipad = false;
    }

    glm::mat4 modelMatrix = Transform3D::Translate(helipadPosition.x, 0, helipadPosition.z);
    RenderSimpleMesh(meshes["helipad"], shaders["TextureShader"], modelMatrix, mapTextures["helipad"]);

    modelMatrix = Transform3D::Translate(helipadPosition.x, 0, helipadPosition.z);
    modelMatrix *= Transform3D::RotateOZ(glm::radians(90.0f));
    RenderMesh(meshes["pickup"], shaders["VertexColor"], modelMatrix);

    modelMatrix = Transform3D::Translate(helipadDropPosition.x, 0, helipadDropPosition.z);
    RenderSimpleMesh(meshes["helipad"], shaders["TextureShader"], modelMatrix, mapTextures["helipad"]);

    modelMatrix = Transform3D::Translate(helipadDropPosition.x, 0, helipadDropPosition.z);
    modelMatrix *= Transform3D::RotateOZ(glm::radians(90.0f));
    RenderMesh(meshes["checkpoint"], shaders["VertexColor"], modelMatrix);

    if(tookCrate == false && !dropCrate) { // this means should stay on the ground
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(helipadPosition.x, 0, helipadPosition.z));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 2.0f, 0)); // 0.5 is the crate and the 1.5 is the helipad
        RenderSimpleMesh(meshes["box"], shaders["TextureShader"], modelMatrix, mapTextures["crate"]);
        RenderArrow(meshes["arrow"], shaders["VertexColor"], helipadPosition);
    }

    for(int i = 0; i < positionTree.size();i++) {
        glm::mat4 modelMatrix;
        modelMatrix = Transform3D::Translate(positionTree[i].x, 0, positionTree[i].z);
        RenderMesh(meshes["tree"], shaders["VertexColor"], modelMatrix);
    }

    for(int i = 0; i < positionSilo.size();i++) {
        glm::mat4 modelMatrix;
        modelMatrix = Transform3D::Translate(positionSilo[i].x, 0, positionSilo[i].z);
        // RenderMesh(meshes["silo"], shaders["VertexColor"], modelMatrix);
        RenderSimpleMesh(meshes["silo"], shaders["TextureShader"], modelMatrix, mapTextures["silo"]);
    }
}

void Game::generateMinimap() {

    // Update minimap camera with correct orientation
    miniMapCamera->position = dronePosition + glm::vec3(0, 20.0f, 0);
    miniMapCamera->forward = glm::vec3(0, -1, 0);    // Looking down
    miniMapCamera->up = glm::vec3(0, 0, 1);          // Changed to positive Z for correct minimap orientation

    // Store main camera and projection
    auto mainCamera = camera;
    auto mainProjection = projectionMatrix;

    // Switch to minimap camera and projection
    camera = miniMapCamera;
    projectionMatrix = glm::ortho(-30.0f, 30.0f,
                                 -30.0f, 30.0f,
                                 0.01f, 200.0f);

    // Render scene for minimap
    generateGame();

    // Render drone arrow indicator
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition);

        glm::vec3 droneDir = glm::normalize(glm::vec3(mainCamera->forward.x, 0, mainCamera->forward.z));

        float angle = atan2(droneDir.z, droneDir.x);

        modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));

        modelMatrix = glm::rotate(modelMatrix, -angle, glm::vec3(0, 0, 1));

        modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0, 0, 1));

        modelMatrix = scale(modelMatrix, glm::vec3(2, 2, 2));
        // Render the arrow
        RenderMesh(meshes["minimapDrone"], shaders["VertexColor"], modelMatrix);
    }

    // Restore main camera and projection
    camera = mainCamera;
    projectionMatrix = mainProjection;
}

void Game::Update(float deltaTimeSeconds)
{
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // Main viewport rendering
    glm::ivec2 resolution = window->GetResolution(true);
    glViewport(0, 0, resolution.x, resolution.y);

    generateDrone(deltaTimeSeconds);
    generateGame();
    placeBorderAndScore();

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    generateMinimap();

}

void Game::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Game::RenderArrow(Mesh *mesh, Shader *shader, glm::vec3 helipad) {

    // this is to calculte the vector to helipad from the drone
    glm::vec2 directionToHelipad = glm::normalize(
        glm::vec2(helipad.x - dronePosition.x, helipad.z - dronePosition.z)
    );

    // to calculate the camera->forward vector, based on this i have to rotate the arrow
    glm::vec2 cameraDirection = glm::normalize(
        glm::vec2(camera->forward.x, camera->forward.z)
    );

    // Calculate the angle between camera direction and helipad direction
    float angle = atan2(directionToHelipad.y, directionToHelipad.x) - // this is to calculte the angle of the directionToHelipad
                 atan2(cameraDirection.y, cameraDirection.x); // this is to calculate the angle of the camera
    // by substracting them i find the angle between them

    // Create model matrix for the arrow
    glm::mat4 arrowMatrix = glm::mat4(1);

    // Position the arrow at a fixed height
    arrowMatrix = glm::translate(arrowMatrix, glm::vec3(0, 2.1f, 0));

    // Rotate the arrow based on the calculated angle
    arrowMatrix = glm::rotate(arrowMatrix, -angle, glm::vec3(0, 1, 0)); // i have to rotate it based on the angle calculated between the camera->forward and the directionTohelipad

    // Apply any additional transformations you need for the arrow's orientation
    arrowMatrix = glm::rotate(arrowMatrix, glm::radians(-110.0f), glm::vec3(1, 0, 0)); // to incline the arrow because is created with the base on 0,0

    RenderObjectView(mesh, shader, arrowMatrix);
}

void Game::placeBorderAndScore() {

    // glm::ivec2 resolution = window->GetResolution(true);
    // cout << resolution.x << " " << resolution.y << endl;
    glm::mat4 modelMatrix = glm::mat4(1);
    // modelMatrix *= Transform3D::Translate(resolution.x / 1000, resolution.y / 1000, 0);
    modelMatrix *= Transform3D::Translate(1.4f, 2.0f, 0);
    modelMatrix *= Transform3D::RotateOY(glm::radians(90.0f));
    RenderObjectView(meshes["border"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat4(1);
    // modelMatrix *= Transform3D::Translate(resolution.x / 1000, resolution.y / 1000, 0);
    modelMatrix *= Transform3D::Translate(1.4f, 2.0f, 0.0001f);
    modelMatrix *= Transform3D::RotateOY(glm::radians(90.0f));
    float scoreWidth = (static_cast<float>(score % (maxScore + 1)) / maxScore);
    modelMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, scoreWidth));
    RenderObjectView(meshes["score"], shaders["VertexColor"], modelMatrix);
}

// functio used for View (cone + health bar)
void Game::RenderObjectView(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) {
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    // Set the view and projection matrices
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

// function used for drone camera
void Game::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) {

    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

// function used for field
void Game::RenderMeshCustom(Mesh *mesh, Shader * shader, const glm::mat4 &modelMatrix)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Get shader location for uniform mat4 "Model"
    auto location = glGetUniformLocation(shader->program, "Model");

    // TODO(student): Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // TODO(student): Get shader location for uniform mat4 "View"
    // glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    // TODO(student): Set shader uniform "View" to viewMatrix
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // TODO(student): Get shader location for uniform mat4 "Projection"
    // glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();

    // TODO(student): Set shader uniform "Projection" to projectionMatrix
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

// function used for textures
void Game::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glUniform1i(shader->GetUniformLocation(""), 0);

    glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID()); // Bind texture1
    int loc_texture1 = glGetUniformLocation(shader->program, "texture_1");
    glUniform1i(loc_texture1, 0); // Send texture unit 0 to the shader

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

    // Unbind textures for cleanliness (optional, but recommended for debugging)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Game::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    // if(isDroneInsideCrate())
    //     cout << "este inauntru";
    float cameraSpeed = 5.0f;
    float droneSpeed = 10.0f;

    // if(isDroneInsideCrate())
    //     cout <<"a intrart";

    if(speed) {
        droneSpeed = 50.0f;
    }

    if (window->KeyHold(GLFW_KEY_W)) {
        if (isDroneInsideTree() || isDroneInsideSilo()) {
            return;
        }
        camera->MoveForward(droneSpeed * deltaTime);
        velocityPropeller += deltaTime * 10.0f;
        glm::vec3 dir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
        dronePosition += dir * deltaTime * droneSpeed;

        if(inclineForwardBckward < glm::radians(20.0f))
            inclineForwardBckward += glm::radians(1.0f);
    } else if (window->KeyHold(GLFW_KEY_S)) {
        if (isDroneInsideTree() || isDroneInsideSilo()) {
            return;
        }
        camera->MoveForward(-droneSpeed * deltaTime);
        velocityPropeller += deltaTime * 10.0f;
        glm::vec3 dir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
        dronePosition -= dir * deltaTime * droneSpeed;

        if(inclineForwardBckward > -glm::radians(20.0f))
            inclineForwardBckward -= glm::radians(1.0f);
    } else {
        if(inclineForwardBckward > glm::radians(0.0f))
            inclineForwardBckward -= glm::radians(0.5f);
        else if(inclineForwardBckward < glm::radians(0.0f))
            inclineForwardBckward += glm::radians(0.5f);
    }


    if (window->KeyHold(GLFW_KEY_A)) {
        // TODO(student): Translate the camera to the left
        if (isDroneInsideTree() || isDroneInsideSilo()) {
            return;
        }
        camera->TranslateRight(droneSpeed * -deltaTime);
        velocityPropeller += deltaTime * 10.0f;
        glm::vec3 dir = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
        dronePosition -= dir * deltaTime * droneSpeed;

        if(inclineRightLeft > -glm::radians(20.0f))
            inclineRightLeft -= glm::radians(1.0f);

    } else if (window->KeyHold(GLFW_KEY_D)) {
        if (isDroneInsideTree() || isDroneInsideSilo()) {
            return;
        }
        // TODO(student): Translate the camera to the right
        camera->TranslateRight(droneSpeed * deltaTime);
        velocityPropeller += deltaTime * 10.0f;
        glm::vec3 dir = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z));
        dronePosition += dir * deltaTime * droneSpeed;

        if(inclineRightLeft < glm::radians(20.0f))
            inclineRightLeft += glm::radians(1.0f);
    } else {
        if(inclineRightLeft > glm::radians(0.0f))
            inclineRightLeft -= glm::radians(0.5f);
        else if(inclineRightLeft < glm::radians(0.0f))
            inclineRightLeft += glm::radians(0.5f);
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        if (isDroneInsideTree() || isDroneInsideSilo()) return;

        float terrainHeight = getTerrainHeight(dronePosition.x, dronePosition.z);
        if (!dropCrate) terrainHeight += 1.0f;

        if (dronePosition.y < terrainHeight) return;

        dronePosition.y -= droneSpeed * deltaTime;
        camera->TranslateUpward(-droneSpeed * deltaTime);
        velocityPropeller += deltaTime * 10.0f;
    }
    // if (window->KeyHold(GLFW_KEY_Q)) {
    //     if (isDroneInsideTree() || isDroneInsideSilo()) {
    //         return;
    //     }
    //     float distanceToField = getTerrainHeight(dronePosition.x, dronePosition.z);
    //     if (!dropCrate) {
    //         distanceToField += 1.0f;
    //     }
    //     if(dronePosition.y < distanceToField) {
    //        return;
    //     }
    //     // TODO(student): Translate the camera downward
    //     camera->TranslateUpward(droneSpeed * -deltaTime);
    //     velocityPropeller += deltaTime * 10.0f;
    //     dronePosition -= glm::vec3(0,1,0) * deltaTime * droneSpeed;
    // }

    if (window->KeyHold(GLFW_KEY_E)) {
        if (isDroneInsideTree()) {
            return;
        }
        // TODO(student): Translate the camera upward
        camera->TranslateUpward(droneSpeed * deltaTime);
        velocityPropeller += deltaTime * 10.0f;
        dronePosition += glm::vec3(0,1,0) * deltaTime * droneSpeed;
    }

    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        camera->RotateThirdPerson_OY(cameraSpeed * -deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_LEFT)) {
        camera->RotateThirdPerson_OY(cameraSpeed * deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        camera->RotateThirdPerson_OX(cameraSpeed * deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_UP)) {
        camera->RotateThirdPerson_OX(cameraSpeed * -deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_ENTER)) {
        if(camera->distanceToTarget > 2) {
        camera->TranslateForward(cameraSpeed * deltaTime);
        camera->distanceToTarget = glm::length(camera->position - dronePosition);
        }
    }
    if (window->KeyHold(GLFW_KEY_RIGHT_SHIFT)) {
        if(camera->distanceToTarget < 13) {
        camera->TranslateForward(-cameraSpeed * deltaTime);
        camera->distanceToTarget = glm::length(camera->position - dronePosition);
        }
    }


}


void Game::OnKeyPress(int key, int mods)
{

    if(key == GLFW_KEY_F) {
        fixed = !fixed;
    }

    if(key == GLFW_KEY_P)
        speed = !speed;

    if(key == GLFW_KEY_SPACE) {
        if(isDroneInsideCrate() && !tookCrate) { // this is to take the crate
            tookCrate = true;
            dronePosition.y = cratePosition.y + 0.5f;
            additionalHeightCrate = 1.0f; // this is the "offset" of the crate
        }
        if(isDroneInsideDropArea() && tookCrate) {
            cratePosition = dronePosition;
            dropCrate = true;
            additionalHeightCrate = 0.0f;
        }
    }

    // if (key == GLFW_KEY_SPACE)
    // {
    //     switch (polygonMode)
    //     {
    //         case GL_POINT:
    //             polygonMode = GL_FILL;
    //         break;
    //         case GL_LINE:
    //             polygonMode = GL_POINT;
    //         break;
    //         default:
    //             polygonMode = GL_LINE;
    //         break;
    //     }
    // }
    // TODO(student): Switch projections

}


void Game::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Game::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {

            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OX(sensivityOY * -deltaY);
            camera->RotateFirstPerson_OY(sensivityOX * -deltaX);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {

            // TODO(student): Rotate the camera in third-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateThirdPerson_OX(sensivityOY * -deltaY);
            camera->RotateThirdPerson_OY(sensivityOX * -deltaX);
        }
    }
}


void Game::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Game::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Game::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Game::OnWindowResize(int width, int height)
{
}
