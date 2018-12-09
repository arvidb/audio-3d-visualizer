#pragma once

#include "common.h"

#include <GLFW/glfw3.h>
#include <linmath/linmath.h>

#include <experimental/optional>
#include <vector>

namespace exp = std::experimental;

class RenderObject {
    
public:
    void setup_shaders() {
        
        GLuint vertex_shader, fragment_shader;
        
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
        glCompileShader(vertex_shader);
        
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
        glCompileShader(fragment_shader);
        
        this->program = glCreateProgram();
        glAttachShader(this->program, vertex_shader);
        glAttachShader(this->program, fragment_shader);
        glLinkProgram(this->program);
        
        this->mvp_location = glGetUniformLocation(this->program, "MVP");
        this->vpos_location = glGetAttribLocation(this->program, "vPos");
        this->vcol_location = glGetAttribLocation(this->program, "vCol");
    }
    
    void init() {
        
        glGenBuffers(1, &vbo_cube_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &vbo_cube_colors);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_t) * this->colors.size(), this->colors.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ibo_cube_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);

        setup_shaders();
    }
    
    virtual void render() {
        
        glUseProgram(this->program);
        
        glEnableVertexAttribArray(this->vpos_location);
        // Describe our vertices array to OpenGL (it can't guess its format automatically)
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
        glVertexAttribPointer(
                              this->vpos_location, // attribute
                              3,                 // number of elements per vertex, here (x,y,z)
                              GL_FLOAT,          // the type of each element
                              GL_FALSE,          // take our values as-is
                              0,                 // no extra data between each position
                              0                  // offset of first element
                              );
        
        glEnableVertexAttribArray(this->vcol_location);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
        glVertexAttribPointer(
                              this->vcol_location, // attribute
                              3,                 // number of elements per vertex, here (R,G,B)
                              GL_FLOAT,          // the type of each element
                              GL_FALSE,          // take our values as-is
                              0,                 // no extra data between each position
                              0                  // offset of first element
                              );
        
        /* Push each element in buffer_vertices to the vertex shader */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
        glDrawElements(GL_TRIANGLES, this->elements.size(), GL_UNSIGNED_SHORT, 0);
        
        glDisableVertexAttribArray(this->vpos_location);
        glDisableVertexAttribArray(this->vcol_location);

    }
    
    virtual void update(float dt, int w, int h) {
        
        float ratio = w / (float) h;
        
        mat4x4_perspective(this->projection_matrix, 45.0, ratio, 0.1, 100.0);
        mat4x4_identity(this->model_matrix);
        mat4x4_translate_in_place(this->model_matrix, this->x, this->y, this->z);
        mat4x4_rotate(this->model_matrix, this->model_matrix, 0.5, 0.5, 0.5, this->rotation);
        mat4x4 mvp;
        mat4x4_mul(mvp, this->projection_matrix, this->model_matrix);
        
        glUseProgram(this->program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    }
    
    void set_roation(float rot) {
        this->rotation = rot;
    }
    
    void set_translation(exp::optional<float> x, exp::optional<float> y, exp::optional<float> z) {
        if (x) {
            this->x = x.value_or(0.0);
        }
        
        if (y) {
            this->y = y.value_or(0.0);
        }
        
        if (z) {
            this->z = z.value_or(0.0);
        }
    }
    
protected:
    float rotation = {};
    float x = {};
    float y = {};
    float z = {};
    
protected:
    GLuint program;
    GLint mvp_location, vpos_location, vcol_location;
    mat4x4 model_matrix, projection_matrix;
    
    GLuint vbo_cube_vertices, vbo_cube_colors;
    GLuint ibo_cube_elements;
    
    std::vector<vertex_t> vertices;
    std::vector<color_t> colors;
    std::vector<GLushort> elements;
};
