//
// Created by tg on 20.12.2020.
//
#include "pyramid.h"
#include <vector>
Pyramid ::Pyramid() {
    std::vector<GLfloat> vertices = {
            -.5f,  0.0f,  -.5f,  1.0f, 0.0f, 0.0f,//0
            .5f,  0.0f,  -.5f,  1.0f, 0.0f, 0.0f,//1
            .5f,  0.0f,  0.5f,  1.0f, 0.0f, 0.0f,//2
            -.5f,  0.0f,  0.5f,  1.0f, 0.0f, 0.0f,//PODSTAWA 3 CZERWONA
            -.5f,  0.0f,  -.5f,  0.0f, 1.0f, 0.0f,//4
            0.5f,  0.0f,  -.5f,  0.0f, 1.0f, 0.0f,//5
            0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//1 sciana 6 ZIELONA
            -.5f,  0.0f,  -.5f,  0.0f, 0.0f, 1.0f,//7
            -.5f,  0.0f,  0.5f,  0.0f, 0.0f, 1.0f,//8
            0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,//2 sciana 9 NIEBIESKA
            0.5f,  0.0f,  -.5f,  0.5f, 0.0f, 0.5f,//10
            0.5f,  0.0f,  0.5f,  0.5f, 0.0f, 0.5f,//11
            0.0f,  1.0f,  0.0f,  0.5f, 0.0f, 0.5f,//3 sciana 12 FIOLETOWA
            -.5f,  0.0f,  0.5f,  0.0f, 0.5f, 0.5f,//13
            0.5f,  0.0f,  0.5f, 0.0f, 0.5f, 0.5f,//14
            0.0f,  1.0f,  0.0f,  0.0f, 0.5f, 0.5f,//4 sciana 15 MORSKA?
    };
    glGenBuffers(2, this->buffers_);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffers_[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //lustrzane odbicie????
    std::vector<GLushort> indices={0,1,2,0,2,3,5,4,6,8,9,7,11,10,12,13,14,15};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers_[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(GLushort),indices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    glGenVertexArrays(1,&this->vao_);
    glBindVertexArray(this->vao_);
    glBindBuffer(GL_ARRAY_BUFFER, this->buffers_[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 6* sizeof(GLfloat),reinterpret_cast<GLvoid *>(3*sizeof(GL_FLOAT)));
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
    glDrawElements(GL_TRIANGLES,18,GL_UNSIGNED_SHORT,reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
}

