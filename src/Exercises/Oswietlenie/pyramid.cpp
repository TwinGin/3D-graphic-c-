//
// Created by tg on 20.12.2020.
//
#include "pyramid.h"
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty/src/stb/stb_image.h"
#include <string>
Pyramid ::Pyramid() {
    int width, height, n_channels;
    auto texture_filename = std::string(PROJECT_DIR) + "/Textures/multicolor.png";
    uint8_t *data = stbi_load(texture_filename.c_str(),&width,&height,&n_channels,0);
    glBindTexture(GL_TEXTURE_2D,diffuse_texture_);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, width, height,0, GL_RGB,GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,0);

    std::vector<GLfloat> vertices = {
            -.5f,  0.0f,  -.5f,  0.191f, 0.5f, //0
            .5f,  0.0f,  -.5f,  0.5f, 0.809f, //1
            .5f,  0.0f,  0.5f,  0.809f, 0.5f, //2
            -.5f,  0.0f,  0.5f,  0.5f, 0.191f, //3
            0.0f,  1.0f,  0.0f,  0.0f, 1.0f, //4 Czubek
            0.0f,  1.0f,  0.0f,  1.0f, 1.0f, //5 Czubek
            0.0f,  1.0f,  0.0f,  1.0f, 0.0f, //6 Czubek
            0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //7 Czubek
    };
    glGenBuffers(2, this->buffers_);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffers_[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //lustrzane odbicie????
    std::vector<GLushort> indices={0,1,2,
                                   0,2,3,
                                   1,0,4,
                                   1,5,2,
                                   3,2,6,
                                   0,3,7};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers_[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(GLushort),indices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    glGenVertexArrays(1,&this->vao_);
    glBindVertexArray(this->vao_);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffers_[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 5* sizeof(GLfloat),reinterpret_cast<GLvoid *>(3*sizeof(GL_FLOAT)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->buffers_[1]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}
Pyramid::~Pyramid() {
    glDeleteVertexArrays(1,&this->vao_);
    glDeleteBuffers(2,this->buffers_);
}
void Pyramid::draw() {
    glBindVertexArray(this->vao_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,diffuse_texture_);
    glDrawElements(GL_TRIANGLES,18,GL_UNSIGNED_SHORT,reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);
}

