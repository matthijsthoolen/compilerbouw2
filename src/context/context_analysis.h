#ifndef _SAC_CONTEXT_ANALYSIS_H_
#define _SAC_CONTEXT_ANALYSIS_H_

extern node *CAprogram(node *arg_node, info *arg_info);
extern node *CAfun(node *arg_node, info *arg_info);
extern node *CAfunparam(node *arg_node, info *arg_info);
extern node *CAvardef(node *arg_node, info *arg_info);
extern node *CAvar(node *arg_node, info *arg_info);

extern node *CAdoContextAnalysis(node *syntaxtree);

#endif
