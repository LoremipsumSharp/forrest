#ifndef _SORTLIB_H_
#define _SORTLIB_H_	1

/******************************************************************************
 * define
 ******************************************************************************/
#define	INT_LEN			32
#define	MAX_NUM_NR		1024
#define	MAX_NUM_WIDTH		32 /* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */
#define MAX_DOTF_NR		1024
#define DOT_LABEL_STR_LEN	1024
#define CMD_MAX			(PATH_MAX + 128)
#define LOWEST_DOT_ID		0
#define INFO_COLOR		WHITE
#define GRAPHVIZ_CMD		"neato"
#define GRAPHVIZ_FILE_EXT	"dot"
#define GRAPHVIZ_FIG_TYPE	"svg"

/* color configuration */
#define	ERR_COLOR		RED
#define	ERR_EMPH_COLOR		RED_B

#ifdef COLORFUL
/* Macros can accept variable number of arguments, @see
 *         http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 * for more information.
 */
#define	F_ERR(fp, ...)		do					\
	{								\
		if(fp)							\
			fclose(fp);					\
		fprintf(stderr, "%s", ERR_COLOR);			\
		fprintf(stderr, ##__VA_ARGS__);				\
		fprintf(stderr, "\n%serrno: %d (%s)%s\n",		\
			ERR_COLOR, errno, strerror(errno), NOCOLOR);	\
		exit(EXIT_FAILURE);					\
	} while (0);
#else
#define	F_ERR(fp, ...)		do {			\
	if(fp)						\
		fclose(fp);				\
	err(EXIT_FAILURE, ##__VA_ARGS__);		\
	} while (0);
#endif

typedef void (*swap_func) (int*, int*);

/******************************************************************************
 * prototype
 ******************************************************************************/
void   xor_swap                (int * p, int * q);
int    rand_in_range_inclusive (int l, int r);
int    atoi_or_abort           (const char * str);
char * get_filename            (int argc, char * argv[]);
char * get_sorted_filename     (const char * unsorted_filename);
int    get_A                   (int * A, int lim, char * filename);
void   print_A                 (int * A, int lim, char * title_fmt, ...);
int    verify_A                (int * A, int lim, char * sorted_filename);
void   write_dot               (const char * prefix, int id, const char *fmt, ...);

#endif	/* sortlib.h */
