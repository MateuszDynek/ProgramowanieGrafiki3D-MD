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
#include "XeEngine/PhongMaterial.h"
#include "XeEngine/lights.h"

struct AllLights {
    xe::PointLight light[24];
    int num_lights;
};

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
	xe::PhongMaterial::init();
	mesh = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/square.obj", std::string(ROOT_DIR) + "/Models");


    auto [w, h] = frame_buffer_size();

    model = glm::mat4(1.0f);

    camera_->look_at(glm::vec3(-2.0f, -1.0f, -3.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 1.0f, 0.0f));

    auto fov = glm::pi<float>() / 4.0;
    auto near = 0.1f;
    auto far = 100.0f;
    auto aspect = (float) w / h;
    camera_->perspective(fov, aspect, near, far);

    // Create and bind the uniform buffer for Transformations interface block
    glGenBuffers(1, &v_buffer_transformation);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_transformation));
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW); // Allocate 64 bytes for the mat4

	glGenBuffers(1, &transform_p);
    glBindBuffer(GL_UNIFORM_BUFFER, transform_p);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(glm::mat3), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, transform_p);

    // Point Lights
    add_light(*new xe::PointLight({ 0.2, 0.2, 0.4 }, { 1.0, 0.0, 0.1 }, { 1.0, 1.0, 1.0 }));
    add_light(*new xe::PointLight({ -0.3, -0.2, 0.4 }, { 0.1, 1.0, 0.0 }, { 1.0, 1.0, 1.0 }));
    add_light(*new xe::PointLight({ 0.2, -0.2, 0.4 }, { 0.0, 0.0, 1.0 }, { 1.0, 1.0, 1.0 }));
    xe::PhongMaterial::set_ambient({ 0.2f, 0.2f, 0.2f });

	glGenBuffers(1, &light_p);
    glBindBuffer(GL_UNIFORM_BUFFER, light_p);
    glBufferData(GL_UNIFORM_BUFFER,sizeof(AllLights), nullptr, GL_STATIC_DRAW);
	
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenVertexArrays(1, &vao_pyramid_);
    glBindVertexArray(vao_pyramid_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, v_buffer_transformation);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, light_p);
    glBindBuffer(GL_UNIFORM_BUFFER, light_p);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    // Setting the background color of the rendering window
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // Setting up an OpenGL viewport of the size of the whole rendering window
    
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
	auto VM = camera_->view() * glm::mat4(1.0f);
    auto R = glm::mat3(VM);
    auto N = glm::mat3(glm::cross(R[1], R[2]), glm::cross(R[2], R[0]), glm::cross(R[0], R[1]));

    glBindBuffer(GL_UNIFORM_BUFFER, transform_p);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(VM), &VM);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(VM), sizeof(N), &N);

    glBindBuffer(GL_UNIFORM_BUFFER, light_p);
    AllLights all_lights{};
    all_lights.num_lights = p_lights_.size();

    for(int i = 0; i < all_lights.num_lights; i++) all_lights.light[i] = p_lights_[i];

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(AllLights), &all_lights);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    auto pvm = camera_->projection() * camera_->view() * model;
    glBindBuffer(GL_UNIFORM_BUFFER, v_buffer_transformation);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &pvm[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindVertexArray(0);

	mesh->draw();
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