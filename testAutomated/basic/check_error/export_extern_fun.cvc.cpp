# 1 "basic/check_error/export_extern_fun.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_error/export_extern_fun.cvc"
extern void f(int a);
export void f(bool a) {}

export extern void g() {}
