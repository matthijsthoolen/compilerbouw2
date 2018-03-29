#ifndef _SAC_CONTEXT_ANALYSIS_FUNCTION_H_
#define _SAC_CONTEXT_ANALYSIS_FUNCTION_H_

extern node *CAFfun(node *arg_node, info *arg_info);
extern node *CAFcall(node *arg_node, info *arg_info);

extern node *CAFdoContextAnalysisFun(node *syntaxtree);

extern void check_fun_calls(info *arg_info);

#endif
