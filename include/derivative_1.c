#include <math.h>

// f'(x) = (f(A + deltaX) – f(A))/deltaX

float Derivative(float A, float deltaX){
    float der = (cosf(A + deltaX) - cosf(A)) / deltaX;
    return der;
}