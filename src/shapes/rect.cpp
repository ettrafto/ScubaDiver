#include "rect.h"
#include "../framework/color.h"

Rect::Rect(Shader & shader, vec2 pos, vec2 size, struct color color)
    : Shape(shader, pos, size, color) {
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Rect::Rect(Rect const& other) : Shape(other) {
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Rect::~Rect() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Rect::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Rect::initVectors() {
    this->vertices.insert(vertices.end(), {
         0.5f, -0.5f,  // x, y of bottom right corner
         0.5f, 0.5f, // top right
         -0.5f, 0.5f, // top left
         -0.5f, -0.5f, // bottom left
    });

    this->indices.insert(indices.end(), {
        0, 1, 2, // First triangle
        1, 2, 3  // Second triangle
    });
}
// Overridden Getters from Shape
float Rect::getLeft() const        { return pos.x - (size.x / 2); }
float Rect::getRight() const       { return pos.x + (size.x / 2); /* placeholder for compilation */ }
float Rect::getTop() const         { return pos.y + (size.y / 2); /* placeholder for compilation */ }
float Rect::getBottom() const      { return pos.y - (size.y / 2); /* placeholder for compilation */ }
