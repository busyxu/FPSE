/* Copyright (c) 2008-2013 Carlos Henrique da Silva Santos
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>/* printf */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ga.h"

/****************************************************************************/
/* Cauchy random number distribution */
static double randcauchy(const double params[7]) {
     /* double min, double max, double mi, double t, double band */
     double na_unif, cauchy_mit, limit_inf, limit_sup;
     double valor;
     double min = params[1], max = params[2], mi = params[3],
	  t = params[4], band = params[5];
     limit_inf = mi - (band*0.5);
     limit_sup = mi + (band*0.5);
     do {
	  na_unif = nlopt_urand(0,1); /* ran2(0,1); */
	  cauchy_mit = t*tan((na_unif-0.5)*3.14159265358979323846) + mi;
     } while ( (cauchy_mit<limit_inf) || (cauchy_mit>limit_sup) );

     if (cauchy_mit < 0)
	  cauchy_mit = -cauchy_mit;
     else
	  cauchy_mit = cauchy_mit + (band*0.5);
     valor  = cauchy_mit/band;
     valor = min+(max-min)*valor;
     return valor;
}

/****************************************************************************/

/* main Individual representation type */
typedef struct IndividualStructure {
     double * parameters;
     double fitness;
} Individual;

static int CompareIndividuals(void *unused, const void *a_, const void *b_) {
     const Individual *a = (const Individual *) a_;
     const Individual *b = (const Individual *) b_;
     (void) unused;
     return a->fitness < b->fitness ? -1 : (a->fitness > b->fitness ? +1 : 0);
}

static void sbxCrossover(int id, Individual *esparents, Individual *esoffsprings, unsigned np,
                         unsigned nparameters, const double *lb, const double *ub, double disC){
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

static void pointOneCrossover(int id, Individual *esparents, Individual *esoffsprings,
                              unsigned np, unsigned nparameters){
    int item;
    int parent1  = nlopt_iurand((int) np);
    int parent2  = nlopt_iurand((int) np);
    unsigned crosspoint = (unsigned) nlopt_iurand((int) nparameters);
    for (item=0; item < crosspoint; item++)
        esoffsprings[id].parameters[item]
        = esparents[parent1].parameters[item];
    for (item=crosspoint; item < nparameters; item++)
        esoffsprings[id].parameters[item]
        = esparents[parent2].parameters[item];
//   parent1  = nlopt_iurand((int) np);
//   parent2  = nlopt_iurand((int) np);
//   crosspoint = (unsigned) nlopt_iurand((int) nparameters);
//   for (item=0; item < crosspoint; item++)
//    esoffsprings[id].parameters[item]
//     = esparents[parent1].parameters[item];
//   for (item=crosspoint; item < nparameters; item++)
//    esoffsprings[id].parameters[item]
//     = esparents[parent2].parameters[item];
}

static void pmMutation(int id, Individual *esoffsprings, unsigned nparameters,
                       const double *lb, const double *ub, double disM){
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
            double delta2 = (ub[item] - esoffsprings[item].parameters[item]) /
                            (ub[item] - lb[item]); // delta2 = (ub-x)/(ub-lb)
            double delta = 1 - pow(2 * (1 - mu) + 2 * (mu - 0.5) * pow((1 - delta2), disM + 1), 1 / (disM + 1)); // delta = 1-(2*(1-mu)+2*(mu-0.5)*(1-delta2)^(disM+1))^(1/(disM+1))
            esoffsprings[item].parameters[item] =
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

static void customMutation(int id, Individual *esoffsprings, unsigned nparameters,
                           const double *lb, const double *ub){
//      int contmutation;
//      int totalmutation = (int) ((no * nparameters) / 10);
//      if (totalmutation < 1) totalmutation = 1;
//      for (contmutation=0; contmutation < totalmutation;
//           contmutation++) {
//           int idoffmutation = nlopt_iurand((int) no);
//           int paramoffmutation = nlopt_iurand((int) nparameters);
////           vetor[1] = lb[paramoffmutation];
////           vetor[2] = ub[paramoffmutation];
////           vetor[7] = vetor[7]+contmutation;
//
//            esoffsprings[idoffmutation].parameters[paramoffmutation] = nlopt_urand(lb[paramoffmutation],ub[paramoffmutation]);
////           esoffsprings[idoffmutation].parameters[paramoffmutation] = vetor[1] + ((double)rand() / RAND_MAX) * (vetor[2] - vetor[1]);
//      }
    int paramoffmutation = nlopt_iurand((int) nparameters);
    esoffsprings[id].parameters[paramoffmutation] = nlopt_urand(lb[paramoffmutation],ub[paramoffmutation]);
}

nlopt_result ga_sbx_pm_strategy(
     unsigned nparameters, /* Number of input parameters */
     nlopt_func f,	/* Recursive Objective Funtion Call */
     void * data_f,	/* Data to Objective Function */
     const double* lb,			/* Lower bound values */
     const double* ub,			/* Upper bound values */
     double* x,				/*in: initial guess, out: minimizer */
     double* minf,
     nlopt_stopping* stop, 		/* nlopt stop condition */
     unsigned np, 			/* Number of Parents */
     unsigned no,           /* Number of Offsprings */
     double *seed,
     int seed_size) {

     /* variables from nlopt */
     nlopt_result ret = NLOPT_SUCCESS;//init ret
     double vetor[8];
     unsigned  i, id, item;
     int  parent1, parent2;
     unsigned crosspoint;  /* crossover parameteres */
     int  contmutation, totalmutation;	/* mutation parameters */
     int  idoffmutation, paramoffmutation;	/* mutation parameters */
     Individual * esparents;			/* Parents population */
     Individual * esoffsprings;		/* Offsprings population */
     Individual * estotal;/* copy containing Parents and Offsprings pops */
     /* It is interesting to maintain the parents and offsprings
      * populations stablished and sorted; when the final iterations
      * is achieved, they are ranked and updated. */

//    srand((unsigned int)time(NULL));
    double proC = 1; //crossover probability
    double disC = 20; //distribution index crossover
    double proM = 1;  //crossover probability
    double disM = 20; //distribution index mutation

    double grad = 1024;
     /*********************************
      * controling the population size
      *********************************/
//    printf("controling the population size\n");
     if (!np) np = 40; //parents
     if (!no) no = 60; //offspring
     if ((np < 1)||(no<1)) {
         nlopt_stop_msg(stop, "populations %d, %d are too small", np, no);
         return NLOPT_INVALID_ARGS;
     }
     esparents    = (Individual*) malloc(sizeof(Individual) * np);
     esoffsprings = (Individual*) malloc(sizeof(Individual) * no);
     estotal 	 = (Individual*) malloc(sizeof(Individual) * (np+no));
     if ((!esparents)||(!esoffsprings)||(!estotal)) {
	  free(esparents); free(esoffsprings); free(estotal);
	  return NLOPT_OUT_OF_MEMORY;
     }
     for (id=0; id < np; id++) esparents[id].parameters = NULL;
     for (id=0; id < no; id++) esoffsprings[id].parameters = NULL;
     /* From here the population is initialized */
     /* we don't handle unbounded search regions;
	    this check is unnecessary since it is performed in nlopt_optimize.
	 for (j = 0; j < nparameters; ++j)
   	  if (nlopt_isinf(low[j]) || nlopt_isinf(up[j]))
	    return NLOPT_INVALID_ARGS;
     */
     /* main vector of parameters to randcauchy */
     vetor[0] = 4; /* ignored */
     vetor[3] = 0;
     vetor[4] = 1;
     vetor[5] = 10;
     vetor[6] = 1;
     vetor[7] = 0; /* ignored */
     /**************************************
      * Initializing parents population
      **************************************/
//    printf("Initializing parents population\n");
     for (id=0; id < np; id++) {
	  esparents[id].parameters =
	       (double*) malloc(sizeof(double) * nparameters);
	  if (!esparents[id].parameters) {
	       ret = NLOPT_OUT_OF_MEMORY;
	       goto done;
	  }
         if(id<seed_size){
             for(item=0; item<nparameters; item++)
                 esparents[id].parameters[item] = seed[id];
         } else{
             for (item=0; item<nparameters; item++) {
               vetor[1] = lb[item];
               vetor[2] = ub[item];
               vetor[7] = vetor[7]+1;
    //	       esparents[id].parameters[item] = randcauchy(vetor);
    //          esparents[id].parameters[item] = vetor[1] + ((double)rand() / RAND_MAX) * (vetor[2] - vetor[1]);
              esparents[id].parameters[item] = nlopt_urand(vetor[1],vetor[2]);//uniform distribution
             }
         }
     }

     memcpy(esparents[0].parameters, x, nparameters * sizeof(double));

//     for(int i =0; i<np; i++){
//         for(int j=0; j<nparameters; j++){
//            printf("%lf ",esparents[i].parameters[j]);
//         }
//         printf("\n");
//     }

     /**************************************
      * Initializing offsprings population
      **************************************/
//    printf("Initializing offsprings population\n");
     for (id=0; id < no; id++) {
	  esoffsprings[id].parameters =
	       (double*) malloc(sizeof(double) * nparameters);
	  if (!esoffsprings[id].parameters) {
	       ret = NLOPT_OUT_OF_MEMORY;
	       goto done;
	  }
	  for (item=0; item<nparameters; item++) {
	       vetor[1] = lb[item];
	       vetor[2] = ub[item];
	       vetor[7]=vetor[7]+1;
//	       esoffsprings[id].parameters[item] = randcauchy(vetor);
//           esoffsprings[id].parameters[item] = vetor[1] + ((double)rand() / RAND_MAX) * (vetor[2] - vetor[1]);
            esoffsprings[id].parameters[item] = nlopt_urand(vetor[1],vetor[2]);//uniform distribution
	  }
     }
     /**************************************
      * Parents fitness evaluation
      **************************************/
//    printf("Parents fitness evaluation\n");
     for (id=0; id < np; id++) {
	  esparents[id].fitness =
	       f(nparameters, esparents[id
           ].parameters, &grad, data_f);
	  estotal[id].fitness = esparents[id].fitness;
	  ++ *(stop->nevals_p);
	  if (*minf > esparents[id].fitness) {
	       *minf = esparents[id].fitness;
//           *(minf+1) = grad; // add by yx
	       memcpy(x, esparents[id].parameters,
		      nparameters * sizeof(double));

          printf("X=[");
          for(int j=0; j<nparameters; j++){
              printf("%e ",esparents[id].parameters[j]);
          }
          printf("]\n");
          printf("fitness: %f; grad: %f\n--------------------\n",esparents[id].fitness, grad);

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
//    printf("Main Loop - Generations\n");
     while (1) {
	  /**************************************
	   * Simulated Binary Crossover
	   **************************************/
      for (id=0; id < no; id++){
          double rd = nlopt_urand(0,1);
          if(rd<=proC) {
              rd = nlopt_urand(0, 1);
              if (rd <= 0.5) {
                  sbxCrossover(id, esparents, esoffsprings, np, nparameters, lb, ub, disC);
              } else {
                  pointOneCrossover(id, esparents, esoffsprings, np, nparameters);
              }
          }
          else{
              //offpring = parents
              for (item=0; item < nparameters; item++)
                  esoffsprings[id].parameters[item] = esparents[id].parameters[item];
          }
	  }
	  /**************************************
	   * Polynomial Mutation
	   **************************************/
         for (id=0; id < no; id++){
             double rd = nlopt_urand(0,1);
             if(rd<=proM/nparameters) {
                 if (rd <= 0.5) {
                     pmMutation(id, esoffsprings, nparameters, lb, ub, disM);
                 } else {
                     customMutation(id, esoffsprings, nparameters, lb, ub);
                 }
             }
         }
	  /**************************************
	   * Offsprings fitness evaluation
	   **************************************/
//         printf("Offsprings fitness evaluation\n");
	  for (id=0; id < no; id++){
//          for(int i=0; i<nparameters; i++){
//              printf("%lf ",esoffsprings[id].parameters[i]);
//          }
//          printf("\n");
	       /*esoffsprings[id].fitness = (double)fitness(esoffsprings[id].parameters, nparameters,fittype);*/
	       esoffsprings[id].fitness = f(nparameters, esoffsprings[id].parameters, &grad, data_f);
	       estotal[id+np].fitness = esoffsprings[id].fitness;
	       ++ *(stop->nevals_p);
	       if (*minf > esoffsprings[id].fitness) {
		        *minf = esoffsprings[id].fitness;
//            *(minf+1) = grad; // add by yx
              memcpy(x, esoffsprings[id].parameters,
			   nparameters * sizeof(double));

               printf("X=[");
               for(int j=0; j<nparameters; j++){
                   printf("%e ",esoffsprings[id].parameters[j]);
               }
               printf("]\n");
               printf("fitness: %f; grad: %f\n--------------------\n",esoffsprings[id].fitness, grad);
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
	  for (i=0; i < np; i++)
	       estotal[i] = esparents[i];
	  for (i=0; i < no; i++)
	       estotal[np+i] = esoffsprings[i];
	  /* Sorting */
	  nlopt_qsort_r(estotal, no+np, sizeof(Individual), NULL,
			CompareIndividuals);
	  /* copy after sorting: */
	  for (i=0; i < no+np; i++) {
	       if (i<np)
		    esparents[i] = estotal[i];
	       else
		    esoffsprings[i-np] = estotal[i];
	  }
     } /* generations loop */

done:
//    printf("%lf\n",*minf);
     for (id=0; id < np; id++) free(esparents[id].parameters);
     for (id=0; id < no; id++) free(esoffsprings[id].parameters);

     if (esparents) 	free(esparents);
     if (esoffsprings) 	free(esoffsprings);
     if (estotal) 		free(estotal);
     return ret;
}
