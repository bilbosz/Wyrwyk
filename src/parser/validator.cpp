#include "parser/validator.hpp"
#include "parser/symbol-defs.hpp"
#include "utils/debug.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

Validator::Validator( const std::string& file ) : m_validPredecessor()
{
    LoadPredecessorTable( file );
}

bool Validator::Validate( const Tokens& tokens ) const
{
    if( tokens.empty() )
    {
        // Expression is empty
        return false;
    }

    // One extra iteration past the last token checks what can end the expression
    auto prevType = SymbolType::Undefined;
    for( size_t i = 0; i <= tokens.size(); ++i )
    {
        auto type = i < tokens.size() ? tokens[ i ].type : SymbolType::Undefined;

        if( !IsValidPredecessor( prevType, type ) )
        {
            return false;
        }

        if( type == SymbolType::RightParenthesis && prevType == SymbolType::LeftParenthesis )
        {
            if( i < 2 || tokens[ i - 2 ].type != SymbolType::Function )
            {
                // Token before left parenthesis is not a function
                return false;
            }
        }

        prevType = type;
    }

    return true;
}

void Validator::LoadPredecessorTable( const std::string& file )
{
    std::ifstream ifs( file );
    if( !ifs.is_open() )
    {
        std::cerr << "Could not open file: " << file << std::endl;
        return;
    }

    std::string skip;
    std::getline( ifs, skip );
    CHECK( ifs );

    int j = 0;
    while( ifs )
    {
        std::string line;
        std::getline( ifs, line );
        if( line.empty() )
        {
            break;
        }
        std::istringstream oss( line );
        CHECK( oss );
        oss >> skip;
        for( size_t i = 0; i < SYMBOL_TYPE_COUNT; ++i )
        {
            CHECK( oss );
            oss >> m_validPredecessor[ j * SYMBOL_TYPE_COUNT + i ];
        }
        ++j;
    }
    CHECK( j == SYMBOL_TYPE_COUNT );
}

bool Validator::IsValidPredecessor( SymbolType previous, SymbolType current ) const
{
    return m_validPredecessor[ static_cast< size_t >( current ) * SYMBOL_TYPE_COUNT + static_cast< size_t >( previous ) ];
}
