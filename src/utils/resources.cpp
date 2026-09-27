#include "utils/resources.hpp"
#include "utils/debug.hpp"
#include <filesystem>
#include <system_error>
#include <vector>

#if defined _WIN32
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    include <windows.h>
#elif defined __APPLE__
#    include <mach-o/dyld.h>
#endif

namespace fs = std::filesystem;

namespace
{
    fs::path s_resourceDir;

    fs::path GetExecutablePath( const char* argv0 )
    {
        std::error_code ec;
#if defined _WIN32
        std::vector< wchar_t > buffer( MAX_PATH );
        while( true )
        {
            auto length = GetModuleFileNameW( nullptr, buffer.data(), static_cast< DWORD >( buffer.size() ) );
            if( length == 0 )
            {
                break;
            }
            if( length < buffer.size() )
            {
                return fs::path( std::wstring( buffer.data(), length ) );
            }
            buffer.resize( buffer.size() * 2 );
        }
#elif defined __APPLE__
        uint32_t size = 0;
        _NSGetExecutablePath( nullptr, &size );
        std::vector< char > buffer( size + 1 );
        if( _NSGetExecutablePath( buffer.data(), &size ) == 0 )
        {
            auto path = fs::canonical( buffer.data(), ec );
            if( !ec )
            {
                return path;
            }
        }
#else
        auto path = fs::read_symlink( "/proc/self/exe", ec );
        if( !ec )
        {
            return path;
        }
#endif
        if( argv0 )
        {
            auto path = fs::absolute( argv0, ec );
            if( !ec )
            {
                return path;
            }
        }
        return {};
    }
} // namespace

bool InitResources( const char* argv0 )
{
    std::vector< fs::path > candidates;
    auto executable = GetExecutablePath( argv0 );
    if( !executable.empty() )
    {
        candidates.push_back( executable.parent_path() / "res" );
    }
    std::error_code ec;
    auto cwd = fs::current_path( ec );
    if( !ec && ( candidates.empty() || candidates.front() != cwd / "res" ) )
    {
        candidates.push_back( cwd / "res" );
    }

    for( const auto& candidate : candidates )
    {
        if( fs::is_directory( candidate / "shaders", ec ) && fs::is_directory( candidate / "parser", ec ) )
        {
            s_resourceDir = candidate;
            return true;
        }
    }

#ifdef DEBUG
    for( const auto& candidate : candidates )
    {
        WARNING( "Could not find resources in: " << candidate.string().c_str() );
    }
#endif
    return false;
}

std::string ResourcePath( const std::string& relative )
{
    return ( s_resourceDir / fs::path( relative ) ).string();
}
