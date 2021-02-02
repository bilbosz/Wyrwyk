#ifndef WYRWYK_SYMBOL_DEFS_HPP
#define WYRWYK_SYMBOL_DEFS_HPP

#include "parser/symbol-def.hpp"
#include "tokenizer.hpp"
#include <string>
#include <vector>

struct SymbolDef;

class SymbolDefs
{
public:
    explicit SymbolDefs( const std::string& file );
    const SymbolDef* Find( const std::string& name );

    bool UpdateTokenDefByName( Token& token ) const;
    bool UpdateTokenDefByType( Token& token ) const;
    bool UpdateTokenDefOperation( Token& token ) const;

private:
    std::vector< SymbolDef > m_defs;

    void Load( const std::string& file );
    void SkipFirstLine( std::ifstream& ifs );
};

#endif // WYRWYK_SYMBOL_DEFS_HPP
