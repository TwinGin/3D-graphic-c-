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

#include "Application/utils.h"

void SimpleShapeApplication::init() {

    this->set_camera(new Camera);
    this->set_controler(new CameraControler(camera()));
    auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
                                      std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");


    if (!program) {
        std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
        std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
    }

    ///UNIFORM PVM
    auto u_transformations_index = glGetUniformBlockIndex(program,"Transformations");
    if(u_transformations_index == GL_INVALID_INDEX){
        std::cout<<"Cannot find Transformations uniform block in program"<<std::endl;
    }else{
        glUniformBlockBinding(program,u_transformations_index,1);
    }

    int w, h;

    std::tie(w, h) = frame_buffer_size();
    camera()->perspective(glm::half_pi<float>()*1.2f,(float)w/h,0.1f,100.0f);
    camera()->look_at(glm::vec3{0.1f, 10.0f, 0.1f},
                     glm::vec3{0.0f, 0.0f, 0.0f},
                     glm::vec3{0.0f,1.0f,0.0f});

    //Tworzenie piramidy
    this->pyramid_ = std::make_shared<Pyramid>();

    GLuint ubo_handle(0u);
    glGenBuffers(1,&ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER,8*sizeof(float), nullptr,GL_STATIC_DRAW);



    //GLuint ubo_handle_PVM(0u);
    glGenBuffers(1,&u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER,u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER,2*sizeof(glm::mat4), nullptr
            ,GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),&camera()->projection()[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),sizeof(glm::mat4),&camera()->view()[0]);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    /////
    glBindBufferBase(GL_UNIFORM_BUFFER,0,ubo_handle);
    glBindBufferBase(GL_UNIFORM_BUFFER,1,u_pvm_buffer_);




    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    this->rotation_period =4.0;
    this->start_ = std::chrono::steady_clock::now();
    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    ///obrot piramidy glownej
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(now - start_).count();
    auto rotation_angle = 2.0f * glm::pi<float>()*elapsed_time/rotation_period;
    auto R = glm::rotate(glm::mat4(1.0f),rotation_angle,glm::vec3(0.0f,1.0f,0.0f));;
    ///Orbita piramidy glownej
    auto orbital_rotation_period =20.0f;
    auto orbital_rotation_angle = 2.0f*glm::pi<float>()*elapsed_time/orbital_rotation_period;
    auto a = 10.0f;
    auto b= 8.0f;
    float x = a * cos(orbital_rotation_angle);
    float z = b * sin(orbital_rotation_angle);
    auto O=glm::translate(glm::mat4(1.0f),glm::vec3{x,0.0,z});
    ///Ksiezyc
    auto moon_rotation_period =10.0f;
    auto moon_rotation_angle = 2.0f*glm::pi<float>()*elapsed_time/moon_rotation_period;
    float moon_x = 3.0f*sin(moon_rotation_angle);
    float moon_z = 3.0f*cos(moon_rotation_angle);
    auto moon_0 = glm::translate(glm::mat4(1.0f),glm::vec3{moon_x,0.0f,moon_z});
    auto moon_R = glm::rotate(glm::mat4(1.0f),moon_rotation_angle,glm::vec3(0.0f,1.0f,0.0f));
    auto moon_S = glm::scale(glm::mat4(1.0f),glm::vec3(0.5f,0.5f,0.5f));
    ///Satelita
    auto satellite_rotation_period=2.0f;
    auto satellite_rotation_angle=2.0f*glm::pi<float>()*elapsed_time/satellite_rotation_period;
    float satellite_x = 1.5f*cos(satellite_rotation_angle);
    float satellite_y = 1.5f*sin(satellite_rotation_angle);
    auto satellite_O = glm::translate(glm::mat4(1.0f),glm::vec3{satellite_x,satellite_y,0.0f});
    auto satellite_R = glm::rotate(glm::mat4(1.0f),satellite_rotation_angle,glm::vec3(0.0f,1.0f,0.0f));
    auto satellite_S = glm::scale(glm::mat4(1.0f),glm::vec3(0.25f,0.25f,0.25f));
    glm::mat4 translations[]={
            O*R,
            O*moon_0*moon_R*moon_S,
            O*satellite_O*satellite_R*satellite_S
    };
    for(int i=0;i<3;i++){
        auto VM= camera()->view()*translations[i];
        glBindBuffer(GL_UNIFORM_BUFFER,u_pvm_buffer_);
        glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),&camera()->projection()[0]);
        glBufferSubData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),sizeof(glm::mat4),&VM[0]);
        glBindBuffer(GL_UNIFORM_BUFFER,0);
        this->pyramid_->draw();
    }
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