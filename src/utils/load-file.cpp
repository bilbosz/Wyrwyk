#include "utils/load-file.hpp"
#include "utils/debug.hpp"
#include <fstream>

std::string LoadFile( const std::string& path )
{
    std::ifstream stream( path );
    CHECK( stream.is_open() );
    return std::string( std::istreambuf_iterator< char >( stream ), std::istreambuf_iterator< char >() );
}