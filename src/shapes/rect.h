#ifndef GRAPHICS_RECT_H
#define GRAPHICS_RECT_H

#include "shape.h"
#include "../framework/shader.h"
#include <iostream>
using glm::vec2, glm::vec3;


class Rect : public Shape {
private:
    /// @brief Initializes the vertices and indices of the square
    void initVectors() override;
public:
    /// @brief Construct a new Square object
    /// @details This constructor will call the InitRenderData function.
    /// @param shader The shader to use
    /// @param pos The position of the square
    /// @param size The size of the square
    /// @param color The color of the square
    Rect(Shader & shader, vec2 pos, vec2 size, struct color color);

    Rect(Rect const& other);

    /// @brief Destroy the Square object and delete it's VAO and VBO
    ~Rect();

    /// @brief Binds the VAO and calls the virtual draw function
    void draw() const override;

    float getLeft() const override;
    float getRight() const override;
    float getTop() const override;
    float getBottom() const override;

    // TODO: decide if it makes sense to use static overlapping functions
    static bool isOverlapping(const Rect& r1, const Rect& r2);
    bool isOverlapping(const Rect& other) const;
    bool isOverlapping(const Shape& other) const override;
};


#endif //GRAPHICS_RECT_H
