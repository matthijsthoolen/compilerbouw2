#include "optimize_ops.h"

#include "print.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "ctinfo.h"
#include "memory.h"
#include "dbug.h"
#include "str.h"

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
