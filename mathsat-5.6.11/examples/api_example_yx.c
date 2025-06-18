/*
 * Description: MathSAT5 returns SAT, hoping to get the byte value of a variable.
 * formula smtlibStr1 is fault smtlibStr1 is success.
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "mathsat.h"


static void example1();

int main()
{
  example1();

  return 0;
}

static void print_model(msat_env env)
{
  /* we use a model iterator to retrieve the model values for all the
   * variables, and the necessary function instantiations */
  msat_model_iterator iter = msat_create_model_iterator(env);
  assert(!MSAT_ERROR_MODEL_ITERATOR(iter));

  printf("Model:\n");
  while (msat_model_iterator_has_next(iter)) {
    msat_term t, v;
    char *s;
    msat_model_iterator_next(iter, &t, &v);
    s = msat_term_repr(t);
    assert(s);
    printf(" %s = ", s);
    msat_free(s);
    s = msat_term_repr(v);
    assert(s);
    printf("%s\n", s);
    msat_free(s);
  }
  msat_destroy_model_iterator(iter);
}


/*
 * This example shows the basic usage of the API for creating formulas,
 * checking satisfiability, and using the solver incrementally
 */
static void example1()
{
  msat_config cfg;
  msat_env env;
  msat_term formula;
  msat_result status;
  int res;
  char *s;

  printf("\nExample 1\n");

  /* first, we create a configuration */
  cfg = msat_create_config();
  /* enable model generation */
  msat_set_option(cfg, "model_generation", "true");

  /* create an environment with the above configuration */
  env = msat_create_env(cfg);

  /// add by yx

  /* create a formula and assert it permanently */
//    formula = create_a_formula(env);

  char *smtlibStr0 = "(set-info :smt-lib-version 2.6)\n"
                     ";;; Processed by pysmt to remove constant-real bitvector literals\n"
                     "(set-logic QF_FP)\n"
                     "(set-info :source |SPARK inspired floating point problems by Florian Schanda|)\n"
                     "(set-info :category \"crafted\")\n"
                     "(set-info :status sat)\n"
                     "(declare-fun a () Float32)\n"
                     "(declare-fun b () Float32)\n"
                     "(declare-fun n () Float32)\n"
                     "(assert (fp.lt a (fp.mul RNE n b)))\n"
                     "(assert (fp.gt b (_ +zero 8 24)))\n"
                     "(assert (not (fp.leq (fp.div RNE a b) n)))\n"
                     "(check-sat)\n"
                     "(exit)";

  char *smtlibStr1 = "(set-info :source |printed by MathSAT|)\n"
                     "(declare-fun a_ack () (_ BitVec 64))\n"
                     "(assert (let ((.def_9 ((_ to_fp 11 53) RNE a_ack)))\n"
                     "(let ((.def_12 (fp.eq .def_9 (fp #b0 #b00000000000 #b0000000000000000000000000000000000000000000000000000))))\n"
                     "(let ((.def_13 (not .def_12)))\n"
                     ".def_13))))";

  formula = msat_from_smtlib2(env, smtlibStr0);

  res = msat_assert_formula(env, formula);
  assert(res == 0);

  /* check satisfiability */
  status = msat_solve(env);
  assert(status == MSAT_SAT);

  /* display the model */
  print_model(env);

  msat_type bvtype = msat_get_bv_type(env, 64);
  msat_decl decl = msat_declare_function(env, "a_ack", bvtype);
  msat_term a = msat_make_constant(env, decl);

  msat_term byte = msat_make_bv_extract(env, 7, 0, a);

  msat_term byteVal = msat_get_model_value(env, byte);

  assert(MSAT_ERROR_TERM(byteVal)==0);

  msat_destroy_env(env);
  msat_destroy_config(cfg);

  printf("\nExample 1 done\n");
}
