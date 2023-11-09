#include "rect.h"

Rect::Rect(Shader & shader, vec2 pos, vec2 size, struct color color)
        : Shape(shader, pos, size, color), outlineColor({1, 0, 0, 1}) {
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Rect::Rect(Rect const& other) : Shape(other), outlineColor({1, 0, 0, 1}) {
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
    glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Rect::initVectors() {
    this->vertices.insert(vertices.end(), {
        -0.5f, 0.5f,   // Top left
         0.5f, 0.5f,   // Top right
        -0.5f, -0.5f,  // Bottom left
         0.5f, -0.5f   // Bottom right
    });

    this->indices.insert(indices.end(), {
        0, 1, 2, // First triangle
        1, 2, 3  // Second triangle
    });
}
// Overridden Getters from Shape
float Rect::getLeft() const        { return pos.x - (size.x / 2); }
float Rect::getRight() const       { return pos.x + (size.x / 2); }
float Rect::getTop() const         { return pos.y + (size.y / 2); }
float Rect::getBottom() const      { return pos.y - (size.y / 2); }

bool Rect::isOverlapping(const Rect &r1, const Rect &r2) {
    // TODO: Implement this method
    // There are only two cases when rectangles are *not* overlapping:
    //    1. when one is to the left of the other
    //    2. when one is above the other
    if(r2.getLeft() >= r1.getRight()){
        return false;
    }
    if(r2.getTop() <= r1.getBottom()){
        return false;
    }
    if(r1.getLeft() >= r2.getRight()){
        return false;
    }
    if(r1.getTop() <= r2.getBottom()){
        return false;
    }
    else{
        return true;
    }
}

bool Rect::isOverlapping(const Rect &other) const {
    return isOverlapping(*this, other);
}

bool Rect::isOverlapping(const Shape &other) const {
    // Dynamic cast to check if the other shape is a Rect
    const Rect* otherRect = dynamic_cast<const Rect*>(&other);
    if (otherRect) {
        return isOverlapping(*this, *otherRect);
    }
    return false;
}

// Function to set the hover state
void Rect::setHover(bool isHovered) {
    this->hovered = isHovered;
}

// Function to check if the rectangle is hovered
bool Rect::isHovered() {
    return this->hovered;
}

// Implementation of isMouseOver method
bool Rect::isMouseOver(double mouseX, double mouseY) const {
    // Adjust for the position and size of the rectangle
    return mouseX >= getLeft() && mouseX <= getRight() &&
           mouseY >= getBottom() && mouseY <= getTop();
}

void Rect::renderOutline(Shader &shader) const {
    // Set the shader to use
    shader.use();

    // Set the color for the outline
    shader.setVector4f("color", outlineColor.vec);

    // Bind the VAO for the outline
    glBindVertexArray(outlineVAO);

    // Draw the outline
    glDrawElements(GL_TRIANGLES, outlineIndices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
}

void Rect::initOutline() {
    float outlineScale = 1.1f; // Adjust this value as needed to make the outline bigger

    outlineVertices = {
            -0.5f * outlineScale,  0.5f * outlineScale,  // Top left
            0.5f * outlineScale,  0.5f * outlineScale,  // Top right
            -0.5f * outlineScale, -0.5f * outlineScale,  // Bottom left
            0.5f * outlineScale, -0.5f * outlineScale   // Bottom right
    };

    outlineIndices = {
            0, 1, 2, // First triangle
            1, 2, 3  // Second triangle
    };

    // Generate and bind the VAO for the outline
    glGenVertexArrays(1, &outlineVAO);
    glBindVertexArray(outlineVAO);

    // Generate and bind the VBO for the outline
    glGenBuffers(1, &outlineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);
    glBufferData(GL_ARRAY_BUFFER, outlineVertices.size() * sizeof(float), outlineVertices.data(), GL_STATIC_DRAW);

    // Generate and bind the EBO for the outline
    glGenBuffers(1, &outlineEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outlineEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, outlineIndices.size() * sizeof(unsigned int), outlineIndices.data(), GL_STATIC_DRAW);

    // Set the vertex attributes pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind the VAO
    glBindVertexArray(0);
}
