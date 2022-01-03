#include "fractal.h"

Fractal::Fractal()
{
    bias_x=0;
    bias_y=0;
    scale=1;
    precision=1;
    angle=0;
    table_coef={{0.5,0,0,0.5,0,0,0.33},
                       {0.5,0,0,0.5,0,3000,0.33},
                {0.5,0,0,0.5,1500,1500,0.33},
                {0.0,0.0,0.0,0.0,0.0,0.0,0.0}};
}

Fractal::Fractal(QVector<QVector<float> > coef,float scl,float prec, int b_x, int b_y,int ang)
{
    bias_x=b_x;
    bias_y=b_y;
    scale=scl;
    precision=prec;
    angle=ang;
    table_coef=coef;
}

