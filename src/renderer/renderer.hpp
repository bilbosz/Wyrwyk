#ifndef WYRWYK_RENDERER_HPP
#define WYRWYK_RENDERER_HPP

#include <cstddef>
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
    using Index = unsigned int;

public:
    Renderer();
    ~Renderer();

    [[nodiscard]] bool Init();
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
    unsigned int m_shader{};
    unsigned int m_vertexArrayObject{};
    unsigned int m_vertexBufferObject{};
    unsigned int m_indexBufferObject{};
    // Graph is rendered to a texture only when something changes and the texture is copied to the window every frame
    unsigned int m_targetFramebuffer{};
    unsigned int m_targetTexture{};
    int m_targetSize[ 2 ] = { 0, 0 };
    std::string m_vertexShader;
    std::string m_fragmentShader;
    mutable std::map< std::string, int > m_locations;
    mutable bool m_dirty = true;
    bool m_initialized = false;

    void InitOpenGlDebug();
    void InitVertexArrayObject();
    void InitVertexBufferObject();
    void InitIndexBufferObject();
    void InitRenderTarget();
    void LoadShaderSources();
    [[nodiscard]] bool InitShaders();
    [[nodiscard]] static unsigned int CompileShader( unsigned int type, const std::string& source );
    [[nodiscard]] static unsigned int CreateShaders( const std::string& vertexShaderSource, const std::string& fragmentShaderSource );
    int GetUniformLocation( const std::string& name ) const;
};

#endif // WYRWYK_RENDERER_HPP
