#include "utils/resources.hpp"
#include "wyrwyk.hpp"

int main( int argc, char* argv[] )
{
    if( !InitResources( argc > 0 ? argv[ 0 ] : nullptr ) )
    {
        return 1;
    }
    Wyrwyk wyrwyk( argc, argv );
    wyrwyk.Run();
    return wyrwyk.ReturnCode();
}
