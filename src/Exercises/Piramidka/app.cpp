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


    auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
                                      std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");


    if (!program) {
        std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
        std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
    }

    std::vector<GLfloat> vertices = {
            0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,//0
            1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,//1
            1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f,//2
            0.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f,//PODSTAWA 3
            0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,//4
            1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,//5
            0.5f,  1.0f,  0.5f,  0.0f, 1.0f, 0.0f,//1 sciana 6
            0.0f,  0.0f,  0.0f,  0.0f, 0.0f, 1.0f,//7
            0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,//8
            0.5f,  1.0f,  0.5f,  0.0f, 0.0f, 1.0f,//2 sciana 9
            1.0f,  0.0f,  0.0f,  0.5f, 0.0f, 0.5f,//10
            1.0f,  0.0f,  1.0f,  0.5f, 0.0f, 0.5f,//11
            0.5f,  1.0f,  0.5f,  0.5f, 0.0f, 0.5f,//3 sciana 12
            0.0f,  0.0f,  1.0f,  0.0f, 0.5f, 0.5f,//13
            1.0f,  0.0f,  1.0f, 0.0f, 0.5f, 0.5f,//14
            0.5f,  1.0f,  0.5f,  0.0f, 0.5f, 0.5f,//4 sciana 15
    };


    glEnable(GL_DEPTH_TEST);
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<GLushort> indices={0,1,2,0,3,2,4,5,6,7,8,9,10,11,12,13,14,15};
    GLuint idx_buffer_handle;
    glGenBuffers(1,&idx_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,idx_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(GLushort),indices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    /////UNIFORM
    auto u_modifiers_index = glGetUniformBlockIndex(program,"Modifiers");
    if(u_modifiers_index == GL_INVALID_INDEX){
        std::cout<<"Cannot find Modifiers uniform block in program";
    }else{
        glUniformBlockBinding(program,u_modifiers_index,0);
    }
    GLuint ubo_handle(0u);
    glGenBuffers(1,&ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER,ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER,8*sizeof(float), nullptr,GL_STATIC_DRAW);
    float strength=0.7;
    float light[3]={0.68,0.9,0.9};
    glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(float),&strength);
    glBufferSubData(GL_UNIFORM_BUFFER,4* sizeof(float),3*sizeof(float),light);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    glBindBufferBase(GL_UNIFORM_BUFFER,0,ubo_handle);
    GLuint ubo_handle_PVM(0u);
    glGenBuffers(1,&ubo_handle_PVM);
    ///UNIFORM PVM
    auto u_transformations_index = glGetUniformBlockIndex(program,"Transformations");
    if(u_transformations_index == GL_INVALID_INDEX){
        std::cout<<"Cannot find Transformations uniform block in program"<<std::endl;
    }else{
        glUniformBlockBinding(program,u_transformations_index,1);
    }
    glBindBuffer(GL_UNIFORM_BUFFER,ubo_handle_PVM);
    /////
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,idx_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 6* sizeof(GLfloat),reinterpret_cast<GLvoid *>(3*sizeof(GL_FLOAT)));
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    ///PVM CD
    auto V =glm::lookAt(glm::vec3{0.0f, 1.0f, -0.75f},
                        glm::vec3{0.5f, 0.5f, 0.5f},
                        glm::vec3{0.0f,1.0f,0.0f});
    auto P = glm::perspective(glm::half_pi<float>(),(float)w/h,0.1f,100.0f);
    auto M = glm::mat4(1.0f);
    glBufferData(GL_UNIFORM_BUFFER,3*sizeof(glm::mat4), nullptr
    ,GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),&P[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),sizeof(glm::mat4),&V[0]);
    glBufferSubData(GL_UNIFORM_BUFFER,2*sizeof(glm::mat4),sizeof(glm::mat4),&M[0]);
    /////
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    glBindBufferBase(GL_UNIFORM_BUFFER,0,ubo_handle);
    glBindBufferBase(GL_UNIFORM_BUFFER,1,ubo_handle_PVM);
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT,reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
}
