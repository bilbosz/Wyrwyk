#include "symbol-def.hpp"
#include <sstream>

SymbolDef::SymbolDef( const std::string& line )
{
    Load( line );
}

void SymbolDef::Load( const std::string& line )
{
    std::stringstream oss;
    oss << line;
    int type_;
    oss >> type_ >> id >> name >> priority >> args;
    type = static_cast< SymbolType >( type_ );

    if( type == SymbolType::Const )
    {
        name = "";
    }
}
