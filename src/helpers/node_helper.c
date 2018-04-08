#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dbug.h"
#include "memory.h"
#include "float.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"

#include "node_helper.h"

/**
 * Get the shortened type name.
 * @param  type [description]
 * @return      [description]
 */
char *getShortType(type type)
{
    DBUG_ENTER("getShortType");

    char *strType = "";

    DBUG_PRINT("GETNODETYPE", ("getShortType for %d", type));

    switch (type) {
        case TY_bool:
            strType = "b";
            break;
        case TY_float:
            strType = "f";
            break;
        case TY_int:
            strType = "i";
            break;
        case TY_unknown:
            break;
        case TY_void:
            break;
    }

    DBUG_RETURN(strType);
}

/**
 * Find the (nested) node type
 * @param  arg_node [description]
 * @return          [description]
 */
type getNodeType(node *arg_node)
{
    DBUG_ENTER("getNodeType");

    type type;

    switch (NODE_TYPE(arg_node)) {
        case N_vardef:
            DBUG_PRINT("GETNODETYPE", ("N_VARDEF"));
            type = VARDEF_TY(arg_node);
            break;
        case N_var:
            DBUG_PRINT("GETNODETYPE", ("N_var"));
            type = getNodeType(VAR_DECL(arg_node));
            break;
        case N_int:
            DBUG_PRINT("GETNODETYPE", ("N_int"));
            type = TY_int;
            break;
        case N_float:
            DBUG_PRINT("GETNODETYPE", ("N_float"));
            type = TY_float;
            break;
        case N_bool:
            DBUG_PRINT("GETNODETYPE", ("N_bool"));
            type = TY_bool;
            break;
        case N_binop:
            DBUG_PRINT("GETNODETYPE", ("N_binop"));
            type = BINOP_TYPE(arg_node);
            break;
        case N_monop:
            DBUG_PRINT("GETNODETYPE", ("N_monop"));
            type = MONOP_TYPE(arg_node);
            break;
        case N_fun:
            DBUG_PRINT("GETNODETYPE", ("N_fun"));
            type = FUN_RETTY(arg_node);
            break;
        case N_funparam:
            DBUG_PRINT("GETNODETYPE", ("N_funparam"));
            type = FUNPARAM_TY(arg_node);
            break;
        case N_call:
            DBUG_PRINT("GETNODETYPE", ("N_call"));
            type = getNodeType(CALL_DECL(arg_node));
            break;
        case N_cast:
            DBUG_PRINT("GETNODETYPE", ("N_cast"));
            type = CAST_TY(arg_node);
            break;
        case N_ternop:
            DBUG_PRINT("GETNODETYPE", ("N_ternop = %d", TERNOP_TYPE(arg_node)));
            type = TERNOP_TYPE(arg_node);
            break;
        case N_return:
            DBUG_PRINT("GETNODETYPE", ("N_return"));
            type = RETURN_TY(arg_node);
            break;
        default:
            DBUG_PRINT("GETNODETYPE", ("default %d", NODE_TYPE(arg_node)));
            type = TY_unknown;
    }

    DBUG_PRINT("GETNODETYPE", ("default %d", NODE_TYPE(arg_node)));

    DBUG_RETURN(type);
}

/**
 * Combination of getShortType and getNodeType
 * @param  arg_node [description]
 * @return          [description]
 */
char *getShortNodeType(node *arg_node)
{
    DBUG_ENTER("getShortNodeType");

    DBUG_RETURN(getShortType(getNodeType(arg_node)));
}
