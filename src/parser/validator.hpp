#ifndef WYRWYK_VALIDATOR_HPP
#define WYRWYK_VALIDATOR_HPP

#include "symbol-def.hpp"
#include <map>
#include <vector>

class SymbolDefs;
struct Token;
using Tokens = std::vector< Token >;

class Validator
{
public:
    explicit Validator( const std::string& file );
    [[nodiscard]] bool Validate( const Tokens& tokens ) const;

private:
    void LoadPredecessorTable( const std::string& file );
    [[nodiscard]] bool IsValidPredecessor( SymbolType previous, SymbolType current ) const;

    bool m_validPredecessor[ SYMBOL_TYPE_COUNT * SYMBOL_TYPE_COUNT ];
};

#endif // WYRWYK_VALIDATOR_HPP
