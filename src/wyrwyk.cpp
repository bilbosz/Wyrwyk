#include "wyrwyk.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "parser/parser.hpp"
#include "utils/debug.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <fstream>

Wyrwyk::Wyrwyk( int argc, char* argv[] ) : m_parser( std::make_unique< Parser >() ), m_timer( std::chrono::high_resolution_clock::now() )
{
    strcpy( m_expressionBuffer, "x == y + -x" );
    m_parser->Parse( m_expressionBuffer, m_symbols );
    m_expression = m_expressionBuffer;
}

Wyrwyk::~Wyrwyk()
{
}

void Wyrwyk::Run()
{
    InitGlfw();
    InitGlew();
    InitOpenGlDebug();

    InitVertexArrayObject();
    InitVertexBufferObject();
    InitIndexBufferObject();

    LoadShaderSources();
    SubstituteShaderSources();
    InitShaders();

    RegisterGlfwCallbacks();
    InitImGui();

    while( !m_markedForExit )
    {
        Update();
        Render();
    }

    Terminate();
}

int Wyrwyk::ReturnCode() const
{
    ASSERT( m_markedForExit, "Application did not finish. Return code not set yet" );
    return m_returnCode;
}

void Wyrwyk::InitImGui()
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL( m_window, true );
    ImGui_ImplOpenGL3_Init( "#version 130" );
    ImGui::StyleColorsLight();
}

void Wyrwyk::DrawImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos( ImVec2{ 0.0f, m_framebuffer[ 1 ] }, 0, ImVec2{ 0.0f, 1.0f } );
    ImGui::SetNextWindowSize( ImVec2{ m_framebuffer[ 0 ], 160.0f } );

    ImGui::Begin( "Parameters", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize );
    {
        ImGui::Text( "Enter the expression with x and y to see the\n"
                     "corresponding implicit curve in the 2D graph" );
        // TODO add function for this
        {
            auto text = "Input";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_framebuffer[ 0 ] - textSize.x - 20.0f );
            if( ImGui::InputText( text, m_expressionBuffer, WYRWYK_MAX_EXPR_LEN ) )
            {
                if( m_parser->Parse( m_expressionBuffer, m_symbols ) )
                {
                    m_expression = m_expressionBuffer;
                    glfwSetWindowTitle( m_window, ( m_baseTitle + " - " + m_expression ).c_str() );
                }
            }
        }
        {
            auto text = "a";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_framebuffer[ 0 ] - textSize.x - 20.0f );
            ImGui::SliderFloat( text, &m_a, -10.0f, 10.0f );
        }
        {
            auto text = "b";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_framebuffer[ 0 ] - textSize.x - 20.0f );
            ImGui::SliderFloat( text, &m_b, -10.0f, 10.0f );
        }
        {
            auto text = "c";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_framebuffer[ 0 ] - textSize.x - 20.0f );
            ImGui::SliderFloat( text, &m_c, -10.0f, 10.0f );
        }
        {
            auto text = "Multisampling";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_framebuffer[ 0 ] - textSize.x - 20.0f );
            if( ImGui::Checkbox( text, &m_isMultisampling ) )
            {
                m_multisampling = m_isMultisampling ? 2 : 1;
            }
        }
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

std::string Wyrwyk::LoadFromFile( const std::string& path )
{
    std::ifstream stream( path );
    CHECK( stream.is_open() );
    return std::string( std::istreambuf_iterator< char >( stream ), std::istreambuf_iterator< char >() );
}

uint Wyrwyk::TryCompileShader( uint type, const std::string& source )
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
        char* message = static_cast< char* >( alloca( length ) );
        glGetShaderInfoLog( id, length, &length, message );
        ASSERT( false, ( type == GL_VERTEX_SHADER ? "Vertex" : "Fragment" ) << " shader error: " << message );
#endif
        glDeleteShader( id );
        return 0;
    }

    return id;
}

uint Wyrwyk::TryCreateShaders( const std::string& vertexShaderSource, const std::string& fragmentShaderSource )
{
    uint program = glCreateProgram();

    uint vertexShader = TryCompileShader( GL_VERTEX_SHADER, vertexShaderSource );
    uint fragmentShader = TryCompileShader( GL_FRAGMENT_SHADER, fragmentShaderSource );

    if( fragmentShader == 0 )
    {
        glDeleteShader( vertexShader );
        return 0;
    }

    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glLinkProgram( program );
    glValidateProgram( program );

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    return program;
}

std::string Wyrwyk::SubstituteExpression( std::string text )
{
    const char SUBSTITUTE[] = "EXPRESSION";
    auto finding = text.find( SUBSTITUTE );
    if( finding != std::string::npos )
    {
        text.replace( finding, sizeof( SUBSTITUTE ) - 1, m_expressionBuffer );
    }
    return text;
}

void Wyrwyk::Exit( int returnCode )
{
    m_returnCode = returnCode;
    m_markedForExit = true;
}

void Wyrwyk::Update()
{
    glfwPollEvents();
    m_t = std::chrono::duration_cast< std::chrono::microseconds >( m_timer - std::chrono::high_resolution_clock::now() ).count() / 1'000'000.0f;
    if( glfwWindowShouldClose( m_window ) )
    {
        Exit( 0 );
    }
}

void Wyrwyk::Render()
{
    glClear( GL_COLOR_BUFFER_BIT );

    glUseProgram( m_shader );
    int location = glGetUniformLocation( m_shader, "u_SupersamplingSide" );
    glUniform1f( location, m_multisampling );
    location = glGetUniformLocation( m_shader, "u_Resolution" );
    glUniform2f( location, m_framebuffer[ 0 ], m_framebuffer[ 1 ] );
    location = glGetUniformLocation( m_shader, "u_BoundingBox" );
    glUniform4f( location, m_boundingBox[ 0 ], m_boundingBox[ 1 ], m_boundingBox[ 2 ], m_boundingBox[ 3 ] );
    location = glGetUniformLocation( m_shader, "u_Params" );
    glUniform1fv( location, 5, &m_a );
    location = glGetUniformLocation( m_shader, "u_Symbols" );
    glUniform2fv( location, 256, m_symbols );
    location = glGetUniformLocation( m_shader, "u_TrueColor" );
    glUniform3f( location, m_trueColor[ 0 ], m_trueColor[ 1 ], m_trueColor[ 2 ] );
    location = glGetUniformLocation( m_shader, "u_FalseColor" );
    glUniform3f( location, m_falseColor[ 0 ], m_falseColor[ 1 ], m_falseColor[ 2 ] );
    glBindVertexArray( 0 );
    glBindVertexArray( m_vertexArrayObject );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject );

    glDrawElements( GL_TRIANGLES, sizeof( m_indices ) / sizeof( m_indices[ 0 ] ), GL_UNSIGNED_INT, nullptr );

    DrawImGui();

    glfwSwapBuffers( m_window );
}

void Wyrwyk::InitGlfw()
{
    VERIFY( glfwInit() );

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    m_window = glfwCreateWindow( m_framebuffer[ 0 ], m_framebuffer[ 1 ], ( m_baseTitle + " - " + m_expression ).c_str(), nullptr, nullptr );
    VERIFY( m_window );
    glfwMakeContextCurrent( m_window );

    glfwSetWindowUserPointer( m_window, static_cast< void* >( this ) );
}

void Wyrwyk::InitGlew()
{
    auto glewInitResult = glewInit();
    VERIFY( glewInitResult == GLEW_OK );
}

void Wyrwyk::InitOpenGlDebug()
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

void Wyrwyk::InitVertexArrayObject()
{
    glGenVertexArrays( 1, &m_vertexArrayObject );
    glBindVertexArray( m_vertexArrayObject );
}

void Wyrwyk::InitVertexBufferObject()
{
    uint vertexBufferObject;
    glGenBuffers( 1, &vertexBufferObject );
    glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject );
    glBufferData( GL_ARRAY_BUFFER, sizeof( m_vertices ), m_vertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), nullptr );
}

void Wyrwyk::InitIndexBufferObject()
{
    glGenBuffers( 1, &m_indexBufferObject );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( m_indices ), m_indices, GL_STATIC_DRAW );
}

void Wyrwyk::LoadShaderSources()
{
    m_vertexShaderIn = LoadFromFile( "res/shaders/default.vert" );
    m_fragmentShaderIn = LoadFromFile( "res/shaders/implicit-2d-v2.frag" );
}

void Wyrwyk::SubstituteShaderSources()
{
    m_vertexShaderOut = m_vertexShaderIn;
    m_fragmentShaderOut = SubstituteExpression( m_fragmentShaderIn );
}

void Wyrwyk::InitShaders()
{
    m_shader = TryCreateShaders( m_vertexShaderOut, m_fragmentShaderOut );
    CHECK( m_shader != 0 );
    glUseProgram( m_shader );
}

void Wyrwyk::RegisterGlfwCallbacks()
{
    int width, height;
    glfwGetFramebufferSize( m_window, &width, &height );
    m_framebuffer[ 0 ] = static_cast< float >( width );
    m_framebuffer[ 1 ] = static_cast< float >( height );
    m_boundingBox[ 2 ] = m_boundingBox[ 3 ] / m_framebuffer[ 1 ] * m_framebuffer[ 0 ];
    glfwSetFramebufferSizeCallback( m_window, []( GLFWwindow* window, int w, int h ) {
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        wyrwyk->m_framebuffer[ 0 ] = static_cast< float >( w );
        wyrwyk->m_framebuffer[ 1 ] = static_cast< float >( h );
        wyrwyk->m_boundingBox[ 2 ] = wyrwyk->m_boundingBox[ 3 ] / wyrwyk->m_framebuffer[ 1 ] * wyrwyk->m_framebuffer[ 0 ];
        glViewport( 0, 0, w, h );
    } );

    glfwSetScrollCallback( m_window, []( GLFWwindow* window, double xoffset, double yoffset ) {
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        float mid[ 2 ] = { wyrwyk->m_boundingBox[ 0 ] + wyrwyk->m_boundingBox[ 2 ] * 0.5f, wyrwyk->m_boundingBox[ 1 ] + wyrwyk->m_boundingBox[ 3 ] * 0.5f };
        wyrwyk->m_boundingBox[ 2 ] *= std::pow( 1.1f, -yoffset );
        wyrwyk->m_boundingBox[ 3 ] *= std::pow( 1.1f, -yoffset );
        wyrwyk->m_boundingBox[ 0 ] = mid[ 0 ] - wyrwyk->m_boundingBox[ 2 ] * 0.5f;
        wyrwyk->m_boundingBox[ 1 ] = mid[ 1 ] - wyrwyk->m_boundingBox[ 3 ] * 0.5f;
    } );

    glfwSetScrollCallback( m_window, []( GLFWwindow* window, double xoffset, double yoffset ) {
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        float mid[ 2 ] = { wyrwyk->m_boundingBox[ 0 ] + wyrwyk->m_boundingBox[ 2 ] * 0.5f, wyrwyk->m_boundingBox[ 1 ] + wyrwyk->m_boundingBox[ 3 ] * 0.5f };
        wyrwyk->m_boundingBox[ 2 ] *= std::pow( 1.1f, -yoffset );
        wyrwyk->m_boundingBox[ 3 ] *= std::pow( 1.1f, -yoffset );
        wyrwyk->m_boundingBox[ 0 ] = mid[ 0 ] - wyrwyk->m_boundingBox[ 2 ] * 0.5f;
        wyrwyk->m_boundingBox[ 1 ] = mid[ 1 ] - wyrwyk->m_boundingBox[ 3 ] * 0.5f;
    } );

    m_rmbState = glfwGetMouseButton( m_window, GLFW_MOUSE_BUTTON_RIGHT );
    glfwSetMouseButtonCallback( m_window, []( GLFWwindow* window, int button, int action, int mods ) {
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        if( button == GLFW_MOUSE_BUTTON_RIGHT && wyrwyk->m_rmbState != action )
        {
            if( action == GLFW_PRESS )
            {
                glfwGetCursorPos( window, wyrwyk->m_startMove, wyrwyk->m_startMove + 1 );
            }
            wyrwyk->m_rmbState = action;
        }
    } );

    glfwSetCursorPosCallback( m_window, []( GLFWwindow* window, double xpos, double ypos ) {
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        if( wyrwyk->m_rmbState == GLFW_PRESS )
        {
            wyrwyk->m_boundingBox[ 0 ] -= static_cast< float >( xpos - wyrwyk->m_startMove[ 0 ] ) / wyrwyk->m_framebuffer[ 0 ] * wyrwyk->m_boundingBox[ 2 ];
            wyrwyk->m_boundingBox[ 1 ] += static_cast< float >( ypos - wyrwyk->m_startMove[ 1 ] ) / wyrwyk->m_framebuffer[ 1 ] * wyrwyk->m_boundingBox[ 3 ];

            wyrwyk->m_startMove[ 0 ] = xpos;
            wyrwyk->m_startMove[ 1 ] = ypos;
        }
    } );
}

void Wyrwyk::Terminate()
{
    glDeleteProgram( m_shader );
    glfwTerminate();
}