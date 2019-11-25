#pragma once

// adapted from https://stackoverflow.com/a/3767883

#   define RELEASE_ASSERT_FATAL(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)

#ifndef NDEBUG
#   define ASSERT_FATAL(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT_FATAL(condition, message) do { condition; } while (false)
#endif

#ifndef NDEBUG
#   define ASSERT_BREAK_FATAL(message) \
    do { \
        std::cerr << "Break assert in " << __FILE__ \
                    << " line " << __LINE__ << ": " << message << std::endl; \
        std::terminate(); \
    } while (false)
#else
#   define ASSERT_BREAK_FATAL(message) do { } while (false)
#endif

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            __debugbreak(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { condition; } while (false)
#endif

#ifndef NDEBUG
#   define ASSERT_BREAK(message) \
    do { \
        std::cerr << "Break assert in " << __FILE__ \
                    << " line " << __LINE__ << ": " << message << std::endl; \
		__debugbreak(); \
    } while (false)
#else
#   define ASSERT_BREAK(message) do { } while (false)
#endif
