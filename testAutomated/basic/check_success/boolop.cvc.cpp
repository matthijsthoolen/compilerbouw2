# 1 "basic/check_success/boolop.cvc"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "basic/check_success/boolop.cvc"
void f()
{
  int a = 2 + 3 + 4 + 5;
  float b = 39.0 + 3.0 + 21.0;
  bool c = false + false;

  int d = 2 - 3 - 4 - 5;
  float e = 39.0 - 3.0 - 21.0;

  int f = 2 * 3 * 1;
  float g = 3.14 * 2.0;
  bool h = true * false;

  int i = 4/3;
  int j = 6/0;
  float k = 5.0/2.0;
  float l = 9.0/0.00;

  int m = 12 % 8 % 3;

  bool n = 20 < 20;
  bool o = 3.0 < 6.1;

  bool p = 20 <= 20;
  bool q = 3.0 <= 6.1;

  bool r = 60 < 50 || 30 > 10;
  bool s = 3.0 > 2.0;

  bool t = 8 != 8;
  bool u = 8 == 8;
  bool v = 1.2 != 2.1;
  bool w = true != false;

}
