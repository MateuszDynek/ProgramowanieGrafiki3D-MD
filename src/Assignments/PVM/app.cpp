//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

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

    // Triangle vertices with color (RGB)
    std::vector<GLfloat> triangleVertices = {
        -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f   
    };

    // Generating the buffer and loading the triangle vertex data into it.
    GLuint v_buffer_triangle;
    glGenBuffers(1, &v_buffer_triangle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_triangle));
    glBufferData(GL_ARRAY_BUFFER, triangleVertices.size() * sizeof(GLfloat), triangleVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Setting up the VAO for the triangle
    glGenVertexArrays(1, &vao_triangle_);
    glBindVertexArray(vao_triangle_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_triangle);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Rectangle vertices with color (RGB)
    std::vector<GLfloat> rectangleVertices = {
        -0.5f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f,  // top right point first triangle
        0.5f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f,  // top left point first triangle
        0.5f, -0.5f, 0.0f, 0.0f, 0.8f, 0.0f,  // bottom right point first triangle
        0.5f, -0.5f, 0.0f, 0.0f, 0.8f, 0.0f,  // bottom right point second triangle
        -0.5f, -0.5f, 0.0f, 0.0f, 0.8f, 0.0f,  // bottom left point second triangle
        -0.5f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f   // top left point second triangle
    };

    // All house vertices with color (RGB)
    std::vector<GLfloat> allHouseVertices = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.8f, 0.0f,  // bottom left point of house base
        0.5f, -0.5f, 0.0f, 0.0f, 0.8f, 0.0f,  // bottom right point of house base
        -0.5f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f,  // top left point of house base
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // top middle point of house roof
        0.5f, -0.5f, 0.0f, 0.0f, 0.8f, 0.0f,  // bottom right point of house base
        0.5f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f   // top right point of house base
    };

    // Generating the buffer and loading the rectangle vertex data into it.
    GLuint v_buffer_rectangle;
    glGenBuffers(1, &v_buffer_rectangle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_rectangle));
    // glBufferData(GL_ARRAY_BUFFER, rectangleVertices.size() * sizeof(GLfloat), rectangleVertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, allHouseVertices.size() * sizeof(GLfloat), allHouseVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<GLushort> indices={0,1,2,3,4,2,3,5,1};
    glGenBuffers(1, &vao_allHouse);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao_allHouse);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

    GLuint ubo_modifier;
    glGenBuffers(1, &ubo_modifier);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_modifier);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_modifier);

    // Load data into the buffer
    float strength = 1.5f; 
    float color[3] = {1.0f, 0.2f, 1.0f};
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, 3 * sizeof(float), &color);

    // Create and bind the uniform buffer for Transformations interface block
    GLuint ubo_transformations;
    glGenBuffers(1, &ubo_transformations);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_transformations);
    glBufferData(GL_UNIFORM_BUFFER, 48, nullptr, GL_STATIC_DRAW); // Allocate 48 bytes for the buffer
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_transformations);

    // Load data into the buffer
    float theta = 1.0 * glm::pi<float>() / 6.0f;
    auto cs = std::cos(theta);
    auto ss = std::sin(theta);
    glm::mat2 rotation{ cs, ss, -ss, cs };
    glm::vec2 translation{ 0.0, -0.25 };
    glm::vec2 scale{ 0.5, 0.5 };
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec2), &scale);
    glBufferSubData(GL_UNIFORM_BUFFER, 8, sizeof(glm::vec2), &translation);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::mat2), &rotation[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(glm::mat2), &rotation[1]);


    // Setting up the VAO for the rectangle
    glGenVertexArrays(1, &vao_rectangle_);
    glBindVertexArray(vao_rectangle_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_rectangle);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Setting the background color of the rendering window
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // Setting up an OpenGL viewport of the size of the whole rendering window
    auto [w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    // Rendering the triangle
    // glBindVertexArray(vao_triangle_);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // glBindVertexArray(0);

    // Rendering the rectangle
    // glBindVertexArray(vao_rectangle_);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // glBindVertexArray(0);
    // Rendering all house
    glBindVertexArray(vao_rectangle_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao_allHouse);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}