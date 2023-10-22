#ifndef _RKS_DATA_STRUCT_H__
#define _RKS_DATA_STRUCT_H__

typedef struct node_t
{
    void *data;
    struct node_t *next;
} node_t;


typedef struct p_gpu {
    char *root;
    char *hwmon;
} p_gpu;

typedef enum argparse_option_type {
	ARG_PARSE_ERR = -1,
	ARG_PARSE_INT = 0,
	ARG_PARSE_STRING,
	ARG_PARSE_FLOAT,
	ARG_PARSE_INT_ARRAY,
	ARG_PARSE_END,
} argparse_option_type;

typedef struct argparse {
	enum argparse_option_type type;	
	// internal context
	int argc;
	char **argv;
	char *optvalue; // current option value
} argparse;

typedef enum opt_id {
	OPT_SET_PWM_MODE = 0,
	OPT_SET_FAN = 1,
} opt_id;

typedef struct argparse_option {
	enum argparse_option_type type;
	enum opt_id id;
	const char *long_name;
	const char *help;
} argparse_option;

typedef int (*fptr)(struct node_t *, const char *);

#define OPT_STRING(...)		{ARG_PARSE_STRING, __VA_ARGS__}
#define OPT_INT(...)		{ARG_PARSE_INT, __VA_ARGS__}
#define OPT_INT_ARR(...)	{ARG_PARSE_INT_ARRAY, __VA_ARGS__}
#define OPT_END()			{ARG_PARSE_END, -1, NULL, NULL}

void push(struct node_t **_node, void *data);
void *pop(struct node_t **_node);
void destroy_node(struct node_t **_node);

const char *get_root(struct node_t *_node);
const char *get_hwmon(struct node_t *_node);
void free_pgpu(struct p_gpu *data);
int pwm_init(struct node_t *_node);
int pwm_control(struct node_t *_node);

// init with args
int __int_args(struct argparse_option *option,
				struct node_t *amdgpu, int argc, char *argv[]);
int pwm_set(struct node_t *_node, const char *mode);

#endif // _RKS_DATA_STRUCT_H__