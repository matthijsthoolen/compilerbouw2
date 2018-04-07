#include "optimize_ops.h"

#include "print.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "ctinfo.h"
#include "memory.h"
#include "dbug.h"
#include "str.h"
#include "free_node.h"
#include "node_helper.h"

struct INFO {
};

static info *MakeInfo()
{
    DBUG_ENTER("MakeInfo");

    info *result;

    result = MEMmalloc(sizeof(info));
    DBUG_RETURN(result);
}

static info *FreeInfo(info *info)
{
    DBUG_ENTER("FreeInfo");

    info = MEMfree(info);

    DBUG_RETURN(info);
}

node *DSOObinop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSOObinop");

    if (BINOP_OP(arg_node) != BO_or && BINOP_OP(arg_node) != BO_and) {
        DBUG_RETURN(arg_node);
    }

    node *TernOp;

    if (BINOP_OP(arg_node) == BO_or) {
        TernOp = TBmakeTernop(BINOP_LEFT(arg_node), TBmakeBool(TRUE), BINOP_RIGHT(arg_node));
        TERNOP_TYPE(arg_node) = BO_or;
    }

    if (BINOP_OP(arg_node) == BO_and) {
        TernOp = TBmakeTernop(BINOP_LEFT(arg_node), BINOP_RIGHT(arg_node), TBmakeBool(FALSE));
        TERNOP_TYPE(arg_node) = BO_and;
    }

    BINOP_LEFT(arg_node) = NULL;
    BINOP_RIGHT(arg_node) = NULL;
    arg_node = FREEbinop(arg_node, arg_info);

    arg_node = TernOp;

    DBUG_RETURN(arg_node);
}

node *DSOOcast(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSOOcast");

    // Try to find (bool) cast which we can change to a ternary operation
    if (getNodeType(CAST_EXPR(arg_node)) == TY_bool) {
        node *ternOp;

        if (CAST_TY(arg_node) == TY_int || CAST_TY(arg_node) == TY_float) {
            DBUG_PRINT("CAST", ("Changed cast from bool to int/float into a ternary operation."));
            ternOp = TBmakeTernop(
                CAST_EXPR(arg_node),
                (CAST_TY(arg_node) == TY_int) ? TBmakeInt(1) : TBmakeFloat(1.0),
                (CAST_TY(arg_node) == TY_int) ? TBmakeInt(0) : TBmakeFloat(1.0)
            );
            TERNOP_TYPE(ternOp) = CAST_TY(arg_node);

            CAST_EXPR(arg_node) = NULL;
            arg_node = FREEcast(arg_node, arg_info);
            arg_node = ternOp;
        }

        DBUG_RETURN(arg_node);
    }

    // Try to find (bool) cast which we can change to a ternary operation
    if (CAST_TY(arg_node) == TY_bool) {
        DBUG_PRINT("CAST", ("Found a bool cast on line %d", NODE_LINE(arg_node)));

        type type = getNodeType(CAST_EXPR(arg_node));

        if (type == TY_int || type == TY_float) {
            node* binOp = TBmakeBinop(
                BO_ne,
                CAST_EXPR(arg_node),
                (type == TY_int) ? TBmakeInt(0) : TBmakeFloat(0.0)
            );

            node *ternOp = TBmakeTernop(binOp, TBmakeBool(TRUE), TBmakeBool(FALSE));
            TERNOP_TYPE(ternOp) = TY_bool;

            CAST_EXPR(arg_node) = NULL;
            arg_node = FREEcast(arg_node, arg_info);
            arg_node = ternOp;
        }
    }

    DBUG_RETURN(arg_node);
}

/*
 * Traversal start function
 */
node *DSOOOptimizeOps(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSOOGenByteCode");

    DBUG_ASSERT((syntaxtree != NULL), "DSOOGenByteCode called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_dsoo);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    info = FreeInfo(info);

    DBUG_RETURN( syntaxtree);
}
