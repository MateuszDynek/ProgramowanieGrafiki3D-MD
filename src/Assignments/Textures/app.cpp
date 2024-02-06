//
// Created by pbialas on 25.09.2020.
//
#define STB_IMAGE_IMPLEMENTATION  1

#include "3rdParty/stb/stb_image.h"
#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Application/utils.h"
#include "XeEngine/Mesh.h"
#include "XeEngine/mesh_loader.h"
#include "XeEngine/ColorMaterial.h"

void SimpleShapeApplication::init() {
    set_camera(new Camera);
    set_controler(new CameraControler(camera()));
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

	xe::ColorMaterial::init();
	pyramid = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/blue_marble.obj", std::string(ROOT_DIR) + "/Models");

    // All pyramid vertices with color (RGB)
    std::vector<GLfloat> pyramidVertices = {
        -0.5f, -0.5f, 0.5f, 0.5f, 0.191f,
        0.5f, -0.5f, 0.5f, 0.809f, 0.5f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f,

        0.5f, -0.5f, 0.5f, 0.5f, 0.191f,
        0.5f, -0.5f, -0.5f, 0.191f, 0.5f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f,

        0.5f, -0.5f, -0.5f, 0.5f, 0.809f,
        -0.5f, -0.5f, -0.5f, 0.809f, 0.5f, 
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, -0.5f, 0.191f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.809f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.5f, -0.5f, -0.5f, 0.5f, 0.191f,
        0.5f, -0.5f, 0.5f, 0.191f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.809f,
        
        -0.5f, -0.5f, 0.5f, 0.5f, 0.191f,
        -0.5f, -0.5f, -0.5f, 0.191f, 0.5f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.809f 
        
    };


    auto [w, h] = frame_buffer_size();

    model = glm::mat4(1.0f);
    std::vector<GLushort> indices={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};

    camera_->look_at(glm::vec3(-2.0f, -1.0f, -3.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 1.0f, 0.0f));

    auto fov = glm::pi<float>() / 4.0;
    auto near = 0.1f;
    auto far = 100.0f;
    auto aspect = (float) w / h;
    camera_->perspective(fov, aspect, near, far);


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

    // Create and bind the uniform buffer for Transformations interface block
    glGenBuffers(1, &v_buffer_transformation);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_transformation));
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW); // Allocate 64 bytes for the mat4
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    // Load texture
    stbi_set_flip_vertically_on_load(true);
    GLint width, height, channels;
    auto texture_file = std::string(ROOT_DIR) + "/Models/multicolor.png";
    auto img = stbi_load(texture_file.c_str(), &width, &height, &channels, 0);

    shader_ = program;
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);

    glBindTexture(GL_TEXTURE_2D, 0);
    uniform_map_Kd_location_ = glGetUniformLocation(shader_, "map_Kd");

    glm::vec4 Kd = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 
    GLboolean is_textures_exist = GL_FALSE;

    if (texture > 0) {
        OGL_CALL(glUniform1i(uniform_map_Kd_location_, 0));
        is_textures_exist = GL_TRUE;
    }

    glGenBuffers(1, &texture_buffer);

    glBindBuffer(GL_UNIFORM_BUFFER, texture_buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) + sizeof(GLboolean), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), &Kd);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4), sizeof(GLboolean), &is_textures_exist);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glGenVertexArrays(1, &vao_pyramid_);
    glBindVertexArray(vao_pyramid_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, texture_buffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, v_buffer_transformation);

    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_pyramid);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_buffer_indices);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    // Setting the background color of the rendering window
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // Setting up an OpenGL viewport of the size of the whole rendering window
    
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    auto pvm = camera_->projection() * camera_->view() * model;
    glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_transformation);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &pvm[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindVertexArray(vao_pyramid_);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);

	pyramid->draw();
}
void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera_->set_aspect((float) w / h);
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    camera()->zoom(yoffset / 30.0f);
}
void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}