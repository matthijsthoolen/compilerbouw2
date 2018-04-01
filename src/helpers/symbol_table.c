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

    node *symbolTable = TBmakeSymboltable(NULL);

    DBUG_RETURN(symbolTable);
}

/**
 * Add a Symbol to the end of the scope's symbol table
 * @param  symbolTable [description]
 * @param  arg_node    [description]
 * @param  name        [description]
 * @param  node_type   [description]
 * @return             [description]
 */
node *addToSymboltable(node* symbolTable, node* arg_node, char* name, type node_type)
{
    DBUG_ENTER("addToSymboltable");
    DBUG_PRINT("HELP", ("Add a row to the symbol table"));

    // if (SYMBOLTABLE_SYMBOLTABLEENTRY(arg_node))
    node *row = TBmakeSymboltableentry(NULL);
    //
    DBUG_PRINT("SymbolTable", ("Created a new symboltable entry"));
    SYMBOLTABLEENTRY_SOURCE(row) = arg_node;
    SYMBOLTABLEENTRY_NAME(row) = name;
    SYMBOLTABLEENTRY_TYPE(row) = node_type;
    // //SYMBOLTABLEENTRY_NESTINGLVL(row) = NULL;
    //
    NODE_LINE(row) = NODE_LINE(arg_node);
    NODE_COL(row) = NODE_COL(arg_node);
    //
    //

    node *lastEntry = SYMBOLTABLE_SYMBOLTABLEENTRY(symbolTable);

    if (lastEntry == NULL) {
        DBUG_PRINT("SymbolTable", ("First entry for this scope"));
        SYMBOLTABLE_SYMBOLTABLEENTRY(symbolTable) = row;
    } else {
        DBUG_PRINT("SymbolTable", ("Entries are already present"));
        while (SYMBOLTABLEENTRY_NEXT(lastEntry)) {
            lastEntry = SYMBOLTABLEENTRY_NEXT(lastEntry);
        }

        DBUG_PRINT("SymbolTable", ("So find the latest"));

        SYMBOLTABLEENTRY_NEXT(lastEntry) = row;

        DBUG_PRINT("SymbolTable", ("Found!"));
    }



    DBUG_RETURN(row);
}
