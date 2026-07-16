#ifndef MANGO_SHADER_H
#define MANGO_SHADER_H

#include <filesystem>
#include <glm/fwd.hpp>
#include <glad/glad.h>

namespace tomato {
    class Shader {
    public:
        constexpr static auto PrimitiveName = "Shader::Primitive";

    private:
        Shader();
        Shader(const char* vsName, const char* fsName);
        Shader(const std::filesystem::path& vsPath, const std::filesystem::path& fsPath);

    public:
        ~Shader();

        static void Cleanup() {}
        static void Create();
        static void Create(const char* vsName, const char* fsName);
        static void Create(const std::filesystem::path& vsPath, const std::filesystem::path& fsPath);

        void Use() const;

        void SetUniformBool(const char* name, bool value) const;
        void SetUniformInt(const char* name, int value) const;
        void SetUniformFloat(const char* name, float value) const;

        void SetUniformVec3(const char* name, float v0, float v1, float v2) const;
        void SetUniformVec3(const char* name, glm::vec3 value) const;
        void SetUniformVec4(const char* name, float v0, float v1, float v2, float v3) const;
        void SetUniformVec4(const char* name, glm::vec4 value) const;

        void SetUniformMat3(const char* name, glm::mat3 value) const;
        void SetUniformMat4(const char* name, glm::mat4 value) const;

    private:
        enum Type {
            Vertex,
            Fragment,
            Program
        };

        static void CheckCompileErrors(GLuint shader, Type type);

        void ReadShaderSourceFile(const char* vsName, const char* fsName);

        GLuint programID_{0};
    };
}

#endif //MANGO_SHADER_H