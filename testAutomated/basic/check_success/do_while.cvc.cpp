# 1 "basic/check_success/do_while.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/do_while.cvc"
void test_do_while2()
{
    int x = 0;
    int y = 10;
    do {
        x = x + 1;
        y = y - 1;
    } while (x <= y);
}
