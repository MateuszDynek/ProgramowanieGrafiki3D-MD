//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // All pyramid vertices with color (RGB)
    std::vector<GLfloat> pyramidVertices = {
        -0.5f, 0.0f, 0.5f, 0.8f ,0.0f ,0.0f,
        0.5f, 0.0f, 0.5f, 0.8f ,0.0f ,0.0f,
        0.0f, 1.0f, 0.0f, 0.8f ,0.0f ,0.0f,

        0.5f, 0.0f, 0.5f, 0.0f ,0.8f ,0.0f,
        0.5f, 0.0f, -0.5f, 0.0f ,0.8f ,0.0f,
        0.0f, 1.0f, 0.0f, 0.0f ,0.8f ,0.0f,

        0.5f, 0.0f, -0.5f, 0.0f ,0.0f ,0.8f,
        -0.5f, 0.0f, -0.5f, 0.0f ,0.0f ,0.8f,
        0.0f, 1.0f, 0.0f, 0.0f ,0.0f ,0.8f,

        -0.5f, 0.0f, -0.5f, 0.2f ,0.2f ,0.0f,
        -0.5f, 0.0f, 0.5f, 0.2f ,0.2f ,0.0f,
        0.0f, 1.0f, 0.0f, 0.2f ,0.2f ,0.0f,

        0.5f, 0.0f, -0.5f, 0.0f ,0.4f ,0.4f,
        0.5f, 0.0f, 0.5f, 0.0f ,0.4f ,0.4f,
        -0.5f, 0.0f, 0.5f, 0.0f ,0.4f ,0.4f,
        
        -0.5f, 0.0f, 0.5f, 0.0f ,0.4f ,0.4f,
        -0.5f, 0.0f, -0.5f, 0.0f ,0.4f ,0.4f,
        0.5f, 0.0f, -0.5f, 0.0f ,0.4f ,0.4f
        
    };

    float strength = 1.5f; 
    float color[3] = {1.0f, 1.0f, 1.0f};

    auto [w, h] = frame_buffer_size();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(2.0f, -1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(w) / h, 0.1f, 100.0f);
    glm::mat4 PVM = projection * view * model;
    std::vector<GLushort> indices={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};

    // Generating the buffer and loading the pyramid indices data into it.
    GLuint v_buffer_indices;
    glGenBuffers(1, &v_buffer_indices);
    OGL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_buffer_indices));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Generating the buffer and loading the pyramid vertex data into it.
    GLuint v_buffer_pyramid;
    glGenBuffers(1, &v_buffer_pyramid);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_pyramid));
    glBufferData(GL_ARRAY_BUFFER, pyramidVertices.size() * sizeof(GLfloat), pyramidVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create buffer for colour and strenght
    GLuint v_buffer_color_strenght;
    glGenBuffers(1, &v_buffer_color_strenght);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_color_strenght));
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, 3 * sizeof(float), &color);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Create and bind the uniform buffer for Transformations interface block
    GLuint v_buffer_transformation;
    glGenBuffers(1, &v_buffer_transformation);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_transformation));
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW); // Allocate 64 bytes for the mat4
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    glGenVertexArrays(1, &vao_pyramid_);
    glBindVertexArray(vao_pyramid_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, v_buffer_color_strenght);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, v_buffer_transformation);

    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_pyramid);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_buffer_indices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    // Setting the background color of the rendering window
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // Setting up an OpenGL viewport of the size of the whole rendering window
    
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    glBindVertexArray(vao_pyramid_);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
}