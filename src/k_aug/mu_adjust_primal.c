/// @file mu_adjust_primal.c
/* @source untitled.c
** beta 01
** June 20th, 2017
** @author: David Thierry (dmolinat@andrew.cmu) dav0@lb2016-1

********************************************************************************

@fun_name ********************************************
**
** Computes mu and adjust the primal variables value if necesary.
** Description
**
** @param [r] 
** @param [r] 
** @param [r] 
** @param [r] 
** @param [r] 
** @param [r] 
** @param [r] 
** @return something
*******************************************************************************/

#include "mu_adjust_primal.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#define HUGE_NUMBER 1e300

void mu_adjust_x(int nvar, double *x, double *lbv, real *zL, real *zU, double log10mu_target, double *logmu0){
    /* find a multiplier-primal combination that allows to compute mu*/
    /* adjust primal */
    int i;
    double mul, muu, mu=0.0;
    double bound_relax_factor = 1e-08;
    double const *brf_ptr = &bound_relax_factor;

    double machine_epsi = DBL_EPSILON;
    double brl = 0, bru = 0;
    /*If zL and zU are not declared, they will have a 0.0 value, presumably
      therefore mul and muu will have a 0.0 value as well.
    */

    for(i=0; i<nvar; i++){

        if (lbv[2*i] == lbv[2*i + 1]){ continue;}
        if (lbv[2*i] > -HUGE_NUMBER){
            brl = *brf_ptr * ((1 > fabs(lbv[2*i])) ? 1 : fabs(lbv[2*i]));
            lbv[2*i] = lbv[2*i] - brl;
        }
        if (lbv[2*i + 1] < HUGE_NUMBER){
            bru = *brf_ptr * ((1 > fabs(lbv[2*i + 1])) ? 1 : fabs(lbv[2*i + 1]));
            lbv[2*i + 1] = lbv[2*i + 1] + bru;
        }
    }


    *logmu0 = log10mu_target;
    for(i=0; i<nvar; i++){
        mul = 0.0;
        muu = 0.0;
        mu  = 0.0;
        if(zL[i] > 0){
            mul = (x[i] - lbv[2*i]) * (zL[i]);
        }
        if(-zU[i] > 0){
            muu = (x[i] - lbv[2*i + 1])*(zU[i]);
        }
        if(mul>0.0 && muu>0.0){
            mu = mul;
        }
        else if (mul > 0.0){
            mu = mul;
        }
        else if (muu > 0.0){
            mu = muu;
        }
        if(mu>0.0){
            if(fabs(*logmu0 - log10(mu)) < 1){
                printf("I[K_AUG]...\t[ADJUST_MU]"
                       "log10(mu) close to the target\t%.g at var_i=%d\n", log10(mu), i);
                *logmu0 = log10(mu);
                break;
            }
            else{
                *logmu0 = log10(mu);
                printf("I[K_AUG]...\t[ADJUST_MU]"
                       "log10(mu) computed=%.g at var_i=%d\n", log10(mu), i);
            }
        }
        else if ((fabs(x[i] - lbv[2*i]) < 1e-08) && (fabs(zL[i]) < 1e-08)){
            printf("I[K_AUG]...\t[ADJUST_MU]"
                   "\tWarning strict complementarity (lb) is not within tol for var:%d\n", i);

        }
        else if ((fabs(x[i] - lbv[2*i + 1]) < 1e-08) && (fabs(zU[i]) < 1e-08)){
            printf("I[K_AUG]...\t[ADJUST_MU]"
                   "\tWarning strict complementarity (ub) is not within tol for var:%d\n", i);

        }


    }
    if(*logmu0 == log10mu_target){
        printf("I[K_AUG]...\t[ADJUST_MU]"
               "\tWarning no relevant info from the problem can predict logmu\n");
    }
    else if(*logmu0 > log10mu_target){
        /*printf("I[K_AUG]...\t[ADJUST_MU]"
               "\tWarning logmu is over -11.0; is this optimal?\n"); */
        ;
    }


    /*mu = (log10(mu) > -8.6) ? exp(-8.6): mu;*/
    /* obsolete! */
    /*
    for(i=0; i<nvar; i++){
        if((fabs(lbv[2*i])<1e+300) && (fabs(lbv[2*i+1])<1e+300) && (fabs(lbv[2*i+1] - lbv[2*i]) < 1e-12)){
            fprintf(stderr, "W[K_AUG]...\t[ADJUST_MU]""variable %d has lb == ub, setting barrier = 0\t %f.\n", i, fabs(lbv[2*i+1] - lbv[2*i+1]));
            continue;}
        if(zL[i]>0 && -zU[i] > 0){
            if((x[i] - lbv[2*i]) * (zL[i]) < mu*0.5 ||  (x[i] - lbv[2*i + 1])*(zU[i]) < mu*0.5){
                x[i] = ((x[i] - lbv[2*i]) * (zL[i])) < ((x[i] - lbv[2*i + 1])*(zU[i])) ? 	mu/(zL[i]) + lbv[2*i]: mu/(zU[i]) +lbv[2*i+1];
            }
        }
        else if(zL[i] > 0){
            if((x[i] - lbv[2*i]) * (zL[i]) < mu*0.5){x[i] = mu/(zL[i]) + lbv[2*i];}
        }
        else if(-zU[i] > 0){
            if((x[i] - lbv[2*i + 1])*(zU[i]) < mu*0.5){x[i] = mu/(zU[i]) + lbv[2*i + 1];}
        }
    }*/
    for(i=0; i<nvar;i++){
        if (lbv[2*i] == lbv[2*i + 1]){ continue;}
        if (lbv[2*i] > -HUGE_NUMBER) {
            if (fabs(x[i] - lbv[2 * i]) < machine_epsi * mu) {
                brl = (pow(machine_epsi,3.0 / 4.0)) * ((1 > fabs(lbv[2 * i])) ? 1 : fabs(lbv[2 * i]));
                lbv[2 * i] = lbv[2 * i] - brl;
                printf("brl %f\n", brl);
            }
        }
        if (lbv[2*i + 1] < HUGE_NUMBER){
            if (fabs(-x[i] + lbv[2 * i + 1]) < machine_epsi * mu) {
                bru = (pow(machine_epsi, 3.0 / 4.0)) * ((1 > fabs(lbv[2*i + 1])) ? 1 : fabs(lbv[2*i + 1]));
                lbv[2*i + 1] = lbv[2*i + 1] + bru;
                printf("bru %f\n", bru);
            }
        }

    }

}

