/*
Inspired from:
https://github.com/adis300/filter-c/tree/master
https://www.analog.com/media/en/technical-documentation/dsp-book/dsp_book_Ch20.pdf
https://mkhangg.com/assets/doc/CSE_3313_AudioFiltering.pdf
http://irphouse.com/ijeee/ijeeev8n1_07.pdf

*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "filter.h"

#define DEBUG  0


ChebFilter* call_205(int P, ChebFilter* filter, double FC, int NP, int LH, double PR) { 

  double rp= -cos(M_PI/(NP*2) + (P-1)*M_PI/NP);
  double ip=  sin(M_PI/(NP*2) + (P-1)*M_PI/NP);

  if(DEBUG>1) {
    printf("\n[call_205 #%d] rp= %.10lf\n", P, rp);
    printf("[call_205 #%d] ip= %.10lf\n", P, ip);
    printf("\n");
  }

  if(PR!=0.0) { 
    double es= sqrt(pow(100.0 / (100.0-PR),2) - 1.0);
    double vx= (1.0/NP) * log( (1.0/es) + sqrt( (1.0/pow(es,2))+1.0) );
    double kx= (1.0/NP) * log( (1.0/es) + sqrt( (1.0/pow(es,2))-1.0) );
          kx= (exp(kx) + exp(-kx))/2;
          rp= rp * ((exp(vx) - exp(-vx))/2.0)/kx;
          ip= ip * ((exp(vx) + exp(-vx))/2.0)/kx;
    if(DEBUG>1) {
      printf("[call_205 #%d PR!=0] rp= %.10lf\n", P, rp);
      printf("[call_205 #%d PR!=0] ip= %.10lf\n", P, ip);
      printf("[call_205 #%d PR!=0] es= %.10lf\n", P, es);
      printf("[call_205 #%d PR!=0] vx= %.10lf\n", P, vx);
      printf("[call_205 #%d PR!=0] kx= %.10lf\n", P, kx);
    }
  }

  double t=2*tan((double)1/2);
  double w= 2*M_PI*FC;
  double m=pow(rp,2) + pow(ip,2);
  double d= 4 - 4*rp*t + m*pow(t,2);
  double x0= pow(t,2)/d;
  double x1= 2*pow(t,2)/d;
  double x2= pow(t,2)/d;
  double y1=(8 - 2*m*pow(t,2))/d;
  double y2=(-4 - 4*rp*t - m*pow(t,2))/d;

  if(DEBUG>1) {
    printf("[call_205 #%d] t= %.10lf\n", P, t);
    printf("[call_205 #%d] w= %.10lf\n", P, w);
    printf("[call_205 #%d] m= %.10lf\n", P, m);
    printf("[call_205 #%d] d= %.10lf\n", P, d);
    printf("[call_205 #%d] x0= %.10lf\n", P, x0);
    printf("[call_205 #%d] x1= %.10lf\n", P, x1);
    printf("[call_205 #%d] x2= %.10lf\n", P, x2);
    printf("[call_205 #%d] y1= %.10lf\n", P, y1);
    printf("[call_205 #%d] y2= %.10lf\n", P, y2);
    printf("\n");
  }

  double k=0;
  if(LH==1) {
    k= -cos(w/2 + (double)1/2)/cos(w/2 - (double)1/2);
  }
  if(LH==0) {
    k=  sin((double)1/2 - w/2)/sin((double)1/2 + w/2);
  }
  if(DEBUG>1) printf("[call_205 #%d] k= %.10lf\n", P, k); 

  d= 1 + y1*k - y2*pow(k, 2);
  filter->a0= (x0 - x1*k + x2*pow(k,2))/d;
  filter->a1= (-2*x0*k + x1 +x1*pow(k,2) - 2*x2*k)/d;
  filter->a2= (x0*pow(k,2) -x1*k +x2)/d;
  filter->b1= (2*k + y1 + y1*pow(k,2) - 2*y2*k)/d;
  filter->b2= (-pow(k,2) - y1*k +y2)/d;

  if(DEBUG>1) {
    printf("[call_205 #%d] d= %.10lf\n", P, d);
    printf("[call_205 #%d] a0= %.10lf\n", P, filter->a0);
    printf("[call_205 #%d] a1= %.10lf\n", P, filter->a1);
    printf("[call_205 #%d] a2= %.10lf\n", P, filter->a2);
    printf("[call_205 #%d] b1= %.10lf\n", P, filter->b1);
    printf("[call_205 #%d] b2= %.10lf\n", P, filter->b2);
  }
  if(LH==1) {
    filter->a1=-filter->a1;
    filter->b1=-filter->b1;
  }

  if(DEBUG>1) {
    printf("[call_205 #%d] a1= %.10lf\n", P, filter->a1);
    printf("[call_205 #%d] b1= %.10lf\n", P, filter->b1);
  }

  return(filter);
}


/*
  NP : Number of poles / Order
  PR: Percent ripple
  LH: 0 for lowpass, 1 for highpass
  FC: cutoff frequency (0 to 0.5), typically fc(Hz)/SR(Hz), with fc<SR/2. Example: (double)1000/44100
*/
ChebFilter* create_che_filter(int NP, double PR, int LH, double FC) {

  ChebFilter* filter = (ChebFilter*)malloc(sizeof(ChebFilter));

  double a[22];
  double b[22];

  double ta[22];
  double tb[22];

  for(int i=0; i<22; i++) {
    a[i]=0;
    b[i]=0;
  }

  a[2]=1;
  b[2]=1;

  for(int i=0; i<22; i++) {
    filter->a[i]=a[i];
    filter->b[i]=b[i];
  }

  filter->NP=NP;


  for(int p=1; p<=NP/2; p++) {
    filter=call_205(p, filter, FC, NP, LH, PR);

    for(int i=0; i<22; i++) {
      ta[i]=a[i];
      tb[i]=b[i];
    }

    for(int i=2; i<22; i++) {
      a[i]=filter->a0*ta[i] + filter->a1*ta[i-1] + filter->a2*ta[i-2];
      b[i]=   tb[i] - filter->b1*tb[i-1] - filter->b2*tb[i-2];
    }
  }

  b[2]=0;
  for(int i=0; i<20; i++) {
    a[i]=a[i+2];
    b[i]=-b[i+2];
  }

  double sa=0;
  double sb=0;

  for(int i=0; i<20; i++) {
    if(LH==0) { 
      sa=sa+a[i];
      sb=sb+b[i];
    }
    if(LH==1) { 
      sa=sa+a[i]*pow(-1, i);
      sb=sb+b[i]*pow(-1, i);
    }
  }

  double gain=sa/(1-sb);

  for(int i=0; i<20; i++) 
    a[i]=a[i]/gain;

  for(int i=0; i<20; i++) {
    filter->a[i]=a[i];
    filter->b[i]=b[i];
  }
  
  if(DEBUG>0) {
    printf("Order: %d\n", NP);
    printf("Fc: %lf\n", FC);
    printf("a0: %lf\n", filter->a0);
    printf("a1: %lf\n", filter->a1);
    printf("a2: %lf\n", filter->a2);
    printf("b1: %lf\n", filter->b1);
    printf("b2: %lf\n", filter->b2);

    printf("a= [");
    for(int i=0; i<=NP; i++) {
      //printf("%.10e ", a[i]);
      printf("%.10lf ", a[i]);
    }
    printf("]\n");

    printf("b= [");
    for(int i=0; i<=NP; i++) {
      //printf("%.10e ", b[i]);
      printf("%.10lf ", b[i]);
    }
    printf("]\n");
  }

  for(int i=0; i<20; i++) {
    filter->X[i]=0;
    filter->Y[i]=0;
  }

  return(filter);
}

dChebFilter* create_bw_low_pass_filter(int NP, double FC) {
  dChebFilter *filter=(dChebFilter*)malloc(sizeof(dChebFilter));

  filter->type=0;
  filter->lp_filter=create_che_filter(NP, 0, 0, FC);
  filter->hp_filter=NULL;

  return(filter);
}

dChebFilter* create_bw_high_pass_filter(int NP, double FC) {
  dChebFilter *filter=(dChebFilter*)malloc(sizeof(dChebFilter));

  filter->type=1;
  filter->lp_filter=NULL;
  filter->hp_filter=create_che_filter(NP, 0, 1, FC);

  return(filter);
}

dChebFilter* create_bw_bp_pass_filter(int NP, double FC, double winwidth) {
  dChebFilter *filter=(dChebFilter*)malloc(sizeof(dChebFilter));

  filter->type=2;
  filter->lp_filter=create_che_filter(NP, 0, 0, FC*(1.0+fabsl(winwidth)));
  filter->hp_filter=create_che_filter(NP, 0, 1, FC*(1.0-fabsl(winwidth)));

  return(filter);
}

double applydfilter(dChebFilter* filter, double X0) {
  double output=X0;

  if(filter->lp_filter!=NULL)
    output=applyfilter(filter->lp_filter, output);

  if(filter->hp_filter!=NULL)
    output=applyfilter(filter->hp_filter, output);

  return(output);
}


ChebFilter* create_che_low_pass_filter(int NP, double FC, double PR) {
  return(create_che_filter(NP, PR, 0, FC));
}

ChebFilter* create_che_high_pass_filter(int NP, double FC, double PR) {
  return(create_che_filter(NP, PR, 1, FC));
}

double applyfilter(ChebFilter* filter, double X0) {

  filter->X[0]=X0;
  double Y=0;
  for(int i=0; i<=filter->NP; i++) 
    Y+= filter->a[i]*filter->X[i] + filter->b[i]*filter->Y[i];

  filter->Y[0]=Y;
  if(DEBUG) {
    printf("\n[bw_low_pass]\n");
    for(int i=0; i<=filter->NP; i++) 
      printf("X[%d]: %.10lf\n", i, filter->X[i]);
    for(int i=0; i<=filter->NP; i++) 
      printf("Y[%d]: %.10lf\n", i, filter->Y[i]);

    printf("X: %.6lf - Y: %.6lf\n", X0, Y);
  }
   
  for(int i=filter->NP; i>0; i--) {
    filter->Y[i]=filter->Y[i-1];
    filter->X[i]=filter->X[i-1];
  }
  
  return(Y);
}

void free_bw_low_pass(ChebFilter* filter){
    free(filter);
}