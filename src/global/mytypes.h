#ifndef _CIVCC_MYTYPES_H_
#define _CIVCC_MYTYPES_H_


#include <stdint.h>


/*
 * This enumeration defines all monops
 */
typedef enum { MO_not, MO_neg, MO_unknown } monop;

/*
 * This enumeration defines all binops
 */
typedef enum { BO_add, BO_sub, BO_mul, BO_div, BO_mod,
               BO_lt, BO_le, BO_gt, BO_ge, BO_eq, BO_ne,
               BO_and, BO_or, BO_unknown } binop;

typedef enum {
    TY_unknown,
    TY_void,
    TY_bool,
    TY_int,
    TY_float,
} type;

enum global_prefix {
    global_prefix_none = 0,
    global_prefix_extern = 1,
    global_prefix_export = 2,
};

typedef struct {
    type ty;
    char *id;
} fun_param;

typedef struct {
    type return_ty;
    char *id;
    uint8_t param_count;
    fun_param *params;
} fun_header;


#endif  /* _CIVCC_MYTYPES_H_ */
