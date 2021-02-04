#ifndef WYRWYK_RENDERER_HPP
#define WYRWYK_RENDERER_HPP

#include <map>
#include <string>

class Renderer
{
    static const size_t VERTICES_COUNT = 4;
    static const size_t INDICES_COUNT = 6;

    struct Vertex
    {
        float x, y;
    };
    using Index = uint;

public:
    Renderer();
    ~Renderer();

    void Init();
    void Render() const;
    void SetFramebufferSize( float w, float h );
    const float* GetFramebufferSize() const;
    [[nodiscard]] float GetFramebufferWidth() const;
    [[nodiscard]] float GetFramebufferHeight() const;

    void SetUniform1fv( const std::string& name, const float* values, int count = 1 );
    void SetUniform2fv( const std::string& name, const float* values, int count = 1 );
    void SetUniform3fv( const std::string& name, const float* values, int count = 1 );
    void SetUniform4fv( const std::string& name, const float* values, int count = 1 );

private:
    float m_framebufferSize[ 2 ] = { 500.0f, 500.0f };
    Vertex m_vertices[ VERTICES_COUNT ] = { { -1.0f, -1.0f }, { -1.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, -1.0f } };
    Index m_indices[ INDICES_COUNT ] = { 0, 1, 2, 2, 3, 0 };
    uint m_shader{};
    uint m_vertexArrayObject{};
    uint m_indexBufferObject{};
    std::string m_vertexShader;
    std::string m_fragmentShader;
    mutable std::map< std::string, int > m_locations;
    mutable bool m_dirty = true;

    void InitOpenGlDebug();
    void InitVertexArrayObject();
    void InitVertexBufferObject();
    void InitIndexBufferObject();
    void LoadShaderSources();
    void InitShaders();
    [[nodiscard]] static uint CompileShader( uint type, const std::string& source );
    [[nodiscard]] static uint CreateShaders( const std::string& vertexShaderSource, const std::string& fragmentShaderSource );
    int GetUniformLocation( const std::string& name ) const;
};

#endif // WYRWYK_RENDERER_HPP
