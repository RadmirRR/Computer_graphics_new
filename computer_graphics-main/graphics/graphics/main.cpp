﻿#include <cstdlib>
#include <iostream>

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.hpp>

#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


GLfloat skyboxVertices[] = {
    // positions          
    -1,  1, -1,
    -1, -1, -1,
     1, -1, -1,
     1, -1, -1,
     1,  1, -1,
    -1,  1, -1,

    -1, -1,  1,
    -1, -1, -1,
    -1,  1, -1,
    -1,  1, -1,
    -1,  1,  1,
    -1, -1,  1,

     1, -1, -1,
     1, -1,  1,
     1,  1,  1,
     1,  1,  1,
     1,  1, -1,
     1, -1, -1,

    -1, -1,  1,
    -1,  1,  1,
     1,  1,  1,
     1,  1,  1,
     1, -1,  1,
    -1, -1,  1,

    -1,  1, -1,
     1,  1, -1,
     1,  1,  1,
     1,  1,  1,
    -1,  1,  1,
    -1,  1, -1,

    -1, -1, -1,
    -1, -1,  1,
     1, -1, -1,
     1, -1, -1,
    -1, -1,  1,
     1, -1,  1
};

GLfloat plane[] = {
    // pos           // tex     // normals
    -18, 0, -18,   0,   0,  0, 1, 0,
     18, 0,  18,   18, 18,  0, 1, 0,
     18, 0, -18,   18,  0,  0, 1, 0,
     18, 0,  18,   18, 18,  0, 1, 0,
    -18, 0, -18,   0,   0,  0, 1, 0,
    -18, 0,  18,   0,  18,  0, 1, 0
};

struct triangle {
    union {
        struct { glm::vec3 a, b, c; };
        glm::vec3 pts[3];
    };
    triangle(GLfloat x1, GLfloat y1, GLfloat z1,
        GLfloat x2, GLfloat y2, GLfloat z2,
        GLfloat x3, GLfloat y3, GLfloat z3) :
        a(x1, y1, z1), b(x2, y2, z2), c(x3, y3, z3) {}
    triangle() {}

    glm::vec3& operator[](int pos) { return pts[pos]; }
    const glm::vec3& operator[](int pos) const { return pts[pos]; }
};



GLfloat cubeVertices[] = {
    // positions  // texture    
    -1, -1, -1,   0, 0, // front
     1,  1, -1,   1, 1,
     1, -1, -1,   1, 0,
    -1, -1, -1,   0, 0,
    -1,  1, -1,   0, 1,
     1,  1, -1,   1, 1,

     1, -1,  1,   0, 0, // back
    -1,  1,  1,   1, 1,
    -1, -1,  1,   1, 0,
     1, -1,  1,   0, 0,
     1,  1,  1,   0, 1,
    -1,  1,  1,   1, 1,

    -1, -1,  1,   0, 0, // right
    -1,  1, -1,   1, 1,
    -1, -1, -1,   1, 0,
    -1, -1,  1,   0, 0,
    -1,  1,  1,   0, 1,
    -1,  1, -1,   1, 1,

     1, -1, -1,   0, 0, // left
     1,  1,  1,   1, 1,
     1, -1,  1,   1, 0,
     1, -1, -1,   0, 0,
     1,  1, -1,   0, 1,
     1,  1,  1,   1, 1,

    -1, -1,  1,   0, 0, // bottom
     1, -1, -1,   1, 1,
     1, -1,  1,   1, 0,
    -1, -1,  1,   0, 0,
    -1, -1, -1,   0, 1,
     1, -1, -1,   1, 1,

    -1,  1, -1,   0, 0, // up
     1,  1,  1,   1, 1,
     1,  1, -1,   1, 0,
    -1,  1, -1,   0, 0,
    -1,  1,  1,   0, 1,
     1,  1,  1,   1, 1
};

void calculateTBN(GLfloat* verticesUV, int triangles, GLfloat* verticesUV_TBN) {

    // for every triangle
    for (int i = 0; i < triangles; i++) {

        glm::vec3 pos1(verticesUV[i * 3 * 5], verticesUV[i * 3 * 5 + 1], verticesUV[i * 3 * 5 + 2]);
        glm::vec3 pos2(verticesUV[(i * 3 + 1) * 5], verticesUV[(i * 3 + 1) * 5 + 1], verticesUV[(i * 3 + 1) * 5 + 2]);
        glm::vec3 pos3(verticesUV[(i * 3 + 2) * 5], verticesUV[(i * 3 + 2) * 5 + 1], verticesUV[(i * 3 + 2) * 5 + 2]);

        glm::vec2 uv1(verticesUV[i * 3 * 5 + 3], verticesUV[i * 3 * 5 + 4]);
        glm::vec2 uv2(verticesUV[(i * 3 + 1) * 5 + 3], verticesUV[(i * 3 + 1) * 5 + 4]);
        glm::vec2 uv3(verticesUV[(i * 3 + 2) * 5 + 3], verticesUV[(i * 3 + 2) * 5 + 4]);

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float sign = deltaUV2.x * deltaUV1.y - deltaUV1.x * deltaUV2.y;

        glm::vec3 tangent(deltaUV2.y * edge1.x - deltaUV1.y * edge2.x, deltaUV2.y * edge1.y - deltaUV1.y * edge2.y, deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(sign * tangent);

        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        glm::vec3 bitangent2 = cross(normal, tangent);

        // for every point (3 per triangle)
        for (int j = 0; j < 3; j++) {
            int index = i * 3 + j;
            verticesUV_TBN[index * 14] = verticesUV[index * 5];         // x
            verticesUV_TBN[index * 14 + 1] = verticesUV[index * 5 + 1]; // y
            verticesUV_TBN[index * 14 + 2] = verticesUV[index * 5 + 2]; // z

            verticesUV_TBN[index * 14 + 3] = verticesUV[index * 5 + 3]; // u
            verticesUV_TBN[index * 14 + 4] = verticesUV[index * 5 + 4]; // v

            verticesUV_TBN[index * 14 + 5] = normal.x;
            verticesUV_TBN[index * 14 + 6] = normal.y;
            verticesUV_TBN[index * 14 + 7] = normal.z;

            verticesUV_TBN[index * 14 + 8] = tangent.x; // tanX
            verticesUV_TBN[index * 14 + 9] = tangent.y; // tanY
            verticesUV_TBN[index * 14 + 10] = tangent.z; // tanZ

            verticesUV_TBN[index * 14 + 11] = bitangent2.x;
            verticesUV_TBN[index * 14 + 12] = bitangent2.y;
            verticesUV_TBN[index * 14 + 13] = bitangent2.z;
        }
    }
}




// Fullscreen for post-effects
GLfloat fullscreenVertices[] = {
    //x  y  z    u  v
     -1, -1, 0,   0, 0,
      1, -1, 0,   1, 0,
     -1,  1, 0,   0, 1,
      1, -1, 0,   1, 0,
      1,  1, 0,   1, 1,
     -1,  1, 0,   0, 1
};

// Billboards
GLfloat billboardVertices[] = {
    //x  y  z    u  v
     -1, -1, 0,   0, 1,
      1, -1, 0,   1, 1,
     -1,  1, 0,   0, 0,
      1, -1, 0,   1, 1,
      1,  1, 0,   1, 0,
     -1,  1, 0,   0, 0
};

std::array<glm::vec3, 4> billboards = {
        glm::vec3(-1, 1, -4),
        glm::vec3(-1, .5f, -4),
        glm::vec3(-2, 1, -4),
        glm::vec3(-3, 1, -4)
};

GLuint loadCubemap(std::array<std::string,6>& faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

GLuint loadTexture(char const* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_uc* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        switch (nrComponents) {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: format = 0;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else std::cout << "Failed to load texture:" << std::endl << path << std::endl;

    stbi_image_free(data);

    return textureID;
}

GLuint TextureCreateEmpty(GLint internalFormat, GLenum format, GLsizei width, GLsizei height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat border[] = { 1,1,1,1 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    // create empty texture
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

    return texture;
}

constexpr int N = 8; // amount of corners in prism
constexpr int M = 5; // amount of corners in prism

constexpr int recursiveReflections = 3; // amount of recursive reflections
constexpr float shadowResolutionReduction = 0.3f; // amount of recursive reflections

class Graphics {
    float horizontalAngle = 0;
    float verticalAngle = 0;

    const float speed = 15.0f;
    const float mouseSpeed = 0.05f;

    float borderInitial = 0.15f; // percentage of the viewport affected by post effect

    // Camera
    glm::vec3 camPos;
    glm::vec3 camRight;
    glm::vec3 camUp;
    glm::vec3 camDir;
    glm::mat4 projection;
    glm::mat4 view;

    // light Camera
    glm::mat4 projectionLight = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -15.0f, 15.0f);
    glm::vec3 lightDirection;
    glm::mat4 viewLight;

    float beta=0; // needed for lightDirection rotation

    double lastTimePressedR = 0;

    glm::mat4 computeViewFromInputs(GLFWwindow* window, int width, int height) {

        // glfwGetTime is called only once, the first time this function is called
        static double lastTime = glfwGetTime();

        // Compute time difference between current and last frame
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);

        beta += 2 * deltaTime;
        if (beta > 2 * 3.14f)beta -= 2 * 3.14f;

        // Get mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Reset mouse position for next frame
        glfwSetCursorPos(window, width / 2, height / 2);

        // Compute new orientation
        horizontalAngle += mouseSpeed * float(width / 2 - xpos) * deltaTime;
        verticalAngle += mouseSpeed * float(height / 2 - ypos) * deltaTime;

        if (verticalAngle >= 3.14159f / 2.0f)verticalAngle = 3.14159f / 2.0f;
        else if (verticalAngle <= -3.14159f / 2.0f)verticalAngle = -3.14159f / 2.0f;

        // Direction : Spherical coordinates to Cartesian coordinates conversion
        camDir = glm::vec3(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );

        // camRight vector
        camRight = glm::vec3(
            sin(horizontalAngle - 3.14f / 2.0f),
            0,
            cos(horizontalAngle - 3.14f / 2.0f)
        );

        // Up vector
        camUp = normalize(glm::cross(camRight, camDir));

        // Move forward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camPos += camDir * deltaTime * speed;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camPos -= camDir * deltaTime * speed;
        }
        // Strafe camRight
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camPos += camRight * deltaTime * speed;
        }
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camPos -= camRight * deltaTime * speed;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camPos.y += deltaTime * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            borderInitial -= deltaTime*4;
            if (borderInitial < 0)borderInitial = 0;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            borderInitial += deltaTime*4;
            if (borderInitial > 1)borderInitial = 1;
        }

        // For the next frame, the "last time" will be "now"
        lastTime = currentTime;

        return glm::lookAt(
            camPos,           // Camera is here
            camPos + camDir, // and looks here : at the same position, plus "direction"
            camUp              // Head is up (set to 0,-1,0 to look upside-down)
        );
    }

    //shaders
    //===============
    Shader  normalMappingShader,
            postEffectShader,
            billboardShader,
            refractShader,
            reflectShader,
            skyboxShader,
            shader,
            shadowShader;

    std::array<std::string, 6> faces{
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    };

    GLuint  cubeVAO, skyboxVAO, planeVAO,       effectsVAO, billboardVAO,
            cubeVBO, skyboxVBO, planeVBO, effectsVBO, billboardVBO;

    GLFWwindow* window;
    float width;
    float height;

    glm::mat4 transparencyRedCubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(-3, 1.0f, 0));
    glm::mat4 transparencyBlueCubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(-3, 1.0f, 2.2f));

    glm::mat4 normalMappingCubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0, 2.0f, 0));
    glm::mat4 normalCubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(2.1f, 1.2f, 2.1f));

    glm::mat4 refractModel = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 1.0f, 0));
    glm::mat4 reflectModel = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 1.0f, -4.0f));
    glm::mat4 modelPrism = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.3f, -3.0f));
    glm::mat4 modelPrism2 = glm::translate(modelPrism, glm::vec3(0, 0, -5.0f));


    GLuint depthFBO, renderFBO;

    glm::vec4 planeFromTriangle(const triangle& triangle) {
        /*
        | x -x0  y -y0  z -z0 |
        | x1-x0  y1-y0  z1-z0 |
        | x2-x0  y2-y0  z2-z0 |
        */
        glm::vec3 delta10 = triangle[1] - triangle[0];
        glm::vec3 delta20 = triangle[2] - triangle[0];

        GLfloat A = delta10.y * delta20.z - delta10.z * delta20.y;
        GLfloat B = delta10.z * delta20.x - delta10.x * delta20.z;
        GLfloat C = delta10.x * delta20.y - delta10.y * delta20.x;

        GLfloat len = sqrt(A * A + B * B + C * C);

        A /= len;
        B /= len;
        C /= len;
        GLfloat D = -A * triangle[0].x - B * triangle[0].y - C * triangle[0].z;

        return glm::vec4(A, B, C, D);
    }

 


public:
    Graphics(const int w, const int h, GLFWwindow* win) :
        normalMappingShader("resources/shaders/normalmapping.vs", "resources/shaders/normalmapping.fs"),
        postEffectShader("resources/shaders/posteffect.vs", "resources/shaders/posteffect.fs"),
        refractShader("resources/shaders/refractReflect.vs", "resources/shaders/refract.fs"),
        reflectShader("resources/shaders/refractReflect.vs", "resources/shaders/reflect.fs"),
        billboardShader("resources/shaders/billboard.vs", "resources/shaders/billboard.fs"),
        skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs"),
        shader("resources/shaders/general.vs", "resources/shaders/general.fs"),
        shadowShader("resources/shaders/shadow.vs", "resources/shaders/shadow.fs"),
        camPos(0, 1, -5), // initial cam position
        projection(glm::perspective(glm::radians(60.0f), (float) w / h, 0.1f, 100.0f)),
        window(win),
        width(w),
        height(h)

    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);   // Accept fragment if it closer to the camera than the former one

        // cube
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

        {
            GLfloat cubeVerticesTBN[12 * 3 * 14];
            calculateTBN(cubeVertices, 12, cubeVerticesTBN);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesTBN), cubeVerticesTBN, GL_STATIC_DRAW);
        }

        glEnableVertexAttribArray(0);   // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);   // texture coords
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);   // normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);   // tangent
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
        glEnableVertexAttribArray(4);   // bitangent
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(11 * sizeof(GLfloat)));

        // skybox
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

        // plane
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));


        // fullscreen rectangle
        glGenVertexArrays(1, &effectsVAO);
        glGenBuffers(1, &effectsVBO);
        glBindVertexArray(effectsVAO);
        glBindBuffer(GL_ARRAY_BUFFER, effectsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenVertices), fullscreenVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        // billboard
        glGenVertexArrays(1, &billboardVAO);
        glGenBuffers(1, &billboardVBO);
        glBindVertexArray(billboardVAO);
        glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(billboardVertices), billboardVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        // load textures
        GLuint planeTexture = loadTexture("resources/textures/tough_grass.jpg");
        GLuint brickwallTexture = loadTexture("resources/textures/brickwall.jpg");
        GLuint brickwallNormal = loadTexture("resources/textures/brickwall_normal.jpg");
        GLuint grassBillboard = loadTexture("resources/textures/awesomeface.png");
        GLuint skyboxTexture = loadCubemap(faces);

        /////////////////////////////////////////////////

        // Texture maps for new renderFBO (post-effect)
        GLuint colorTexture = TextureCreateEmpty(GL_RGBA8, GL_RGB, width, height);
        GLuint depthTexture = TextureCreateEmpty(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, width, height);


        GLuint depthTextureFromLight = TextureCreateEmpty(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT,
            width * shadowResolutionReduction, height * shadowResolutionReduction); // shadow map

        refractShader.use();
        refractShader.set("skybox", 0);
        refractShader.set("model", refractModel);
        refractShader.set("refractRatio", 1.1f); // 1.00 for air

        reflectShader.use();
        reflectShader.set("skybox", 0);

        skyboxShader.use();
        skyboxShader.set("skybox", 0);

        billboardShader.use();
        billboardShader.set("texture1", 2);
        billboardShader.set("size", 0.4f);

        normalMappingShader.use();
        normalMappingShader.set("texture1", 3);
        normalMappingShader.set("textureNormals", 4);
        normalMappingShader.set("shadowMap", 6);

        shader.use();
        shader.set("shadowMap", 6);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, colorTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, grassBillboard);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, brickwallTexture);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, brickwallNormal);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, planeTexture);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, depthTextureFromLight);


        glGenFramebuffers(1, &depthFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        // remove color in depthFBO
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        // Attaching depth buffer to depthFBO
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureFromLight, 0);

        // Error cheching
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Something went wrong with light depthFBO" << std::endl;
            throw - 4;
        };

        glGenFramebuffers(1, &renderFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

        // Error cheching
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Something went wrong with renderFBO" << std::endl;
            throw - 4;
        }


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_CULL_FACE); // Cull triangles which normal is not towards the camera
    }

    void renderSceneDepth() {
        glm::mat4 PV(projectionLight * viewLight);
        shadowShader.use();

        glBindVertexArray(planeVAO);
        shadowShader.set("PVM", PV);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(cubeVAO);
        shadowShader.set("PVM", PV * normalMappingCubeModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        shadowShader.set("PVM", PV * refractModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        shadowShader.set("PVM", PV * reflectModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        shadowShader.set("PVM", PV * normalCubeModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);

    }



    void renderSceneCore(const glm::mat4& view, const glm::mat4& PV, const glm::vec3& camPosition) {

        // skybox
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        skyboxShader.set("PVM", projection * glm::mat4(glm::mat3(view))); // remove translation from the view matrix
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // cube
        normalMappingShader.use();
        normalMappingShader.set("PV", PV);
        normalMappingShader.set("lightPV", projectionLight * viewLight);
        normalMappingShader.set("lightDir", lightDirection);
        normalMappingShader.set("camPos", camPosition);
        normalMappingShader.set("model", normalMappingCubeModel);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // refract
        refractShader.use();
        refractShader.set("PVM", PV * refractModel);
        refractShader.set("cameraPos", camPosition);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        reflectShader.use();
        reflectShader.set("PVM", PV * reflectModel);
        reflectShader.set("model", reflectModel);
        reflectShader.set("cameraPos", camPosition);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // cube and plane
        shader.use();
        shader.set("PV", PV);
        shader.set("lightPV", projectionLight * viewLight);
        shader.set("lightDir", lightDirection);
        shader.set("camPos", camPosition);
        shader.set("model", normalCubeModel);
        shader.set("texture1", 3); // brickwall
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(planeVAO);
        shader.set("model", glm::mat4(1.0f));
        shader.set("texture1", 5); // plane grass texture
        glDrawArrays(GL_TRIANGLES, 0, 6);

    
    }

   
    void renderloop() {
        
        while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
            // prerender computations

            lightDirection = glm::vec3(cos(beta),1,sin(beta));
            viewLight = glm::lookAt(lightDirection, -lightDirection, glm::vec3(0, 1, 0)); // position, direction, up

            view = computeViewFromInputs(window, width, height);

            // sort billboards
            for (int i = 0, n = billboards.size(); i + 1 < n; i++) {
                for (int j = i; j < n; j++) {
                    glm::vec3 delta1 = billboards[i] - camPos;
                    glm::vec3 delta2 = billboards[j] - camPos;
                    if (dot(delta1, delta1) < dot(delta2, delta2)) {
                        glm::vec3 tmp = billboards[i];
                        billboards[i] = billboards[j];
                        billboards[j] = tmp;
                    }
                }
            }

            glm::vec3 camPos1, camDir1;
            glm::mat4 viewMirror1, PV1;

            glm::vec3 camPos2, camDir2;
            glm::mat4 viewMirror2, PV2;

            glm::mat4 PV = projection * view;

            // render
            // =====================================
            {
                glViewport(0, 0, width * shadowResolutionReduction, height * shadowResolutionReduction);
                // shadow map
                glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // disable colors
                glClear(GL_DEPTH_BUFFER_BIT);

                glCullFace(GL_FRONT);
                renderSceneDepth();
                glCullFace(GL_BACK);

                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                glViewport(0, 0, width, height);
            }
            {   // main camera
                
                glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderSceneCore(view, PV, camPos);

                // billboards
                billboardShader.use();
                billboardShader.set("PVM", PV);
                billboardShader.set("cameraRight", camRight);
                billboardShader.set("cameraUp", camUp);
                glBindVertexArray(billboardVAO);

                // draw billboards
                for (int i = 0, n = billboards.size(); i < n; i++) {
                    billboardShader.set("billboardPos", billboards[i]);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
            
            {
                // post effect
                glBindFramebuffer(GL_FRAMEBUFFER, 0); // default FBO
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                postEffectShader.use();
                postEffectShader.set("colorTexture", 1); // renderFBO
                postEffectShader.set("border", borderInitial); // 0 - all the way to the left; 1 - right
                glBindVertexArray(effectsVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();

        } // Check if the ESC key was pressed or the window was closed
    }

    ~Graphics() {
        glDeleteVertexArrays(1, &effectsVAO);
        glDeleteVertexArrays(1, &billboardVAO);
        glDeleteVertexArrays(1, &planeVAO);
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &skyboxVAO);

        glDeleteBuffers(1, &effectsVBO);
        glDeleteBuffers(1, &billboardVBO);
        glDeleteBuffers(1, &planeVBO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &skyboxVBO);
    }
};

int main() {

    // Window size
    const int width = 1600;
    const int height = 1024;

    // Initialise GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(width, height, "Graphics", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    Graphics graphics(width, height, window);
    graphics.renderloop();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}
