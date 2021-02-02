#ifndef WYRWYK_PARSER_HPP
#define WYRWYK_PARSER_HPP

#include "parser/symbol-defs.hpp"
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct SymbolDef;
class Tokenizer;
class Translator;
class Validator;

class Parser
{
public:
    Parser();
    ~Parser();
    bool Parse( const char* expression, float* output );
    bool IsSymbolUsed( float* expression, const std::string& name ) const;

private:
    std::unique_ptr< SymbolDefs > m_symbolDefs;
    std::unique_ptr< Tokenizer > m_tokenizer;
    std::unique_ptr< Validator > m_validator;
    std::unique_ptr< Translator > m_translator;
};

#endif // WYRWYK_PARSER_HPP
