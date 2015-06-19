
#ifndef IRISSPECTRUMAPI_H
#define IRISSPECTRUMAPI_H

#ifdef IRISSPECTRUM_BUILT_AS_STATIC
#  define IRISSPECTRUMAPI
#  define IRISSPECTRUM_NO_EXPORT
#else
#  ifndef IRISSPECTRUMAPI
#    ifdef irisspectrum_EXPORTS
        /* We are building this library */
#      define IRISSPECTRUMAPI __declspec(dllexport)
#    else
        /* We are using this library */
#      define IRISSPECTRUMAPI __declspec(dllimport)
#    endif
#  endif

#  ifndef IRISSPECTRUM_NO_EXPORT
#    define IRISSPECTRUM_NO_EXPORT 
#  endif
#endif

#ifndef IRISSPECTRUM_DEPRECATED
#  define IRISSPECTRUM_DEPRECATED __declspec(deprecated)
#endif

#ifndef IRISSPECTRUM_DEPRECATED_EXPORT
#  define IRISSPECTRUM_DEPRECATED_EXPORT IRISSPECTRUMAPI IRISSPECTRUM_DEPRECATED
#endif

#ifndef IRISSPECTRUM_DEPRECATED_NO_EXPORT
#  define IRISSPECTRUM_DEPRECATED_NO_EXPORT IRISSPECTRUM_NO_EXPORT IRISSPECTRUM_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define IRISSPECTRUM_NO_DEPRECATED
#endif

#endif
