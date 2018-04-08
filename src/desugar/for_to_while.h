#ifndef _DS_FOR_TO_WHILE_H_
#define _DS_FOR_TO_WHILE_H_

extern node *DSFWfun(node *arg_node, info *arg_info);
extern node *DSFWstmts(node *arg_node, info *arg_info);
extern node *DSFWinnerblock(node *arg_node, info *arg_info);
extern node *DSFWfor(node *arg_node, info *arg_info);

extern node *DSFWforToWhile(node *syntaxtree);

#endif
