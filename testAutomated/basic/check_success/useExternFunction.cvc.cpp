# 1 "basic/check_success/useExternFunction.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/useExternFunction.cvc"
int i = 4189369;

extern void printInt(int i);

export int main()
{
  printInt(i);
  return 0;
}
