#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"
#include "hash_list.h"
#include "list.h"

#include "for_to_while.h"

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

node *DSFWfor(node *arg_node, info *arg_info)
{
    DBUG_ENTER("DSEfor");

    DBUG_RETURN(arg_node);
}

node *DSFWforToWhile(node *syntaxtree)
{
    info *info;

    DBUG_ENTER("DSforToWhile");

    DBUG_ASSERT((syntaxtree != NULL), "DSforToWhile called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_dse);

    syntaxtree = TRAVdo(syntaxtree, info);

    TRAVpop();

    FreeInfo(info);

    DBUG_RETURN(syntaxtree);
}
