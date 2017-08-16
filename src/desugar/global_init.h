#ifndef _SAC_DESUGAR_GLOBAL_INIT_H_
#define _SAC_DESUGAR_GLOBAL_INIT_H_

#include "types.h"

extern node *DSGIprogram(node *arg_node, info *arg_info);
extern node *DSGIvardef(node *arg_node, info *arg_info);

extern void construct_init(node *syntaxtree, info *info);
extern node *generate_init(info *info);

extern node *DSGIdoGlobalInit(node *syntaxtree);

#endif
