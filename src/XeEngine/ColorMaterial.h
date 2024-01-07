//
// Created by Piotr Białas on 20/11/2021.
//

#pragma once

#include "Material.h"

#include <string>

namespace xe {
    class ColorMaterial : public Material {
    public:

        static void init();

        static GLuint program() { return shader_; }

        ColorMaterial(const glm::vec4 color, GLuint texture, GLuint texture_unit) : Kd_(color), texture_(texture),
                                                                                    texture_unit_(texture_unit) {}

        ColorMaterial(const glm::vec4 color, GLuint texture) : ColorMaterial(color, texture, 0) {}

        ColorMaterial(const glm::vec4 color) : ColorMaterial(color, 0) {}

        void set_texture(GLuint tex) { texture_ = tex; }

        GLuint get_texture() { return texture_; }

        void set_texture_unit(GLuint tex_unit) { texture_unit_ = tex_unit; }

        GLuint get_texture_unit() { return texture_unit_; }

        void bind() override;

        void unbind() override;


    private:

        static GLuint shader_;
        static GLuint color_uniform_buffer_;
        static GLint uniform_map_Kd_location_;

        glm::vec4 Kd_;
        GLuint texture_;
        GLuint texture_unit_;
    };


    GLuint create_texture(const std::string &name);

}




