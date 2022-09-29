#ifndef MAINWINDOW_H
#define MAINWINDOW_H

# include <QMainWindow>
# include <QFileDialog>
# include <iostream>
# include <math.h>
# include <random>
using namespace std;

# include <opencv2/highgui/highgui.hpp>
# include <opencv2/core/types.hpp>
# include <opencv2/core/core.hpp>
# include <opencv2/imgproc.hpp>
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Mat MatImgIn, kmeansData, kmeansResult, label, center;
    QImage QImgIn, QImgOut, QImgBarGray, QImgBarRGB;
    QString colorData01, colorData02;

    int imgCols, imgRows, pseudoColorTable[256][3];

    double checkPixel(double pixel);
    double LAB_hq_func(double q);
    void showColorBar();

private slots:
    void openImg();
    void RGB();
    void CMY();
    void HSI();
    void XYZ();
    void LAB();
    void YUV();
    void pseudoColor();
    void changeColor();
    void kMeans();
};

#endif // MAINWINDOW_H
