#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QSlider>
#include <QComboBox>
#include <QTableWidget>
#include <QSignalMapper>
#include <QVector>
#include <QTimer>
#include <math.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    resize(1200,900);
    srand(time(NULL));

    create_list();
    frac=List_of_fractals["Davis"];

    connect(&timer, &QTimer::timeout, this, &Widget::draw);

    QVBoxLayout *main_box=new QVBoxLayout(this);

    createSpinBoxbar(*main_box);

    QHBoxLayout *botton_box=new QHBoxLayout();

    QPushButton *run=new QPushButton("Run",this);
    connect(run,&QPushButton::clicked,this,&Widget::onBtm);
    botton_box->addWidget(run);

    QPushButton *stop=new QPushButton("stop",this);
    connect(stop,&QPushButton::clicked,this,&Widget::Stop);
    botton_box->addWidget(stop);

    dock.resize(300,200);
    createHideBar();
    QPushButton *settings=new QPushButton("More settings",this);
    connect(settings,&QPushButton::clicked,this,&Widget::showHideBar);
    botton_box->addWidget(settings);

    main_box->addLayout(botton_box);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *e) {

    Q_UNUSED(e);

    QPainter painter(this);

    QPen *pen=new QPen(Qt::black, 1);
    painter.setPen(*pen);
    painter.translate(frac.bias_x,frac.bias_y);
    painter.scale(frac.scale,frac.scale);
    painter.rotate(frac.angle);

    for(auto point:points){
        painter.setPen(QPen(point.color,1));
        painter.drawPoint(point.point);
        }

}

void Widget::createFractal(int count=1000)
{
    //при маленьких значениях count(<10) теряется точность прорисовки фрактала из-за инициалзации x,y каждый раз 0
    int length=points.size();
    float x=0,y=0;
    if(length>=max_iter)
    {
        return;
    }

    int precision=frac.precision;

    QVector<QVector<float>> param=frac.table_coef;

    QVector<float> probabilities;
    probabilities.append(param[0][param[0].length()-1]);

    for(int i=1;i<param.length();i++)
    {
        probabilities.append(param[i][param[0].length()-1]+probabilities[i-1]);
    }

    for (int iter=0;iter<count;iter++) {
        float p = rand()/(double)(RAND_MAX);

        int choice=0;
        for(int i=0;i<probabilities.length()-1;i++){
            if ((p>probabilities[i]) && (p<=probabilities[i+1])){
                choice=i+1;
                break;
            }
        }

        QVector<float> coef_transform=param[choice];
        float temp_x=x;
        float temp_y=y;
        x=coef_transform[0]*temp_x+coef_transform[1]*temp_y+coef_transform[4];
        y=coef_transform[2]*temp_x+coef_transform[3]*temp_y+coef_transform[5];

        QColor color(fmin(255,abs(x)*grad),fmin(255,abs(y)*grad),fmin(255,abs(x+y)*grad));
        points.append({QPointF(x*precision,y*precision),color});
    }
}

void Widget::createSpinBoxbar(QVBoxLayout &main_box)
{
    QHBoxLayout *box_names=new QHBoxLayout();
    QVector<QString> names={"bias_x","bias_y","scale","size","angle","speed"};

    QFont font("Times",12);

    for(auto name:names)
    {
        QLabel *label=new QLabel(name);
        label->setAlignment(Qt::AlignCenter);
        label->setFont(font);
        box_names->addWidget(label);
    }

    box_bar=new QHBoxLayout();

    QDoubleSpinBox *bias_x = new QDoubleSpinBox();
    bias_x->setRange(0,width());
    bias_x->setSingleStep(20);
    bias_x->setValue(frac.bias_x);
    bias_x->setDecimals(0);
    bias_x->setFont(font);
    connect(bias_x,SIGNAL(valueChanged(double)),this,SLOT(setBias_x(double)));
    box_bar->addWidget(bias_x);
    List_spin.append(bias_x);

    QDoubleSpinBox *bias_y = new QDoubleSpinBox();
    bias_y->setRange(0,height());
    bias_y->setSingleStep(20);
    bias_y->setValue(frac.bias_y);
    bias_y->setDecimals(0);
    bias_y->setFont(font);
    connect(bias_y,SIGNAL(valueChanged(double)),this,SLOT(setBias_y(double)));
    box_bar->addWidget(bias_y);
    List_spin.append(bias_y);

    QDoubleSpinBox *scale = new QDoubleSpinBox();
    scale->setRange(0,10000);
    scale->setSingleStep(1);
    scale->setValue(frac.scale);
    scale->setFont(font);
    connect(scale,SIGNAL(valueChanged(double)),this,SLOT(setScale(double)));
    box_bar->addWidget(scale);
    List_spin.append(scale);

    QDoubleSpinBox *precision = new QDoubleSpinBox();
    precision->setRange(0,10000);
    precision->setSingleStep(1);
    precision->setValue(frac.precision);
    precision->setFont(font);
    connect(precision,SIGNAL(valueChanged(double)),this,SLOT(setPrecision(double)));
    box_bar->addWidget(precision);
    List_spin.append(precision);

    QDoubleSpinBox *angle = new QDoubleSpinBox();
    angle->setRange(0,360);
    angle->setSingleStep(5);
    angle->setValue(frac.angle);
    angle->setDecimals(0);
    angle->setWrapping(true);
    angle->setFont(font);
    connect(angle,SIGNAL(valueChanged(double)),this,SLOT(setAngle(double)));
    box_bar->addWidget(angle);
    List_spin.append(angle);

    QSpinBox *count = new QSpinBox();
    count->setRange(10,10000);
    count->setSingleStep(100);
    count->setValue(speed);
    count->setFont(font);
    connect(count,SIGNAL(valueChanged(int)),this,SLOT(setSpeed(int)));
    box_bar->addWidget(count);

    main_box.setAlignment(Qt::AlignBottom);
    main_box.addLayout(box_names);
    main_box.addLayout(box_bar);
}

void Widget::createHideBar()
{
    QFont font("Times",12);

    QVBoxLayout *hide_bar = new QVBoxLayout();

    QVector<QString> names={"max_iteration","gradient","Fractal","parameters"};
    QVector<QLabel*> label_names;
    for(auto name :names)
    {
        QLabel *label = new QLabel(name);
        label->setAlignment(Qt::AlignCenter);
        label->setFont(font);
        label_names.append(label);
    }

    hide_bar->addWidget(label_names[0]);
    QSpinBox *max_iteration = new QSpinBox();
    max_iteration->setRange(100,1000000);
    max_iteration->setSingleStep(10000);
    max_iteration->setValue(max_iter);
    max_iteration->setFont(font);
    connect(max_iteration,SIGNAL(valueChanged(int)),this,SLOT(setMaxIter(int)));
    hide_bar->addWidget(max_iteration);

    hide_bar->addWidget(label_names[1]);
    QSpinBox *gradation = new QSpinBox();
    gradation->setRange(0,500);
    gradation->setSingleStep(5);
    gradation->setValue(grad);
    gradation->setFont(font);
    connect(gradation,SIGNAL(valueChanged(int)),this,SLOT(setGrad(int)));
    hide_bar->addWidget(gradation);

    hide_bar->addWidget(label_names[2]);
    QComboBox *fractals=new QComboBox();
    fractals->insertItem(0,"MyFractal");
    fractals->insertItems(1,List_of_fractals.keys());
    fractals->setFont(font);
    connect(fractals,SIGNAL(currentTextChanged(QString)),this,SLOT(setFractal(QString)));
    hide_bar->addWidget(fractals);

    QVector<QString> param_names={"a","b","c","d","e","f","probability"};
    QHBoxLayout *box_param_names=new QHBoxLayout();
    for(auto name :param_names)
    {
        QLabel *label = new QLabel(name);
        label->setAlignment(Qt::AlignCenter);
        label->setFont(font);
        box_param_names->addWidget(label);
    }
    hide_bar->addLayout(box_param_names);

    QTableWidget *table=new QTableWidget(4,7);
    int width=frac.table_coef[0].size();
    for(int i=0;i<frac.table_coef.size();i++)
    {
        for(int j=0;j<frac.table_coef[i].size();j++)
        {
            QDoubleSpinBox *item=new QDoubleSpinBox();
            item->setRange(-10000,10000);
            item->setSingleStep(1);
            item->setValue(frac.table_coef[i][j]);
            item->setFont(font);
            table->setCellWidget(i,j,item);

            QSignalMapper *mapper = new QSignalMapper(this);
            mapper->setMapping(item, i*width+j);
            connect(item, SIGNAL(valueChanged(double)), mapper,SLOT(map()));
            connect(mapper, SIGNAL(mapped(int)),this,SLOT(setParams(int)));

            List_params.append(item);
        }
    }

    hide_bar->addWidget(table);

    QWidget *multiWidget = new QWidget();
    multiWidget->setLayout(hide_bar);
    dock.setWidget(multiWidget);

}

void Widget::onBtm()
{
    points.clear();
    timer.start(0);
}

void Widget::Stop()
{
    timer.stop();
}

void Widget::showHideBar()
{
    dock.show();
}

void Widget::draw()
{
    createFractal(speed);
    repaint();
}

void Widget::setBias_x(double d)
{
    frac.bias_x=d;
}

void Widget::setBias_y(double d)
{
    frac.bias_y=d;
}

void Widget::setScale(double d)
{
    frac.scale=d;
}

void Widget::setPrecision(double d)
{
    frac.precision=d;
}

void Widget::setAngle(double d)
{
    frac.angle=d;
}

void Widget::setSpeed(int i)
{
    speed=i;
}

void Widget::setMaxIter(int i)
{
    max_iter=i;
}

void Widget::setGrad(int i)
{
    grad=i;
}

void Widget::setFractal(QString text)
{
    points.clear();
    frac=List_of_fractals[text];

    List_spin[0]->setValue(frac.bias_x);
    List_spin[1]->setValue(frac.bias_y);
    List_spin[2]->setValue(frac.scale);
    List_spin[3]->setValue(frac.precision);
    List_spin[4]->setValue(frac.angle);

    int i=0;
    int j=0;
    for(auto coef:List_params)
    {
        coef->setValue(frac.table_coef[i][j]);
        j++;
        if(j>=7)
        {
            j=0;
            i++;
        }
    }
}

void Widget::setParams(int id)
{
    int width=frac.table_coef[0].size();
    int column=id%width;
    int row=id/width;
    frac.table_coef[row][column]=List_params[id]->value();
}

void Widget::create_list()
{
    List_of_fractals["Triengle_Sierpinski"]=Fractal({{0.5,0,0,0.5,0,0,0.33},
                                         {0.5,0,0,0.5,0,3000,0.33},
                                         {0.5,0,0,0.5,1500,1500,0.33},
                                         {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.00}},
                                                    0.15,1,0,0);

    List_of_fractals["Curl"]=Fractal({{0.04,	0.22,	0.31,	-0.03,	0.63,	-1.74,	0.13},
                                       {-0.02,	0.00,	-0.32,	0.26,	-0.17,	-1.35,	0.01},
                                       {0.79,	0.06,	-0.03,	0.73,	-0.02,	1.03,	0.74},
                                       {-0.03,	-0.30,	0.35,	-0.04,	-0.68,	-0.94,	0.12}},
                                                    3,40,width()/2,height()/3);

    List_of_fractals["Tree"]=Fractal({{0.0,	0.0,	0.0,	0.5,	0.0,	0.0,	0.05},
                                      {0.42,	-0.42,	0.42,	0.42,	0.0,	0.2,	0.4},
                                      {0.42,	0.42,	-0.42,	0.42,	0.0,	0.2,	0.4},
                                      {0.1,	0.0,	0.0,	0.1,	0.0,	0.2,	0.15}},
                                                    7,200,width()/2,height()/4);

    List_of_fractals["Davis"]=Fractal({{-0.50,	-0.30,	0.30,	-0.50,	-6.07,	-3.58,	0.33},
                                      {-0.50,	-0.29,	0.29,	-0.50,	5.92,	3.50,	0.33},
                                      {0.00,	-0.59,	0.59,	0.00,	-0.09,	-0.04,	0.33},
                                      {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.00}},
                                                    6,6,width()/2,height()/2);

    List_of_fractals["Leaf"]=Fractal({{0.14,	0.01,	0.00,	0.51,	-0.08,	-1.31,	0.06},
                                       {0.43,	0.52,	-0.45,	0.50,	1.49,	-0.75,	0.37},
                                       {0.45,	-0.49,	0.47,	0.47,	-1.62,	-0.74,	0.36},
                                       {0.49,	0.00,	0.00,	0.51,	0.02,	1.62,	0.21}},
                                                    5,25,width()/2,height()/2);

    List_of_fractals["Binary"]=Fractal({{0.5,	0.0,	0.0,	0.5,	-2.563477,	-0.000003,	0.333333},
                                       {0.5,	0.0,	0.0,	0.5,	2.436544,	-0.000003,	0.333333},
                                       {0.0,	-0.5,	0.5,	0.0,	4.873085,	7.563492,	0.333333},
                                       {0.0,	0.0,    0.0,	0.0,	0.0,	0.0,	0.0}},
                                                    2.5,25,width()/3,height()/4);

    List_of_fractals["Coral"]=Fractal({{0.307692,	-0.531469,	-0.461538,	-0.293706,	5.401953,	8.655175,	0.40},
                                       {0.307692,	-0.076923,	0.153846,	-0.447552,	-1.295248,	4.152990,	0.15},
                                       {0.0,	0.545455,	0.692308,	-0.195804,	-4.893637,	7.269794,	0.45},
                                       {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0}},
                                                    2.5,25,width()/3,height()/4);

    List_of_fractals["Crystal"]=Fractal({{0.696970,	-0.481061,	-0.393939,	-0.662879,	2.147003,	10.310288,	0.747826},
                                         {0.090909,	-0.443182,	0.515152,	-0.094697,	4.286558,	2.925762,	0.252174},
                                         {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
                                         {0.0,	0.0,	0.0,	0.0,	0.0,	0.0, 0.0}},
                                                    2,30,width()/2,height()/6);

    List_of_fractals["Dragon"]=Fractal({{0.824074,	0.281482,	-0.212346,	0.864198,	-1.882290,	-0.110607,	0.787473},
                                        {0.088272,	0.520988,	-0.463889,	-0.377778,	0.785360,	8.095795,	.212527},
                                        {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
                                        {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0}},
                                                    2,30,width()/2,height()/5);

    List_of_fractals["Fern"]=Fractal({{0.0,	0.0,	0.0,	0.16,	0.0,	0.00,	0.01},
                                      {0.85,	0.04,	-0.04,	0.85,	0.0,	1.6,	0.85},
                                      {0.2,	-0.26,	0.23,	0.22,	0.0,	1.6,	0.07},
                                      {-0.15,	0.28,	0.26,	0.24,	0.0,	0.44,	0.07}},
                                                    2,30,width()/2,height()/5);

    List_of_fractals["Floor"]=Fractal({{0.0,	-0.5,	0.5,	0.0,	-1.732366,	3.366182,	0.333333},
                                       {0.5,	0.0,	0.0,	0.5,	-0.027891,	5.014877,	0.333333},
                                       {0.0,	0.5,	-0.5,	0.0,	1.620804,	3.310401,	0.333333},
                                       {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0}},
                                                    2,30,width()/2,height()/5);

    List_of_fractals["Koch"]=Fractal({{0.3333,	0.0,	0.0,	0.3333,	0.0,	0.0,	0.25},
                                      {0.1667,	-0.2887,	0.2887,	0.1667,	0.3333,	0.0,	0.25},
                                      {-0.1667,	0.2887,	0.2887,	0.1667,	0.6666,	0.0,	0.25},
                                      {0.3333,	0.0,	0.0,	0.3333,	0.6666,	0.0,	0.25}},
                                                    8,100,width()/6,height()/2);

    List_of_fractals["Spiral"]=Fractal({{0.787879,	-.424242,	.242424,	0.859848,	1.758647,	1.408065,	0.895652},
                                        {-0.121212,	0.257576,	0.151515,	0.053030,	-6.721654,	1.377236,	0.052174},
                                        {.181818,	-.136364,	.090909,	.181818,	6.086107,	1.568035,	.052174},
                                        {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0}},
                                                    2,30,width()/2,height()/5);

    List_of_fractals["Swirl"]=Fractal({{0.745455,	-0.459091,	0.406061,	0.887121,	1.460279,	0.691072,	0.912675},
                                       {-0.424242,	-0.065152,	-0.175758,	-0.218182,	3.809567,	6.741476,	0.087325},
                                       {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
                                       {0.0,	0.0,	0.0,	0.0,	0.0,	0.0	,0.0}},
                                                    2,30,width()/2,height()/5);

    List_of_fractals["Triangle"]=Fractal({{-0.40,	0.00,	0.00,	-0.40,	0.24,	0.37,	0.23},
                                          {0.50,	0.00,	0.00,	0.50,	-1.37,	0.25,	0.36},
                                          {0.21,	0.00,	0.00,	0.21,	1.00,	1.47,	0.06},
                                          {0.50,	0.00,	0.00,	0.50,	0.76,	-1.16,	0.36}},
                                                    3,50,width()/2,height()/2);

    List_of_fractals["Zigzag"]=Fractal({{-0.632407,	-0.614815,	-0.545370,	0.659259,	3.840822,	1.282321,	0.888128},
                                        {-0.036111,	0.444444,	0.210185,	0.037037,	2.071081,	8.330552,	0.111872},
                                        {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
                                        {0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	0.0}},
                                                    2,30,width()/2,height()/5);

    List_of_fractals["Koch2"]=Fractal({{0.307692,	0.0,	0.0,	0.294118,	4.119164,	1.604278,	0.2},
                                       {0.192308,	-0.205882,	0.653846,	0.088235,	-0.688840,	5.978916,	0.3},
                                       {-0.192308,	0.205882,	0.653846,	0.088235,	0.668580,	5.962514,	0.3},
                                       {0.307692,	0.0,	0.0,	0.294118,	-4.136530,	1.604278,	0.2}},
                                                    2,30,width()/2,height()/6);
}
