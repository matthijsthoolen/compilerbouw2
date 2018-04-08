main:
    esr 18
    iloadc_0
    istore 17
    iloadc_1
    istore 11
    iloadc 0
    istore 10
    iloadc 0
    istore 16
    iloadc_1
    ineg
    istore 9
    iloadc_0
    istore 8
    iloadc_0
    istore 15
    iloadc 1
    istore 7
    iloadc 0
    istore 6
    iloadc 2
    istore 14
    iloadc 3
    ineg
    istore 5
    iloadc_0
    istore 4
    iloadc_0
    istore 12
    iloadc_1
    istore 3
    iloadc 4
    istore 2
    iloadc_0
    istore 13
    iloadc_1
    istore 1
    iloadc 5
    istore 0
0_while:
    iload 11
    iloadc_0
    igt
    branch_f 0_ternop_else
    iload 17
    iload 10
    ilt
    jump 0_ternop_end
0_ternop_else:
    iload 17
    iload 10
    igt
0_ternop_end:
    branch_f 0_while_end
    isrg
    iload 17
    jsre 0
    isrg
    iloadc_1
    jsre 1
    iload 17
    iload 11
    iadd
    istore 17
    jump 0_while
0_while_end:
    isrg
    iloadc_1
    jsre 2
1_while:
    iload 9
    iloadc_0
    igt
    branch_f 1_ternop_else
    iload 16
    iload 8
    ilt
    jump 1_ternop_end
1_ternop_else:
    iload 16
    iload 8
    igt
1_ternop_end:
    branch_f 1_while_end
    isrg
    iload 16
    jsre 0
    isrg
    iloadc_1
    jsre 1
    iload 16
    iload 9
    iadd
    istore 16
    jump 1_while
1_while_end:
    isrg
    iloadc_1
    jsre 2
2_while:
    iload 7
    iloadc_0
    igt
    branch_f 2_ternop_else
    iload 15
    iload 6
    ilt
    jump 2_ternop_end
2_ternop_else:
    iload 15
    iload 6
    igt
2_ternop_end:
    branch_f 2_while_end
    isrg
    iload 15
    jsre 0
    isrg
    iloadc_1
    jsre 1
    iload 15
    iload 7
    iadd
    istore 15
    jump 2_while
2_while_end:
    isrg
    iloadc_1
    jsre 2
3_while:
    iload 5
    iloadc_0
    igt
    branch_f 3_ternop_else
    iload 14
    iload 4
    ilt
    jump 3_ternop_end
3_ternop_else:
    iload 14
    iload 4
    igt
3_ternop_end:
    branch_f 3_while_end
    isrg
    iload 14
    jsre 0
    isrg
    iloadc_1
    jsre 1
    iload 14
    iload 5
    iadd
    istore 14
    jump 3_while
3_while_end:
4_while:
    iload_3
    iloadc_0
    igt
    branch_f 4_ternop_else
    iload 12
    iload_2
    ilt
    jump 4_ternop_end
4_ternop_else:
    iload 12
    iload_2
    igt
4_ternop_end:
    branch_f 4_while_end
5_while:
    iload_1
    iloadc_0
    igt
    branch_f 5_ternop_else
    iload 13
    iload_0
    ilt
    jump 5_ternop_end
5_ternop_else:
    iload 13
    iload_0
    igt
5_ternop_end:
    branch_f 5_while_end
    isrg
    iload 12
    jsre 0
    isrg
    iloadc_1
    jsre 1
    isrg
    iload 13
    jsre 0
    isrg
    iloadc_1
    jsre 1
    iload 13
    iload_1
    iadd
    istore 13
    jump 5_while
5_while_end:
    isrg
    iloadc_1
    jsre 2
    iload 12
    iload_3
    iadd
    istore 12
    jump 4_while
4_while_end:
    iloadc_0
    ireturn

.const int 10
.const int 2
.const int 7
.const int 3
.const int 5
.const int 4
.exportfun "main" int main
.importfun "printInt" void int
.importfun "printSpaces" void int
.importfun "printNewlines" void int
