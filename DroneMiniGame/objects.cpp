//
// Created by Dragan Dragos on 10.12.2024.
//

#include "objects.h"

Mesh *objects::CreateParalelipiped(
    glm::vec3 color,
    const std::string &name,
    float width,
    float length,
    float height) {
    std::vector<VertexFormat> vertices =
    {
        // bigger surface of the paralelipiped (bottom-up parts)
        VertexFormat(glm::vec3(-width / 2, -height / 2, -length / 2), color),
        VertexFormat(glm::vec3(-width / 2, -height / 2, length / 2), color),
        VertexFormat(glm::vec3(width / 2, -height / 2, length / 2), color),
        VertexFormat(glm::vec3(width / 2, -height / 2, -length / 2), color),

        // up face of the paralelipiped
        VertexFormat(glm::vec3(-width / 2, height / 2, -length / 2), color),
        VertexFormat(glm::vec3(-width / 2, height / 2, length / 2), color),
        VertexFormat(glm::vec3(width / 2, height / 2, length / 2), color),
        VertexFormat(glm::vec3(width / 2, height / 2, -length / 2), color),
    };

    std::vector<unsigned int> indices =
    {
        // bottom face
        0, 1, 2,
        2, 3, 0,
        // front face
        0, 1, 5,
        5, 4, 0,
        // right face
        1, 2, 6,
        6, 5, 1,
        // left face
        0, 4, 3,
        3, 4, 7,
        // back face
        2, 3, 7,
        2, 7, 6,
        // up face
        4, 5, 6,
        6, 7, 4
    };
    Mesh *paralelipiped = new Mesh(name);
    paralelipiped->InitFromData(vertices, indices); // Assuming Mesh has this method
    return paralelipiped;
}

Mesh *objects::CreateTree(
    glm::vec3 colorLeaf,
    glm::vec3 colorRoot,
    const std::string &name,
    float height,
    float width
) {
    float eqDistance = height / 4;
    int numPoints = 200;
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // TODO: trunk of the tree
    float angleStep = 2.0f * glm::pi<float>() / numPoints;

    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;

        float pointX = 0.5f * width * sin(angle);
        float pointZ = 0.5f * width * cos(angle);

        vertices.push_back(VertexFormat(glm::vec3(pointX, 0.0f, pointZ), colorRoot));
    }

    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;

        float pointX = 0.5f * width * sin(angle);
        float pointZ = 0.5f * width * cos(angle);

        vertices.push_back(VertexFormat(glm::vec3(pointX, eqDistance, pointZ), colorRoot));
    }


    for (int i = 0; i < numPoints; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + numPoints);

        indices.push_back(numPoints + i);
        indices.push_back(i + 1);
        indices.push_back(numPoints + i + 1);
    }

    // to combine last and first
    indices.push_back(numPoints - 1);
    indices.push_back(0);
    indices.push_back(2 * numPoints - 1);

    indices.push_back(2 * numPoints - 1);
    indices.push_back(0);
    indices.push_back(numPoints);

    //TODO: BOTTOM Cone
    int offset = 2 * numPoints;
    //HIGHT OF THE BOTTOM CONE
    vertices.push_back(VertexFormat(glm::vec3(0, 2.5 * eqDistance, 0), colorLeaf));

    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;

        float pointX = 1.5f * width * sin(angle);
        float pointZ = 1.5f * width * cos(angle);

        vertices.push_back(VertexFormat(glm::vec3(pointX, eqDistance, pointZ), colorLeaf));
    }

    for (int i = offset; i < offset + numPoints; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(offset);
    }

    {
        indices.push_back(3 * numPoints);
        indices.push_back(offset);
        indices.push_back(offset + 1);
    }

    offset += numPoints + 1;

    // TODO: MIDDLE CONE
    vertices.push_back(VertexFormat(glm::vec3(0, 3.5f * eqDistance, 0), colorLeaf));
    // 601
    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;

        float pointX = 1.2f * width * sin(angle);
        float pointZ = 1.2f * width * cos(angle);

        vertices.push_back(VertexFormat(glm::vec3(pointX, 2 * eqDistance, pointZ), colorLeaf));
    }
    //801

    for (int i = offset; i < offset + numPoints; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(offset);
    }

    {
        indices.push_back(offset);
        indices.push_back(4 * numPoints + 1);
        indices.push_back(offset + 1);
    }

    offset += numPoints + 1;
    //TODO: LAST CONE
    vertices.push_back(VertexFormat(glm::vec3(0, 5.0f * eqDistance, 0), colorLeaf));
    // 601
    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;

        float pointX = 1.0f * width * sin(angle);
        float pointZ = 1.0f * width * cos(angle);

        vertices.push_back(VertexFormat(glm::vec3(pointX, 3.0f * eqDistance, pointZ), colorLeaf));
    }
    //1202

    for (int i = offset; i < offset + numPoints; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(offset);
    }

    {
        indices.push_back(offset);
        indices.push_back(5 * numPoints + 1);
        indices.push_back(offset + 1);
    }

    //a doua oara generez 360 de puncte pentru cercul de sus
    Mesh *tree = new Mesh(name);
    tree->InitFromData(vertices, indices); // Assuming Mesh has this method
    return tree;
}


Mesh *objects::CreateField(
    const std::string &name,
    glm::vec3 colorField,
    int length,
    int width) {

    std::vector<VertexFormat> vertices;

    for(int i = -length; i < length; i++) {
        for(int j = -width; j < width; j++) {
            vertices.push_back(VertexFormat(glm::vec3(i, 0, j), colorField));
            vertices.push_back(VertexFormat(glm::vec3(i, 0, j + 1), colorField));
            vertices.push_back(VertexFormat(glm::vec3(i + 1, 0, j + 1), colorField));
            vertices.push_back(VertexFormat(glm::vec3(i + 1, 0, j), colorField));
        }
    }
    std::vector<unsigned int> indices;

    for(int i = 0; i < 16 * width * length; i += 4) {
        // first triangle for a tile
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);

        // second triangle for a tile
        indices.push_back(i);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
    }

    Mesh *field = new Mesh(name);
    field->InitFromData(vertices, indices); // Assuming Mesh has this method
    return field;
}

Mesh *objects::CreateSilo(
    const std::string &name,
    glm::vec3 colorSilo,
    float widthSilo,
    float heightSilo) {

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    int numPoints = 200;

    float eqDistance = heightSilo / 3;
    float angleStep = 2.0f * glm::pi<float>() / numPoints;

    // Base circle
    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;

        float pointX = widthSilo * sin(angle);
        float pointZ = widthSilo * cos(angle);

        // Calculate texture coordinates
        // Map angle to U coordinate (0 to 1)
        float u = static_cast<float>(i) / numPoints;
        // V coordinate is 0 for bottom
        float v = 0.0f;

        vertices.push_back(VertexFormat(glm::vec3(pointX, 0.0f, pointZ), colorSilo, glm::vec3(0), glm::vec2(u, v)));
    }

    // Middle circle
    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;

        float pointX = widthSilo * sin(angle);
        float pointZ = widthSilo * cos(angle);

        // Calculate texture coordinates
        float u = static_cast<float>(i) / numPoints;
        // V coordinate is 0.5 for middle section
        float v = 0.5f;
        vertices.push_back(VertexFormat(glm::vec3(pointX, 2 * eqDistance, pointZ), colorSilo * 2.0f, glm::vec3(0), glm::vec2(u, v)));
    }

    // Indices for cylinder body remain the same
    for (int i = 0; i < numPoints; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + numPoints);

        indices.push_back(numPoints + i);
        indices.push_back(i + 1);
        indices.push_back(numPoints + i + 1);
    }

    // Combine last and first
    indices.push_back(numPoints - 1);
    indices.push_back(0);
    indices.push_back(2 * numPoints - 1);

    indices.push_back(2 * numPoints - 1);
    indices.push_back(0);
    indices.push_back(numPoints);

    // Top cone point
    vertices.push_back(VertexFormat(glm::vec3(0, heightSilo, 0), colorSilo * 2.0f, glm::vec3(0), glm::vec2(0.5f, 1.0f)));

    // Indices for cone
    for (int i = numPoints; i < 2 * numPoints; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(2 * numPoints);
    }

    // Last cone triangle
    indices.push_back(2 * numPoints - 1);
    indices.push_back(numPoints);
    indices.push_back(2 * numPoints);

    Mesh *silo = new Mesh(name);
    silo->InitFromData(vertices, indices);
    return silo;
}

Mesh *objects::CreateHelipad(
    const std::string &name,
    glm::vec3 colorHelipad,
    float radius,
    float height) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    int numPoints = 200;
    float angleStep = 2.0f * glm::pi<float>() / numPoints;

    // Bottom circle - no texture
    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;
        float pointX = radius * sin(angle);
        float pointZ = radius * cos(angle);

        vertices.push_back(VertexFormat(
            glm::vec3(pointX, 0.0f, pointZ),
            colorHelipad,
            glm::vec3(0),
            glm::vec2(0, 0)  // No texture
        ));
    }

    // top circle no texture
    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;
        float pointX = radius * sin(angle);
        float pointZ = radius * cos(angle);

        vertices.push_back(VertexFormat(
            glm::vec3(pointX, height, pointZ),
            colorHelipad,
            glm::vec3(0),
            glm::vec2(0, 0)  // No texture
        ));
    }

    // Indices for cylinder sides
    for (int i = 0; i < numPoints - 1; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + numPoints);

        indices.push_back(i + numPoints);
        indices.push_back(i + 1);
        indices.push_back(i + numPoints + 1);
    }

    // Close the cylinder sides
    indices.push_back(numPoints - 1);
    indices.push_back(0);
    indices.push_back(2 * numPoints - 1);

    indices.push_back(2 * numPoints - 1);
    indices.push_back(0);
    indices.push_back(numPoints);

    // Top circle - with texture coordinates
    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;
        float pointX = radius * sin(angle);
        float pointZ = radius * cos(angle);

        // Map texture coordinates for top circle points
        float u = (sin(angle) + 1.0f) * 0.5f;
        float v = (cos(angle) + 1.0f) * 0.5f;

        vertices.push_back(VertexFormat(
            glm::vec3(pointX, height, pointZ),
            colorHelipad,
            glm::vec3(0),
            glm::vec2(u, v)  // Texture coordinates only for top face
        ));
    }

    // Center point for top texture
    vertices.push_back(VertexFormat(
        glm::vec3(0, height, 0),
        colorHelipad,
        glm::vec3(0),
        glm::vec2(0.5f, 0.5f)  // Center of texture
    ));

    // Indices for textured top face
    for (int i = 2 * numPoints; i < 3 * numPoints; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(3 * numPoints);
    }
    // Close the top face
    indices.push_back(2 * numPoints);
    indices.push_back(3 * numPoints);
    indices.push_back(3 * numPoints - 1);


    Mesh *helipad = new Mesh(name);
    helipad->InitFromData(vertices, indices);
    return helipad;
}

Mesh *objects::CreateArrow(
    const std::string &name,
    glm::vec3 colorArrow,
    float radius,
    float height) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Number of segments in the base circle
    const int numSegments = 32;

    // Add the tip vertex
    vertices.push_back(VertexFormat(glm::vec3(0, height, 0), glm::vec3(0, 1, 0)));

    // Add the center of the base
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), colorArrow));

    // Generate the vertices for the base circle
    for (int i = 0; i < numSegments; i++) {
        float angle = 2.0f * M_PI * i / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back(VertexFormat(
            glm::vec3(x, 0, z),  // position
            colorArrow           // color
        ));
    }

    // Generate indices for the sides
    for (int i = 0; i < numSegments; i++) {
        // Current and next vertex on the base circle
        unsigned int current = i + 2;
        unsigned int next = (i + 1) % numSegments + 2;

        // Triangle from tip to base edge
        indices.push_back(0);         // tip
        indices.push_back(current);   // current base vertex
        indices.push_back(next);      // next base vertex

        // Triangle for the base
        indices.push_back(1);         // base center
        indices.push_back(next);      // next base vertex
        indices.push_back(current);   // current base vertex
    }

    // Create the mesh
    Mesh* arrow = new Mesh(name);
    arrow->InitFromData(vertices, indices);
    return arrow;
}

Mesh *objects::CreateBorder(
    const std::string &name,
    glm::vec4 color,
    float length,
    float height) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0, 0, 0), color),
        VertexFormat(glm::vec3(0, 0, length), color),
        VertexFormat(glm::vec3(0, height, length), color),
        VertexFormat(glm::vec3(0, height, 0), color),
    };
    std::vector<unsigned int> indices = {
    0, 1, 2,
    2, 3, 0
    };
    Mesh* arrow = new Mesh(name);
    arrow->InitFromData(vertices, indices);
    return arrow;
}