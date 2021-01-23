#ifndef WYRWYK_TOKENIZER_HPP
#define WYRWYK_TOKENIZER_HPP

#include "parser/symbol-def.hpp"
#include <vector>

class SymbolDefs;

struct Token
{
    const char* begin = nullptr;
    const char* end = nullptr;
    int args = -1;
    float value = 0.0f;
    SymbolType type = SymbolType::Undefined;
    const SymbolDef* def = nullptr;
};

using Tokens = std::vector< Token >;

class Tokenizer
{
public:
    explicit Tokenizer( SymbolDefs& symbolDefs );
    [[nodiscard]] Tokens CreateTokens() const;
    bool Tokenize( const char* expression, Tokens& tokens ) const;
    void PrintTokens( const Tokens& tokens ) const;

private:
    SymbolDefs& m_symbolDefs;
};

#endif // WYRWYK_TOKENIZER_HPP
