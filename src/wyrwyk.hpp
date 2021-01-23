#ifndef WYRWYK_WYRWYK_HPP
#define WYRWYK_WYRWYK_HPP

#include "config.hpp"
#include "utils/debug.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class GLFWwindow;

class Parser;

class Wyrwyk final
{
    static const size_t VERTICES_COUNT = 4;
    static const size_t INDICES_COUNT = 6;
    struct Vertex
    {
        float x, y;
    };
    using Index = uint;

public:
    Wyrwyk( int argc, char* argv[] );
    ~Wyrwyk();
    void Run();

    [[nodiscard]] int ReturnCode() const;

private:
    void InitImGui();
    void DrawImGui();
    [[nodiscard]] static uint TryCompileShader( uint type, const std::string& source );
    [[nodiscard]] static uint TryCreateShaders( const std::string& vertexShaderSource, const std::string& fragmentShaderSource );
    [[nodiscard]] static std::string LoadFromFile( const std::string& path );
    std::string SubstituteExpression( std::string text );
    void Exit( int returnCode );
    void Update();
    void Render();

    bool m_markedForExit = false;
    int m_returnCode = 0;
    GLFWwindow* m_window{};
    char m_expressionBuffer[ WYRWYK_MAX_EXPR_LEN ]{};
    float m_a = 1.0f, m_b = 2.0f, m_c = 3.0f, m_t = 0.0f, m_pi = 2.0f * std::asin( 1.0f );
    uint m_shader{};
    float m_framebuffer[ 2 ] = { 1000.0f, 1000.0f };
    Vertex m_vertices[ VERTICES_COUNT ] = { { -1.0f, -1.0f }, { -1.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, -1.0f } };
    Index m_indices[ INDICES_COUNT ] = { 0, 1, 2, 2, 3, 0 };
    std::string m_vertexShaderIn;
    std::string m_vertexShaderOut;
    std::string m_fragmentShaderIn;
    std::string m_fragmentShaderOut;
    uint m_vertexArrayObject{};
    uint m_indexBufferObject{};
    std::string m_expression;
    float m_boundingBox[ 4 ] = { -10.0f, -10.0f, 20.0f, 20.0f };
    int m_rmbState;
    double m_startMove[ 2 ] = { 0.0, 0.0 };
    std::string m_baseTitle = "Wyrwyk - Implicit Curve Rasterizer";
    float m_falseColor[ 3 ] = { 0.98f, 0.98f, 1.0f };
    float m_trueColor[ 3 ] = { 0.0f, 0.0f, 0.0f };
    float m_symbols[ 256 * 2 ];
    std::unique_ptr< Parser > m_parser;
    std::chrono::high_resolution_clock::time_point m_timer;
    int m_supersampling = 1;

    void InitGlfw();
    void InitGlew();
    void InitOpenGlDebug();
    void InitVertexArrayObject();
    void InitVertexBufferObject();
    void InitIndexBufferObject();
    void LoadShaderSources();
    void SubstituteShaderSources();
    void InitShaders();
    void RegisterGlfwCallbacks();
    void Terminate();
};

#endif // WYRWYK_WYRWYK_HPP
