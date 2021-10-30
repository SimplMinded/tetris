#pragma once

struct Matrix
{
    float elems[16];
};

Matrix makeMatrix(float e11, float e12, float e13, float e14,
                  float e21, float e22, float e23, float e24,
                  float e31, float e32, float e33, float e34,
                  float e41, float e42, float e43, float e44);
