
/**
 * @file print.h
 *
 * Functions to print node structures
 *
 */

#ifndef _SAC_PRT_NODE_H_
#define _SAC_PRT_NODE_H_

#include "types.h"

extern node *PRTstmts (node * arg_node, info * arg_info);
extern node *PRTassign (node * arg_node, info * arg_info);
extern node *PRTvar (node * arg_node, info * arg_info);
extern node *PRTbinop (node * arg_node, info * arg_info);
extern node *PRTfloat (node * arg_node, info * arg_info);
extern node *PRTint (node * arg_node, info * arg_info);
extern node *PRTbool (node * arg_node, info * arg_info);
extern node *PRTerror (node * arg_node, info * arg_info);

extern node *PRTsymboltable (node * arg_node, info * arg_info);
extern node *PRTsymboltableentry (node * arg_node, info * arg_info);

extern node *PRTprogram (node * arg_node, info * arg_info);
extern node *PRTvardef (node * arg_node, info * arg_info);
extern node *PRTvardeflist (node * arg_node, info * arg_info);
extern node *PRTfun (node * arg_node, info * arg_info);
extern node *PRTfunparamlist (node * arg_node, info * arg_info);
extern node *PRTfunparam (node * arg_node, info * arg_info);
extern node *PRTinnerblock (node * arg_node, info * arg_info);
extern node *PRTif (node * arg_node, info * arg_info);
extern node *PRTwhile (node * arg_node, info * arg_info);
extern node *PRTdowhile (node * arg_node, info * arg_info);
extern node *PRTfor (node * arg_node, info * arg_info);
extern node *PRTreturn (node * arg_node, info * arg_info);
extern node *PRTexprlist (node * arg_node, info * arg_info);
extern node *PRTcall (node * arg_node, info * arg_info);
extern node *PRTcast (node * arg_node, info * arg_info);
extern node *PRTmonop (node * arg_node, info * arg_info);

extern node *PRTarray (node * arg_node, info * arg_info);
extern node *PRTnewarray (node * arg_node, info * arg_info);

extern node *PRTdoPrint( node *syntaxtree);

extern char *get_type_name(type ty);

#endif /* _SAC_PRT_NODE_H_ */
