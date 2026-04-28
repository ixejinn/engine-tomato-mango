#include <glm/glm.hpp>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include "Resource/Render/Shader.h"
#include "Resource/AssetRegistry.h"
#include "Utils/Logger.h"

namespace tomato {
    Shader::Shader() {
        ReadShaderSourceFile("Resources/Engine/Shaders/shader.vs", "Resources/Engine/Shaders/shader.fs");
    }

    Shader::Shader(const char* vsName, const char* fsName)
    {
        ReadShaderSourceFile(vsName, fsName);
    }

    Shader::~Shader()
    {
        if (programID_)
            glDeleteProgram(programID_);
    }

    void Shader::Create()
    {
        std::unique_ptr<Shader> shaderPtr{new Shader};
        AssetRegistry<Shader>::GetInstance().Register(PrimitiveName, std::move(shaderPtr));

        // std::unique_ptr<Shader> fShaderPtr{ new Shader{"Shaders/text.vs", "Shaders/text.fs"} };
        // AssetRegistry<Shader>::GetInstance().Register("FontShader", std::move(fShaderPtr));
    }

    void Shader::Create(const char* vsName, const char* fsName)
    {
        std::unique_ptr<Shader> ptr{new Shader(vsName, fsName)};
        AssetRegistry<Shader>::GetInstance().Register(fsName, std::move(ptr));
    }

    void Shader::Use() const
    {
        glUseProgram(programID_);
    }

    void Shader::SetUniformBool(const char* name, bool value) const
    {
        SetUniformInt(name, static_cast<int>(value));
    }

    void Shader::SetUniformInt(const char* name, int value) const
    {
        glProgramUniform1i(programID_, glGetUniformLocation(programID_, name), value);
    }

    void Shader::SetUniformFloat(const char* name, float value) const
    {
        glProgramUniform1f(programID_, glGetUniformLocation(programID_, name), value);
    }

    void Shader::SetUniformVec3(const char* name, float v0, float v1, float v2) const
    {
        glProgramUniform3f(programID_, glGetUniformLocation(programID_, name), v0, v1, v2);
    }

    void Shader::SetUniformVec3(const char* name, glm::vec3 value) const
    {
        glProgramUniform3fv(programID_, glGetUniformLocation(programID_, name), 1, glm::value_ptr(value));
    }

    void Shader::SetUniformVec4(const char* name, float v0, float v1, float v2, float v3) const
    {
        glProgramUniform4f(programID_, glGetUniformLocation(programID_, name), v0, v1, v2, v3);
    }

    void Shader::SetUniformVec4(const char* name, glm::vec4 value) const
    {
        glProgramUniform4fv(programID_, glGetUniformLocation(programID_, name), 1, glm::value_ptr(value));
    }

    void Shader::SetUniformMat3(const char* name, glm::mat3 value) const
    {
        glProgramUniformMatrix3fv(programID_, glGetUniformLocation(programID_, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetUniformMat4(const char* name, glm::mat4 value) const
    {
        glProgramUniformMatrix4fv(programID_, glGetUniformLocation(programID_, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::CheckCompileErrors(const GLuint shader, const Type type)
    {
        int success;
        char infoLog[512];

        switch (type)
        {
            case Vertex:
            case Fragment:
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                    TMT_ERR << (type == Vertex ? "Vertex" : "Fragment") << " shader compile error\n"
                            << infoLog;
                }
                break;

            case Program:
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(shader, 512, nullptr, infoLog);
                    TMT_ERR << "Program link error\n"
                            << infoLog;
                }
                break;
        }
    }

    void Shader::ReadShaderSourceFile(const char* vsName, const char* fsName)
    {
        // Read shader source file
        // for vertex shader
        std::ifstream vsFile;
        std::string vsSrcStr;

        // for fragment shader
        std::ifstream fsFile;
        std::string fsSrcStr;

        vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // Open files
            vsFile.open(vsName);
            fsFile.open(fsName);

            // Read file's buffer contents into stream
            std::stringstream vsSs, fsSs;
            vsSs << vsFile.rdbuf();
            fsSs << fsFile.rdbuf();

            // Close file handlers
            vsFile.close();
            fsFile.close();

            // Convert stream into string
            vsSrcStr = vsSs.str();
            fsSrcStr = fsSs.str();
        }
        catch (std::ifstream::failure& e)
        {
            TMT_ERR << "Failed to open file or read error: " << e.what();
            return;
        }

        const char* vsSrc = vsSrcStr.c_str();
        const char* fsSrc = fsSrcStr.c_str();

        // Compile shaders
        // vertex shader
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsSrc, nullptr);
        glCompileShader(vs);
        CheckCompileErrors(vs, Vertex);

        // fragment shader
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsSrc, nullptr);
        glCompileShader(fs);
        CheckCompileErrors(fs, Fragment);

        // link shaders
        programID_ = glCreateProgram();
        glAttachShader(programID_, vs);
        glAttachShader(programID_, fs);
        glLinkProgram(programID_);
        CheckCompileErrors(programID_, Program);

        // Delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
}