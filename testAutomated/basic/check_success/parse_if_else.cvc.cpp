# 1 "basic/check_success/parse_if_else.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/parse_if_else.cvc"
void foo() {
    if (true) {
        foo();
    } else {
        foo();
    }

    if (false)
        foo();
    else
        foo();


    if ( true )
foo();

else {


    foo();
}}
