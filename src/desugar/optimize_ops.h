#ifndef _OPTIMIZE_OPS_H_
#define _OPTIMIZE_OPS_H_
#include "types.h"

extern node *DSOObinop(node *arg_node, info *arg_info);
extern node *DSOOcast(node *arg_node, info *arg_info);

extern node *DSOOGenByteCode(node *syntaxtree);

#endif
