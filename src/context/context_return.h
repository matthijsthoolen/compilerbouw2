#ifndef _SAC_CONTEXT_ANALYSIS_RETURN_H_
#define _SAC_CONTEXT_ANALYSIS_RETURN_H_

#include "types.h"

extern node *CARCfun(node *arg_node, info *arg_info);
extern node *CARCreturn(node *arg_node, info *arg_info);
extern node *CARCif(node *arg_node, info *arg_info);
extern node *CARCstmts(node *arg_node, info *arg_info);

extern node *CARCdoContextReturnCheck(node *syntaxtree);

#endif
