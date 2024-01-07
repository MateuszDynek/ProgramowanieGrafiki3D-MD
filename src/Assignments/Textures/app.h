//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/gl.h"
#include "camera.h"
#include "camera_controler.h"

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug) {}

    void init() override;

    void frame() override;

    void set_camera(Camera *camera) { camera_ = camera; }
    Camera *camera() { return camera_; }
    ~SimpleShapeApplication() {
        if (camera_) {
            delete camera_;
        }
    }

private:
    GLuint vao_pyramid_;
    Camera *camera_;
    glm::mat4 model;
    GLuint v_buffer_transformation;
    CameraControler *controler_;

    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override;

    void set_controler(CameraControler *controler) { controler_ = controler; }

    void mouse_button_callback(int button, int action, int mods) override;

    void cursor_position_callback(double x, double y) override;
};