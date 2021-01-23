#include "wyrwyk.hpp"

int main( int argc, char* argv[] )
{
    Wyrwyk wyrwyk( argc, argv );
    wyrwyk.Run();
    return wyrwyk.ReturnCode();
}