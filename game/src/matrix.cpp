#include "matrix.h"

Matrix makeMatrix(float e11, float e12, float e13, float e14,
                  float e21, float e22, float e23, float e24,
                  float e31, float e32, float e33, float e34,
                  float e41, float e42, float e43, float e44)
{
    return Matrix{
        e11, e21, e31, e41,
        e12, e22, e32, e42,
        e13, e23, e33, e43,
        e14, e24, e34, e44
    };
}
