#ifndef GRAPHICS_SHADERMANAGER_H
#define GRAPHICS_SHADERMANAGER_H

#include "shader.h"

#include <map>
#include <iostream>

class ShaderManager {
public:
    /// @brief Default constructor
    ShaderManager() = default;
    /// @brief Default destructor
    /// @details Clears the shaders map
    ~ShaderManager();


    /// @brief Calls loadShaderFromFile() and stores the shader in the shaders map
    /// @param vShaderFile The vertex shader file
    /// @param fShaderFile The fragment shader file
    /// @param gShaderFile The geometry shader file (optional)
    /// @param name Name used for the shader in the shaders map
    /// @return The shader that was loaded
    Shader loadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name);

    /// @brief Returns a reference to the shader with the given name in the shaders map
    /// @param name The name of the shader
    /// @return The shader with the given name
    Shader& getShader(std::string name);

     /// @brief Clears the shaders map
    void clear();

private:
    /// @brief A map of shaders, with the key being the name of the shader
    std::map<std::string, Shader> shaders;

     /// @brief Loads and compiles a shader from a file
     /// @details This function is private because we only want to load shaders from within this class
     /// @param vShaderFile The vertex shader file
     /// @param fShaderFile The fragment shader file
     /// @param gShaderFile The geometry shader file (optional)
     /// @return The shader that was loaded
    Shader loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile=nullptr);};

#endif //GRAPHICS_SHADERMANAGER_H
