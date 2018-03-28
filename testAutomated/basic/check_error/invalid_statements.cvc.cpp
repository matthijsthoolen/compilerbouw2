# 1 "basic/check_error/invalid_statements.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_error/invalid_statements.cvc"
void sum_arrayexp()
{
  int i = 1 + [0];
  1 + 2;
}

void empty()
{
  ;
}
