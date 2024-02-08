//
// Created by Piotr Białas on 20/11/2021.
//

#pragma once

#include "Material.h"

#include <string>

namespace xe {
    class PhongMaterial : public Material {
    public:

        static void init();

        static GLuint program() { return shader_; }

        PhongMaterial(const glm::vec4 color, const glm::vec3 ambient, const glm::vec3 specular, float specularStrength) : Kd_(color), Ka_(ambient), Ks_(specular), Ns(specularStrength), map_Kd_(0) {}

        PhongMaterial(const glm::vec4 color, GLuint map_Kd_, GLuint map_Kd_unit_) : Kd_(color) {}

        PhongMaterial(const glm::vec4 color, GLuint map_Kd_) : PhongMaterial(color, map_Kd_, 0){}

        void set_texture(GLuint tex) { map_Kd_ = tex; }

		void set_texture_unit(GLuint textureUnit) {
            map_Kd_unit_ = textureUnit;
        }

		static void set_ambient(const glm::vec3 ambient) {
            glUseProgram(program());
            glUniform3f(uniform_ambient_location_, ambient.x, ambient.y, ambient.z);
            glUseProgram(0);
        }

        void bind() override;

        void unbind() override;


    private:

        static GLuint shader_;
        static GLuint material_uniform_buffer_;
        static GLint uniform_map_Kd_location_;
		static GLint uniform_ambient_location_;

		float Ns;
		glm::vec3 Ka_;
        glm::vec3 Ks_;
        glm::vec4 Kd_;
        GLuint map_Kd_;
        GLboolean use_map_Kd_;
        GLuint map_Kd_unit_;
    };


    GLuint create_texture(const std::string &name);

}




