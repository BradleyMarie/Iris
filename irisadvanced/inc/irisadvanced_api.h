
#ifndef IRISADVANCEDAPI_H
#define IRISADVANCEDAPI_H

#ifdef IRISADVANCED_BUILT_AS_STATIC
#  define IRISADVANCEDAPI
#  define IRISADVANCED_NO_EXPORT
#else
#  ifndef IRISADVANCEDAPI
#    ifdef irisadvanced_EXPORTS
        /* We are building this library */
#      define IRISADVANCEDAPI __declspec(dllexport)
#    else
        /* We are using this library */
#      define IRISADVANCEDAPI __declspec(dllimport)
#    endif
#  endif

#  ifndef IRISADVANCED_NO_EXPORT
#    define IRISADVANCED_NO_EXPORT 
#  endif
#endif

#ifndef IRISADVANCED_DEPRECATED
#  define IRISADVANCED_DEPRECATED __declspec(deprecated)
#  define IRISADVANCED_DEPRECATED_EXPORT IRISADVANCEDAPI __declspec(deprecated)
#  define IRISADVANCED_DEPRECATED_NO_EXPORT IRISADVANCED_NO_EXPORT __declspec(deprecated)
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define IRISADVANCED_NO_DEPRECATED
#endif

#endif
