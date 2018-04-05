#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"

#include "clean.h"

node *DCCmonop(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DCCmonop");

    if (MONOP_OP(arg_node) == MO_neg) {
        DBUG_PRINT("DCC", ("Try to add the MO_neg to the int var if possible"));

        node *optimize = NULL;

        if (NODE_TYPE(MONOP_EXPR(arg_node)) == N_int) {
            DBUG_PRINT("DCC", ("Negative int detected"));
            optimize = TBmakeInt(- INT_VALUE(MONOP_EXPR(arg_node)));
        } else if (NODE_TYPE(MONOP_EXPR(arg_node)) == N_float) {
            DBUG_PRINT("DCC", ("Negative float detected"));
            optimize = TBmakeFloat(- FLOAT_VALUE(MONOP_EXPR(arg_node)));
        }

        if (optimize != NULL) {
            NODE_LINE(optimize) = NODE_LINE(arg_node);
            NODE_COL(optimize)  = NODE_COL(arg_node);

            arg_node = MEMfree(arg_node);
            arg_node = optimize;

            DBUG_PRINT("DCC", ("Succesfully replaced negative float or int"));
        }
    } else {
        TRAVopt(MONOP_EXPR(arg_node), arg_info);
    }

    DBUG_RETURN(arg_node);
}

node *DCCdoCodeClean(node *syntaxtree)
{
    DBUG_ENTER("DCCdoCodeClean");

    DBUG_ASSERT((syntaxtree != NULL), "DCCdoClean called with a empty syntaxtree");

    TRAVpush(TR_dcc);

    info *info;

    syntaxtree = TRAVdo( syntaxtree, info);

    TRAVpop();

    DBUG_RETURN(syntaxtree);
}
