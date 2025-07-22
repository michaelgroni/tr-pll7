#include "Fir.h"

#include <cmath>

using namespace std;

double Fir::sinc(double x)
{
   return x==0 ? 1 : sin(M_PI*x) / (M_PI*x);
}
