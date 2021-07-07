//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include "3rdParty/src/stb/stb_image.h"
#include "Application/utils.h"
#include "quad.h"
#include "quad.cpp"

void SimpleShapeApplication::init() {
    glEnable(GL_FRAMEBUFFER_SRGB);
    this->set_camera(new Camera);
    this->set_controler(new CameraControler(camera()));
    auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
                                      std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");


    if (!program) {
        std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
        std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
    }

    xe::utils::set_uniform_block_binding(program,"Transformations",1);
    xe::utils::set_uniform_block_binding(program,"Light",2);
    xe::utils::set_uniform_block_binding(program,"Material",3);
    int w, h;

    std::tie(w, h) = frame_buffer_size();
    camera()->perspective(glm::half_pi<float>()*1.2f,(float)w/h,0.1f,100.0f);
    camera()->look_at(glm::vec3{-2.0f, 8.0f, -9.75f},
                      glm::vec3{0.0f, 0.0f, 0.0f},
                      glm::vec3{0.0f,1.0f,0.0f});


//lustrzane odbicie????
    this->quad_ = new Quad;

    GLuint ubo_handle(0u);
    glGenBuffers(1,&ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER,8*sizeof(float), nullptr,GL_STATIC_DRAW);

    //tekstura
    GLuint diffuse_texture;
    glGenTextures(1,&diffuse_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,diffuse_texture);
    xe::utils::load_texture(std::string(PROJECT_DIR) + "/Textures/plastic.png");
    glBindTexture(GL_TEXTURE_2D,0);


    GLuint shininess_texture;
    glGenTextures(1,&shininess_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,shininess_texture);
    xe::utils::load_texture(std::string(PROJECT_DIR)+"/Textures/shininess.png");
    glBindTexture(GL_TEXTURE_2D,0);

    auto material = new PhongMaterial();
    material->Kd = glm::vec3(1.0, 1.0, 1.0);
    material->Kd_map = diffuse_texture;
    material->Ks = glm::vec3(0.05, 0.05, 0.05);
    material->Ks_map = 0;
    material->Ns = 1000.0f;
    material->Ns_map = shininess_texture;
    this->quad_->set_material(material);

    glGenBuffers(1,&u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER,u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER,2*sizeof(glm::mat4)+3*sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),&camera()->projection()[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),sizeof(glm::mat4),&camera()->view()[0]);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    /////
    glBindBufferBase(GL_UNIFORM_BUFFER,0,ubo_handle);
    glBindBufferBase(GL_UNIFORM_BUFFER,1,u_pvm_buffer_);

    glGenBuffers(1,&u_light_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER,u_light_buffer_);
    glBufferData(GL_UNIFORM_BUFFER,4*sizeof(glm::vec4),nullptr,GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER,0,4*sizeof(GLfloat),&this->light_.position[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,4*sizeof(GLfloat),4*sizeof(GLfloat),&this->light_.color[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,2*4*sizeof(GLfloat),4*sizeof(GLfloat),&this->light_.a[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,3*4*sizeof(GLfloat),4*sizeof(GLfloat),&this->light_.ambient[0]);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    glBindBufferBase(GL_UNIFORM_BUFFER,2,u_light_buffer_);
    this->light_.a=glm::vec4 (1.0f, 1.0f, 0.0f, 1.0f);
    this->light_.color=glm::vec4(0.5f,0.5f,0.5f,1.0f);
    this->light_.position = camera()->view() * glm::vec4(0.0f,1.0f,0.0f,1.0f);
    this->light_.ambient=glm::vec4(0.35f,0.35f,0.35f,1.0f);


    glGenBuffers(1,&u_material_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER,u_material_buffer_);
    glBufferData(GL_UNIFORM_BUFFER,sizeof(PhongMaterial), nullptr,GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    glBindBufferBase(GL_UNIFORM_BUFFER,3,u_material_buffer_);

    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
    xe::utils::set_uniform1i(program,"diffuse_map",0);
    xe::utils::set_uniform1i(program,"specular_map",1);
    xe::utils::set_uniform1i(program,"shininess_map",2);
}

void SimpleShapeApplication::frame() {
    auto VM = camera()->view();
    auto P = camera()->projection();
    auto R = glm::mat3(VM);
    auto N = glm::transpose(glm::inverse(R));
    glBindBuffer(GL_UNIFORM_BUFFER,u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),&P[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),sizeof(glm::mat4),&VM[0]);;
    glBufferSubData(GL_UNIFORM_BUFFER,2*sizeof(glm::mat4),sizeof(glm::vec4),&N[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,2*sizeof(glm::mat4) + sizeof(glm::vec4),sizeof(glm::vec4),&N[1]);
    glBufferSubData(GL_UNIFORM_BUFFER,2*sizeof(glm::mat4) + 2*sizeof(glm::vec4),sizeof(glm::vec4),&N[2]);
    glBindBuffer(GL_UNIFORM_BUFFER,0);

    glBindBuffer(GL_UNIFORM_BUFFER,u_light_buffer_);
    light_.position = glm::vec4(0.0f,1.0f,0.0f,1.0f);
    light_.position_in_vs = VM*light_.position;
    glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::vec4),&light_.position_in_vs[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,4*sizeof(GLfloat),4*sizeof(GLfloat),&this->light_.color[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,8*sizeof(GLfloat),4*sizeof(GLfloat),&this->light_.a[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,12*sizeof(GLfloat),4*sizeof(GLfloat),&this->light_.ambient[0]);
    glBindBuffer(GL_UNIFORM_BUFFER,u_material_buffer_);
    auto material = quad_->material();
    if(material->Kd_map>0){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,material->Kd_map);
    }
    if(material->Ks_map>0){
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,material->Ks_map);
    }
    if(material->Ns_map>0){
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,material->Ns_map);
    }
    glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(PhongMaterial),quad_->material());
    this->quad_->draw();
    glBindBuffer(GL_UNIFORM_BUFFER,0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0,0,w,h);
    auto aspect = (float) w / h;
    camera()->set_aspect(aspect);
}
void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset,yoffset);
    camera()->zoom(yoffset/30.0f);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button,action,mods);
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

void SimpleShapeApplication::cleanup() {
    delete quad_;
}