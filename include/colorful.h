/* Example:
 *         #include "colorful.h"
 *         printf("%sblah blah blah%s", RED, NOCOLOR);
 */
#ifndef	_COLORFUL_H
#define	_COLORFUL_H	1

/* switch */
#define COLORFUL

/* color configuration */
#define	ERR_COLOR		RED
#define	ERR_EMPH_COLOR		RED_B

/* colors */
#ifdef COLORFUL
#define BLACK     "\033[0;30m"
#define RED       "\033[0;31m"
#define GREEN     "\033[0;32m"
#define YELLOW    "\033[0;33m"
#define BLUE      "\033[0;34m"
#define MAGENTA   "\033[0;35m"
#define CYAN      "\033[0;36m"
#define WHITE     "\033[0;37m"
/* bold */
#define BLACK_B   "\033[0;1;30m"
#define RED_B     "\033[0;1;31m"
#define GREEN_B   "\033[0;1;32m"
#define YELLOW_B  "\033[0;1;33m"
#define BLUE_B    "\033[0;1;34m"
#define MAGENTA_B "\033[0;1;35m"
#define CYAN_B    "\033[0;1;36m"
#define WHITE_B   "\033[0;1;37m"
/* reset */
#define NOCOLOR   "\033[0m"
#else
#define BLACK     ""
#define RED       ""
#define GREEN     ""
#define YELLOW    ""
#define BLUE      ""
#define MAGENTA   ""
#define CYAN      ""
#define WHITE     ""
#define BLACK_B   ""
#define RED_B     ""
#define GREEN_B   ""
#define YELLOW_B  ""
#define BLUE_B    ""
#define MAGENTA_B ""
#define CYAN_B    ""
#define WHITE_B   ""
#define NOCOLOR   ""
#endif

#endif	/* colorful.h */
