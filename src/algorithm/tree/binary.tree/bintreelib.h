#ifndef _BINTREELIB_H_
#define _BINTREELIB_H_	1

/******************************************************************************
 * define
 ******************************************************************************/
#define POOL_SIZE		1024
#define STACK_SIZE		1024
#define MAX_DOTF_NR		1024
#define DOT_LABEL_STR_LEN	1024
#define CMD_MAX			(PATH_MAX + 128)
#define LOWEST_DOT_ID		0
#define INFO_COLOR		WHITE
#define GRAPHVIZ_CMD		"dot"
#define GRAPHVIZ_FILE_EXT	"dot"
#define GRAPHVIZ_FIG_TYPE	"svg"

/* color configuration */
#define	ERR_COLOR		RED
#define	ERR_EMPH_COLOR		RED_B

/******************************************************************************
 * prototype
 ******************************************************************************/
void   write_dot(const char * prefix, const char * figpath, int id, const char *fmt, ...);
void   push(void * p);
void * pop(void);

#endif	/* bintreelib.h */
