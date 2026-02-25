#ifndef C_FILTER_H 
#define C_FILTER_H 

void filter(const double* x, double* y, int xlen, double* a, double* b, int nfilt, double* zi);
void trmul(double *a, double *b, double *c, int m, int n, int k);
int rinv(double *a, int n);
int filtfilt(double* x, double* y, int xlen, double* a, double* b, int nfilt);
  
#endif