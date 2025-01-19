#pragma once

#include "components/simple_scene.h"
#include "lab_m1/DroneMiniGame/lab_camera.h"


namespace m1
{
    class Game : public gfxc::SimpleScene
    {
     public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };
        Game();
        ~Game();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMeshCustom(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix);
        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        // helpers
        bool isDroneInsideTree();
        bool isDroneInsideSilo();
        void collision(glm::vec3 objectPos, float collisionThreshold);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1);
        void RenderObjectView(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix);
        void placeBorderAndScore();
        void generateMinimap();
        void RenderArrow(Mesh *mesh, Shader *shader, glm::vec3 helipad);
        bool isDroneInsideCrate();
        bool isDroneInsideDropArea();
        void placeHelipadAndCrate();
        void generateDrone(float deltaTimeSeconds);
        void generateSilos();
        void generateGame();
        void generateObjects();
        void placeExtensions(float deltaTimeSeconds, glm::mat4 droneMatrix);
        void placePropellers(float deltaTimeSeconds, glm::mat4 droneMatrix);
        std::unordered_map<std::string, Texture2D *> mapTextures;

        //variables for trees
        std::vector<glm::vec3> positionTree;
        std::vector<glm::vec3> positionSilo;

        //variables for camera
        implemented::DroneCamera *camera;
        glm::mat4 projectionMatrix;
        implemented::DroneCamera *miniMapCamera;
        ViewportArea miniViewportArea;
        GLenum polygonMode;

        //variables for drone paraleilipiped
        float lengthParalelipiped;
        float widthParalelipiped;
        float heightParalelipiped;

        //variables for drone
        bool fixed;
        float velocityPropeller;
        glm::vec3 dronePosition;
        float inclineForwardBckward;
        float inclineRightLeft;
        bool speed;
        int score;
        int maxScore;
        float additionalHeightCrate;

        //variables for silo
        float widthSilo;
        float heightSilo;

        // variables for crate and silo
        bool replaceHelipad;
        bool tookCrate;
        bool dropCrate;
        glm::vec3 helipadPosition;
        glm::vec3 helipadDropPosition;
        glm::vec3 cratePosition;
    };
}   // namespace m1
