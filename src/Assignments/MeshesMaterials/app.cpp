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

    auto pyramid = new xe::Mesh;
    auto texture = xe::create_texture(std::string(ROOT_DIR) + "/Models/multicolor.png");

    xe::ColorMaterial::init();

    pyramid->allocate_vertex_buffer(pyramidVertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_vertices(0, pyramidVertices.size() * sizeof(GLfloat), pyramidVertices.data());
    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);
    pyramid->vertex_attrib_pointer(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));

    pyramid->allocate_index_buffer(indices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_indices(0, indices.size() * sizeof(GLfloat), indices.data());

    pyramid->add_submesh(0, 3, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 1) );
    pyramid->add_submesh(3, 6, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 2) );
    pyramid->add_submesh(6, 9, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 3) );
    pyramid->add_submesh(9, 12, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 4) );
    pyramid->add_submesh(12, 18, new xe::ColorMaterial({1.0f, 1.0f, 1.0f, 1.0f}, texture, 5) );
    add_submesh(pyramid);

    glGenBuffers(1, &v_buffer_transformation);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_transformation));
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW); // Allocate 64 bytes for the mat4
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenVertexArrays(1, &vao_pyramid_);
    glBindVertexArray(vao_pyramid_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, v_buffer_transformation);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    // Setting the background color of the rendering window
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // Setting up an OpenGL viewport of the size of the whole rendering window
    
    glViewport(0, 0, w, h);

}

void SimpleShapeApplication::frame() {
    auto pvm = camera_->projection() * camera_->view() * model;
    glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_transformation);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &pvm[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for (auto m: meshes_) {
        m->draw();
    }

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