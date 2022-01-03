#ifndef FRACTAL_H
#define FRACTAL_H
#include <QVector>
#include <QWidget>

class Fractal
{
public:
    Fractal();
    Fractal(QVector<QVector<float>>,float scl=1,float prec=1,int b_x=0,int b_y=0,int ang=0);
    QVector<QVector<float>> table_coef={};
    int bias_x;
    int bias_y;
    float scale;
    float precision;
    int angle;
};

#endif // FRACTAL_H
