#include "wyrwyk.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "parser/parser.hpp"
#include "renderer/renderer.hpp"
#include "utils/debug.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <fstream>

Wyrwyk::Wyrwyk( int argc, char* argv[] )
    : m_parser( std::make_unique< Parser >() ), m_timer( std::chrono::high_resolution_clock::now() ), m_renderer( std::make_unique< Renderer >() )
{
    m_parser->Parse( m_expression, m_symbols );
}

Wyrwyk::~Wyrwyk()
{
}

void Wyrwyk::Run()
{
    InitGlfw();
    InitGlew();
    m_renderer->Init();
    m_renderer->SetFramebufferSize( 500.0f, 500.0f );
    m_renderer->SetUniform1fv( "u_SupersamplingSide", &m_multisampling, 1 );
    m_renderer->SetUniform2fv( "u_Resolution", m_renderer->GetFramebufferSize(), 1 );
    m_renderer->SetUniform4fv( "u_BoundingBox", m_boundingBox, 1 );
    m_renderer->SetUniform1fv( "u_Params", m_params, WYRWYK_PARAMS_COUNT );
    m_renderer->SetUniform3fv( "u_FalseColor", m_falseColor, 1 );
    m_renderer->SetUniform3fv( "u_TrueColor", m_trueColor, 1 );
    m_renderer->SetUniform2fv( "u_Symbols", m_symbols, WYRWYK_MAX_EXPR_LEN );

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
    ImGui::StyleColorsClassic();
}

void Wyrwyk::UpdateImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos( ImVec2{ 0.0f, m_renderer->GetFramebufferHeight() }, 0, ImVec2{ 0.0f, 1.0f } );
    ImGui::SetNextWindowSize( ImVec2{ m_renderer->GetFramebufferWidth(), 160.0f } );

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
            ImGui::SetNextItemWidth( m_renderer->GetFramebufferWidth() - textSize.x - 20.0f );
            if( ImGui::InputText( text, m_expression, WYRWYK_MAX_EXPR_LEN ) )
            {
                if( m_parser->Parse( m_expression, m_symbols ) )
                {
                    m_renderer->SetUniform2fv( "u_Symbols", m_symbols, WYRWYK_MAX_EXPR_LEN );
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
            auto textSize = ImGui::CalcTextSize( text );
            ImGui::SetNextItemWidth( m_renderer->GetFramebufferWidth() - textSize.x - 20.0f );
            if( ImGui::Checkbox( text, &m_isMultisampling ) )
            {
                m_multisampling = m_isMultisampling ? 2.0f : 1.0f;
                m_renderer->SetUniform1fv( "u_SupersamplingSide", &m_multisampling, 1 );
            }
        }
    }
    ImGui::PopStyleColor();
    static bool once = true;
    if( once )
    {
        once = false;
        ImGui::SetKeyboardFocusHere();
    }
    ImGui::End();
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
    m_params[ 3 ] = std::chrono::duration_cast< std::chrono::microseconds >( m_timer - std::chrono::high_resolution_clock::now() ).count() / 1'000'000.0f;
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
    glfwSwapBuffers( m_window );
}

void Wyrwyk::InitGlfw()
{
    VERIFY( glfwInit() );

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    m_window = glfwCreateWindow(
        m_renderer->GetFramebufferWidth(), m_renderer->GetFramebufferHeight(), ( m_baseTitle + " - " + std::string( m_expression ) ).c_str(), nullptr, nullptr );
    CHECK( m_window );
    glfwMakeContextCurrent( m_window );

    glfwSetWindowUserPointer( m_window, static_cast< void* >( this ) );
}

void Wyrwyk::InitGlew()
{
    auto glewInitResult = glewInit();
    CHECK( glewInitResult == GLEW_OK );
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
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        wyrwyk->m_renderer->SetFramebufferSize( static_cast< float >( w ), static_cast< float >( h ) );
        wyrwyk->m_renderer->SetUniform2fv( "u_Resolution", wyrwyk->m_renderer->GetFramebufferSize(), 1 );
        wyrwyk->m_boundingBox[ 2 ] = wyrwyk->m_boundingBox[ 3 ] / wyrwyk->m_renderer->GetFramebufferHeight() * wyrwyk->m_renderer->GetFramebufferWidth();
        wyrwyk->m_renderer->SetUniform4fv( "u_BoundingBox", wyrwyk->m_boundingBox, 1 );
        glViewport( 0, 0, w, h );
    } );

    glfwSetScrollCallback( m_window, []( GLFWwindow* window, double xoffset, double yoffset ) {
        auto wyrwyk = static_cast< Wyrwyk* >( glfwGetWindowUserPointer( window ) );
        double cursorPos[ 2 ];
        glfwGetCursorPos( window, cursorPos, cursorPos + 1 );
        cursorPos[ 0 ] /= wyrwyk->m_renderer->GetFramebufferWidth();
        cursorPos[ 1 ] /= wyrwyk->m_renderer->GetFramebufferHeight();
        cursorPos[ 1 ] = 1.0 - cursorPos[ 1 ];
        float underCursor[ 2 ] = { static_cast< float >( cursorPos[ 0 ] ) * wyrwyk->m_boundingBox[ 2 ] + wyrwyk->m_boundingBox[ 0 ],
                                   static_cast< float >( cursorPos[ 1 ] ) * wyrwyk->m_boundingBox[ 3 ] + wyrwyk->m_boundingBox[ 1 ] };
        wyrwyk->m_boundingBox[ 2 ] *= std::pow( 1.1f, -yoffset );
        wyrwyk->m_boundingBox[ 3 ] *= std::pow( 1.1f, -yoffset );
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
            wyrwyk->m_boundingBox[ 0 ] -=
                static_cast< float >( xpos - wyrwyk->m_startMove[ 0 ] ) / wyrwyk->m_renderer->GetFramebufferWidth() * wyrwyk->m_boundingBox[ 2 ];
            wyrwyk->m_boundingBox[ 1 ] +=
                static_cast< float >( ypos - wyrwyk->m_startMove[ 1 ] ) / wyrwyk->m_renderer->GetFramebufferHeight() * wyrwyk->m_boundingBox[ 3 ];
            wyrwyk->m_renderer->SetUniform4fv( "u_BoundingBox", wyrwyk->m_boundingBox, 1 );

            wyrwyk->m_startMove[ 0 ] = xpos;
            wyrwyk->m_startMove[ 1 ] = ypos;
        }
    } );
}

void Wyrwyk::Terminate()
{
    glfwTerminate();
}

void Wyrwyk::RenderImGui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}
