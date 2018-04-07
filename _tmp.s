
; function 'gcd' with 2 parameters and 0 local vars
gcd:
    iload_0
    iloadc_0
    ieq
    branch_f 0_end
    iload_0
    ireturn
0_end:
    isrg
    iload_0
    iload_0
    iload_0
    irem
    jsr 2 gcd
    ireturn

; function 'main' with 0 parameters and 2 local vars
main:
    esr 2
    iloadc 0
    istore 0
    iloadc 1
    istore 1
    isrg
    iload_0
    iload_1
    jsr 2 gcd
    istore 0
    isrg
    iload_0
    jsre 0
    isrg
    iloadc_1
    jsre 5
    iloadc_0
    ireturn

 ; constants: 
.const int 560
.const int 320

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


