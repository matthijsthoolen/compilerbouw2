# 1 "basic/check_success/parse_for.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/parse_for.cvc"
void foo() {
    for (int i = 0, 10) {
        foo();
        foo();
    }

    for (int i = 0, 10)
        foo();

    for (int i = 10, 0, -1)
        foo();
}
