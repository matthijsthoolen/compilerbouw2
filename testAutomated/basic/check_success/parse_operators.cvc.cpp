# 1 "basic/check_success/parse_operators.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/parse_operators.cvc"
void foo() {
    int a;
    bool b;
    a = 1 + 2;
    a = a - 1;
    a = a * 45;
    a = a / 9;
    a = a % 4;
    b = a < 1;
    b = a > 2;
    b = a <= 3;
    b = a >= 4;
    b = a == 5;
    b = a != 6;
    b = a != 6 && a >= 4;
    b = a == 6 || a <= 4;
    a = -a;
    b = !b;

    a = a - -1;
    a = ------a;
}
