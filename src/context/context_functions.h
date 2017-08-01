#ifndef _SAC_CONTEXT_ANALYSIS_H_
#define _SAC_CONTEXT_ANALYSIS_H_ 

void registerFunction(node *function, info *arg_info);
void registerFunctions(node *functions, info *arg_info);
node *CAscope(node *arg_node, info *arg_info);
node *CAfun(node *arg_node, info *arg_info);
node *CAcall(node *arg_node, info *arg_info);
node *CAdoContextAnalysisFun(node *syntaxtree);

#endif 
