/**
 * @file vardef_split.h
 *
 * Split the vardef in a declaration and assignment node
 *
 */

#ifndef _DSVS_SPLIT_VARDEF_H_
#define _DSVS_SPLIT_VARDEF_H_

#include "types.h"

extern node *DSVSblock(node *arg_node, info *arg_info);
extern node *DSVSfun(node *arg_node, info *arg_info);
extern node *DSVSinnerblock(node *arg_node, info *arg_info);
extern node *DSVSvardeflist(node *arg_node, info *arg_info);
extern node *DSVSvardef(node *arg_node, info *arg_info);
extern node *DSVSstmts(node *arg_node, info *arg_info);

extern node *DSVSdoVardefSplit(node *syntaxtree);

#endif
