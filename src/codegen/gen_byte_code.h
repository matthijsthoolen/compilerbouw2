#ifndef _GEN_BYTE_CODE_H_
#define _GEN_BYTE_CODE_H_
#include "types.h"

extern node *GBCprogram(node *arg_node, info *arg_info);
extern node *GBCfun(node *arg_node, info *arg_info);
extern node *GBCreturn(node *arg_node, info *arg_info);
extern node *GBCcall(node *arg_node, info *arg_info);
extern node *GBCvardeflist(node *arg_node, info *arg_info);
extern node *GBCvardef(node *arg_node, info *arg_info);
extern node *GBCassign(node *arg_node, info *arg_info);
extern node *GBCvar(node *arg_node, info *arg_info);
extern node *GBCcast(node *arg_node, info *arg_info);
extern node *GBCmonop(node *arg_node, info *arg_info);
extern node *GBCbinop(node *arg_node, info *arg_info);
extern node *GBCternop(node *arg_node, info *arg_info);
extern node *GBCfloat(node *arg_node, info *arg_info);
extern node *GBCint(node *arg_node, info *arg_info);
extern node *GBCbool(node *arg_node, info *arg_info);
extern node *GBCinnerblock(node *arg_node, info *arg_info);
extern node *GBCif(node *arg_node, info *arg_info);
extern node *GBCwhile(node *arg_node, info *arg_info);
extern node *GBCdowhile(node *arg_node, info *arg_info);
extern node *GBCfor(node *arg_node, info *arg_info);

extern void export_assembly(info *info);

extern node *GBCdoGenByteCode( node *syntaxtree);

#endif
