# 1 "basic/check_error/invalid_for_loop_type.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_error/invalid_for_loop_type.cvc"
void foo() {


    for (float f = 0.0, 10.0) {
        foo();
    }

    for (bool b = true, false) {
        foo();
    }
}
