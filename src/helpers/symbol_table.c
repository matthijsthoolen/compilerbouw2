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
 * @param  source_node    [description]
 * @param  name        [description]
 * @param  node_type   [description]
 * @return             [description]
 */
node *addToSymboltable(node* symbolTable, node* source_node, char* name, type node_type, int index, int lvl)
{
    DBUG_ENTER("addToSymboltable");
    DBUG_PRINT("HELP", ("Add a row to the symbol table"));

    node *row = TBmakeSymboltableentry(NULL);
    //
    DBUG_PRINT("SymbolTable", ("Created a new symboltable entry"));
    SYMBOLTABLEENTRY_SOURCE(row) = source_node;
    SYMBOLTABLEENTRY_NAME(row) = name;
    SYMBOLTABLEENTRY_TYPE(row) = node_type;
    SYMBOLTABLEENTRY_INDEX(row) = index;
    SYMBOLTABLEENTRY_NESTINGLVL(row) = lvl;

    NODE_LINE(row) = NODE_LINE(source_node);
    NODE_COL(row) = NODE_COL(source_node);

    node *lastEntry = SYMBOLTABLE_SYMBOLTABLEENTRY(symbolTable);

    if (lastEntry == NULL) {
        DBUG_PRINT("SymbolTable", ("First entry for this scope"));
        SYMBOLTABLE_SYMBOLTABLEENTRY(symbolTable) = row;
        if (NODE_TYPE(source_node) == N_vardef) {
            SYMBOLTABLE_VARCOUNT(symbolTable) = 1;
        } else if (NODE_TYPE(source_node) == N_funparam) {
            SYMBOLTABLE_PARAMCOUNT(symbolTable) = 1;
        }
    } else {
        DBUG_PRINT("SymbolTable", ("Entries are already present"));
        while (SYMBOLTABLEENTRY_NEXT(lastEntry)) {
            lastEntry = SYMBOLTABLEENTRY_NEXT(lastEntry);
        }

        SYMBOLTABLEENTRY_NEXT(lastEntry) = row;

        if (NODE_TYPE(source_node) == N_vardef) {
            SYMBOLTABLE_VARCOUNT(symbolTable)++;
        } else if (NODE_TYPE(source_node) == N_funparam) {
            SYMBOLTABLE_PARAMCOUNT(symbolTable)++;
        }
    }

    DBUG_RETURN(row);
}
