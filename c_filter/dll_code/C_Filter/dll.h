#ifndef _DLL_H_
#define _DLL_H_

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif


DLLIMPORT void bandpass_filter(double* x, double* y, int x_length, double* a_bandpass, double* b_bandpass, int nfilt);

DLLIMPORT void bandstop_filter(double* x, double* y, int x_length, double* a_bandstop, double* b_bandstop, int nfilt);


#endif
