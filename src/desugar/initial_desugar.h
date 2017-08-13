/**
 * @file initial_desugar.h
 *
 * Initial desugaring functions
 *
 */

#ifndef _DSE_INIT_DESUGAR_H_
#define _DSE_INIT_DESUGAR_H_

#include "types.h"

extern node *DSEblock(node *arg_node, info *arg_info);
extern node *DSEinnerblock(node *arg_node, info *arg_info);
extern node *DSEfun(node *arg_node, info *arg_info);
extern node *DSEwhile(node *arg_node, info *arg_info);
extern node *DSEfor(node *arg_node, info *arg_info);
extern node *DSEdowhile(node *arg_node, info *arg_info);
extern node *DSEstmts(node *arg_node, info *arg_info);

extern node *DSEdoEarlyDesugar(node *syntaxtree);

#endif
