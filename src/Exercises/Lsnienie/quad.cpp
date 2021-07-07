//
// Created by tg on 20.12.2020.
//
#include "quad.h"
#include <vector>
Quad ::Quad() {

    std::vector<GLfloat> vertices = {
            -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,1.0f,0.0f, //0
            1.0f,   0.0f, -1.0f, 1.0f, 1.0f,  0.0f,1.0f,0.0f,
            1.0f,   0.0f,  1.0f, 1.0f, 0.0f,  0.0f,1.0f,0.0f,
            -1.0f,  0.0f,  1.0f, 0.0f, 0.0f,  0.0f,1.0f,0.0f,

    };
    glGenBuffers(2, this->buffers_);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffers_[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //lustrzane odbicie????
    std::vector<GLushort> indices={0,2,1,
                                   0,3,2,};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers_[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(GLushort),indices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    glGenVertexArrays(1,&this->vao_);
    glBindVertexArray(this->vao_);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffers_[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 8* sizeof(GLfloat),reinterpret_cast<GLvoid *>(3*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT, GL_FALSE,8*sizeof(GL_FLOAT),reinterpret_cast<GLvoid *>(5*sizeof(GL_FLOAT)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->buffers_[1]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}
Quad::~Quad() {
    glDeleteVertexArrays(1,&this->vao_);
    glDeleteBuffers(2,this->buffers_);
}
void Quad::draw() {
    glBindVertexArray(this->vao_);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D,diffuse_texture_);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D,0);
}

