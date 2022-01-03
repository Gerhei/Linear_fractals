#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <fractal.h>
#include <QMap>
#include <QTimer>
#include <QGridLayout>
#include <QDockWidget>
#include <QSpinBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

struct Point{
    QPointF point;
    QColor color;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QMap<QString,Fractal> List_of_fractals;
private:
    Ui::Widget *ui;
    QDockWidget dock;   
    QHBoxLayout *box_bar;

    QVector<QDoubleSpinBox*> List_spin;
    QVector<QDoubleSpinBox*> List_params;

    QTimer timer;
    Fractal frac;//хранит информацию о фрактале(таблицу коэффициентов, смещение, масштаб)
    QVector<Point> points;
    int max_iter=100000;
    int speed=500;//точек за тик таймера
    int grad=30;
    //QVector<QColor> color_list={Qt::green,Qt::red,Qt::blue,Qt::black};//for debug
    void create_list();
    void createFractal(int count);
    void createSpinBoxbar(QVBoxLayout&);
    void createHideBar();

private slots:
    void onBtm();
    void Stop();
    void showHideBar();
    void draw();

    void setBias_x(double);
    void setBias_y(double);
    void setScale(double);
    void setPrecision(double);
    void setAngle(double);
    void setSpeed(int);

    void setMaxIter(int);
    void setGrad(int);
    void setFractal(QString);
    void setParams(int);
protected:
    void paintEvent(QPaintEvent *event);

};
#endif // WIDGET_H
