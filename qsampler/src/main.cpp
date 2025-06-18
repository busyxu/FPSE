//===------------------------------------------------------------*- C++ -*-===//
//
// This file is distributed under MIT License. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright (c) 2017 University of Kaiserslautern.
//

#include "llvm/Support/CommandLine.h"
#include "ExprAnalyzer/FPExprAnalyzer.h"
#include "CodeGen/FPExprLibGenerator.h"
#include "CodeGen/FPExprCodeGenerator.h"
#include "IRGen/FPIRGenerator.h"
#include "Utils/FPAUtils.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/TargetSelect.h"
#include "Optimizer/ModelValidator.h"
#include <nlopt.h>
#include <Optimizer/NLoptOptimizer.h>
#include <iomanip>
#include <sys/stat.h>  // For mkdir()
#include <unistd.h>
#include <dirent.h>     // opendir, readdir

typedef std::numeric_limits<double> dbl;

enum goSATMode {
    kUndefinedMode = 0,
    kFormulaAnalysis,
    kCCodeGeneration,
    kNativeSolving
};

enum goSATAlgorithm {
    kUndefinedAlg = 0,
    kCRS2 = NLOPT_GN_CRS2_LM,
    kISRES = NLOPT_GN_ISRES,
    kMLSL = NLOPT_G_MLSL,
    kDirect = NLOPT_GN_DIRECT_L,
    kESCH   = NLOPT_GN_ESCH,
    kBYTEEA = NLOPT_GN_BYTEEA,
    kGA = NLOPT_GN_GA,
    kMOEA = NLOPT_GN_MOEA,
    kMOCEA = NLOPT_GN_MOCEA,
    kMOCEASP2 = NLOPT_GN_MOCEASP2,
    kMOCEASP3 = NLOPT_GN_MOCEASP3,
    kMOCEADS2 = NLOPT_GN_MOCEADS2,
    kMOCEADS3 = NLOPT_GN_MOCEADS3,
    kMOEADDS = NLOPT_GN_MOEADDS,
    kSOEACov = NLOPT_GN_SOEACov,
    kSOEADis = NLOPT_GN_SOEADis,
    kNSGA2 = NLOPT_GN_NSGA2
};

llvm::cl::OptionCategory
        SolverCategory("Solver Options", "Options for controlling FPA solver.");

static llvm::cl::opt<int> max_time(
        "max_time", llvm::cl::Optional,
        llvm::cl::desc("Maximum solving execution time (default 3600 seconds)"),
        llvm::cl::init(3600));

static llvm::cl::opt<int> pop_size(
        "pop_size", llvm::cl::Optional,
        llvm::cl::desc("Maximum population size (default 100)"),
        llvm::cl::init(100));

static llvm::cl::opt<int> model_maxSize(
        "model_maxSize", llvm::cl::Optional,
        llvm::cl::desc("Maximum models size (default 10000)"),
        llvm::cl::init(10000));

static llvm::cl::opt<unsigned long long> ExportModelN(
        "export-model-n", llvm::cl::init(0),
        llvm::cl::desc("Randomly pick N models to export, 0 means all are selected (default 0)"));

static llvm::cl::opt<unsigned long long> ExportModelPct(
        "export-model-pct", llvm::cl::init(100),
        llvm::cl::desc("Randomly pick N\% of models to export, 100 means all are selected (default 100)"));

static llvm::cl::opt<bool> smart_seed(
        "smart_seed", llvm::cl::Optional,
        llvm::cl::desc("Initialization preprocessing (default true)"),
        llvm::cl::init(true));

static llvm::cl::opt<std::string>
        opt_input_file(llvm::cl::Required,
                       "f",
                       llvm::cl::desc("path to smt file"),
                       llvm::cl::value_desc("filename"),
                       llvm::cl::cat(SolverCategory));

static llvm::cl::opt<std::string> OutputDirectory(
        "output-dir", llvm::cl::init(""),
        llvm::cl::desc("Output directory. If not set automatically create one"));

static llvm::cl::opt<bool>
        validate_model(llvm::cl::Optional,
                       "c",
                       llvm::cl::desc(
                               "validates sat model using z3 if applicable"),
                       llvm::cl::value_desc("filename"),
                       llvm::cl::cat(SolverCategory));

static llvm::cl::opt<bool>
        int_type(llvm::cl::Optional,
                       "intType",
                       llvm::cl::desc(
                               "int type module"),
                       llvm::cl::value_desc("filename"),
                       llvm::cl::cat(SolverCategory));

static llvm::cl::opt<goSATMode>
        opt_tool_mode("mode", llvm::cl::Optional,
                      llvm::cl::desc("Tool operation mode:"),
                      llvm::cl::cat(SolverCategory),
                      llvm::cl::values(clEnumValN(kNativeSolving,
                                                  "go",
                                                  "SMT solving for FP (default) "),
                                       clEnumValN(kFormulaAnalysis,
                                                  "fa",
                                                  "formula analysis"),
                                       clEnumValN(kCCodeGeneration,
                                                  "cg",
                                                  "C code generation")));

static llvm::cl::opt<gosat::LibAPIGenMode>
        opt_api_dump_mode("fmt", llvm::cl::Optional,
                          llvm::cl::desc(
                                  "API dump format in code generation mode:"),
                          llvm::cl::cat(SolverCategory),
                          llvm::cl::values(clEnumValN(gosat::kPlainAPI,
                                                      "plain",
                                                      "CSV dump of API (default)"),
                                           clEnumValN(gosat::kCppAPI,
                                                      "cpp",
                                                      "C++ dump of API")));

static llvm::cl::opt<goSATAlgorithm>
        opt_go_algorithm("alg", llvm::cl::Optional,
                         llvm::cl::desc("Applied GO algorithm:"),
                         llvm::cl::cat(SolverCategory),
                         llvm::cl::values(clEnumValN(kDirect,
                                                     "direct",
                                                     "Direct algorithm"),
                                          clEnumValN(kCRS2,
                                                     "crs2",
                                                     "CRS2 algorithm (default)"),
                                          clEnumValN(kISRES,
                                                     "isres",
                                                     "ISRES algorithm"),
                                          clEnumValN(kMLSL,
                                                     "mlsl",
                                                     "MLSL algorithm"),
                                          clEnumValN(kMOEA,
                                                     "moea",
                                                     "MOEA algorithm"),
                                          clEnumValN(kMOCEA,
                                                     "mocea",
                                                     "MOCEA algorithm"),
                                          clEnumValN(kMOCEASP2,
                                                     "moceasp2",
                                                     "MOCEA-SP2 Sampling algorithm"),
                                          clEnumValN(kMOCEASP3,
                                                     "moceasp3",
                                                     "MOCEA-SP3 Sampling algorithm"),
                                          clEnumValN(kMOCEADS2,
                                                     "moceads2",
                                                     "MOCEA-DS2 Sampling algorithm"),
                                          clEnumValN(kMOCEADS3,
                                                     "moceads3",
                                                     "MOCEA-DS3 Sampling algorithm"),
                                          clEnumValN(kMOEADDS,
                                                     "moeadds",
                                                     "MOEAD_DS Sampling algorithm"),
                                          clEnumValN(kSOEACov,
                                                     "soeacov",
                                                     "SOEACov algorithm"),
                                          clEnumValN(kSOEADis,
                                                     "soeadis",
                                                     "SOEADis algorithm"),
                                          clEnumValN(kNSGA2,
                                                     "nsga2",
                                                     "NSGA2 algorithm")));

static llvm::cl::opt<bool> smtlib_compliant_output(
    "smtlib-output", llvm::cl::cat(SolverCategory),
    llvm::cl::desc("Make output SMT-LIBv2 compliant (default false)"),
    llvm::cl::init(false));

void versionPrinter()
{
    std::cout << "goSAT v0.1 \n"
              << "Copyright (c) 2017 University of Kaiserslautern\n";
}

inline float elapsedTimeFrom(std::chrono::steady_clock::time_point& st_time)
{
    const auto res = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::steady_clock::now() - st_time).count();
    return static_cast<float>(res) / 1000;
}

bool isFileExist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

int main(int argc, const char** argv)
{
    //llvm::cl::SetVersionPrinter(versionPrinter);
    llvm::cl::HideUnrelatedOptions(SolverCategory);
    llvm::cl::ParseCommandLineOptions
            (argc, argv,
             "goSAT v0.1 Copyright (c) 2017 University of Kaiserslautern\n");

    if (!isFileExist(opt_input_file.c_str())) {
        std::cerr << "Input file does not exists!" << std::endl;
        std::exit(1);
    }
    try {

        z3::context smt_ctx;
        Z3_ast smt_ast = Z3_parse_smtlib2_file(smt_ctx, opt_input_file.c_str(), 0, 0, 0, 0, 0, 0);
//        z3::expr smt_expr = smt_ctx.parse_file(opt_input_file.c_str());
        z3::expr smt_expr(smt_ctx, smt_ast);

        if (opt_tool_mode == kFormulaAnalysis) {
            gosat::FPExprAnalyzer analyzer;
            analyzer.analyze(smt_expr);
            analyzer.prettyPrintSummary(
                    gosat::FPExprCodeGenerator::getFuncNameFrom(
                            opt_input_file));
            return 0;
        }
        if (opt_tool_mode == kCCodeGeneration) {
            using namespace gosat;
            FPExprCodeGenerator code_generator;
            std::string func_name =
                    FPExprCodeGenerator::getFuncNameFrom(opt_input_file);
            code_generator.genFuncCode(func_name, smt_expr);
            if (opt_api_dump_mode != kUnsetAPI) {
                FPExprLibGenerator lib_generator;
                // TODO: implement error handling for output files
                lib_generator.init(opt_api_dump_mode);
                auto func_sig =
                        FPExprCodeGenerator::genFuncSignature(func_name);
                lib_generator.appendFunction
                        (code_generator.getVarCount(),
                         func_name,
                         func_sig,
                         code_generator.getFuncCode());
                std::cout << func_name << ". code generated successfully!"
                          << "\n";
            } else {
                std::cout << code_generator.getFuncCode() << "\n";
            }
            return 0;
        }
        std::chrono::steady_clock::time_point
                time_start = std::chrono::steady_clock::now();
        using namespace llvm;
        std::string func_name = gosat::FPExprCodeGenerator::getFuncNameFrom(
                opt_input_file);

        // JIT formula to an objective function
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        atexit(llvm_shutdown);
        atexit(Z3_finalize_memory);

        LLVMContext context;
        std::unique_ptr<Module> module = std::make_unique<Module>(
                StringRef(func_name), context);

        //浮点ir生成器
        gosat::FPIRGenerator ir_gen(&context, module.get());
        std::vector<double> init_number;
        std::vector<int> init_number_int;
        auto ll_func_ptr = ir_gen.genFunction(smt_expr, init_number);

        std::string err_str;
        std::unique_ptr<ExecutionEngine> exec_engine(
                EngineBuilder(std::move(module))
                        .setEngineKind(EngineKind::JIT)
                        .setOptLevel(CodeGenOpt::Less)
                        .setErrorStr(&err_str)
                        .create());
        if (exec_engine == nullptr) {
            std::cerr << func_name << ": Failed to construct ExecutionEngine: "
                      << err_str
                      << "\n";
            return 1;
        }
        ir_gen.addGlobalFunctionMappings(exec_engine.get());
        exec_engine->finalizeObject();
        auto func_ptr = reinterpret_cast<nlopt_func>(exec_engine->
                getPointerToFunction(ll_func_ptr));

        // Now working with optimization backend
        goSATAlgorithm current_alg = (opt_go_algorithm == kUndefinedAlg) ? kMOCEADS3 : opt_go_algorithm;
    //        goSATAlgorithm::kBYTEEA;
    //        goSATAlgorithm current_alg = kDirect; kCRS2; kISRES; kMLSL; kESCH; kBYTEEA; kGA;

        gosat::NLoptOptimizer nl_opt(static_cast<nlopt_algorithm>(current_alg));
    ////        gosat::NLoptOptimizer nl_opt;
    //        nl_opt.Config.MaxEvalCount = 5000000;
    //        nl_opt.Config.RelTolerance = 1e-10;

        auto start = std::chrono::high_resolution_clock::now();
        int status = 0;
        double minima = 1024; /* minimum getValue */
//        std::vector<double> model_vec(ir_gen.getVarCount(), 0);
        double *model_vec = new double[ir_gen.getVarCount()];
        double **model = new double*[model_maxSize];
        for(int i=0; i<model_maxSize; i++){
            model[i] = new double[ir_gen.getVarCount()];
        }
        int model_size[1] = {0};

        if (ir_gen.getVarCount() == 0) {
            // const function
            double cov=1024;
            double totalCov = 0;
    //            double func_val = func(dim, x, &cov, &totalCov);
            minima = (func_ptr)(0, nullptr, &cov, &totalCov);
        } else {
            if(smart_seed){
                for(int i=0; i<init_number.size()&&i<ir_gen.getVarCount(); i++){
    //                  printf("init_number>>%e\n",init_number[i]);
                    model_vec[i] = init_number[i];
//                    model_vec[i] = 1.555287099108682e-06;
                }
                double *seed = new double[init_number.size()];
                for(int i=0; i < init_number.size(); i++){
                    seed[i] = init_number[i];
                }

                status = nl_opt.optimize(func_ptr,
                                         static_cast<unsigned>(ir_gen.getVarCount()),
                                         model_vec, // size = n_var; found all init + with 0 fill
                                         seed,
                                         init_number.size(),
                                         &minima,
                                         max_time,
                                         pop_size,
                                         model,
                                         model_size,
                                         model_maxSize);
                delete[] seed;
            }
            else{
                status = nl_opt.optimize(func_ptr,
                                         static_cast<unsigned>(ir_gen.getVarCount()),
                                         model_vec, // size = n_var; found all init + with 0 fill
                                         NULL,
                                         0,
                                         &minima,
                                         max_time,
                                         pop_size,
                                         model,
                                         model_size,
                                         model_maxSize);
            }
    //          auto end = std::chrono::high_resolution_clock::now();
    //          std::chrono::duration<double> duration = end - start;
    //          double milliseconds = duration.count() * 1000.0;
    //          std::cout << ">>> exec time: " << milliseconds << " ms\n";
        }

        if (ir_gen.isFoundUnsupportedSMTExpr()) {
            std::cout<< "unsupported\n";
            assert(false && "unsupport expr !");
        }
    //        std::string result = (minima == 0 && !ir_gen.isFoundUnsupportedSMTExpr())
    //                             ? "sat" : "unknown";
        std::string result = ((minima == 0||status==2) && !ir_gen.isFoundUnsupportedSMTExpr())
                             ? "sat" : "unknown";
        if (smtlib_compliant_output) {
            std::cout << result << std::endl;
        }
        else {
            if (status < 0) {
                std::cout << std::setprecision(4);
                std::cout << func_name << "," << result << ","
                          << elapsedTimeFrom(time_start)
                          << ",INF," << status;
            } else {
                std::cout << std::setprecision(4);
                std::cout << func_name << "," << result << ",";
                std::cout << elapsedTimeFrom(time_start) << ",";
                std::cout << std::setprecision(dbl::digits10) << minima << ","
                          << status;
            }

            auto end = std::chrono::high_resolution_clock::now();
            llvm::outs() << "Solving time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
            llvm::outs() << "\n=============Output Info=============\n";
            printf("\nsampling count: %d\n", *model_size);
            if(minima==0){
                for(int j=0; j<*model_size; j++){
                    for(int i=0; i<ir_gen.getVarCount(); i++){
                        printf("val %d: %e ", i, model[j][i]);
                    }
                    printf("\n");
                }
            }
        }

        delete[] model;
        delete[] model_vec;
//        delete[] decls;
//        delete[] sort;
//        delete[] kind;

    } catch (const z3::exception &exp) {
        std::cerr << "Error occurred while processing your input: "
                  << exp.msg() << std::endl;
        std::exit(2);
    }
    return 0;
}
