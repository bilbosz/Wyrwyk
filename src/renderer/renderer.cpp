#include "renderer/renderer.hpp"
#include "utils/debug.hpp"
#include "utils/load-file.hpp"
#include <GL/glew.h>

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
    glDeleteProgram( m_shader );
}

void Renderer::Init()
{
    InitOpenGlDebug();
    InitVertexArrayObject();
    InitVertexBufferObject();
    InitIndexBufferObject();
    LoadShaderSources();
    InitShaders();
}

void Renderer::SetFramebufferSize( float w, float h )
{
    m_framebufferSize[ 0 ] = w;
    m_framebufferSize[ 1 ] = h;
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

void Renderer::InitOpenGlDebug()
{
#ifdef DEBUG
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback(
        []( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
            if( type == GL_DEBUG_TYPE_ERROR )
            {
                ASSERT( false, "GL Error: " << message );
            }
        },
        nullptr );
#endif
}

void Renderer::Render() const
{
    if( m_dirty )
    {
        m_dirty = false;
        glClear( GL_COLOR_BUFFER_BIT );

        glUseProgram( m_shader );

        glBindVertexArray( 0 );
        glBindVertexArray( m_vertexArrayObject );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject );

        glDrawElements( GL_TRIANGLES, sizeof( m_indices ) / sizeof( m_indices[ 0 ] ), GL_UNSIGNED_INT, nullptr );
    }
}

void Renderer::InitVertexArrayObject()
{
    glGenVertexArrays( 1, &m_vertexArrayObject );
    glBindVertexArray( m_vertexArrayObject );
}

void Renderer::InitVertexBufferObject()
{
    uint vertexBufferObject;
    glGenBuffers( 1, &vertexBufferObject );
    glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject );
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

void Renderer::LoadShaderSources()
{
    m_vertexShader = LoadFile( "res/shaders/default.vert" );
    m_fragmentShader = LoadFile( "res/shaders/implicit-2d-v2.frag" );
}

void Renderer::InitShaders()
{
    m_shader = CreateShaders( m_vertexShader, m_fragmentShader );
    CHECK( m_shader != 0 );
    glUseProgram( m_shader );
}

uint Renderer::CompileShader( uint type, const std::string& source )
{
    uint id = glCreateShader( type );
    const char* source_ = source.c_str();
    glShaderSource( id, 1, &source_, nullptr );
    glCompileShader( id );

    int result;
    glGetShaderiv( id, GL_COMPILE_STATUS, &result );
    if( result == GL_FALSE )
    {
#ifdef DEBUG
        int length;
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length );
        auto message = static_cast< char* >( alloca( length ) );
        glGetShaderInfoLog( id, length, &length, message );
        ASSERT( false, ( type == GL_VERTEX_SHADER ? "Vertex" : "Fragment" ) << " shader error: " << message );
#endif
        glDeleteShader( id );
        return 0;
    }

    return id;
}

uint Renderer::CreateShaders( const std::string& vertexShaderSource, const std::string& fragmentShaderSource )
{
    uint program = glCreateProgram();

    uint vertexShader = CompileShader( GL_VERTEX_SHADER, vertexShaderSource );
    uint fragmentShader = CompileShader( GL_FRAGMENT_SHADER, fragmentShaderSource );

    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glLinkProgram( program );
    glValidateProgram( program );

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

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