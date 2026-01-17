#include "Fir.h"

#include <numbers>
#include <cmath>

using namespace std;

double Fir::sinc(double x)
{
   return x==0 ? 1 : sin(numbers::pi *x) / (numbers::pi*x);
}
