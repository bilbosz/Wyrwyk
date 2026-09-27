#ifndef WYRWYK_RESOURCES_HPP
#define WYRWYK_RESOURCES_HPP

#include <string>

// Locates the "res" directory. It is searched next to the executable first and then in the current working directory,
// so the application works regardless of the directory it was started from.
bool InitResources( const char* argv0 );

// Returns full path to the resource, relative path is relative to the "res" directory.
std::string ResourcePath( const std::string& relative );

#endif // WYRWYK_RESOURCES_HPP
