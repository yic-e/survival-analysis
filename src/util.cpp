#include <iostream>
#include "util.h"
using namespace std;
double evalKernel(Kernels &kernel, double sigma, double t1, double t2){         
  double tmp = 0.0;                                                                
  switch (kernel){                                                                 
    case rbf:                                                                      
      tmp = abs(t1 - t2);                                                          
      return exp(-0.5 / sigma * tmp * tmp);                                        
      break;                                                                       
    case rbf_24h:                                                                  
      tmp = fmod(abs(t1 - t2), 24.0); // mod 1d, 24h                               
      if(tmp >= 12.0){                                                             
        tmp = 24.0 - tmp;                                                          
      }                                                                            
      return exp(-0.5 / sigma * tmp * tmp);                                        
      break;                                                                       
    case rbf_7d:                                                                   
      tmp = fmod(abs(t1 - t2), 168.0); // mod 168h (7d)                            
      if(tmp >= 84){                                                               
        tmp = 168 - tmp;                                                           
      }                                                                            
      return exp(-0.5 / sigma * tmp * tmp);                                        
      break;                                                                       
  }                                                                                
  assert(false); // should not reach here !!!                                   
  return -0.0;                                                                     
} 
