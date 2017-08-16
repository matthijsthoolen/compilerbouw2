#ifndef _SAC_CONTEXT_ANALYSIS_H_
#define _SAC_CONTEXT_ANALYSIS_H_ 

extern void registerFunction(node *function, info *arg_info);
extern void registerFunctions(node *functions, info *arg_info);
extern node *CAblock(node *arg_node, info *arg_info);
extern node *CAfun(node *arg_node, info *arg_info);
extern node *CAcall(node *arg_node, info *arg_info);

extern node *CAdoContextAnalysisFun(node *syntaxtree);

extern void check_fun_calls(info *arg_info);

#endif 
