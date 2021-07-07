//
// Created by tg on 20.12.2020.
//
#pragma once
#include "glad/glad.h"
#include "phong_material.h"

#ifndef INC_3DGRAPHICSEXAMPLES_PYRAMID_H
#define INC_3DGRAPHICSEXAMPLES_PYRAMID_H


class Quad {
public:
    Quad();
    ~Quad();
    void draw();
    Quad(const Quad& rhs)=delete;
    Quad &operator =(const Quad& rhs) = delete;
    void operator=(Quad&& rhs)=delete;
    Quad(Quad &&rhs)=delete;
    void set_material (PhongMaterial *material){material_=material;}
    PhongMaterial *material(){return material_;}
private:
    GLuint vao_;
    GLuint  buffers_[2];
    GLuint diffuse_texture_;
    PhongMaterial *material_;
};

#endif //INC_3DGRAPHICSEXAMPLES_PYRAMID_H
