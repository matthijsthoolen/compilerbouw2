#include "gen_byte_code.h"

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

node *GBCprogram(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCprogram");

    PROGRAM_HEAD(arg_node) = TRAVdo(PROGRAM_HEAD(arg_node), arg_info);

    PROGRAM_NEXT(arg_node) = TRAVopt(PROGRAM_NEXT(arg_node), arg_info);

    DBUG_RETURN(arg_node);
}

node *GBCfun(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCfun");

    DBUG_RETURN(arg_node);
}

node *GBCvardeflist(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvardeflist");
   
    VARDEFLIST_HEAD(arg_node) = TRAVdo(VARDEFLIST_HEAD(arg_node), arg_info);
    
    VARDEFLIST_NEXT(arg_node) = TRAVopt(VARDEFLIST_NEXT(arg_node), arg_info);
 
    DBUG_RETURN(arg_node);
}

node *GBCvardef(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCvardef");

    DBUG_RETURN(arg_node);
}

node *GBCinnerblock(node *arg_node, info *arg_info)
{
    DBUG_ENTER("GBCinnerblock");

    DBUG_RETURN(arg_node);
}

void export_assembly(info *info) {
}


/*
 * Traversal start function
 */

node *GBCdoGenByteCode( node *syntaxtree)
{
    info *info;

    DBUG_ENTER("GBCdoGenByteCode");

    DBUG_ASSERT((syntaxtree != NULL), "GBCdoGenByteCode called with empty syntaxtree");

    info = MakeInfo();

    TRAVpush(TR_gbc);

    syntaxtree = TRAVdo(syntaxtree, info);

    export_assembly(info);

    TRAVpop();
    
    info = FreeInfo(info);    

    DBUG_RETURN( syntaxtree);
}
