//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

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

    // Generating the buffer and loading the rectangle vertex data into it.
    GLuint v_buffer_rectangle;
    glGenBuffers(1, &v_buffer_rectangle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_rectangle));
    glBufferData(GL_ARRAY_BUFFER, rectangleVertices.size() * sizeof(GLfloat), rectangleVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
    glBindVertexArray(vao_triangle_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    // Rendering the rectangle
    glBindVertexArray(vao_rectangle_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}