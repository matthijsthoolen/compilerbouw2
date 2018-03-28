# 1 "basic/check_success/return_in_void.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/return_in_void.cvc"
void returnMe() {
    return;
}

export int main() {
    returnMe();

    return 1;
}
