#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

extern node *addToSymboltable(node* symbolTable, node* arg_node, char* name, type node_type);
extern node *createNewSymbolTable(node* arg_node);

#endif
