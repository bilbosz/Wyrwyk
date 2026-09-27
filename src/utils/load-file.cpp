#include "utils/load-file.hpp"
#include "utils/debug.hpp"
#include <fstream>
#include <iterator>

std::string LoadFile( const std::string& path )
{
    std::ifstream stream( path, std::ios::binary );
    if( !stream.is_open() )
    {
        ASSERT( false, "Could not open file: " << path.c_str() );
        return {};
    }
    return std::string( std::istreambuf_iterator< char >( stream ), std::istreambuf_iterator< char >() );
}
