/* Replace "dll.h" with the name of your header */
#include "dll.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "c_filter.h" 


DLLIMPORT void bandpass_filter(double* x, double* y, int x_length, double* a_bandpass, double* b_bandpass, int nfilt)
{
    // 应用filtfilt进行带通滤波
    filtfilt(x, y, x_length, a_bandpass, b_bandpass, nfilt);
}


DLLIMPORT void bandstop_filter(double* x, double* y, int x_length, double* a_bandstop, double* b_bandstop, int nfilt)
{
    // 应用filtfilt进行带阻滤波
    filtfilt(x, y, x_length, a_bandstop, b_bandstop, nfilt);
}

BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        break;

      case DLL_PROCESS_DETACH:
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}