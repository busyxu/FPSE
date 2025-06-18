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

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include "byteea.h"


/****************************************************************************/
static int randByteInt(const double params[7]) {

//  int xl = params[1];
//  int xu = params[2];
    int xl = 0;
    int xu = 255;

  // Generate random integer in range [0, 99]
  int random_integer = rand() % (xu-xl+1);
//  double rndi = (double) random_integer;
  return random_integer;
}

static void doubleToBytes(double *x, int nvar, double *parameters){
    for (int i = 0; i < nvar; i++) {
        uint8_t bytes[8];
        memcpy(bytes, &x[i], sizeof(double));
        for (int j = 0; j < 8; j++) {
            parameters[i*8+j] = bytes[j];
        }
    }
//    return;
}

static void bytesToDouble(double *parameters, int nvar, double *dval){
//  printf("bytes:\n");
//  for(int j=0; j<nparameters; j++){
//    printf("%d ",(int)parameters[j]);
//  }
//  printf("\n");
//  int nvar = nparameters/8;
//  double *dval = malloc(sizeof(double) * nvar);
  for (int i = 0; i < nvar; i++) {
    uint8_t bytes[8];
    for (int j = 0; j < 8; j++) {
      bytes[j] = parameters[i*8+j];
    }
    double d;
    memcpy(&d, bytes, sizeof(double));
    dval[i] = d;
  }
//  return dval;
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

nlopt_result byteevolutionarystrategy(
     unsigned nvar, /* Number of input parameters */
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
     int seed_size) { 			/* Number of Offsprings */

     double *gradPr = malloc(sizeof(double));
     *gradPr = 1024;
      unsigned nparameters = nvar*8; // add by yx a variable is 8 byte

      srand(time(NULL));

     /* variables from nlopt */
     nlopt_result ret = NLOPT_SUCCESS;//init ret
//     double vetor[8];
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

     /*********************************
      * controling the population size
      *********************************/
//    printf("np:%d no:%d\n",np,no);
     if (!np) np = 40;
     if (!no) no = 60;
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
//     vetor[0] = 4; /* ignored */
//     vetor[3] = 0;
//     vetor[4] = 1;
//     vetor[5] = 10;
//     vetor[6] = 1;
//     vetor[7] = 0; /* ignored */
     /**************************************
      * Initializing parents population
      **************************************/
//    printf("nparameters:%d\n",nparameters);
    double *x_t = malloc(sizeof(double) * nvar);
    double *seedParam = malloc(sizeof(double) * nparameters);
     for (id=0; id < np; id++) {
	  esparents[id].parameters = (double*) malloc(sizeof(double) * nparameters);
	  if (!esparents[id].parameters) {
	       ret = NLOPT_OUT_OF_MEMORY;
	       goto done;
	  }

      if(id<seed_size){
          for(int j=0; j<nvar; j++){
              x_t[j] = seed[id];
          }
          doubleToBytes(x_t, nvar, seedParam);
          memcpy(esparents[id].parameters, seedParam, nparameters * sizeof(double));
          for(item=0; item<nparameters; item++)
              esparents[id].parameters[item] = seed[id];
      } else{
          for (item=0; item<nparameters; item++) {
//              vetor[1] = lb[item];
//              vetor[2] = ub[item];
//              vetor[7] = vetor[7]+1;
              //	       int rnd = randByteInt(vetor);
              esparents[id].parameters[item] = nlopt_iurand(256);
          }
      }
     }

    double *initParam = malloc(sizeof(double) * nparameters);
    doubleToBytes(x, nvar, initParam);
     memcpy(esparents[np-1].parameters, initParam, nparameters * sizeof(double));
    free(initParam);
    free(x_t);
    free(seedParam);
//    for(int i=0; i<np; i++){
//        for(int j=0; j<nparameters; j++){
//            printf("%f ",esparents[i].parameters[j]);
//        }
//        printf("\n");
//    }
     /**************************************
      * Initializing offsprings population
      **************************************/
//    printf("no:%d\n",no);
     for (id=0; id < no; id++) {
	  esoffsprings[id].parameters =
	       (double*) malloc(sizeof(double) * nparameters);
	  if (!esoffsprings[id].parameters) {
	       ret = NLOPT_OUT_OF_MEMORY;
	       goto done;
	  }
	  for (item=0; item<nparameters; item++) {
//	       vetor[1] = lb[item];
//	       vetor[2] = ub[item];
//	       vetor[7] = vetor[7]+1;
	       esoffsprings[id].parameters[item] = nlopt_iurand(256);
	  }
     }
     /**************************************
      * Parents fitness evaluation
      **************************************/
//    printf("fitness evaluation\n");
     for (id=0; id < np; id++) {
//     double *dval = bytesToDouble(esoffsprings[id].parameters, nparameters);
      double *dval = malloc(sizeof(double) * nvar);
      bytesToDouble(esparents[id].parameters, nvar, dval);
	  esparents[id].fitness = f(nparameters, dval, gradPr, data_f);

	  estotal[id].fitness = esparents[id].fitness;
	  ++ *(stop->nevals_p);
	  if (*minf > esparents[id].fitness) {
	       *minf = esparents[id].fitness;
//           *(minf+1) = grad; // add by yx
//	       memcpy(x, esparents[id].parameters,
//		      nparameters * sizeof(double));

          for(int j=0; j<nparameters; j++){
              if(j%8==0) printf("\n");
              printf("%f ",esparents[id].parameters[j]);
          }
          printf("\n");
          for(int j=0; j<nvar; j++){
              printf("dval: %lf; %e\n",*(dval+j),*(dval+j));
          }
          printf("fitness: %f; grad: %f\n--------------------\n",esparents[id].fitness, *gradPr);

          memcpy(x, dval, nvar * sizeof(double));
	  }
      free(dval);
	  if (nlopt_stop_forced(stop)) ret = NLOPT_FORCED_STOP;
	  else if (*minf <= stop->minf_max) ret = NLOPT_MINF_MAX_REACHED;//modfiy by yx
//      else if (*(minf+1) <= stop->minf_max) ret = NLOPT_MINF_MAX_REACHED;//add by yx
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
	   * Crossover 单点交叉  交叉300次，产生300个子代
	   **************************************/
	  for (id=0; id < no; id++)
	  {
	       parent1  = nlopt_iurand((int) np);// select fa
	       parent2  = nlopt_iurand((int) np);// select ma
	       crosspoint = (unsigned) nlopt_iurand((int) nparameters);//select crosspoint point
	       for (item=0; item < crosspoint; item++)
		    esoffsprings[id].parameters[item]
			 = esparents[parent1].parameters[item];
	       for (item=crosspoint; item < nparameters; item++)
		    esoffsprings[id].parameters[item]
			 = esparents[parent2].parameters[item];
	  }
	  /**************************************
	   * Gaussian Mutation
	   **************************************/
//         printf("Gaussian Mutation\n");
	  totalmutation = (int) ((no * nparameters) / 10);//变异数量等于子代数量*决策变量数/10
	  if (totalmutation < 1) totalmutation = 1;
	  for (contmutation=0; contmutation < totalmutation; contmutation++) {
	       idoffmutation = nlopt_iurand((int) no);//select mutation off
	       paramoffmutation = nlopt_iurand((int) nparameters);//select mutation point
//	       vetor[1] = lb[paramoffmutation];//找到该决策变量的上界
//	       vetor[2] = ub[paramoffmutation];//找到该决策变量的下界
//	       vetor[7] = vetor[7]+contmutation;
	       esoffsprings[idoffmutation].parameters[paramoffmutation] = nlopt_iurand(256);
	  }
	  /**************************************
	   * Offsprings fitness evaluation
	   **************************************/
//         printf("Offsprings fitness evaluation -no: %d\n",no);
	  for (id=0; id < no; id++){
	       /*esoffsprings[id].fitness = (double)fitness(esoffsprings[id].parameters, nparameters,fittype);*/
//           double *dval = bytesToDouble(esoffsprings[id].parameters, nparameters);
           double *dval = malloc(sizeof(double) * nvar);
           bytesToDouble(esoffsprings[id].parameters, nvar, dval);
	       esoffsprings[id].fitness = f(nparameters, dval, gradPr, data_f);
           estotal[id+np].fitness = esoffsprings[id].fitness;
	       ++ *(stop->nevals_p);
	       if (*minf > esoffsprings[id].fitness) {
		    *minf = esoffsprings[id].fitness;
//            *(minf+1) = grad; // add by yx
//		    memcpy(x, esoffsprings[id].parameters,
//			   nparameters * sizeof(double));

               for(int j=0; j<nparameters; j++){
                   if(j%8==0) printf("\n");
                   printf("%f ",esoffsprings[id].parameters[j]);
               }
               printf("\n");
               for(int j=0; j<nvar; j++){
                   printf("dval: %lf; %e\n",*(dval+j),*(dval+j));
               }
               printf("fitness: %f; grad: %f\n--------------------\n",esoffsprings[id].fitness, *gradPr);

            memcpy(x, dval, nvar * sizeof(double));
	       }
           free(dval);
	       if (nlopt_stop_forced(stop)) ret = NLOPT_FORCED_STOP;
	       else if (*minf <= stop->minf_max) ret = NLOPT_MINF_MAX_REACHED; // modify by yx
//           else if (*(minf+1) <= stop->minf_max) ret = NLOPT_MINF_MAX_REACHED; //add by yx
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
	  nlopt_qsort_r(estotal, no+np, sizeof(Individual), NULL, CompareIndividuals);

//     for(int i=0; i<np+no; i++){
//         for(int j=0; j<nparameters; j++){
//             printf("%f ",estotal[i].parameters[j]);
//         }
//         printf("\n%f",estotal[i].fitness);
//         printf("\n");
//     }
//	  printf("best solution:\n");
//	  for(int j=0; j<nparameters; j++){
//        printf("%d ",(int)estotal[0].parameters[j]);
//      }
//      printf("\n");
//	  printf("bad solution:\n");
//       for(int j=0; j<nparameters; j++){
//         printf("%d ",(int)estotal[np+no-1].parameters[j]);
//       }
//       printf("\n");

	  /* copy after sorting: */
	  for (i=0; i < no+np; i++) {
	       if (i<np)
		    esparents[i] = estotal[i];
	       else
		    esoffsprings[i-np] = estotal[i];
	  }
     } /* generations loop */
//    printf("done\n");
done:
     for (id=0; id < np; id++) free(esparents[id].parameters);
     for (id=0; id < no; id++) free(esoffsprings[id].parameters);

     if (esparents) 	free(esparents);
     if (esoffsprings) 	free(esoffsprings);
     if (estotal) 		free(estotal);

     if(gradPr) free(gradPr);
//    printf("ret:%d\n",ret);
     return ret;
}
