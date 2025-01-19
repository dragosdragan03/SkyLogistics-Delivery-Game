//
// Created by Dragan Dragos on 10.12.2024.
//
#pragma once

#include "core/gpu/mesh.h"


namespace objects {
    Mesh* CreateParalelipiped(glm::vec3 color, const std::string &name, float length, float width, float height);
    Mesh* CreateTree(glm::vec3 colorLeaf, glm::vec3 colorRoot, const std::string &name, float height, float width);
    Mesh* CreateField(const std::string &name, glm::vec3 colorField, int length, int width);
    Mesh* CreateSilo(const std::string &name, glm::vec3 colorSilo, float widthSilo, float heightSilo);
    Mesh* CreateHelipad(const std::string &name, glm::vec3 colorHelipad, float radius, float height);
    Mesh* CreateArrow(const std::string &name, glm::vec3 colorArrow, float radius, float height);
    Mesh* CreateBorder(const std::string &name, glm::vec4 color, float length, float height);
}

#pragma once


