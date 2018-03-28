# 1 "basic/check_error/return_multiple_types.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_error/return_multiple_types.cvc"
int testMultipleReturns() {
    if (1 == 1) {
        return 5.0;
    } else {
        return false;
    }

    while (1 > 2) {
        return 2.5;
    }

    return 5;
}


export int main() {
    return testMultipleReturns();
}
