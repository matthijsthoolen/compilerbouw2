#ifndef _SAC_CONTEXT_ANALYSIS_VARS_H_
#define _SAC_CONTEXT_ANALYSIS_VARS_H_

node *CAVscope(node *arg_node, info *arg_info);
node *CAVfun(node *arg_node, info *arg_info);
node *CAVvardef(node *arg_node, info *arg_info);

node *CAVdoContextAnalysisVar(node *syntaxtree);

#endif
