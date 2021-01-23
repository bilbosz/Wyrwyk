#include "parser/translator.hpp"
#include "config.hpp"
#include "parser/symbol-defs.hpp"
#include "utils/debug.hpp"
#include <map>

Translator::Translator( SymbolDefs& symbolDefs ) : m_symbolDefs( symbolDefs )
{
}

bool Translator::Translate( const Tokens& tokens, Tokens& outNotation ) const
{
    Tokens stack;

    auto begin = tokens.cbegin();
    auto it = tokens.cbegin();
    auto end = tokens.cend();

    while( it != end )
    {
        if( it->type == SymbolType::Const || it->type == SymbolType::Parameter || it->type == SymbolType::Variable )
        {
            outNotation.emplace_back( *it );
        }
        else if( it->type == SymbolType::Function )
        {
            stack.emplace_back( *it );
            stack.back().args = 0;
        }
        else if( it->type == SymbolType::Separator )
        {
            UnwindStack( outNotation, stack, nullptr );
            if( stack.empty() )
            {
                // Function separator does not have corresponding left parenthesis
                return false;
            }
            const auto& top = stack.back();
            CHECK( top.type == SymbolType::LeftParenthesis );
            if( stack.size() >= 2 )
            {
                auto& func = stack[ stack.size() - 2 ];
                if( func.type == SymbolType::Function )
                {
                    ++func.args;
                }
                else
                {
                    // Did not find function
                    return false;
                }
            }
            else
            {
                // Left parenthesis only found - not a function
                return false;
            }
        }
        else if( it->type == SymbolType::Operator )
        {
            if( it->args == 1 )
            {
                stack.emplace_back( *it );
            }
            else
            {
                UnwindStack( outNotation, stack, [ it ]( const Token& top ) -> bool {
                    if( top.type == SymbolType::Operator )
                    {
                        return it->def->priority < top.def->priority;
                    }
                    else
                    {
                        return false;
                    }
                } );
                stack.emplace_back( *it );
            }
        }
        else if( it->type == SymbolType::LeftParenthesis )
        {
            stack.emplace_back( *it );
        }
        else if( it->type == SymbolType::RightParenthesis )
        {
            bool noArgs = false;
            if( it - 1 >= begin )
            {
                const auto& prev = it - 1;
                if( prev->type == SymbolType::LeftParenthesis )
                {
                    noArgs = true;
                }
            }
            UnwindStack( outNotation, stack, nullptr );
            if( stack.empty() )
            {
                // Right parenthesis does not have corresponding left one
                return false;
            }
            auto& top = stack.back();
            CHECK( top.type == SymbolType::LeftParenthesis );
            stack.pop_back();
            if( !stack.empty() )
            {
                top = stack.back();
                if( top.type == SymbolType::Function )
                {
                    if( noArgs )
                    {
                        top.args = 0;
                    }
                    else
                    {
                        ++top.args;
                    }
                    outNotation.emplace_back( top );
                    if( !m_symbolDefs.UpdateTokenDefOperation( outNotation.back() ) )
                    {
                        // No overload of a function with the given number of arguments was found
                        return false;
                    }
                    stack.pop_back();
                }
            }
        }
        ++it;
    }
    UnwindStack( outNotation, stack, nullptr );
    if( !stack.empty() )
    {
        // Left and right parentheses are not matching
        return false;
    }
    return true;
}

Tokens Translator::CreatePostfixNotation() const
{
    return Tokens();
}

void Translator::UnwindStack( std::vector< Token >& output, std::vector< Token >& stack, const std::function< bool( const Token& ) >& stopCondition ) const
{
    while( !stack.empty() )
    {
        const auto& top = stack.back();
        if( top.type == SymbolType::LeftParenthesis || ( stopCondition && stopCondition( top ) ) )
        {
            break;
        }
        else if( top.type == SymbolType::Operator )
        {
            output.emplace_back( top );
        }
        stack.pop_back();
    }
}

void Translator::PrintNotation( const Tokens& outNotation ) const
{
    std::map< SymbolType, std::string > typeNames = { { SymbolType::Const, "Const" },
                                                      { SymbolType::Parameter, "Parameter" },
                                                      { SymbolType::Variable, "Variable" },
                                                      { SymbolType::Operator, "Operator" },
                                                      { SymbolType::Function, "Function" },
                                                      { SymbolType::LeftParenthesis, "LeftParenthesis" },
                                                      { SymbolType::RightParenthesis, "RightParenthesis" },
                                                      { SymbolType::Separator, "Separator" } };
    MESSAGE( "TRANSLATION---------------------------" );
    for( const auto& token : outNotation )
    {
        MESSAGE( "{" << std::string( token.begin, token.end ).c_str() << "} : " << typeNames[ token.type ].c_str() << " " << token.args );
    }
}

bool Translator::AssignOutputData( const Tokens& outNotation, float* outData )
{
    if( outNotation.size() + 1 > WYRWYK_MAX_EXPR_LEN )
    {
        return false;
    }

    int i = 0;
    for( const auto& symbol : outNotation )
    {
        float& type = outData[ 2 * i + 0 ];
        float& value = outData[ 2 * i + 1 ];
        CHECK(
            symbol.type == SymbolType::Const || symbol.type == SymbolType::Parameter || symbol.type == SymbolType::Variable || symbol.type == SymbolType::Operator ||
            symbol.type == SymbolType::Function )
        type = static_cast< float >( symbol.type );
        if( symbol.type == SymbolType::Const )
        {
            value = symbol.value;
        }
        else
        {
            value = static_cast< float >( symbol.def->id );
        }
        ++i;
    }
    outData[ 2 * i + 0 ] = 0.0f;
    outData[ 2 * i + 1 ] = 0.0f;

    return true;
}

void Translator::PrintOutput( const float* output ) const
{
    MESSAGE( "OUTPUT---------------------------" );
    while( *output )
    {
        const float& type = *( output + 0 );
        const float& value = *( output + 1 );
        MESSAGE( type << " " << value );
        output += 2;
    }
}