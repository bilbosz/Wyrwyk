#include "parser/symbol-defs.hpp"
#include "utils/debug.hpp"
#include <algorithm>
#include <fstream>

SymbolDefs::SymbolDefs( const std::string& file )
{
    Load( file );
}

const SymbolDef* SymbolDefs::Find( const std::string& name )
{
    auto it = std::find_if( m_defs.cbegin(), m_defs.cend(), [ &name ]( const auto& el ) { return el.name == name; } );
    return &*it;
}

void SymbolDefs::Load( const std::string& file )
{
    std::ifstream ifs( file );
    CHECK( ifs.is_open() );

    SkipFirstLine( ifs );
    while( ifs )
    {
        std::string line;
        std::getline( ifs, line );
        if( !line.empty() )
        {
            m_defs.emplace_back( line );
        }
    }
}

void SymbolDefs::SkipFirstLine( std::ifstream& ifs )
{
    std::string line;
    CHECK( ifs );
    std::getline( ifs, line );
}

bool SymbolDefs::UpdateTokenDefByName( Token& token ) const
{
    for( const auto& def : m_defs )
    {
        if( std::equal( def.name.cbegin(), def.name.cend(), token.begin, token.end ) )
        {
            token.def = &def;
            return true;
        }
    }
    return false;
}

bool SymbolDefs::UpdateTokenDefByType( Token& token ) const
{
    CHECK( token.type != SymbolType::Undefined );
    for( const auto& def : m_defs )
    {
        if( def.type == token.type )
        {
            token.def = &def;
            return true;
        }
    }
    return false;
}

bool SymbolDefs::UpdateTokenDefOperation( Token& token ) const
{
    for( const auto& def : m_defs )
    {
        if( def.type != token.type || def.args != token.args )
        {
            continue;
        }
        auto len = token.end - token.begin;
        if( def.name.length() > len )
        {
            continue;
        }
        if( std::equal( token.begin, token.begin + def.name.length(), def.name.cbegin(), def.name.cend() ) )
        {
            token.def = &def;
            return true;
        }
    }
    return false;
}