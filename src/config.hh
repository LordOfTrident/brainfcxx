#ifndef __CONFIG_HH_HEADER_GUARD__
#define __CONFIG_HH_HEADER_GUARD__

//#define BF_DONT_USE_BITSHIFT // Use a union instead of
                               // bit shifting in BF
#define UTILS_USE_GNU_READLINE // If the platform is Linux,
                               // use GNU readline/readline.h

/*
 *  if there are problems with readline/readline.h (library
 *  not found etc) or you just dont like it, you cant
 *  comment out the #define UTILS_USE_GNU_READLINE and use
 *  UTILS_USE_GNU_READLINE=false in make parameters which
 *  will make it not link readline and not use
 *  readline/readline.h
 */

#endif // __CONFIG_HH_HEADER_GUARD__
