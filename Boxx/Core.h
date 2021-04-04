#pragma once
///[Settings] block: indent

///[Heading] Macros

///[Title] BOXX
/// Defined if any Boxx library file is included
#define BOXX

#ifdef __linux__ 
///[Title] BOXX_LINUX
/// Defined if the operating system is Linux.
#define BOXX_LINUX
#elif _WIN32
///[Title] BOXX_WINDOWS
/// Defined if the operating system is Windows.
#define BOXX_WINDOWS
#else
///[Title] BOXX_UNKNOWN_SYSTEM
/// Defined if the operating system is not Windows or Linux.
#define BOXX_UNKNOWN_SYSTEM
#endif

#define _BOXX_STR(s) #s
#define BOXX_STR(s) _BOXX_STR(s)

#define _BOXX_ARG4(A1, A2, A3, A4, ...) A4
#define _BOXX_EXPAND(x) x

///[Title] BOXX_VERSION_V1
/// The first number of the library version.
#define BOXX_VERSION_V1 3

///[Title] BOXX_VERSION_V2
/// The second number of the library version.
#define BOXX_VERSION_V2 4

///[Title] BOXX_VERSION_V3
/// The third number of the library version.
#define BOXX_VERSION_V3 2

///[Title] BOXX_VERSION
/// A string containing the current library version.
///[Code] #define BOXX_VERSION
#define BOXX_VERSION BOXX_STR(BOXX_VERSION_V1.BOXX_VERSION_V2.BOXX_VERSION_V3)

#define _BOXX_VERSION_EQ3(V1, V2, V3) (BOXX_VERSION_V1 == V1 && BOXX_VERSION_V2 == V2 && BOXX_VERSION_V3 == V3)
#define _BOXX_VERSION_NE3(V1, V2, V3) (BOXX_VERSION_V1 != V1 || BOXX_VERSION_V2 != V2 || BOXX_VERSION_V3 != V3)
#define _BOXX_VERSION_LT3(V1, V2, V3) (BOXX_VERSION_V1 < V1 || (BOXX_VERSION_V1 == V1 && BOXX_VERSION_V2 < V2) || (BOXX_VERSION_V1 == V1 && BOXX_VERSION_V2 == V2 && BOXX_VERSION_V3 < V3))
#define _BOXX_VERSION_LE3(V1, V2, V3) (_BOXX_VERSION_EQ3(V1, V2, V3) || _BOXX_VERSION_LT3(V1, V2, V3))
#define _BOXX_VERSION_GT3(V1, V2, V3) (BOXX_VERSION_V1 > V1 || (BOXX_VERSION_V1 == V1 && BOXX_VERSION_V2 > V2) || (BOXX_VERSION_V1 == V1 && BOXX_VERSION_V2 == V2 && BOXX_VERSION_V3 > V3))
#define _BOXX_VERSION_GE3(V1, V2, V3) (_BOXX_VERSION_EQ3(V1, V2, V3) || _BOXX_VERSION_GT3(V1, V2, V3))

#define _BOXX_VERSION_EQ2(V1, V2) (BOXX_VERSION_V1 == V1 && BOXX_VERSION_V2 == V2)
#define _BOXX_VERSION_NE2(V1, V2) (BOXX_VERSION_V1 != V1 || BOXX_VERSION_V2 != V2)
#define _BOXX_VERSION_LT2(V1, V2) (BOXX_VERSION_V1 < V1 || (BOXX_VERSION_V1 == V1 && BOXX_VERSION_V2 < V2))
#define _BOXX_VERSION_LE2(V1, V2) (_BOXX_VERSION_EQ2(V1, V2) || _BOXX_VERSION_LT2(V1, V2))
#define _BOXX_VERSION_GT2(V1, V2) (BOXX_VERSION_V1 > V1 || (BOXX_VERSION_V1 == V1 && BOXX_VERSION_V2 > V2))
#define _BOXX_VERSION_GE2(V1, V2) (_BOXX_VERSION_EQ2(V1, V2) || _BOXX_VERSION_GT2(V1, V2))

#define _BOXX_VERSION_EQ1(V1) (BOXX_VERSION_V1 == V1)
#define _BOXX_VERSION_NE1(V1) (BOXX_VERSION_V1 != V1)
#define _BOXX_VERSION_LT1(V1) (BOXX_VERSION_V1 < V1)
#define _BOXX_VERSION_LE1(V1) (_BOXX_VERSION_EQ1(V1) || _BOXX_VERSION_LT1(V1))
#define _BOXX_VERSION_GT1(V1) (BOXX_VERSION_V1 > V1)
#define _BOXX_VERSION_GE1(V1) (_BOXX_VERSION_EQ1(V1) || _BOXX_VERSION_GT1(V1))

///[Title] BOXX_VERSION_EQ(...)
/// Checks if the specified version numbers refer to the current version of the library.
///[para] The number of arguments to the macro determines how many version numbers are used for the comparison.
///[Code] #define BOXX_VERSION_EQ(...)
#define BOXX_VERSION_EQ(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_EQ3, _BOXX_VERSION_EQ2, _BOXX_VERSION_EQ1, ))(__VA_ARGS__))

///[Title] BOXX_VERSION_NE(...)
/// Checks if the specified version numbers do not refer to the current version of the library.
///[para] The number of arguments to the macro determines how many version numbers are used for the comparison.
///[Code] #define BOXX_VERSION_NE(...)
#define BOXX_VERSION_NE(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_NE3, _BOXX_VERSION_NE2, _BOXX_VERSION_NE1, ))(__VA_ARGS__))

///[Title] BOXX_VERSION_LT(...)
/// Checks if the specified version numbers refer to an earlier version of the library.
///[para] The number of arguments to the macro determines how many version numbers are used for the comparison.
///[Code] #define BOXX_VERSION_LT(...)
#define BOXX_VERSION_LT(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_LT3, _BOXX_VERSION_LT2, _BOXX_VERSION_LT1, ))(__VA_ARGS__))

///[Title] BOXX_VERSION_LE(...)
/// Checks if the specified version numbers refer to an earlier version or the current version of the library.
///[para] The number of arguments to the macro determines how many version numbers are used for the comparison.
///[Code] #define BOXX_VERSION_LE(...)
#define BOXX_VERSION_LE(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_LE3, _BOXX_VERSION_LE2, _BOXX_VERSION_LE1, ))(__VA_ARGS__))

///[Title] BOXX_VERSION_GT(...)
/// Checks if the specified version numbers refer to a later version of the library.
///[para] The number of arguments to the macro determines how many version numbers are used for the comparison.
///[Code] #define BOXX_VERSION_GT(...)
#define BOXX_VERSION_GT(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_GT3, _BOXX_VERSION_GT2, _BOXX_VERSION_GT1, ))(__VA_ARGS__))

///[Title] BOXX_VERSION_GE(...)
/// Checks if the specified version numbers refer to a later version or the current version of the library.
///[para] The number of arguments to the macro determines how many version numbers are used for the comparison.
///[Code] #define BOXX_VERSION_GE(...)
#define BOXX_VERSION_GE(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_GE3, _BOXX_VERSION_GE2, _BOXX_VERSION_GE1, ))(__VA_ARGS__))

///[Title] BOXX_ENUM_FLAGS(T)
/// Adds bitwise operations to an enum class to allow the enum class to be used as bit flags.
/// Use this macro after the enum class declaration.
///[para] If the enum class is declared in a class or struct, use this macro after the class or struct.
///[Arg] T: The enum class to add the operators to.
///[Code] #define BOXX_ENUM_FLAGS(T)
#define BOXX_ENUM_FLAGS(T) \
inline T operator~(T v)        {return(T)(~static_cast<std::underlying_type_t<T>>(v));} \
inline T operator|(T v1, T v2) {return(T)(static_cast<std::underlying_type_t<T>>(v1) | static_cast<std::underlying_type_t<T>>(v2));} \
inline T operator&(T v1, T v2) {return(T)(static_cast<std::underlying_type_t<T>>(v1) & static_cast<std::underlying_type_t<T>>(v2));} \
inline T operator^(T v1, T v2) {return(T)(static_cast<std::underlying_type_t<T>>(v1) ^ static_cast<std::underlying_type_t<T>>(v2));} \
inline void operator|=(T& v1, T v2) {v1 = (T)(static_cast<std::underlying_type_t<T>>(v1) | static_cast<std::underlying_type_t<T>>(v2));} \
inline void operator&=(T& v1, T v2) {v1 = (T)(static_cast<std::underlying_type_t<T>>(v1) & static_cast<std::underlying_type_t<T>>(v2));} \
inline void operator^=(T& v1, T v2) {v1 = (T)(static_cast<std::underlying_type_t<T>>(v1) ^ static_cast<std::underlying_type_t<T>>(v2));}

///[Title] BOXX_ENUM_FLAG_FRIEND(T)
/// Allows the {BOXX_ENUM_FLAGS} macro to be used on a private or protected enum class.
/// Use this macro directly after the enum class declaration.
///[Arg] T: The enum class.
///[Code] #define BOXX_ENUM_FLAG_FRIEND(T)
#define BOXX_ENUM_FLAG_FRIEND(T) \
friend T operator~(T v); \
friend T operator|(T v1, T v2); \
friend T operator&(T v1, T v2); \
friend T operator^(T v1, T v2); \
friend void operator|=(T& v1, T v2); \
friend void operator&=(T& v1, T v2); \
friend void operator^=(T& v1, T v2);
