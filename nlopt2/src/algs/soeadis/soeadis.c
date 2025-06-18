#include <stdio.h>/* printf */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ga.h"
#include <float.h>

/****************************************************************************/

typedef enum {
    Mutate_SigPointCover = 0,
    Mutate_UniCover,
    Mutate_Shift,
    Mutate_Inversion,
    Mutate_Swap,
    Mutate_ChangeVar,
    Mutate_SBX,
    Mutate_PM,
    Mutate_ChangeByte,
    Mutate_SpecailNum,
    Mutate_ExponentByte,
    Mutate_FractionByte
};

/* main Individual representation type */
typedef struct IndividualStructure {
    int id;
    double * parameters;
    double disObj;
    double covObj;
    double crowd_dt;
} Individual;

static int CompDis(void *unused, const void *a_, const void *b_) {
    const Individual *a = (const Individual *) a_;
    const Individual *b = (const Individual *) b_;
    (void) unused;
    return a->disObj < b->disObj ? -1 : (a->disObj > b->disObj ? +1 : 0);
}

static int CompCov(void *unused, const void *a_, const void *b_) {
    const Individual *a = (const Individual *) a_;
    const Individual *b = (const Individual *) b_;
    (void) unused;
    return a->covObj < b->covObj ? -1 : (a->covObj > b->covObj ? +1 : 0);
}

static int CompCrowding(void *unused, const void *a_, const void *b_) {
    const Individual *a = (const Individual *) a_;
    const Individual *b = (const Individual *) b_;
    (void) unused;
    return a->crowd_dt > b->crowd_dt ? -1 : (a->crowd_dt < b->crowd_dt ? +1 : 0);
}

static void IndividualDeepCopy(Individual *from, Individual *to, unsigned nparameters){
//    to.parameters = (double*) malloc(sizeof(double) * nparameters);
    for (int i=0; i<nparameters; i++){
        to->parameters[i] = from->parameters[i];
    }
//    to->id = from->id;
    to->disObj = from->disObj;
    to->covObj = from->covObj;
    to->crowd_dt = from->crowd_dt;
}

static void Mutate_SigPointCover_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters){
    int rnd1 = nlopt_iurand(np);
    int rnd2 = nlopt_iurand(np);
    while(rnd1==rnd2){
        rnd2 = nlopt_iurand(np);
    }
//    esoffsprings[id] = esparents[rnd1];
//    esoffsprings[id+1] = esparents[rnd2];
    IndividualDeepCopy(&esparents[rnd1], &esoffsprings[id], nparameters);
    IndividualDeepCopy(&esparents[rnd2], &esoffsprings[id+1], nparameters);
    int mutate_p = nlopt_iurand(nparameters);
    int i;
    for(i=0; i<mutate_p; i++){
        esoffsprings[id].parameters[i] = esparents[rnd2].parameters[i];
        esoffsprings[id+1].parameters[i] = esparents[rnd1].parameters[i];
    }
//    for(i=mutate_p; i<nparameters; i++){
//        esoffsprings[id].parameters[i] = esparents[rnd2].parameters[i];
//        esoffsprings[id+1].parameters[i] = esparents[rnd1].parameters[i];
//    }
}

static void Mutate_UniCover_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters){
    int rnd1 = nlopt_iurand(np);
    int rnd2 = nlopt_iurand(np);
    while(rnd1==rnd2){
        rnd2 = nlopt_iurand(np);
    }
//    esoffsprings[id] = esparents[rnd1];
//    esoffsprings[id+1] = esparents[rnd2];
    IndividualDeepCopy(&esparents[rnd1], &esoffsprings[id], nparameters);
    IndividualDeepCopy(&esparents[rnd2], &esoffsprings[id+1], nparameters);
    int mutate_p = nlopt_iurand(nparameters);
    int size = nlopt_iurand(nparameters);
    int i;
    for(i=0; i<size; i++){
        int ii = (i+mutate_p) % nparameters;
        esoffsprings[id].parameters[ii] = esparents[rnd2].parameters[ii];
        esoffsprings[id+1].parameters[ii] = esparents[rnd1].parameters[ii];
    }
}

static void Mutate_Shift_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters){
    int rndP = nlopt_iurand(np);
    int step = nlopt_iurand(nparameters);
    int i;
    for(i=0; i<nparameters; i++){
        esoffsprings[id].parameters[i] = esparents[rndP].parameters[(i+step)%nparameters];
    }
}

static void Mutate_Inversion_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters){
    int rndP = nlopt_iurand(np);
    int i;
    for(i=0; i<nparameters; i++){
        esoffsprings[id].parameters[i] = esparents[rndP].parameters[nparameters-i-1];
    }
}

static void Mutate_Swap_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters){
    int rndP = nlopt_iurand(np);
    int rnd1 = nlopt_iurand(np);
    int rnd2 = nlopt_iurand(np);
    while(rnd1==rnd2){
        rnd2 = nlopt_iurand(np);
    }
//    esoffsprings[id] = esparents[rndP];
    IndividualDeepCopy(&esparents[rndP], &esoffsprings[id], nparameters);
    int step = nlopt_iurand(nparameters);
    int i;
    for(i=0; i<step; i++){
        esoffsprings[id].parameters[(rnd1+i)%nparameters] = esparents[rndP].parameters[(rnd2+i)%nparameters];
    }
}

static void Mutate_SpecailNum_Fun(Individual *esparents, Individual *esoffsprings,
                              int id, int np, unsigned nparameters, double *seed, int seed_size){

    int limit_size = seed_size+12;
    double *specailNum = malloc(sizeof(double ) *(limit_size));
    for(int i=0; i<seed_size; i++){
        specailNum[i] = seed[i];
    }
    specailNum[seed_size] = DBL_MAX;
    specailNum[seed_size+1] = DBL_MIN;
    specailNum[seed_size+2] = FLT_MAX;
    specailNum[seed_size+3] = FLT_MIN;
    specailNum[seed_size+4] = -DBL_MAX;
    specailNum[seed_size+5] = -DBL_MIN;
    specailNum[seed_size+6] = -FLT_MAX;
    specailNum[seed_size+7] = -FLT_MIN;
    specailNum[seed_size+8] = -0;
    specailNum[seed_size+9] = 0;
    specailNum[seed_size+10] = -1;
    specailNum[seed_size+11] = 1;

    int rndP = nlopt_iurand(np);
//    esoffsprings[id] = esparents[rndP];
    IndividualDeepCopy(&esparents[rndP], &esoffsprings[id], nparameters);
    int rnd = nlopt_iurand(nparameters);
    int rnd_seed = nlopt_iurand(10+seed_size);
    esoffsprings[id].parameters[rnd] = specailNum[rnd_seed];

    free(specailNum);
}

static void Mutate_ChangeVar_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters,
                             double *lb, double *ub){
    int rndP = nlopt_iurand(np);
//    esoffsprings[id] = esparents[rndP];
    IndividualDeepCopy(&esparents[rndP], &esoffsprings[id], nparameters);
    int rnd = nlopt_iurand(nparameters);//mutate var
//    esoffsprings[id].parameters[rnd] = nlopt_urand(lb[rnd], ub[rnd]);
    esoffsprings[id].parameters[rnd] = nlopt_urand_full_range();
}

static void Mutate_ChangeByte_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters){
    int rndP = nlopt_iurand(np);
//    esoffsprings[id] = esparents[rndP];
    IndividualDeepCopy(&esparents[rndP], &esoffsprings[id], nparameters);
    int rnd = nlopt_iurand(nparameters);//mutate var
    double curV = esoffsprings[id].parameters[rnd];
    u_int8_t bytes[8];
    memcpy(bytes, &curV, sizeof(double ));
    for(int i=0; i<8; i++){
        if(nlopt_urand(0,1)<0.5)
            bytes[i] = nlopt_iurand(256);
    }
    double newV;
    memcpy(&newV, bytes, sizeof(double ));
    esoffsprings[id].parameters[rnd] = newV;
}


static void Mutate_SBX_Fun(unsigned id, Individual *esparents, Individual *esoffsprings,
                           unsigned np, unsigned nparameters, const double *lb, const double *ub){
    double disC = 20; //distribution index crossover
    int item;
    int parent1  = nlopt_iurand((int) np);
    int parent2  = nlopt_iurand((int) np);
    // every Individual has nparameters parameters
    for(item=0; item<nparameters; item++){
        double mu = nlopt_urand(0,1); //[0,1)
        double beta = pow(2*mu,1/(disC+1));
        if(mu>0.5)
            beta = pow(1/(2*(1-mu)),1/(disC+1));
        Individual p1 = esparents[parent1];
        Individual p2 = esparents[parent2];
        //offpring = (p1+p2)/2 + beta*(p1-p2)/2
        esoffsprings[id].parameters[item] = (p1.parameters[item]+p2.parameters[item])/2
                                            + beta*(p1.parameters[item]-p2.parameters[item])/2;
        // check the boundary
        if (esoffsprings[id].parameters[item] < lb[item]) {
            esoffsprings[id].parameters[item] = lb[item];
        }
        if (esoffsprings[id].parameters[item] > ub[item]) {
            esoffsprings[id].parameters[item] = ub[item];
        }
    }
}

static void Mutate_PM_Fun(unsigned id, Individual *esparents, Individual *esoffsprings,
                          unsigned np, unsigned nparameters, const double *lb, const double *ub){
    double disM = 20; //distribution index mutation
    int rndP = nlopt_iurand(np);
//    esoffsprings[id] = esparents[rndP];
    IndividualDeepCopy(&esparents[rndP], &esoffsprings[id], nparameters);
    int item;
    for(item=0; item<nparameters; item++) {
        double mu = nlopt_urand(0, 1); //[0,1)
        if (mu <= 0.5) {
            double delta1 = (esoffsprings[id].parameters[item] - lb[item]) /
                            (ub[item] - lb[item]); // delta1 = (x-lb)/(ub-lb)
            double delta = pow(2 * mu + (1 - 2 * mu) * pow((1 - delta1), disM + 1), 1 / (disM + 1)) - 1; // delta = (2*mu+(1-2*mu)*(1-delta1)^(disM+1))^(1/(disM+1))-1
            esoffsprings[id].parameters[item] =
                    esoffsprings[id].parameters[item] + delta * (ub[item] - lb[item]); // x = x + delta*(ub-lb)
            // check the boundary
            if (esoffsprings[id].parameters[item] < lb[item]) {
                esoffsprings[id].parameters[item] = lb[item];
            }
            if (esoffsprings[id].parameters[item] > ub[item]) {
                esoffsprings[id].parameters[item] = ub[item];
            }
        } else {
            double delta2 = (ub[item] - esoffsprings[id].parameters[item]) /
                            (ub[item] - lb[item]); // delta2 = (ub-x)/(ub-lb)
            double delta = 1 - pow(2 * (1 - mu) + 2 * (mu - 0.5) * pow((1 - delta2), disM + 1), 1 / (disM + 1)); // delta = 1-(2*(1-mu)+2*(mu-0.5)*(1-delta2)^(disM+1))^(1/(disM+1))
            esoffsprings[id].parameters[item] =
                    esoffsprings[id].parameters[item] + delta * (ub[item] - lb[item]); // x = x + delta*(ub-lb)
            //check the boundary
            if (esoffsprings[id].parameters[item] < lb[item]) {
                esoffsprings[id].parameters[item] = lb[item];
            }
            if (esoffsprings[id].parameters[item] > ub[item]) {
                esoffsprings[id].parameters[item] = ub[item];
            }
        }
    }
}

static void Mutate_Exponent_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters){
    int rndP = nlopt_iurand(np);
//    esoffsprings[id] = esparents[rndP];
    IndividualDeepCopy(&esparents[rndP], &esoffsprings[id], nparameters);
    int rnd = nlopt_iurand(nparameters);//mutate var
    double curV = esoffsprings[id].parameters[rnd];
    u_int8_t bytes[8];
    memcpy(bytes, &curV, sizeof(double ));
    if(nlopt_urand(0,1)<0.3)
        bytes[0] = nlopt_iurand(256);
    else if(nlopt_urand(0,1)>0.7)
        bytes[1] = nlopt_iurand(256);
    else
        for(int i=0; i<2; i++){//exponent byte
            if(nlopt_urand(0,1)<0.5)
                bytes[i] = nlopt_iurand(256);
        }
    double newV;
    memcpy(&newV, bytes, sizeof(double ));
    esoffsprings[id].parameters[rnd] = newV;
}

static void Mutate_Fraction_Fun(Individual *esparents, Individual *esoffsprings, int id, int np, unsigned nparameters){
    int rndP = nlopt_iurand(np);
//    esoffsprings[id] = esparents[rndP];
    IndividualDeepCopy(&esparents[rndP], &esoffsprings[id], nparameters);
    int rnd = nlopt_iurand(nparameters);//mutate var
    double curV = esoffsprings[id].parameters[rnd];
    u_int8_t bytes[8];
    memcpy(bytes, &curV, sizeof(double ));
    for(int i=2; i<8; i++){//fraction byte
        if(nlopt_urand(0,1)<0.5)
            bytes[i] = nlopt_iurand(256);
    }
    double newV;
    memcpy(&newV, bytes, sizeof(double ));
    esoffsprings[id].parameters[rnd] = newV;
}

//// 判断个体之间的支配关系
//static int jude_dominating(Individual p, Individual q) {
//    return ((p.disObj<q.disObj && p.covObj<=q.covObj) || (p.disObj<=q.disObj && q.covObj < q.covObj));
//}
//
//static int non_dominated_sorting(Individual *population, int population_size, int **fronts, int *fronts_cnt) {
//    int **dominates = (int **)malloc(population_size * sizeof(int *)); // dominate individual set
//    int *dominates_cnt = (int *)malloc(population_size * sizeof(int)); // dominate individual count
//    int **dominateds = (int **)malloc(population_size * sizeof(int *)); // individual dominated set
//    int *dominateds_cnt = (int *)malloc(population_size * sizeof(int)); // individual dominated count
//
//    for (int i = 0; i < population_size; i++) {
//        fronts_cnt[i] = 0;
//        dominates_cnt[i] = 0;
//        dominateds_cnt[i] = 0;
////        fronts[i] = (int *)malloc(population_size * sizeof(int));
//        dominates[i] = (int *)malloc(population_size * sizeof(int));
//        dominateds[i] = (int *)malloc(population_size * sizeof(int));
//    }
//
//    // 计算每个个体的支配关系
//    for (int i = 0; i < population_size; i++) {
////        dominates_cnt[i] = 0;
////        dominateds_cnt[i] = 0;
////        dominates[i] = (int *)malloc(population_size * sizeof(int));
//        for (int j = 0; j < population_size; j++) {
//            if (i == j) continue;
//            if (jude_dominating(population[i], population[j])) {
//                dominates[i][dominates_cnt[i]++] = j; // 记录i支配的个体j
//            } else if (jude_dominating(population[j], population[i])) {
//                dominateds_cnt[i]++; // 统计支配i的个体数
//            }
//        }
//    }
//
//    // 找出第一层非支配层
////    fronts[0] = (int *)malloc(population_size * sizeof(int)); // 记录第一层非支配层
////    fronts_cnt[0] = 0; // 第一层非支配层个数
//    for (int i = 0; i < population_size; i++) {
//        if (dominateds_cnt[i] == 0) {
//            fronts[0][fronts_cnt[0]++] = i; // 添加到第一层非支配层
//        }
//    }
//
//    // 逐层扩展非支配层
//    int front_index = 0;
//    while (fronts_cnt[front_index] > 0) {
////        fronts[front_index] = (int *)malloc(population_size * sizeof(int));
//        for (int i = 0; i < fronts_cnt[front_index]; i++) { // loop front individual
//            int p_idx = fronts[front_index][i];
//            for (int j = 0; j < dominates_cnt[p_idx]; j++) { // loop p_idx dominate set
//                int q_idx = dominates[p_idx][j];
//                dominateds_cnt[q_idx]--;
//                if (dominateds_cnt[q_idx] == 0) {
//                    fronts[front_index+1][fronts_cnt[front_index+1]++] = q_idx; // 添加到下一层非支配层
//                }
//            }
//        }
//
//        front_index++;
//    }
//
////    for (int idx=0; idx<front_index; idx++){
////        printf("front %d:\n", idx);
////        for (int i = 0; i < fronts_cnt[idx]; i++) {
////            int id = fronts[idx][i];
////            printf("(%d) Individual %d: %lf %lf\n", id, population[id].id, population[id].disObj, population[id].covObj);
////        }
////    }
//
//    // 释放内存
//    for (int i = 0; i < population_size; i++) free(dominates[i]);
//    for (int i = 0; i < population_size; i++) free(dominateds[i]);
//
//    if(dominates) free(dominates);
//    if(dominateds) free(dominateds);
//    if(dominates_cnt) free(dominates_cnt);
//    if(dominateds_cnt) free(dominateds_cnt);
//
//    return front_index;
//}

//static void crowding_distance_assignment(Individual *population, int population_size) {
//    double f_max1 = population[0].disObj;
//    double f_min1 = population[0].disObj;
//    double f_max2 = population[0].covObj;
//    double f_min2 = population[0].covObj;
//
//    // 找出每个目标函数的最大值和最小值
//    for (int i = 0; i < population_size; i++) {
//        if (population[i].disObj > f_max1) {
//            f_max1 = population[i].disObj;
//        }
//        if (population[i].disObj < f_min1) {
//            f_min1 = population[i].disObj;
//        }
//        if (population[i].covObj > f_max1) {
//            f_max1 = population[i].covObj;
//        }
//        if (population[i].covObj < f_min1) {
//            f_min1 = population[i].covObj;
//        }
//    }
//
//    nlopt_qsort_r(population, population_size, sizeof(Individual), NULL,CompDis);
//    population[0].crowd_dt = population[population_size - 1].crowd_dt = DBL_MAX;
//    for (int j = 1; j < population_size - 1; j++) {
//        population[j].crowd_dt = fabs(population[j + 1].disObj - population[j - 1].disObj) / (f_max1 - f_min1 + DBL_MIN);
//    }
//
//    nlopt_qsort_r(population, population_size, sizeof(Individual), NULL,CompCov);
//    population[0].crowd_dt = population[population_size - 1].crowd_dt = DBL_MAX;
//    for (int j = 1; j < population_size - 1; j++) {
//        population[j].crowd_dt += fabs(population[j + 1].covObj - population[j - 1].covObj) / (f_max2 - f_min2 + DBL_MIN);
//    }
//
//}

static void printPopInfo(Individual *population, unsigned population_size, unsigned nparameters){
    printf("Population Info!>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    for(int id=0; id<population_size; id++){
        printf("individual %d: %f %f %f\n",
               population[id].id, population[id].disObj, population[id].covObj, population[id].crowd_dt);
        printf("X=[");
        for(int j=0; j<nparameters; j++){
            printf("%e ",population[id].parameters[j]);
        }
        printf("]\n");
    }
}

nlopt_result SOEADisMain(
        unsigned nparameters, /* Number of input parameters */
        nlopt_func f,	/* Recursive Objective Funtion Call */
        void * data_f,	/* Data to Objective Function */
        const double* lb,			/* Lower bound values */
        const double* ub,			/* Upper bound values */
        double* x,				/*in: initial guess, out: minimizer */
        double* minf,
        nlopt_stopping* stop, 		/* nlopt stop condition */
        unsigned np, 			/* Population Size */
        double *seed,
        int seed_size) {

    double Cov = 1024;
    double totalCov = 0;
    /* variables from nlopt */
    nlopt_result ret = NLOPT_SUCCESS;//init ret

    /*********************************
     * controling the population size
     *********************************/
//    printf("controling the population size\n");
    if (np<1) np = 100; //population size
    Individual *esparents    = (Individual*) malloc(sizeof(Individual) * np);
    Individual *esoffsprings = (Individual*) malloc(sizeof(Individual) * np);
    Individual *estotal 	 = (Individual*) malloc(sizeof(Individual) * (np+np));
    if ((!esparents)||(!esoffsprings)||(!estotal)) {
        free(esparents); free(esoffsprings); free(estotal);
        return NLOPT_OUT_OF_MEMORY;
    }
    for (int id=0; id < np; id++) {
        esparents[id].parameters = (double*) malloc(sizeof(double) * nparameters);
        if (!esparents[id].parameters) {
            ret = NLOPT_OUT_OF_MEMORY;
            goto done;
        }
        esparents[id].id = id;
    }
    for (int id=0; id < np; id++) {
        esoffsprings[id].parameters = (double*) malloc(sizeof(double) * nparameters);
        if (!esoffsprings[id].parameters) {
            ret = NLOPT_OUT_OF_MEMORY;
            goto done;
        }
        esoffsprings[id].id = id;
    }
    for (int id=0; id < np+np; id++) {
        estotal[id].parameters = (double*) malloc(sizeof(double) * nparameters);
        if (!estotal[id].parameters) {
            ret = NLOPT_OUT_OF_MEMORY;
            goto done;
        }
        estotal[id].id = id;
    }

//    int **fronts = (int **)malloc((np+np) * sizeof(int *)); //individual index in the fronts
//    for(int i=0; i<np+np; i++){
//        fronts[i] = (int *) malloc((np+np) * sizeof(int));
//        if(!fronts[i]){
//            ret = NLOPT_OUT_OF_MEMORY;
//            goto done;
//        }
//    }
//
//    int *fronts_cnt = (int *) malloc((np+np) * sizeof(int *)); //each front count
//    if ((!fronts)||(!fronts_cnt)) {
//        free(fronts);
//        free(fronts_cnt);
//        return NLOPT_OUT_OF_MEMORY;
//    }

    /**************************************
     * Initializing parents population
     **************************************/
//    printf("Initializing parents population\n");
    for (int id=0; id < np; id++) {
        if(id<seed_size){
            for(int item=0; item<nparameters; item++)
                esparents[id].parameters[item] = seed[id];
        } else{
            for (int item=0; item<nparameters; item++) {
//                double t = nlopt_urand(lb[item], ub[item]);
                double t = nlopt_urand_full_range();
                esparents[id].parameters[item] = t;//uniform distribution
            }
        }
    }
    memcpy(esparents[np-1].parameters, x, nparameters * sizeof(double));

    /**************************************
     * Parents fitness evaluation
     **************************************/
//    printf("Parents fitness evaluation\n");
    for (int id=0; id < np; id++) {
        esparents[id].disObj = f(nparameters, esparents[id].parameters, &Cov, &totalCov);
        esparents[id].covObj = totalCov - Cov;
        estotal[id].disObj = esparents[id].disObj;
        estotal[id].covObj = esparents[id].covObj;
        ++ *(stop->nevals_p);
//        double fitness = esparents[id].disObj + esparents[id].covObj;
        double fitness = esparents[id].disObj;
        if (*minf > fitness) {
            *minf = fitness;
            memcpy(x, esparents[id].parameters,nparameters * sizeof(double));
//            printf("Update best Individual! (%d)\n", *(stop->nevals_p));
//            printf("individual %d: %f %f %f\n",
//                   esparents[id].id, esparents[id].disObj, esparents[id].covObj, esparents[id].crowd_dt);
//            printf("X=[");
//            for(int j=0; j<nparameters; j++){
//                printf("%e ",esparents[id].parameters[j]);
//            }
//            printf("]\n");
        }
        if (nlopt_stop_forced(stop)) ret = NLOPT_FORCED_STOP;
        else if (*minf <= stop->minf_max) ret = NLOPT_MINF_MAX_REACHED;//modify by yx
        else if (nlopt_stop_evals(stop)) ret = NLOPT_MAXEVAL_REACHED;
        else if (nlopt_stop_time(stop)) ret = NLOPT_MAXTIME_REACHED;
        if (ret != NLOPT_SUCCESS) goto done;
    }
    /**************************************
     * Main Loop - Generations
     **************************************/
    while (1) {
//        printf("evoluation %d\n", *(stop->nevals_p));
//        printPopInfo(esparents,np,nparameters);
        /**************************************
         * Offspring Generate
         **************************************/
        for (int id=0; id < np;){
             // select parent
            int rint_mutate = nlopt_iurand(10); // select mutation strategy
            switch (rint_mutate) {
                case Mutate_SigPointCover:
//                    printf("Mutate_SigPointCover!\n");
                    if(id+2>=np) break;
                    Mutate_SigPointCover_Fun(esparents, esoffsprings, id, np, nparameters);
                    id+=2;
                    break;
                case Mutate_UniCover:
//                    printf("Mutate_UniCover!\n");
                    if(id+2>=np) break;
                    Mutate_UniCover_Fun(esparents, esoffsprings, id, np, nparameters);
                    id+=2;
                    break;
                case Mutate_Shift:
//                    printf("Mutate_Shift!\n");
                    Mutate_Shift_Fun(esparents, esoffsprings, id, np, nparameters);
                    id++;
                    break;
                case Mutate_Inversion:
//                    printf("Mutate_Inversion!\n");
                    Mutate_Inversion_Fun(esparents, esoffsprings, id, np, nparameters);
                    id++;
                    break;
//                case Mutate_Swap:
////                    printf("Mutate_Swap!\n");
//                    Mutate_Swap_Fun(esparents, esoffsprings, id, np, nparameters);
//                    id++;
//                    break;
                case Mutate_ChangeByte:
//                    printf("Mutate_ChangeByte!\n");
                    Mutate_ChangeByte_Fun(esparents, esoffsprings, id, np, nparameters);
//                    Mutate_ChangeVar_Fun(esparents, esoffsprings, id, np, nparameters, lb, ub);
                    id++;
                    break;
                case Mutate_ChangeVar:
//                    printf("Mutate_ChangeVar!\n");
                    Mutate_ChangeVar_Fun(esparents, esoffsprings, id, np, nparameters, lb, ub);
                    id++;
                    break;
                case Mutate_SpecailNum:
//                    printf("Mutate_SpecailNum!\n");
                    Mutate_SpecailNum_Fun(esparents, esoffsprings, id, np, nparameters, seed, seed_size);
                    id++;
                    break;
                case Mutate_SBX:
//                    printf("Mutate_SBX!\n");
                    Mutate_SBX_Fun(id, esparents, esoffsprings, np, nparameters, lb, ub);
                    id++;
                    break;
                case Mutate_PM:
//                    printf("Mutate_PM!\n");
                    Mutate_PM_Fun(id, esparents, esoffsprings, np, nparameters, lb, ub);
                    id++;
                    break;
                case Mutate_ExponentByte:
//                    printf("Mutate_ChangeByte!\n");
                    Mutate_Exponent_Fun(esparents, esoffsprings, id, np, nparameters);
//                    Mutate_ChangeVar_Fun(esparents, esoffsprings, id, np, nparameters, lb, ub);
                    id++;
                    break;
//                case Mutate_FractionByte:
////                    printf("Mutate_ChangeByte!\n");
//                    Mutate_Fraction_Fun(esparents, esoffsprings, id, np, nparameters);
////                    Mutate_ChangeVar_Fun(esparents, esoffsprings, id, np, nparameters, lb, ub);
//                    id++;
//                    break;
                default:
                    break;
            }
        }
//        printPopInfo(esparents,np,nparameters);
//        printPopInfo(esoffsprings,np,nparameters);
        /**************************************
         * Offsprings fitness evaluation
         **************************************/
//         printf("Offsprings fitness evaluation\n");
        for (int id=0; id < np; id++){
            esoffsprings[id].disObj = f(nparameters, esoffsprings[id].parameters, &Cov, &totalCov);
            esoffsprings[id].covObj = totalCov - Cov;
            estotal[id+np].disObj = esoffsprings[id].disObj;
            estotal[id+np].covObj = esoffsprings[id].covObj;
            ++ *(stop->nevals_p);
//            double fitness = esoffsprings[id].disObj+esoffsprings[id].covObj;
            double fitness = esoffsprings[id].disObj;
            if (*minf > fitness) {
                *minf = fitness;
//            *(minf+1) = grad; // add by yx
                memcpy(x, esoffsprings[id].parameters,nparameters * sizeof(double));
//                printf("Update best Individual (%d)!\n", *(stop->nevals_p));
//                printf("Individual %d: %f %f %f\n",
//                       esoffsprings[id].id, esoffsprings[id].disObj, esoffsprings[id].covObj, esoffsprings[id].crowd_dt);
//                printf("X=[");
//                for(int j=0; j<nparameters; j++){
//                    printf("%e ",esoffsprings[id].parameters[j]);
//                }
//                printf("]\n");
            }
            if (nlopt_stop_forced(stop)) ret = NLOPT_FORCED_STOP;
            else if (*minf <= stop->minf_max) ret = NLOPT_MINF_MAX_REACHED;//modify by yx
            else if (nlopt_stop_evals(stop)) ret = NLOPT_MAXEVAL_REACHED;
            else if (nlopt_stop_time(stop)) ret = NLOPT_MAXTIME_REACHED;
            if (ret != NLOPT_SUCCESS) goto done;
        }
        /**************************************
         * Individual selection
         **************************************/
//         printf("Individual selection\n");
        /* all the individuals are copied to one vector to easily identify best solutions */

        for (int i=0; i < np; i++)
//            estotal[i] = esparents[i]; //same pointer is feasible
            IndividualDeepCopy(&esparents[i], &estotal[i], nparameters);
        for (int i=0; i < np; i++)
//            estotal[np+i] = esoffsprings[i]; //same pointer is feasible
            IndividualDeepCopy(&esoffsprings[i], &estotal[np+i], nparameters);
//        /* Sorting */
//        int front_index = non_dominated_sorting(estotal,np+np,fronts, fronts_cnt);

        /* Sorting */
        nlopt_qsort_r(estotal, np+np, sizeof(Individual), NULL,CompDis);
        /* copy after sorting: */
//        for (int i=0; i < np+np; i++) {
//            if (i<np)
//                esparents[i] = estotal[i];
//            else
//                esoffsprings[i-np] = estotal[i];
//        }
        for(int j=0; j<np; j++){
            IndividualDeepCopy(&estotal[j], &esparents[j], nparameters);
        }

//        int pop_idx = 0;
//        int f_idx = 0;
//        while(pop_idx+fronts_cnt[f_idx]<=np){
//            for(int j=0;j<fronts_cnt[f_idx];j++){
//                int individual_id = fronts[f_idx][j];
//                IndividualDeepCopy(&estotal[individual_id], &esparents[pop_idx++], nparameters);
//            }
//            f_idx++;
//        }
//        if(pop_idx<np){
//            Individual *temp_pop = (Individual*) malloc(sizeof(Individual) * fronts_cnt[f_idx]);
//            if (!temp_pop) {
//                free(temp_pop);
//                return NLOPT_OUT_OF_MEMORY;
//            }
//            for (int id=0; id < fronts_cnt[f_idx]; id++) {
//                temp_pop[id].parameters = (double*) malloc(sizeof(double) * nparameters);
//                if (!temp_pop[id].parameters) {
//                    ret = NLOPT_OUT_OF_MEMORY;
//                    goto done;
//                }
//                temp_pop[id].id = id;
//            }
//            for(int j=0; j<fronts_cnt[f_idx]; j++){
//                IndividualDeepCopy(&estotal[fronts[f_idx][j]], &temp_pop[j], nparameters);
//            }
////            printf("temp_pop1:\n");
////            printPopInfo(temp_pop,fronts_cnt[f_idx],nparameters);
//            crowding_distance_assignment(temp_pop,fronts_cnt[f_idx]);
////            printf("temp_pop2:\n");
////            printPopInfo(temp_pop,fronts_cnt[f_idx],nparameters);
//            nlopt_qsort_r(temp_pop, fronts_cnt[f_idx], sizeof(Individual), NULL,CompCrowding);//before fronts_cnt member sort
////            printf("temp_pop3:\n");
////            printPopInfo(temp_pop,fronts_cnt[f_idx],nparameters);
//            for(int j=0; j<np-pop_idx && j<fronts_cnt[f_idx]; j++){
//                IndividualDeepCopy(&temp_pop[j], &esparents[pop_idx++], nparameters);
//            }
//
//            for (int id=0; id < fronts_cnt[f_idx]; id++) free(temp_pop[id].parameters);
//            if (temp_pop)        free(temp_pop);
//        }

    } /* generations loop */

    done:
//    printf("%lf\n",*minf);
    for (int id=0; id < np; id++) free(esparents[id].parameters);
    for (int id=0; id < np; id++) free(esoffsprings[id].parameters);
    for (int id=0; id < np+np; id++) free(estotal[id].parameters);

//    for (int id=0; id < np+np; id++) free(fronts[id]);

//    if (fronts)         free(fronts);
//    if (fronts_cnt)     free(fronts_cnt);
    if (esparents) 	    free(esparents);
    if (esoffsprings) 	free(esoffsprings);
    if (estotal) 		free(estotal);

    return ret;
}
