#include "array_init.h"

#include "copy.h"
#include "dbug.h"
#include "globals.h"
#include "str.h"
#include "err.h"
#include "print.h"
#include "traverse.h"

node *ARRassign(node *arg_node, info *arg_info) {
    DBUG_ENTER("ARRAssign");

    DBUG_RETURN( arg_node);
}

node *DSEdoArrayInit(node *syntaxtree) {
    DBUG_ENTER("ARRdoArrayInit");

    DBUG_ASSERT((syntaxtree != NULL), "ARRdoArrayInit called with empty syntaxtree");

    PRTdoPrint(syntaxtree);

    TRAVpush(TR_arr);

    syntaxtree = TRAVdo(syntaxtree, NULL);

    TRAVpop();

    DBUG_RETURN( syntaxtree);
}
