#include "parser/parser.hpp"
#include "parser/tokenizer.hpp"
#include "parser/translator.hpp"
#include "parser/validator.hpp"

Parser::Parser()
    : m_symbolDefs( std::make_unique< SymbolDefs >( "res/parser/symbol-defs.tsv" ) )
    , m_tokenizer( std::make_unique< Tokenizer >( *m_symbolDefs.get() ) )
    , m_validator( std::make_unique< Validator >( "res/parser/valid-predecessors.tsv", *m_symbolDefs.get() ) )
    , m_translator( std::make_unique< Translator >( *m_symbolDefs.get() ) )
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