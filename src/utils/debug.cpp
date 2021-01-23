#include "utils/debug.hpp"

#ifdef DEBUG

bool Debug::s_expensive = true;
const std::chrono::high_resolution_clock::time_point Debug::s_start = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::duration Debug::s_pauseDuration;

void Debug::Breakpoint()
{
    std::chrono::high_resolution_clock::time_point pauseStart = std::chrono::high_resolution_clock::now();
#    if defined _WIN32
    __debugbreak();
#    elif defined __linux__
    __builtin_trap();
#    else
    ::abort();
#    endif
    s_pauseDuration += std::chrono::high_resolution_clock::now() - pauseStart;
}

void Debug::Message( const std::wstring& type, const std::string& file, uint32_t line, const std::wstring& message, bool abort )
{
    const auto now = std::chrono::high_resolution_clock::now();
    const auto diff = std::chrono::duration_cast< std::chrono::microseconds >( now - Debug::s_start ) - s_pauseDuration;

    std::wostringstream out;
    out << L"\"" << type << L"\";" << std::setprecision( 4 ) << std::fixed << diff.count() / 1'000'000.0 << L";\"" << file.c_str() << L":" << line << L"\";\"" << message
        << L"\"";
    std::wcerr << out.str() << std::endl;
    if( abort )
    {
        Breakpoint();
    }
}

#endif