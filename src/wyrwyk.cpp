#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "wyrwyk.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "parser/parser.hpp"
#include "renderer/renderer.hpp"
#include "utils/debug.hpp"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glad/gl.h>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stb_image_write.h>

Wyrwyk::Wyrwyk( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] )
    : m_parser( std::make_unique< Parser >() ), m_timer( std::chrono::high_resolution_clock::now() ), m_renderer( std::make_unique< Renderer >() )
{
    m_parser->Parse( m_expression, m_symbols );
}

Wyrwyk::~Wyrwyk()
{
}

void Wyrwyk::Run()
{
    m_renderer->SetFramebufferSize( 800.0f, 800.0f );
    if( !InitGlfw() || !InitGlad() || !m_renderer->Init() )
    {
        Exit( 1 );
        Terminate();
        return;
    }
    m_renderer->SetUniform1fv( "u_SupersamplingSide", &m_multisampling, 1 );
    m_renderer->SetUniform2fv( "u_Resolution", m_renderer->GetFramebufferSize(), 1 );
    m_renderer->SetUniform4fv( "u_BoundingBox", m_boundingBox, 1 );
    m_renderer->SetUniform1fv( "u_Params", m_params, WYRWYK_PARAMS_COUNT );
    m_renderer->SetUniform3fv( "u_FalseColor", m_falseColor, 1 );
    m_renderer->SetUniform3fv( "u_TrueColor", m_trueColor, 1 );
    m_renderer->SetUniform4fv( "u_Symbols", m_symbols, WYRWYK_MAX_EXPR_LEN / 2 );

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
    // Layout is fixed, there is nothing worth saving to imgui.ini
    ImGui::GetIO().IniFilename = nullptr;
    ImGui_ImplGlfw_InitForOpenGL( m_window, true );
    ImGui_ImplOpenGL3_Init( "#version 330 core" );
    ImGui::StyleColorsClassic();
}

void Wyrwyk::UpdateImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Display size is in window coordinates which differ from framebuffer pixels on HiDPI screens
    const auto displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos( ImVec2{ 0.0f, displaySize.y }, 0, ImVec2{ 0.0f, 1.0f } );
    ImGui::SetNextWindowSize( ImVec2{ displaySize.x, 160.0f } );

    auto color = ImGui::GetStyleColorVec4( ImGuiCol_WindowBg );
    color.w = 1.0f;
    ImGui::PushStyleColor( ImGuiCol_WindowBg, color );

    ImGui::Begin( "Parameters", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize );
    {
        ImGui::Text( "Enter the expression with x and y to see the\n"
                     "corresponding implicit curve in the 2D graph" );
        // TODO add function for this
        {
            auto text = "Input";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( displaySize.x - textSize.x - 20.0f );
            static bool once = true;
            if( once )
            {
                once = false;
                ImGui::SetKeyboardFocusHere();
            }
            if( ImGui::InputText( text, m_expression, WYRWYK_MAX_EXPR_LEN ) )
            {
                if( m_parser->Parse( m_expression, m_symbols ) )
                {
                    m_renderer->SetUniform4fv( "u_Symbols", m_symbols, WYRWYK_MAX_EXPR_LEN / 2 );
                    glfwSetWindowTitle( m_window, ( m_baseTitle + " - " + std::string( m_expression ) ).c_str() );
                }
            }
        }
        {
            auto text = "a";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_renderer->GetFramebufferWidth() - textSize.x - 20.0f );
            if( ImGui::SliderFloat( text, m_params + 0, -10.0f, 10.0f ) && m_parser->IsSymbolUsed( m_symbols, "a" ) )
            {
                m_renderer->SetUniform1fv( "u_Params", m_params, WYRWYK_PARAMS_COUNT );
            }
        }
        {
            auto text = "b";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_renderer->GetFramebufferWidth() - textSize.x - 20.0f );
            if( ImGui::SliderFloat( text, m_params + 1, -10.0f, 10.0f ) && m_parser->IsSymbolUsed( m_symbols, "b" ) )
            {
                m_renderer->SetUniform1fv( "u_Params", m_params, WYRWYK_PARAMS_COUNT );
            }
        }
        {
            auto text = "c";
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_renderer->GetFramebufferWidth() - textSize.x - 20.0f );
            if( ImGui::SliderFloat( text, m_params + 2, -10.0f, 10.0f ) && m_parser->IsSymbolUsed( m_symbols, "c" ) )
            {
                m_renderer->SetUniform1fv( "u_Params", m_params, WYRWYK_PARAMS_COUNT );
            }
        }
        {
            auto text = "Multisampling";
            if( ImGui::Checkbox( text, &m_isMultisampling ) )
            {
                m_multisampling = m_isMultisampling ? 16.0f : 1.0f;
                m_renderer->SetUniform1fv( "u_SupersamplingSide", &m_multisampling, 1 );
            }
        }
        ImGui::SameLine();
        {
            auto text = "Screenshot";
            if( ImGui::Button( text ) )
            {
                m_isScreenshotRequested = true;
            }
        }
        ImGui::SameLine();
        {
            if( !m_isRecording )
            {
                auto text = "Start Recording";
                if( ImGui::Button( text ) )
                {
                    StartRecording();
                }
            }
            else
            {
                auto text = "Stop Recording";
                if( ImGui::Button( text ) )
                {
                    StopRecording();
                }
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

void Wyrwyk::Exit( int returnCode )
{
    m_returnCode = returnCode;
    m_markedForExit = true;
}

void Wyrwyk::Update()
{
    glfwPollEvents();
    UpdateImGui();
    static auto t = 0.0f;
    t += 0.1f;
    //    auto t = std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::high_resolution_clock::now() - m_timer ).count() / 1'000'000.0f;
    m_params[ 3 ] = t;
    if( m_parser->IsSymbolUsed( m_symbols, "t" ) )
    {
        m_renderer->SetUniform1fv( "u_Params", m_params, WYRWYK_PARAMS_COUNT );
    }
    if( glfwWindowShouldClose( m_window ) )
    {
        Exit( 0 );
    }
}

void Wyrwyk::Render()
{
    m_renderer->Render();
    RenderImGui();
    // Content of the back buffer is undefined after swapping, so the pixels are read before
    if( m_isScreenshotRequested )
    {
        m_isScreenshotRequested = false;
        MakeScreenshot();
    }
    if( m_isRecording )
    {
        MakeScreenshot( "recording" );
    }
    glfwSwapBuffers( m_window );
}

bool Wyrwyk::InitGlfw()
{
#ifdef DEBUG
    glfwSetErrorCallback( []( int error, const char* description ) { WARNING( "GLFW error " << error << ": " << description ); } );
#endif
    if( !glfwInit() )
    {
        ASSERT( false, "Could not initialize GLFW" );
        return false;
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    // Required on macOS to get core profile context
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );

    m_window = glfwCreateWindow(
        static_cast< int >( m_renderer->GetFramebufferWidth() ),
        static_cast< int >( m_renderer->GetFramebufferHeight() ),
        ( m_baseTitle + " - " + std::string( m_expression ) ).c_str(),
        nullptr,
        nullptr );
    if( !m_window )
    {
        ASSERT( false, "Could not create window with OpenGL 3.3 core profile context" );
        return false;
    }
    glfwMakeContextCurrent( m_window );
    glfwSwapInterval( 1 );

    glfwSetWindowUserPointer( m_window, static_cast< void* >( this ) );
    return true;
}

bool Wyrwyk::InitGlad()
{
    if( !gladLoadGL( glfwGetProcAddress ) )
    {
        ASSERT( false, "Could not load OpenGL functions" );
        return false;
    }
    return true;
}

void Wyrwyk::GetWindowSize( double& width, double& height ) const
{
    int w, h;
    glfwGetWindowSize( m_window, &w, &h );
    width = std::max( w, 1 );
    height = std::max( h, 1 );
}

void Wyrwyk::RegisterGlfwCallbacks()
{
    int width, height;
    glfwGetFramebufferSize( m_window, &width, &height );
    m_renderer->SetFramebufferSize( static_cast< float >( width ), static_cast< float >( height ) );
    m_renderer->SetUniform2fv( "u_Resolution", m_renderer->GetFramebufferSize(), 1 );
    m_boundingBox[ 2 ] = m_boundingBox[ 3 ] / m_renderer->GetFramebufferHeight() * m_renderer->GetFramebufferWidth();
    m_renderer->SetUniform4fv( "u_BoundingBox", m_boundingBox, 1 );
    glfwSetFramebufferSizeCallback( m_window, []( GLFWwindow* window, int w, int h ) {
        if( w <= 0 || h <= 0 )
        {
            // Minimized window
            return;
        }
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        wyrwyk->m_renderer->SetFramebufferSize( static_cast< float >( w ), static_cast< float >( h ) );
        wyrwyk->m_renderer->SetUniform2fv( "u_Resolution", wyrwyk->m_renderer->GetFramebufferSize(), 1 );
        wyrwyk->m_boundingBox[ 2 ] = wyrwyk->m_boundingBox[ 3 ] / wyrwyk->m_renderer->GetFramebufferHeight() * wyrwyk->m_renderer->GetFramebufferWidth();
        wyrwyk->m_renderer->SetUniform4fv( "u_BoundingBox", wyrwyk->m_boundingBox, 1 );
    } );

    glfwSetScrollCallback( m_window, []( GLFWwindow* window, double xoffset, double yoffset ) {
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        double cursorPos[ 2 ];
        double windowSize[ 2 ];
        glfwGetCursorPos( window, cursorPos, cursorPos + 1 );
        wyrwyk->GetWindowSize( windowSize[ 0 ], windowSize[ 1 ] );
        cursorPos[ 0 ] /= windowSize[ 0 ];
        cursorPos[ 1 ] /= windowSize[ 1 ];
        cursorPos[ 1 ] = 1.0 - cursorPos[ 1 ];
        float underCursor[ 2 ] = { static_cast< float >( cursorPos[ 0 ] ) * wyrwyk->m_boundingBox[ 2 ] + wyrwyk->m_boundingBox[ 0 ],
                                   static_cast< float >( cursorPos[ 1 ] ) * wyrwyk->m_boundingBox[ 3 ] + wyrwyk->m_boundingBox[ 1 ] };
        const auto zoom = std::pow( 1.1f, static_cast< float >( -yoffset ) );
        wyrwyk->m_boundingBox[ 2 ] *= zoom;
        wyrwyk->m_boundingBox[ 3 ] *= zoom;
        wyrwyk->m_boundingBox[ 0 ] = underCursor[ 0 ] - wyrwyk->m_boundingBox[ 2 ] * static_cast< float >( cursorPos[ 0 ] );
        wyrwyk->m_boundingBox[ 1 ] = underCursor[ 1 ] - wyrwyk->m_boundingBox[ 3 ] * static_cast< float >( cursorPos[ 1 ] );
        wyrwyk->m_renderer->SetUniform4fv( "u_BoundingBox", wyrwyk->m_boundingBox, 1 );
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
            double windowSize[ 2 ];
            wyrwyk->GetWindowSize( windowSize[ 0 ], windowSize[ 1 ] );
            wyrwyk->m_boundingBox[ 0 ] -= static_cast< float >( ( xpos - wyrwyk->m_startMove[ 0 ] ) / windowSize[ 0 ] ) * wyrwyk->m_boundingBox[ 2 ];
            wyrwyk->m_boundingBox[ 1 ] += static_cast< float >( ( ypos - wyrwyk->m_startMove[ 1 ] ) / windowSize[ 1 ] ) * wyrwyk->m_boundingBox[ 3 ];
            wyrwyk->m_renderer->SetUniform4fv( "u_BoundingBox", wyrwyk->m_boundingBox, 1 );

            wyrwyk->m_startMove[ 0 ] = xpos;
            wyrwyk->m_startMove[ 1 ] = ypos;
        }
    } );

    glfwSetKeyCallback( m_window, []( GLFWwindow* window, int key, int scancode, int action, int mods ) {
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        if( action == GLFW_PRESS && key == GLFW_KEY_ESCAPE )
        {
            wyrwyk->Exit( 0 );
        }
    } );
}

void Wyrwyk::Terminate()
{
    if( ImGui::GetCurrentContext() )
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    // OpenGL objects have to be released while the context still exists
    m_renderer.reset();
    if( m_window )
    {
        glfwDestroyWindow( m_window );
        m_window = nullptr;
    }
    glfwTerminate();
}

void Wyrwyk::RenderImGui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

void Wyrwyk::MakeScreenshot( const std::string& prefix ) const
{
    auto w = static_cast< int >( m_renderer->GetFramebufferWidth() );
    auto h = static_cast< int >( m_renderer->GetFramebufferHeight() );
    const auto comp = 4;
    std::vector< unsigned char > data( static_cast< size_t >( w ) * h * comp );
    {
        glPixelStorei( GL_PACK_ALIGNMENT, 1 );
        glReadPixels( 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data.data() );
        stbi_flip_vertically_on_write( true );

        std::string file;
        {
            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            std::ostringstream out;
            out << prefix << std::setfill( '0' ) << std::setw( std::numeric_limits< decltype( now ) >::digits10 ) << now << ".png";
            file = out.str();
        }

        if( !stbi_write_png( file.c_str(), w, h, comp, data.data(), w * comp ) )
        {
            WARNING( "Could not save screenshot: " << file.c_str() );
        }
    }
}

void Wyrwyk::StartRecording()
{
    m_isRecording = true;
}

void Wyrwyk::StopRecording()
{
    m_isRecording = false;
}
