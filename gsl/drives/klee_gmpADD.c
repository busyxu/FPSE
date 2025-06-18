#include <gmp.h>
#include <klee/klee.h>

int main(){
  mpf_t a, b, c;     //申明变量a
  mpf_init(a);  //初始化a

  double a_v, b_v;     //申明新的变量a_v并符号化
  klee_make_symbolic(&a_v, sizeof(a_v), "a_v");
  klee_make_symbolic(&b_v, sizeof(b_v), "b_v");

  mpf_set_d(a, a_v);  //设置a的值为a_v，间接符号化变量a
  mpf_set_d(b, b_v);

  mpf_add(c, a, b);
}


