#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "Resource/Render/Mesh.h"
#include "Resource/AssetRegistry.h"
#include "Utils/Logger.h"

namespace tomato {
    Mesh::Mesh(Primitive type) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        switch (type) {
            case Primitive::Plain:
                Plain(vertices, indices);
                break;

            case Primitive::Cube:
                Cube(vertices, indices);
                break;

            case Primitive::Sphere:
                Sphere(vertices, indices);
                break;

            case Primitive::Cylinder:
                Cylinder(vertices, indices);
                break;

            default:
                TMT_WARN << "Invalid primitive mesh type";
                break;
        }

        SetMesh(vertices, indices);
    }

    Mesh::~Mesh() {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ebo_);
    }

    void Mesh::Create() {
        auto& registry = AssetRegistry<Mesh>::GetInstance();
        for (int i = 0; i < static_cast<int>(Primitive::COUNT); ++i)
        {
            auto type = static_cast<Primitive>(i);

            std::unique_ptr<Mesh> ptr{new Mesh(type)};
            registry.Register(GetPrimitiveName(type), std::move(ptr));
        }
    }

    void Mesh::Bind() const {
        glBindVertexArray(vao_);
    }

    void Mesh::Draw(bool drawLine) const {
        glDrawElements(drawLine ? GL_LINE_LOOP : GL_TRIANGLES, vertexCnt_, GL_UNSIGNED_INT, nullptr);
    }

    void Mesh::Plain(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        vertices.reserve(4);    // 4 vertices per plain
        indices.reserve(6);     // 2 triangles per plain, 3 vertices per triangle

        // v0---v3
        // |     |
        // v1---v2
        glm::vec3 v0{-0.5f,  0.5f, 0.0f};
        glm::vec3 v1{-0.5f, -0.5f, 0.0f};
        glm::vec3 v2{ 0.5f, -0.5f, 0.0f};
        glm::vec3 v3{ 0.5f,  0.5f, 0.0f};

        FillMeshData(v0, v1, v2, v3, vertices, indices);
    }

    void Mesh::Cube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        vertices.reserve(24);   // 6 plains per cube
        indices.reserve(36);

        // top
        glm::vec3 v0{-0.5f,  0.5f, -0.5f};
        glm::vec3 v1{-0.5f,  0.5f,  0.5f};
        glm::vec3 v2{ 0.5f,  0.5f,  0.5f};
        glm::vec3 v3{ 0.5f,  0.5f, -0.5f};

        // bottom
        glm::vec3 v4{-0.5f, -0.5f, -0.5f};
        glm::vec3 v5{-0.5f, -0.5f,  0.5f};
        glm::vec3 v6{ 0.5f, -0.5f,  0.5f};
        glm::vec3 v7{ 0.5f, -0.5f, -0.5f};

        FillMeshData(v0, v1, v2, v3, vertices, indices);   // top
        FillMeshData(v1, v5, v6, v2, vertices, indices);
        FillMeshData(v2, v6, v7, v3, vertices, indices);
        FillMeshData(v3, v7, v4, v0, vertices, indices);
        FillMeshData(v0, v4, v5, v1, vertices, indices);
        FillMeshData(v5, v4, v7, v6, vertices, indices); // bottom
    }

    void Mesh::Sphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        constexpr int sectorCnt = 8;    // divide sphere by longitude
        constexpr int stackCnt = 6;     // divide sphere by latitude

        vertices.resize(4 * sectorCnt * (stackCnt - 2) + 3 * sectorCnt * 2);    // pole consists of triangles
        indices.resize(6 * sectorCnt * (stackCnt - 2) + 3 * sectorCnt * 2);

        const auto pi = glm::pi<float>();
        const float sectorStep = 2 * pi / sectorCnt;
        const float stackStep = pi / stackCnt;

        float lon = 0.f;        // longitude
        float lat = 0.5f * pi;  // latitude

        glm::vec3 northPole{0.f, 0.5f, 0.f};
        glm::vec3 coords[stackCnt - 1][sectorCnt];
        glm::vec3 southPole{0.f, -0.5f, 0.f};
        for (auto& row : coords) {
            lat -= stackStep;
            lon = 0.f;

            for (auto& coord : row) {
                coord = {
                        glm::cos(lat) * glm::sin(lon),
                        glm::sin(lat),
                        glm::cos(lat) * glm::cos(lon)
                };
                coord *= 0.5f;

                lon += sectorStep;
            }
        }

        auto& northRow = coords[0];
        for (int j = 0; j < sectorCnt - 1; j++)
            FillMeshData(northPole, northRow[j], northRow[j + 1], vertices, indices);
        FillMeshData(northPole, northRow[sectorCnt - 1], northRow[0], vertices, indices);

        for (int i = 1; i < stackCnt - 1; i++) {
            const int _i = i - 1;

            int j;
            for (j = 0; j < sectorCnt - 1; j++)
            {
                const int j_= j + 1;

                FillMeshData(coords[_i][j], coords[i][j], coords[i][j_], coords[_i][j_], vertices, indices);
            }
            FillMeshData(coords[_i][j], coords[i][j], coords[i][0], coords[_i][0], vertices, indices);
        }

        auto& southRow = coords[stackCnt - 2];
        for (int j = 0; j < sectorCnt - 1; j++)
            FillMeshData(southPole, southRow[j + 1], southRow[j], vertices, indices);
        FillMeshData(southPole, southRow[0], southRow[sectorCnt - 1], vertices, indices);
    }

    void Mesh::Capsule(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {

    }

    void Mesh::Cylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        constexpr int sectorCnt = 10;

        vertices.resize(4 * sectorCnt + 3 * sectorCnt * 2); // top and bottom consist of triangles
        indices.resize(6 * sectorCnt + 3 * sectorCnt * 2);

        const auto pi = glm::pi<float>();
        const float sectorStep = 2 * pi / sectorCnt;

        float lon = 0.f;

        glm::vec3 topCenter{0.f, 0.5f, 0.f};
        glm::vec3 bottomCenter{0.f, -0.5f, 0.f};
        glm::vec3 coords[2][sectorCnt];
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < sectorCnt; j++) {
                coords[i][j] = {
                        glm::cos(lon),
                        -2 * i + 1,
                        -glm::sin(lon)
                };
                coords[i][j] *= 0.5f;

                lon += sectorStep;
            }
        }

        auto& topCircle = coords[0];
        auto& bottomCircle = coords[1];

        for (int j = 0; j < sectorCnt - 1; j++)
        {
            const int j_ = j + 1;
            FillMeshData(topCenter, topCircle[j], topCircle[j_], vertices, indices);
            FillMeshData(bottomCenter, bottomCircle[j_], bottomCircle[j], vertices, indices);
        }
        FillMeshData(topCenter, topCircle[sectorCnt - 1], topCircle[0], vertices, indices);
        FillMeshData(bottomCenter, bottomCircle[0], bottomCircle[sectorCnt - 1], vertices, indices);

        for (int j = 0; j < sectorCnt - 1; j++)
            FillMeshData(topCircle[j], bottomCircle[j], bottomCircle[j + 1], topCircle[j + 1], vertices, indices);
        FillMeshData(topCircle[sectorCnt - 1], bottomCircle[sectorCnt - 1], bottomCircle[0], topCircle[0],
                     vertices, indices);
    }

    void Mesh::FillMeshData(
        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
        const glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        const auto vOffset = vertices.size();
        vertices.emplace_back(v0, normal, glm::vec2{0.f, 1.f});
        vertices.emplace_back(v1, normal, glm::vec2{0.f, 0.f});
        vertices.emplace_back(v2, normal, glm::vec2{1.f, 0.f});
        vertices.emplace_back(v3, normal, glm::vec2{1.f, 1.f});

        indices.emplace_back(vOffset + 1);  // triangle v1 → v3 → v0
        indices.emplace_back(vOffset + 3);
        indices.emplace_back(vOffset + 0);
        indices.emplace_back(vOffset + 1);  // triangle v1 → v2 → v3
        indices.emplace_back(vOffset + 2);
        indices.emplace_back(vOffset + 3);
    }

    void Mesh::FillMeshData(
        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
        std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
        const glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        const auto vOffset = vertices.size();
        vertices.emplace_back(v0, normal, glm::vec2{0.5f, 1.0f});
        vertices.emplace_back(v1, normal, glm::vec2{0.f, 0.f});
        vertices.emplace_back(v2, normal, glm::vec2{1.f, 0.f});

        indices.emplace_back(vOffset + 0);
        indices.emplace_back(vOffset + 1);
        indices.emplace_back(vOffset + 2);
    }

    void Mesh::SetMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        vertexCnt_ = static_cast<int>(indices.size());

        // Generate vertex array object and buffer objects
        glCreateVertexArrays(1, &vao_);
        glCreateBuffers(1, &vbo_);
        glCreateBuffers(1, &ebo_);

        // Upload vertex data to VBO
        glNamedBufferData(vbo_, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
        // Bind VBO to vertex binding slot 0 of VAO
        glVertexArrayVertexBuffer(vao_, 0, vbo_, 0, sizeof(Vertex));

        // Vertex attribute 0: Vertex::position
        // Enable attribute slot in shader
        glEnableVertexArrayAttrib(vao_, 0);
        // Link attribute 0 to vertex binding slot 0 (where VBO is bound)
        glVertexArrayAttribBinding(vao_, 0, 0);
        // Define the data layout (starting at Vertex::position offset)
        glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));

        // Vertex attribute 1: Vertex::normal
        glEnableVertexArrayAttrib(vao_, 1);
        glVertexArrayAttribBinding(vao_, 1, 0);
        glVertexArrayAttribFormat(vao_, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

        // Vertex attribute 2: Vertex::uv
        glEnableVertexArrayAttrib(vao_, 2);
        glVertexArrayAttribBinding(vao_, 2, 0);
        glVertexArrayAttribFormat(vao_, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

        // Upload index(element) data to EBO
        glNamedBufferData(ebo_, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);
        // Bind EBO to element binding slot of VAO
        glVertexArrayElementBuffer(vao_, ebo_);
    }
}