#include "parser/tokenizer.hpp"
#include "parser/symbol-defs.hpp"
#include "utils/debug.hpp"
#include <cstring>
#include <map>

Tokenizer::Tokenizer( SymbolDefs& symbolDefs ) : m_symbolDefs( symbolDefs )
{
}

Tokens Tokenizer::CreateTokens() const
{
    return Tokens();
}

bool Tokenizer::Tokenize( const char* expression, Tokens& tokens ) const
{
    auto begin = expression;
    auto it = begin;
    auto end = begin + strlen( expression );

    while( it != end )
    {
        Token token;
        if( std::isspace( *it ) )
        {
            ++it;
            continue;
        }
        else if( std::isalpha( *it ) || *it == '_' )
        {
            token.begin = it;
            ++it;
            while( it != end && ( std::isalnum( *it ) || *it == '_' ) )
            {
                ++it;
            }
            token.end = it;
            auto found = m_symbolDefs.UpdateTokenDefByName( token );
            if( !found )
            {
                // Did not find identifier
                return false;
            }
            token.type = token.def->type;
        }
        else if( std::isdigit( *it ) || *it == '.' )
        {
            token.begin = it;
            // Safe because buffer is originally char[], not from c_str()
            char* numberEnd = const_cast< char* >( end );
            auto number = std::strtof( it, &numberEnd );
            if( it == numberEnd )
            {
                // Error in number interpretation
                return false;
            }
            it = numberEnd;
            token.end = it;
            token.value = number;
            token.type = SymbolType::Const;
            CHECK( m_symbolDefs.UpdateTokenDefByType( token ) );
        }
        else if( *it == '(' )
        {
            token.begin = it;
            ++it;
            token.end = it;
            token.type = SymbolType::LeftParenthesis;
            CHECK( m_symbolDefs.UpdateTokenDefByType( token ) );
        }
        else if( *it == ')' )
        {
            token.begin = it;
            ++it;
            token.end = it;
            token.type = SymbolType::RightParenthesis;
            CHECK( m_symbolDefs.UpdateTokenDefByType( token ) );
        }
        else if( *it == ',' )
        {
            token.begin = it;
            ++it;
            token.end = it;
            token.type = SymbolType::Separator;
            CHECK( m_symbolDefs.UpdateTokenDefByType( token ) );
        }
        else
        {
            token.args = 1;
            if( !tokens.empty() )
            {
                const auto& prevType = tokens.back().type;
                if( prevType == SymbolType::Function )
                {
                    // Function name in the wrong place
                    return false;
                }
                if( prevType == SymbolType::Const || prevType == SymbolType::Parameter || prevType == SymbolType::Variable || prevType == SymbolType::RightParenthesis )
                {
                    token.args = 2;
                }
            }

            token.begin = it;
            token.end = end;
            token.type = SymbolType::Operator;
            auto found = m_symbolDefs.UpdateTokenDefOperation( token );
            if( !found )
            {
                // Did not find matching operator
                return false;
            }
            it = token.begin + token.def->name.length();
            token.end = it;
        }
        CHECK( token.type != SymbolType::Undefined );
        tokens.emplace_back( token );
    }

    return true;
}

void Tokenizer::PrintTokens( const Tokens& tokens ) const
{
    std::map< SymbolType, std::string > typeNames = { { SymbolType::Const, "Const" },
                                                      { SymbolType::Parameter, "Parameter" },
                                                      { SymbolType::Variable, "Variable" },
                                                      { SymbolType::Operator, "Operator" },
                                                      { SymbolType::Function, "Function" },
                                                      { SymbolType::LeftParenthesis, "LeftParenthesis" },
                                                      { SymbolType::RightParenthesis, "RightParenthesis" },
                                                      { SymbolType::Separator, "Separator" } };
    MESSAGE( "TOKENS---------------------------" );
    for( const auto& token : tokens )
    {
        MESSAGE( "{" << std::string( token.begin, token.end ).c_str() << "} : " << typeNames[ token.type ].c_str() << " " << token.args );
    }
}
