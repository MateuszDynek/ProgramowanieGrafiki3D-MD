//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/gl.h"
#include "camera.h"

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
    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override;
private:
    GLuint vao_pyramid_;
    Camera *camera_;
    
    GLuint u_pvm_buffer_;

    float fov_;
    float aspect_;
    float near_;
    float far_; 

    glm::mat4 P_;
    glm::mat4 V_; 
    glm::mat4 M_;
};