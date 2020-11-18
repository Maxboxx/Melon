#pragma once
#define BOXX

///N Core

#ifdef __linux__ 
///H Operating System
///T Linux
/// Defined if the operating system is Linux
#define BOXX_LINUX
#elif _WIN32
///T Windows
/// Defined if the operating system is Windows
#define BOXX_WINDOWS
#else
///T Unknown System
/// Defined if the operating system is not Windos or Linux
#define BOXX_UNKNOWN_SYSTEM
#endif

#define _BOXX_STR(s) #s
#define BOXX_STR(s) _BOXX_STR(s)

#define _BOXX_ARG4(A1, A2, A3, A4, ...) A4
#define _BOXX_EXPAND(x) x

///H Version

///T Version number 1
/// The first number of the current version
#define BOXX_VERSION_V1 3

///T Version number 2
/// The middle number of the current version
#define BOXX_VERSION_V2 2

///T Version number 3
/// The last number of the current version
#define BOXX_VERSION_V3 3

///T Version string
/// A string containing the current version
///S #define BOXX_VERSION
#define BOXX_VERSION BOXX_STR(BOXX_VERSION_V1.BOXX_VERSION_V2.BOXX_VERSION_V3)

///H Version comparison

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

///T Comparisons
/// Compares the current version with the specified numbers
///A ...: The number to compare with
/// If one number argument is supplied, it compares the number to the first version number of the current version
/// If two number arguments are supplied, it compares the numbers with the first two numbers of the current version
/// If three number arguments are supplied, it compares the numbers with all numbers of the current version
///S #define BOXX_VERSION_EQ(...)
///S #define BOXX_VERSION_NE(...)
///S #define BOXX_VERSION_LT(...)
///S #define BOXX_VERSION_LE(...)
///S #define BOXX_VERSION_GT(...)
///S #define BOXX_VERSION_GE(...)
#define BOXX_VERSION_EQ(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_EQ3, _BOXX_VERSION_EQ2, _BOXX_VERSION_EQ1, ))(__VA_ARGS__))
#define BOXX_VERSION_NE(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_NE3, _BOXX_VERSION_NE2, _BOXX_VERSION_NE1, ))(__VA_ARGS__))
#define BOXX_VERSION_LT(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_LT3, _BOXX_VERSION_LT2, _BOXX_VERSION_LT1, ))(__VA_ARGS__))
#define BOXX_VERSION_LE(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_LE3, _BOXX_VERSION_LE2, _BOXX_VERSION_LE1, ))(__VA_ARGS__))
#define BOXX_VERSION_GT(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_GT3, _BOXX_VERSION_GT2, _BOXX_VERSION_GT1, ))(__VA_ARGS__))
#define BOXX_VERSION_GE(...) _BOXX_EXPAND(_BOXX_EXPAND(_BOXX_ARG4(__VA_ARGS__, _BOXX_VERSION_GE3, _BOXX_VERSION_GE2, _BOXX_VERSION_GE1, ))(__VA_ARGS__))

///T Enum flags
/// Enables an enum class to be used as bit flags
///A T: The enum class to use as bit flags
/// Put this after the enum class declaration 
/// If the enum class is declared in a class or struct put this after the class or struct declaration
/// In this case the name of the enum class has to be prefixed with the class or struct name as a namespace
///S #define BOXX_ENUM_FLAGS(T)
#define BOXX_ENUM_FLAGS(T) \
inline T operator~(T v)        {return(T)(~static_cast<std::underlying_type_t<T>>(v));} \
inline T operator|(T v1, T v2) {return(T)(static_cast<std::underlying_type_t<T>>(v1) | static_cast<std::underlying_type_t<T>>(v2));} \
inline T operator&(T v1, T v2) {return(T)(static_cast<std::underlying_type_t<T>>(v1) & static_cast<std::underlying_type_t<T>>(v2));} \
inline T operator^(T v1, T v2) {return(T)(static_cast<std::underlying_type_t<T>>(v1) ^ static_cast<std::underlying_type_t<T>>(v2));} \
inline void operator|=(T& v1, T v2) {v1 = (T)(static_cast<std::underlying_type_t<T>>(v1) | static_cast<std::underlying_type_t<T>>(v2));} \
inline void operator&=(T& v1, T v2) {v1 = (T)(static_cast<std::underlying_type_t<T>>(v1) & static_cast<std::underlying_type_t<T>>(v2));} \
inline void operator^=(T& v1, T v2) {v1 = (T)(static_cast<std::underlying_type_t<T>>(v1) ^ static_cast<std::underlying_type_t<T>>(v2));}

///T Enum flag friend
/// If an enum class is used as bit flags and is declared private or protected in a class or struct, put this after the enum class declaration in the class or struct
///A T: The name of the enum class to use as bit flags
///S #define BOXX_ENUM_FLAG_FRIEND(T)
#define BOXX_ENUM_FLAG_FRIEND(T) \
friend T operator~(T v); \
friend T operator|(T v1, T v2); \
friend T operator&(T v1, T v2); \
friend T operator^(T v1, T v2); \
friend void operator|=(T& v1, T v2); \
friend void operator&=(T& v1, T v2); \
friend void operator^=(T& v1, T v2);
