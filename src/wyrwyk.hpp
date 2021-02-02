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
class Renderer;

class Wyrwyk final
{
public:
    Wyrwyk( int argc, char* argv[] );
    ~Wyrwyk();
    void Run();

    [[nodiscard]] int ReturnCode() const;

private:
    bool m_markedForExit = false;
    int m_returnCode = 0;
    GLFWwindow* m_window{};
    char m_expression[ WYRWYK_MAX_EXPR_LEN ]{};
    float m_params[ WYRWYK_PARAMS_COUNT ] = { 1.0f, 2.0f, 3.0f, 0.0f, 2.0f * std::asin( 1.0f ) };
    float m_boundingBox[ 4 ] = { -10.0f, -10.0f, 20.0f, 20.0f };
    int m_rmbState{};
    double m_startMove[ 2 ] = { 0.0, 0.0 };
    std::string m_baseTitle = "Wyrwyk - Implicit Curve Rasterizer";
    float m_falseColor[ 3 ] = { 0.98f, 0.98f, 1.0f };
    float m_trueColor[ 3 ] = { 0.0f, 0.0f, 0.0f };
    float m_symbols[ WYRWYK_MAX_EXPR_LEN * 2 ];
    std::unique_ptr< Parser > m_parser;
    std::chrono::high_resolution_clock::time_point m_timer;
    float m_multisampling = 1.0f;
    bool m_isMultisampling = false;
    std::unique_ptr< Renderer > m_renderer;

    void InitImGui();
    void UpdateImGui();
    void RenderImGui();
    void Exit( int returnCode );
    void Update();
    void Render();
    void InitGlfw();
    void InitGlew();
    void RegisterGlfwCallbacks();
    void Terminate();
};

#endif // WYRWYK_WYRWYK_HPP
