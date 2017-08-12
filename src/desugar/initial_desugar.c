#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "str.h"
#include "ctinfo.h"

#include "initial_desugar.h"

node *DSEwhile(node *arg_node, info *arg_info) {
    DBUG_ENTER("DSEwhile");

    DBUG_RETURN(arg_node);
}

node *DSEfor(node *arg_node, info *arg_info) {
    DBUG_ENTER("DSEfor");

    DBUG_RETURN(arg_node);
}

node *DSEdowhile(node *arg_node, info *arg_info) {
    DBUG_ENTER("DSEdowhile");

    DBUG_RETURN(arg_node);
}

node *DSEstmts(node *arg_node, info *arg_info) {
    DBUG_ENTER("DSEstmts");

    DBUG_RETURN(arg_node);
}

node *DSEdoEarlyDesugar(node *syntaxtree) {
    DBUG_ENTER("DSdoEarlyDesugar");

    

    DBUG_RETURN(syntaxtree);
}
