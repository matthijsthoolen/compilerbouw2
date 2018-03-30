#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "dbug.h"
#include "str.h"

#include "node_basic.h"
#include "tree_basic.h"
#include "traverse.h"

#include "symbol_table.h"

node *createNewSymbolTable(node* arg_node)
{
    DBUG_ENTER("createNewSymbolTable");

    node *symbolTable = TBmakeSymboltable(TBmakeSymboltableentry(NULL));

    DBUG_RETURN(symbolTable);
}

node *addToSymboltable(node* symbolTable, node* arg_node, char* name, type node_type)
{
    DBUG_ENTER("addToSymboltable");
    DBUG_PRINT("HELP", ("Add a row to the symbol table"));

    // node *row = TBmakeSymboltablerow(NULL);
    //
    // SYMBOLTABLEENTRY_SOURCE(row) = arg_node;
    // SYMBOLTABLEENTRY_NAME(row) = name;
    // SYMBOLTABLEENTRY_TYPE(row) = node_type;
    // //SYMBOLTABLEENTRY_NESTINGLVL(row) = NULL;
    //
    // NODE_LINE(row) = NODE_LINE(arg_node);
    // NODE_COL(row) = NODE_COL(arg_node);
    //
    // SYMBOLTABLEENTRY_NEXT(symbolTable) = row;

    DBUG_RETURN(NULL);
}
