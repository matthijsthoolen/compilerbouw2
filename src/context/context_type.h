#ifndef _CATC_TYPE_CHECK_H_
#define _CATC_TYPE_CHECK_H_
#include "types.h"

extern node *CATCprogram(node *arg_node, info *arg_info);
extern node *CATCfun(node *arg_node, info *arg_info);
extern node *CATCvardeflist(node *arg_node, info *arg_info);
extern node *CATCvardef(node *arg_node, info *arg_info);
extern node *CATCinnerblock(node *arg_node, info *arg_info);
extern node *CATCassign(node *arg_node, info *arg_info);
extern node *CATCstmts(node *arg_node, info *arg_info);

extern node *CATCvar(node *arg_node, info *arg_info);
extern node *CATCmonop(node *arg_node, info *arg_info);
extern node *CATCbinop(node *arg_node, info *arg_info);
extern node *CATCint(node *arg_node, info *arg_info);
extern node *CATCfloat(node *arg_node, info *arg_info);
extern node *CATCbool(node *arg_node, info *arg_info);
extern node *CATCcall(node *arg_node, info *arg_info);
extern node *CATCcast(node *arg_node, info *arg_info);

extern char *pretty_print_type(type prt);

extern node *CATCdoContextTypeCheck( node *syntaxtree);

#endif
