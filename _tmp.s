
; function 'is_prime' with 1 parameters and 1 local vars
is_prime:
    esr 1
    iloadc 0
    istore 0
0_while:
    iload_0
    iload_0
    imul
    iload_0
    ile
    branch_f 0_while_end
    iload_0
    iload_0
    rem
    iloadc_0
    eq
    branch_f 0_end
    bloadc_f
    breturn
0_end:
    iload_0
    iloadc_1
    iadd
    istore 0
    jump 0_while
0_while_end:
    bloadc_t
    breturn

; function 'main' with 0 parameters and 2 local vars
main:
    esr 2
    iloadc 1
    istore 0
    iloadc 1
    istore 1
1_while:
    iload_0
    iloadc_0
    igt
    branch_f 1_while_end
    isrg
    iload_1
    jsr 1 is_prime
    branch_f 1_end
    iload_0
    iloadc_1
    isub
    istore 0
    isrg
    iload_1
    jsre 0
    isrg
    iloadc_1
    jsre 5
1_end:
    iload_1
    iloadc_1
    iadd
    istore 1
    jump 1_while
1_while_end:
    iloadc_0
    ireturn

 ; constants: 
.const int 2
.const int 100

 ; export functions:
.exportfun "main" int main

 ; global vardefs

 ; import functions:
.importfun "printInt" void int
.importfun "printFloat" void float
.importfun "scanInt" int
.importfun "scanFloat" float
.importfun "printSpaces" void int
.importfun "printNewlines" void int


