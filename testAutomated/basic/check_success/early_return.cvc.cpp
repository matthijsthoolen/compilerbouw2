# 1 "basic/check_success/early_return.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/early_return.cvc"
void foo(int p) {
    int i = 1;
    if (p > 1)
        return;
    i = 2;
}
