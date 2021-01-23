#ifndef WYRWYK_TRANSLATOR_HPP
#define WYRWYK_TRANSLATOR_HPP

#include <functional>
#include <vector>

class SymbolDefs;
struct Token;
using Tokens = std::vector< Token >;

class Translator
{
public:
    explicit Translator( SymbolDefs& symbolDefs );

    [[nodiscard]] Tokens CreatePostfixNotation() const;
    bool Translate( const Tokens& tokens, Tokens& outNotation ) const;
    void PrintNotation( const Tokens& outNotation ) const;
    bool AssignOutputData( const Tokens& outNotation, float* outData );
    void PrintOutput( const float* output ) const;

private:
    SymbolDefs& m_symbolDefs;
    void UnwindStack( std::vector< Token >& output, std::vector< Token >& stack, const std::function< bool( const Token& ) >& stopCondition ) const;
};

#endif // WYRWYK_TRANSLATOR_HPP
