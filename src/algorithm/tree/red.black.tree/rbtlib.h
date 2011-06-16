#ifndef _RBTLIB_H_
#define _RBTLIB_H_	1

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
#define GRAPHVIZ_CMD		"neato"
#define GRAPHVIZ_FILE_EXT	"dot"
#define GRAPHVIZ_FIG_TYPE	"svg"

/* color configuration */
#define	ERR_COLOR		RED
#define	ERR_EMPH_COLOR		RED_B

/* coordinate calculation */
#define LEFT_X	(x - (hdelta) / 2.0)
#define LEFT_Y	(y - vdelta)
#define RIGHT_X	(x + (hdelta) / 2.0)
#define RIGHT_Y	(y - vdelta)

/******************************************************************************
 * enum & struct
 ******************************************************************************/
enum RB {R, B};			/* R: Red; B: Black */

typedef struct _rbnode {
	int              val;
	enum RB          rb;
	struct _rbnode * L;
	struct _rbnode * R;
	struct _rbnode * P;
} NODE;

/******************************************************************************
 * prototype
 ******************************************************************************/
int    atoi_or_abort(const char * str);
void   write_dot(const char * prefix, const char * figpath, int id,
		 const char *fmt, ...);
void   push(void * p);
void * pop(void);
NODE * alloc_node(int v, enum RB rb);
void   free_node(NODE * p);
NODE * rotate_left(NODE * root, NODE * x);
NODE * rotate_right(NODE * root, NODE * y);
void   preorder_traverse_recursive(const char * prefix, const char * path,
				   int id, NODE * T, NODE * hl_node,
				   double hdelta, double vdelta,
				   double x, double y);
void   verify_rbtree(NODE * T, int depth);
void   print_rbtree(const char * path, NODE * head, NODE * hl_node,
		    char * label_fmt, ...);

/* globals */
extern NODE * nil;
extern int node_cnt;
extern void (*______________________________)(const char * path, NODE * head, NODE * hl_node, char * label_fmt, ...);
extern void (*______________________________I)(const char * path, NODE * head, NODE * hl_node, char * label_fmt, ...);
extern void (*______________________________D)(const char * path, NODE * head, NODE * hl_node, char * label_fmt, ...);

#endif	/* rbtlib.h */
