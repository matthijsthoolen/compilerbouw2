#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

extern node *addToSymboltable(node* symbolTable, node* source_node, char* name, type node_type, int lvl);
extern node *createNewSymbolTable(node* arg_node);

#endif
