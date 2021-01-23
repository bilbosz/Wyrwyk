#pragma once
#ifdef DEBUG

#    include <chrono>
#    include <iomanip>
#    include <iostream>
#    include <sstream>
#    include <string>

class Debug
{
public:
    static void Breakpoint();
    static void Message( const std::wstring& type, const std::string& file, uint32_t line, const std::wstring& message, bool abort );
    static void SetExpensive( bool value )
    {
        s_expensive = value;
    }
    static bool IsExpensive()
    {
        return s_expensive;
    }

private:
    static bool s_expensive;
    static const std::chrono::high_resolution_clock::time_point s_start;
    static std::chrono::high_resolution_clock::duration s_pauseDuration;
};

#endif

// clang-format off
#ifdef DEBUG
#	define ASSERT(test,message) { if( !( test ) ) { std::wostringstream out; out << message; Debug::Message( L"A", __FILE__, __LINE__, out.str(), true ); } }
#else
#	define ASSERT(test,message)
#endif
// clang-format on

// clang-format off
#ifdef DEBUG
#	define CHECK(test) { if( !( test ) ) Debug::Breakpoint(); }
#else
#	define CHECK(test)
#endif
// clang-format on

// clang-format off
#ifdef DEBUG
#	define MESSAGE(message) { std::wostringstream out; out << message; Debug::Message( L"M", __FILE__, __LINE__, out.str(), false ); }
#else
#	define MESSAGE(message)
#endif
// clang-format on

// clang-format off
#ifdef DEBUG
#	define VERIFY(test) { if( !( test ) ) Debug::Breakpoint(); }
#else
#	define VERIFY(test) { ( test ); }
#endif
// clang-format on

// clang-format off
#ifdef DEBUG
#	define WARNING(message) { std::wostringstream out; out << message; Debug::Message( L"W", __FILE__, __LINE__, out.str(), false ); }
#else
#	define WARNING(message)
#endif
// clang-format on