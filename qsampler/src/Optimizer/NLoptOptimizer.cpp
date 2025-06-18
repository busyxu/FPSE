//===------------------------------------------------------------*- C++ -*-===//
//
// This file is distributed under MIT License. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright (c) 2017 University of Kaiserslautern.
//

#include "NLoptOptimizer.h"
#include <assert.h>
#include <cmath>
#include <vector>
#include <cfloat>
#include <iostream>
#include "z3_api.h"

namespace gosat {
OptConfig::OptConfig() :
        MaxEvalCount{500000},
        MaxLocalEvalCount{50000},
        RelTolerance{1e-10},
        Bound{1e9},
        StepSize{0.5},
        InitialPopulation{0}
{}

OptConfig::OptConfig(nlopt_algorithm global_alg, nlopt_algorithm local_alg) :
        MaxEvalCount{500000000},
        MaxLocalEvalCount{100000},
        RelTolerance{1e-10},
        Bound{DBL_MAX/2},
//        Bound(MAXFLOAT),
//        Bound(1e9),
        StepSize{0.5},
        InitialPopulation{0}
{
    assert(local_alg == NLOPT_LN_BOBYQA &&
           "Invalid local optimization algorithms!");
    if (global_alg == NLOPT_G_MLSL_LDS || global_alg == NLOPT_G_MLSL) {
        MaxEvalCount = 500000;
        RelTolerance = 1e-5;
    }
}

NLoptOptimizer::NLoptOptimizer() :
        m_global_opt_alg{NLOPT_GN_DIRECT},
        m_local_opt_alg{NLOPT_LN_BOBYQA}
{}

NLoptOptimizer::NLoptOptimizer(nlopt_algorithm global_alg,
                               nlopt_algorithm local_alg) :
        m_global_opt_alg{global_alg},
        m_local_opt_alg{local_alg},
        Config{global_alg, local_alg}
{}

int
NLoptOptimizer::optimize
        (nlopt_func func, unsigned dim, double* x, double* seed, int seed_size, double* min, int max_time, int pop_size,
         double **model, int *model_size, int model_maxSize) const noexcept
{
    assert(NLoptOptimizer::isSupportedGlobalOptAlg(m_global_opt_alg)
           && "Unsupported global optimization algorithm");
    nlopt_opt opt;
    gosat::OptConfig Config(m_global_opt_alg, m_local_opt_alg);
    opt = nlopt_create(m_global_opt_alg, dim);//add by yx.  a double variable is 8 bytes
    nlopt_set_min_objective(opt, func, NULL);
//    nlopt_set_max_objective(opt, func, NULL);
//    std::cout<<"bound:"<<Config.Bound<<"\n"<<DBL_MAX<<"\n"<<DBL_MAX<<"\n";
    nlopt_set_upper_bounds1(opt, Config.Bound);
    nlopt_set_lower_bounds1(opt, -Config.Bound);
//    nlopt_set_upper_bounds1(opt, 1.0);
//    nlopt_set_lower_bounds1(opt, 0.0);
    std::vector<double> step_size_arr(dim, Config.StepSize);
    nlopt_set_initial_step(opt, step_size_arr.data());
    nlopt_set_stopval(opt, 0);
//    nlopt_set_xtol_rel(opt, Config.RelTolerance);
//    nlopt_set_maxeval(opt, Config.MaxEvalCount);
    nlopt_set_maxtime(opt, max_time);
//    nlopt_set_population(opt, pop_size);
//    nlopt_set_maxtime(opt, 90);//30s
//    nlopt_set_population(opt, 300);
    nlopt_set_outSeed(opt, seed, seed_size);//add by yx, where, *grad is init value.What is the effect of the value
    nlopt_set_outModel(opt, model, model_size, model_maxSize);//add by yx
//    nlopt_set_inputFile(opt, (char *)inputFile.c_str());//add by yx
//    nlopt_set_outDecl(opt, decls, decl_size);//add by yx
//    nlopt_set_outSort(opt, sorts, sort_size);//add by yx
//    nlopt_set_outKind(opt, kinds, kind_size);//add by yx
//    nlopt_set_outCTX(opt, smt_ctx);//add by yx
//    nlopt_set_outAST(opt, smt_ast);//add by yx

    if (NLoptOptimizer::isRequirePopulation(m_global_opt_alg)) {
//        nlopt_set_population(opt, Config.InitialPopulation);
        nlopt_set_population(opt, pop_size);
    }
    if (!NLoptOptimizer::isRequireLocalOptAlg(m_global_opt_alg)) {
        auto status = nlopt_optimize(opt, x, min);
        nlopt_destroy(opt);
        return status;
    }
    assert(NLoptOptimizer::isSupportedLocalOptAlg(m_local_opt_alg)
           && "Unsupported local optimization algorithm!");
    nlopt_opt local_opt;
    local_opt = nlopt_create(m_local_opt_alg, dim);
    nlopt_set_min_objective(local_opt, func, NULL);
//    nlopt_set_max_objective(local_opt, func, NULL);
    nlopt_set_initial_step(local_opt, step_size_arr.data());
    nlopt_set_stopval(local_opt, 0);
    nlopt_set_maxeval(local_opt, Config.MaxLocalEvalCount);
    nlopt_set_local_optimizer(opt, local_opt);
    auto status = nlopt_optimize(opt, x, min);
    nlopt_destroy(local_opt);
    nlopt_destroy(opt);
    return status;
}

bool
NLoptOptimizer::isSupportedLocalOptAlg(nlopt_algorithm local_opt_alg) noexcept
{
    return (local_opt_alg == NLOPT_LN_BOBYQA
            || local_opt_alg == NLOPT_LN_SBPLX);
}

bool
NLoptOptimizer::isRequireLocalOptAlg(nlopt_algorithm opt_alg) noexcept
{
    return opt_alg == NLOPT_G_MLSL || opt_alg == NLOPT_G_MLSL_LDS;
}

bool
NLoptOptimizer::isSupportedGlobalOptAlg(nlopt_algorithm opt_alg) noexcept
{
    switch (opt_alg) {
        case NLOPT_GN_DIRECT:
        case NLOPT_GN_DIRECT_L:
        case NLOPT_GN_DIRECT_L_RAND:
        case NLOPT_GN_ORIG_DIRECT:
        case NLOPT_GN_ORIG_DIRECT_L:
        case NLOPT_GN_MLSL_LDS:
        case NLOPT_G_MLSL:
        case NLOPT_G_MLSL_LDS:
        case NLOPT_GN_CRS2_LM:
        case NLOPT_GN_ISRES:
        case NLOPT_GN_ESCH:
        case NLOPT_GN_BYTEEA://add by yx
        case NLOPT_GN_GA://add by yx
        case NLOPT_GN_MOEA://add by yx
        case NLOPT_GN_MOCEA://add by yx
        case NLOPT_GN_MOCEASP2://add by yx
        case NLOPT_GN_MOCEASP3://add by yx
        case NLOPT_GN_MOCEADS2://add by yx
        case NLOPT_GN_MOCEADS3://add by yx
        case NLOPT_GN_SOEACov: //add by yx
        case NLOPT_GN_SOEADis: //add by yx
        case NLOPT_GN_NSGA2: //add by yx
            return true;
        default:
            return false;
    }
}

bool
NLoptOptimizer::isRequirePopulation(nlopt_algorithm opt_alg) noexcept
{
    switch (opt_alg) {
        case NLOPT_GN_MLSL:
        case NLOPT_GN_CRS2_LM:
        case NLOPT_GN_ISRES:
        case NLOPT_GN_ESCH:
        case NLOPT_GN_BYTEEA:
        case NLOPT_GN_GA://add by yx
        case NLOPT_GN_MOEA://add by yx
        case NLOPT_GN_MOCEA://add by yx
        case NLOPT_GN_MOCEASP2://add by yx
        case NLOPT_GN_MOCEASP3://add by yx
        case NLOPT_GN_MOCEADS2://add by yx
        case NLOPT_GN_MOCEADS3://add by yx
        case NLOPT_GN_SOEACov://add by yx
        case NLOPT_GN_SOEADis://add by yx
        case NLOPT_GN_NSGA2://add by yx
            return true;
        default:
            return false;
    }
}

int
NLoptOptimizer::refineResult
        (nlopt_func func, unsigned dim, double* x, double* grad, double* min)
{
    nlopt_opt opt;
    opt = nlopt_create(NLOPT_LN_BOBYQA, dim);
    nlopt_set_min_objective(opt, func, NULL);
    nlopt_set_initial_step(opt, &Config.StepSize);
    nlopt_set_xtol_rel(opt, Config.RelTolerance);
    nlopt_set_maxeval(opt, Config.MaxLocalEvalCount);
    auto status = nlopt_optimize(opt, x, min);
    nlopt_destroy(opt);
    return status;
}

bool
NLoptOptimizer::existsRoundingError
        (nlopt_func func,
         unsigned int dim,
         const double* x,
         const double* min) const noexcept
{
    return func(dim, x, nullptr, nullptr) != *min;
}

void
NLoptOptimizer::fixRoundingErrorNearZero(nlopt_func const func,
                                         unsigned dim,
                                         double* x,
                                         double* min) const noexcept
{
    if (*min == 0 || std::fabs(*min) > 1e-6) {
        return;
    }
    for (unsigned int i = 0; i < dim; ++i) {
        double int_part;
        std::modf(x[i], &int_part);
        if (std::fabs(x[i] - int_part) < 1e-6) {
            double temp = x[i];
            x[i] = int_part;
            const auto min_x = func(dim, x, nullptr, nullptr);
            if (*min < min_x || std::fpclassify(min_x) == FP_NAN) {
                x[i] = temp;
            }
        }
    }
    *min = func(dim, x, nullptr, nullptr);
}

double NLoptOptimizer::eval
        (nlopt_func func, unsigned dim, const double* x) const noexcept
{
    return func(dim, x, nullptr, nullptr);
}
}
