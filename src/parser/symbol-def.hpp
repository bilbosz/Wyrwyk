#ifndef WYRWYK_SYMBOL_DEF_HPP
#define WYRWYK_SYMBOL_DEF_HPP

#include <string>

enum class SymbolType
{
    Undefined = 0,
    Const = 1,
    Parameter = 2,
    Variable = 3,
    Operator = 4,
    Function = 5,
    LeftParenthesis = 6,
    RightParenthesis = 7,
    Separator = 8,
    Count
};
const size_t SYMBOL_TYPE_COUNT = static_cast< size_t >( SymbolType::Count );

struct SymbolDef
{
    explicit SymbolDef( const std::string& line );

    SymbolType type = SymbolType::Undefined;
    float id = 0.0f;
    std::string name;
    int priority = 0;
    int args = 0;

private:
    void Load( const std::string& line );
};

#endif // WYRWYK_SYMBOL_DEF_HPP
