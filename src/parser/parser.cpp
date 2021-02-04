#include "parser/parser.hpp"
#include "parser/tokenizer.hpp"
#include "parser/translator.hpp"
#include "parser/validator.hpp"
#include "utils/debug.hpp"

Parser::Parser()
    : m_symbolDefs( std::make_unique< SymbolDefs >( "res/parser/symbol-defs.tsv" ) )
    , m_tokenizer( std::make_unique< Tokenizer >( *m_symbolDefs.get() ) ) // NOLINT(readability-redundant-smartptr-get)
    , m_validator( std::make_unique< Validator >( "res/parser/valid-predecessors.tsv" ) )
    , m_translator( std::make_unique< Translator >( *m_symbolDefs ) ) // NOLINT(readability-redundant-smartptr-get)
{
}

bool Parser::Parse( const char* expression, float* output )
{
    auto tokens = m_tokenizer->CreateTokens();
    if( !m_tokenizer->Tokenize( expression, tokens ) )
    {
        return false;
    }
    if( !m_validator->Validate( tokens ) )
    {
        return false;
    }
    auto postfix = m_translator->CreatePostfixNotation();
    if( !m_translator->Translate( tokens, postfix ) )
    {
        return false;
    }
    if( !m_translator->AssignOutputData( postfix, output ) )
    {
        return false;
    }
    return true;
}

Parser::~Parser()
{
}

bool Parser::IsSymbolUsed( float* expression, const std::string& name ) const
{
    auto symbol = m_symbolDefs->Find( name );
    auto it = expression;
    while( static_cast< bool >( *it ) )
    {
        auto type = *( it );
        auto value = *( it + 1 );
        if( static_cast< float >( symbol->type ) == type && symbol->id == value )
        {
            return true;
        }
        it += 2;
    }
    return false;
}
