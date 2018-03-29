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

    node *symbolTable = TBmakeSymboltable(arg_node);

    DBUG_RETURN(symbolTable);
}

node *addToSymboltable(node* symbolTable, node* arg_node)
{
    DBUG_ENTER("addToSymboltable");
    DBUG_PRINT("HELP", ("Add a row to the symbol table"));

    node *row = TBmakeSymboltablerow(NULL);

    SYMBOLTABLEROW_SOURCE(row) = arg_node;
    // SYMBOLTABLEROW_NAME(row) = NULL;
    // SYMBOLTABLEROW_TYPE(row) = NULL;
    //SYMBOLTABLEROW_NESTINGLVL(row) = NULL;

    NODE_LINE(row) = NODE_LINE(arg_node);
    NODE_COL(row) = NODE_COL(arg_node);

    SYMBOLTABLEROW_NEXT(symbolTable) = row;

    DBUG_RETURN(row);
}
