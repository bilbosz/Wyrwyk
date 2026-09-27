#include "utils/load-file.hpp"
#include <fstream>
#include <iostream>
#include <iterator>

std::string LoadFile( const std::string& path )
{
    std::ifstream stream( path, std::ios::binary );
    if( !stream.is_open() )
    {
        std::cerr << "Could not open file: " << path << std::endl;
        return {};
    }
    return std::string( std::istreambuf_iterator< char >( stream ), std::istreambuf_iterator< char >() );
}
