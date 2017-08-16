#ifndef _SAC_CONTEXT_ANALYSIS_VARS_H_
#define _SAC_CONTEXT_ANALYSIS_VARS_H_

extern node *CAVblock(node *arg_node, info *arg_info);
extern node *CAVfun(node *arg_node, info *arg_info);
extern node *CAVfunparam(node *arg_node, info *arg_info);
extern node *CAVvardef(node *arg_node, info *arg_info);
extern node *CAVvardeflist(node *arg_node, info *arg_info); 
extern node *CAVvar(node *arg_node, info *arg_info);

extern node *CAVdoContextAnalysisVar(node *syntaxtree);

#endif
