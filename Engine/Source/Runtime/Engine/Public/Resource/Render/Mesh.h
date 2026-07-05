#ifndef MANGO_MESH_H
#define MANGO_MESH_H

#include <vector>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace tomato {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        Vertex() = default;
        Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& uv)
            : position(pos), normal(normal), uv(uv) {}
    };

    class Mesh
    {
    public:
#define TMT_MESH_PRIMITIVE_LIST(X)  \
    X(Plain,    "Plain")            \
    X(LBPlain,  "LBPlain")          \
    X(Cube,     "Cube")             \
    X(Sphere,   "Sphere")           \
    X(Cylinder, "Cylinder")         

    enum class Primitive
    {
#define X(Enum, Display) Enum,
        TMT_MESH_PRIMITIVE_LIST(X)
#undef X
        COUNT
    };

    struct PrimitiveMeta
    {
        Primitive primitive;
        const char* name;
    };

    static constexpr PrimitiveMeta PrimitiveMetas[] =
    {
#define X(Enum, Display) {Primitive::Enum, Display},
        TMT_MESH_PRIMITIVE_LIST(X)
#undef X
    };
#undef TMT_MESH_PRIMITIVE_LIST

        static constexpr const char* GetPrimitiveName(Primitive type)
        {
            switch (type) {
                case Primitive::Plain:
                    return "Primitive::Plain";
                case Primitive::LBPlain:
                    return "Primitive::LBPlain";

                case Primitive::Cube:
                default:
                    return "Primitive::Cube";

                case Primitive::Sphere:
                    return "Primitive::Sphere";

                case Primitive::Cylinder:
                    return "Primitive::Cylinder";
            }
        }

    private:
        explicit Mesh(Primitive type);

    public:
        ~Mesh();

        static void Cleanup() {}
        static void Create();

        void Bind() const;
        void Draw(bool drawLine = false) const;

    private:
        static void Plain(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
        static void LBPlain(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
        static void Cube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
        static void Sphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
        static void Capsule(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
        static void Cylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

        /**
         * @brief Populates vertex and index buffers for a single face (quad).
         * @param vertices Reference to the vertex vector to ve populated.
         * @param indices Reference to the index vector to be populated.
         *
         * v0---v3
         * |  / |
         * | /  |
         * v1---v2
         */
        static void FillMeshData(
            const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
            std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

        /**
         * @brief Populates vertex and index buffers for a single face (triangle).
         * @param vertices Reference to the vertex vector to ve populated.
         * @param indices Reference to the index vector to be populated.
         *
         *    v0
         *   /  \
         * v1---v2
         */
        static void FillMeshData(
            const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
            std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

        void SetMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

        /// Stores the state related to vertex attribute settings.
        GLuint vao_{0};

        /// Stores the actual vertex data (positions, texture coordinates, ...)
        GLuint vbo_{0};

        /// Stores the indices used for element drawing. Indices array.
        GLuint ebo_{0};

        int vertexCnt_{0};
    };
}

#endif //MANGO_MESH_H