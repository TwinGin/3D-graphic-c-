//
// Created by pbialas on 05.08.2020.
//
#include "utils.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <3rdParty/src/stb/stb_image.h>

#include "glad/glad.h"

namespace xe {
    namespace utils {
        std::string get_gl_description(void) {
            std::stringstream ss;
            auto vendor = glGetString(GL_VENDOR);
            auto renderer = glGetString(GL_RENDERER);
            auto version = glGetString(GL_VERSION);
            auto glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);


            int gl_major_version;
            int gl_minor_version;
            glGetIntegerv(GL_MAJOR_VERSION, &gl_major_version);
            glGetIntegerv(GL_MINOR_VERSION, &gl_minor_version);

            ss << vendor << " " << renderer << " OpenGL " << gl_major_version << "." << gl_minor_version << "\n";
            ss << version << " " << glsl_version;

            return ss.str();
        }

        std::string error_msg(GLenum status) {
            switch (status) {
                case GL_INVALID_ENUM:
                    return "INVALID ENUM";
                case GL_INVALID_VALUE:
                    return "INVALID VALUE";
                case GL_INVALID_OPERATION:
                    return "INVALID OPERATION";
                case GL_STACK_OVERFLOW:
                    return "STACK OVERFLOW";
                case GL_STACK_UNDERFLOW:
                    return "STACK UNDERFLOW";
                case GL_OUT_OF_MEMORY:
                    return "OUT OF MEMORY";
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    return "INVALID FRAMEBUFFER OPERATION";
                default:
                    return "UNKNOWN ERROR";
            }
        }
        uint8_t *load_image(const std::string &filename, int *width, int *height, int *n_channels) {
            stbi_set_flip_vertically_on_load(true);
            auto data = stbi_load(filename.c_str(), width, height, n_channels, 0);
            if (data == nullptr) {
                std::cerr << "cannot load image from file `" << filename << "'\n";
            } else {
                std::cout << "read " << *width << "x" << *height << "x" << *n_channels << " image from file "
                          << filename
                          << "\n";
            }
            return data;
        }

        void load_texture(const std::string &filename) {
            int width, height, n_channels;
            auto data = xe::utils::load_image(filename, &width, &height, &n_channels);
            if (data) {
                if (n_channels == 3)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                else if (n_channels == 4)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                else if (n_channels == 2)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
                else if (n_channels == 1)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                else {
                    std::cerr << "Unsuported number of chanels ( " << n_channels << ")  in texture \n";
                }
                auto status = glGetError();
                if (status != GL_NO_ERROR) {
                    std::cerr << "Error " << status << " " << xe::utils::error_msg(status)
                              << " while loading a texture "
                              << std::endl;
                }
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
            stbi_image_free(data);
        }

        std::string load_file(const std::string &path) {
            std::ifstream file(path, std::ios::in | std::ios::binary);
            if (file) {
                std::ostringstream contents;
                contents << file.rdbuf();
                file.close();
                return (contents.str());
            }
            return {};
        }

        std::string shader_type(GLenum type) {
            switch (type) {
                case GL_VERTEX_SHADER:
                    return "Vertex";
                case GL_FRAGMENT_SHADER:
                    return "Fragment";
                case GL_GEOMETRY_SHADER:
                    return "Geometry";
            }
            return "Unknown";
        }
        void set_uniform1i(GLuint program, const std::string &name, int value) {
            auto location = glGetUniformLocation(program, name.c_str());
            if (location == -1) {
                std::cerr << "Cannot find uniform `" << name << "'" << std::endl;
            } else {
                glUniform1i(location, value);
            }
        }

        void set_uniform_block_binding(GLuint program, const std::string &name, GLuint binding) {
            auto block_index = glGetUniformBlockIndex(program, name.c_str());
            if (block_index == GL_INVALID_INDEX) {
                std::cout << "Cannot find `" << name << "' in program " << program << std::endl;
            } else {
                glUniformBlockBinding(program, block_index, binding);
            }
        }
    }


    GLuint link_program(GLuint program) {
        glLinkProgram(program);
        GLint link_status;
        glGetProgramiv(program, GL_LINK_STATUS, &link_status);
        if (!link_status) {
            std::cerr << "Error linking program\n";
            GLint max_log_length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_log_length);
            return 0;
        }


        return program;
    }

    GLuint create_program(const std::string &vs_path, const std::string &fs_path) {

        auto vs_shader = create_shader_from_file(GL_VERTEX_SHADER, vs_path);
        if (vs_shader == 0)
            return 0;

        auto fs_shader = create_shader_from_file(GL_FRAGMENT_SHADER, fs_path);
        if (fs_shader == 0)
            return 0;

        glReleaseShaderCompiler();

        GLuint program = glCreateProgram();
        if (program == 0) {
            std::cerr << "Error creating program" << "\n";
            glDeleteShader(vs_shader);
            glDeleteShader(fs_shader);
            return 0;
        }
        {
            glAttachShader(program, vs_shader);
            auto status = glGetError();
            if (status != GL_NO_ERROR) {
                glDeleteProgram(program);
                glDeleteShader(vs_shader);
                glDeleteShader(fs_shader);
                return 0;
            }
        }
        {
            glAttachShader(program, fs_shader);
            auto status = glGetError();
            if (status != GL_NO_ERROR) {
                glDeleteProgram(program);
                glDeleteShader(vs_shader);
                glDeleteShader(fs_shader);
                return 0;
            }
        }


        if (link_program(program) > 0) {
            return program;
        } else {
            glDeleteProgram(program);
            glDeleteShader(vs_shader);
            glDeleteShader(fs_shader);
            return 0;
        }
    }


    GLuint create_shader_from_file(GLenum type, const std::string &path) {
        auto shader_source = utils::load_file(path);
        if (shader_source.empty()) {
            std::cerr << "Cannot read `" << path << "' file.\n";
            return 0;
        }
        auto shader = glCreateShader(type);
        if (shader == 0) {
            std::cerr << "Error creating shader" << "\n";
            return 0;
        }

        const char *source = shader_source.c_str();

        glShaderSource(shader, 1, &source, nullptr);

        glCompileShader(shader);
        GLint is_compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
        if (!is_compiled) {
            GLint max_log_length = 0u;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_log_length);
            std::string error_log;
            error_log.resize(max_log_length);
            glGetShaderInfoLog(shader, max_log_length, &max_log_length, &error_log[0]);
            glDeleteShader(shader);
            std::cerr << utils::shader_type(type) << " shader\n"
                      << " compilation error: " << error_log << std::endl;
            return 0;
        }
        return shader;
    }
}

