#include "parser/validator.hpp"
#include "parser/symbol-defs.hpp"
#include "utils/debug.hpp"
#include <fstream>
#include <sstream>

Validator::Validator( const std::string& file ) : m_validPredecessor()
{
    LoadPredecessorTable( file );
}

bool Validator::Validate( const Tokens& tokens ) const
{
    auto begin = tokens.cbegin();
    auto it = tokens.cbegin();
    auto end = tokens.cend();

    if( it == end )
    {
        // Expression is empty
        return false;
    }

    auto prevType = SymbolType::Undefined;
    while( it != end + 1 )
    {
        auto type = it != end ? it->type : SymbolType::Undefined;

        if( !IsValidPredecessor( prevType, type ) )
        {
            return false;
        }

        if( type == SymbolType::RightParenthesis && prevType == SymbolType::LeftParenthesis )
        {
            auto prev2 = it - 2;
            if( prev2 < begin || prev2->type != SymbolType::Function )
            {
                // Token before left parenthesis is not a function
                return false;
            }
        }

        prevType = it->type;
        ++it;
    }

    return true;
}

void Validator::LoadPredecessorTable( const std::string& file )
{
    std::ifstream ifs( file );
    CHECK( ifs.is_open() );

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
        for( int i = 0; i < SYMBOL_TYPE_COUNT; ++i )
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
    return m_validPredecessor[ static_cast< ptrdiff_t >( current ) * SYMBOL_TYPE_COUNT + static_cast< ptrdiff_t >( previous ) ];
}
