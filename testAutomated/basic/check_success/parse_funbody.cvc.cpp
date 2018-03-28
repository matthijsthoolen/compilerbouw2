# 1 "basic/check_success/parse_funbody.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/parse_funbody.cvc"
void vardec() {
    int a;
}

void vardec_stat() {
    int a;
    a = 2;
}

int vardec_ret() {
    int a;
    return 2;
}

int vardec_stat_ret() {
    int a;
    a = 2;
    return a;
}

int stat_ret() {
    vardec();
    return 2;
}

int ret() {
    return 2;
}
