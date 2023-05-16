#include <math.h>
#include <main.h>
#include "system.h"
#include "CalcDoseRate.h"

float CpsToUsv_h(float parama, float paramb, float paramc, float count)
{   
    float ret = paramc * parama * count * exp(paramb*count);

    return ret;
}