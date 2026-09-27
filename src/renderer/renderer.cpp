#include "renderer/renderer.hpp"
#include "utils/debug.hpp"
#include "utils/load-file.hpp"
#include "utils/resources.hpp"
#include <glad/gl.h>
#include <iostream>
#include <vector>

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
    // Requires the OpenGL context to be still current
    if( m_initialized )
    {
        glDeleteProgram( m_shader );
        glDeleteFramebuffers( 1, &m_targetFramebuffer );
        glDeleteTextures( 1, &m_targetTexture );
        glDeleteBuffers( 1, &m_indexBufferObject );
        glDeleteBuffers( 1, &m_vertexBufferObject );
        glDeleteVertexArrays( 1, &m_vertexArrayObject );
    }
}

bool Renderer::Init()
{
    InitOpenGlDebug();
    InitVertexArrayObject();
    InitVertexBufferObject();
    InitIndexBufferObject();
    glGenFramebuffers( 1, &m_targetFramebuffer );
    glGenTextures( 1, &m_targetTexture );
    m_initialized = true;
    InitRenderTarget();
    LoadShaderSources();
    return InitShaders();
}

void Renderer::SetFramebufferSize( float w, float h )
{
    m_framebufferSize[ 0 ] = w;
    m_framebufferSize[ 1 ] = h;
    if( m_initialized )
    {
        InitRenderTarget();
    }
}

const float* Renderer::GetFramebufferSize() const
{
    return m_framebufferSize;
}

float Renderer::GetFramebufferWidth() const
{
    return m_framebufferSize[ 0 ];
}

float Renderer::GetFramebufferHeight() const
{
    return m_framebufferSize[ 1 ];
}

#ifdef DEBUG
static void GLAPIENTRY OpenGlDebugCallback(
    [[maybe_unused]] GLenum source,
    GLenum type,
    [[maybe_unused]] GLuint id,
    [[maybe_unused]] GLenum severity,
    [[maybe_unused]] GLsizei length,
    [[maybe_unused]] const GLchar* message,
    [[maybe_unused]] const void* userParam )
{
    if( type == GL_DEBUG_TYPE_ERROR )
    {
        ASSERT( false, "GL Error: " << message );
    }
}
#endif

void Renderer::InitOpenGlDebug()
{
#ifdef DEBUG
    // Debug output is not available everywhere, e.g. on macOS
    if( GLAD_GL_KHR_debug )
    {
        glEnable( GL_DEBUG_OUTPUT );
        glDebugMessageCallback( OpenGlDebugCallback, nullptr );
    }
#endif
}

void Renderer::Render() const
{
    if( m_targetSize[ 0 ] <= 0 || m_targetSize[ 1 ] <= 0 )
    {
        return;
    }

    if( m_dirty )
    {
        m_dirty = false;
        glBindFramebuffer( GL_FRAMEBUFFER, m_targetFramebuffer );
        glViewport( 0, 0, m_targetSize[ 0 ], m_targetSize[ 1 ] );
        glClear( GL_COLOR_BUFFER_BIT );

        glUseProgram( m_shader );
        glBindVertexArray( m_vertexArrayObject );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject );

        glDrawElements( GL_TRIANGLES, sizeof( m_indices ) / sizeof( m_indices[ 0 ] ), GL_UNSIGNED_INT, nullptr );
    }

    glBindFramebuffer( GL_READ_FRAMEBUFFER, m_targetFramebuffer );
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
    glBlitFramebuffer(
        0, 0, m_targetSize[ 0 ], m_targetSize[ 1 ], 0, 0, m_targetSize[ 0 ], m_targetSize[ 1 ], GL_COLOR_BUFFER_BIT, GL_NEAREST );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_targetSize[ 0 ], m_targetSize[ 1 ] );
}

void Renderer::InitVertexArrayObject()
{
    glGenVertexArrays( 1, &m_vertexArrayObject );
    glBindVertexArray( m_vertexArrayObject );
}

void Renderer::InitVertexBufferObject()
{
    glGenBuffers( 1, &m_vertexBufferObject );
    glBindBuffer( GL_ARRAY_BUFFER, m_vertexBufferObject );
    glBufferData( GL_ARRAY_BUFFER, sizeof( m_vertices ), m_vertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), nullptr );
}

void Renderer::InitIndexBufferObject()
{
    glGenBuffers( 1, &m_indexBufferObject );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( m_indices ), m_indices, GL_STATIC_DRAW );
}

void Renderer::InitRenderTarget()
{
    auto w = static_cast< int >( m_framebufferSize[ 0 ] );
    auto h = static_cast< int >( m_framebufferSize[ 1 ] );
    if( w <= 0 || h <= 0 || ( w == m_targetSize[ 0 ] && h == m_targetSize[ 1 ] ) )
    {
        // Minimized window or no change
        return;
    }
    m_targetSize[ 0 ] = w;
    m_targetSize[ 1 ] = h;

    glBindTexture( GL_TEXTURE_2D, m_targetTexture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glBindFramebuffer( GL_FRAMEBUFFER, m_targetFramebuffer );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_targetTexture, 0 );
    CHECK( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    m_dirty = true;
}

void Renderer::LoadShaderSources()
{
    m_vertexShader = LoadFile( ResourcePath( "shaders/default.vert" ) );
    m_fragmentShader = LoadFile( ResourcePath( "shaders/implicit-2d-v2.frag" ) );
}

bool Renderer::InitShaders()
{
    m_shader = CreateShaders( m_vertexShader, m_fragmentShader );
    if( m_shader == 0 )
    {
        return false;
    }
    glUseProgram( m_shader );
    return true;
}

unsigned int Renderer::CompileShader( unsigned int type, const std::string& source )
{
    GLuint id = glCreateShader( type );
    const char* source_ = source.c_str();
    glShaderSource( id, 1, &source_, nullptr );
    glCompileShader( id );

    GLint result;
    glGetShaderiv( id, GL_COMPILE_STATUS, &result );
    if( result == GL_FALSE )
    {
        GLint length = 0;
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );
        std::vector< char > message( static_cast< size_t >( length ) + 1 );
        glGetShaderInfoLog( id, static_cast< GLsizei >( message.size() ), nullptr, message.data() );
        std::cerr << ( type == GL_VERTEX_SHADER ? "Vertex" : "Fragment" ) << " shader compilation failed:\n" << message.data() << std::endl;
        glDeleteShader( id );
        return 0;
    }

    return id;
}

unsigned int Renderer::CreateShaders( const std::string& vertexShaderSource, const std::string& fragmentShaderSource )
{
    GLuint vertexShader = CompileShader( GL_VERTEX_SHADER, vertexShaderSource );
    GLuint fragmentShader = CompileShader( GL_FRAGMENT_SHADER, fragmentShaderSource );
    if( vertexShader == 0 || fragmentShader == 0 )
    {
        glDeleteShader( vertexShader );
        glDeleteShader( fragmentShader );
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glLinkProgram( program );

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    GLint result;
    glGetProgramiv( program, GL_LINK_STATUS, &result );
    if( result == GL_FALSE )
    {
        GLint length = 0;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &length );
        std::vector< char > message( static_cast< size_t >( length ) + 1 );
        glGetProgramInfoLog( program, static_cast< GLsizei >( message.size() ), nullptr, message.data() );
        std::cerr << "Shader program linking failed:\n" << message.data() << std::endl;
        glDeleteProgram( program );
        return 0;
    }

    return program;
}

void Renderer::SetUniform1fv( const std::string& name, const float* values, int count )
{
    auto location = GetUniformLocation( name );
    glUniform1fv( location, count, values );
    m_dirty = true;
}

void Renderer::SetUniform2fv( const std::string& name, const float* values, int count )
{
    auto location = GetUniformLocation( name );
    glUniform2fv( location, count, values );
    m_dirty = true;
}

void Renderer::SetUniform3fv( const std::string& name, const float* values, int count )
{
    auto location = GetUniformLocation( name );
    glUniform3fv( location, count, values );
    m_dirty = true;
}

void Renderer::SetUniform4fv( const std::string& name, const float* values, int count )
{
    auto location = GetUniformLocation( name );
    glUniform4fv( location, count, values );
    m_dirty = true;
}

int Renderer::GetUniformLocation( const std::string& name ) const
{
    auto it = m_locations.find( name );
    int location;
    if( it != m_locations.cend() )
    {
        location = it->second;
    }
    else
    {
        location = glGetUniformLocation( m_shader, name.c_str() );
        m_locations.emplace( name, location );
    }
    return location;
}
