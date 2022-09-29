#ifndef MAINWINDOW_H
#define MAINWINDOW_H

# include <QMainWindow>

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
    Mat MatImgIn, MatImgInForFuse, MatForZero;
    QImage QImgIn, QImgOut;

    int imgCols, imgRows, QImgOutSizeX, QImgOutSizeY, pixelB, pixelG, pixelR, pixel;
    int maskSize, threshold, *forHough, houghI, houghJ;
    double a11, a12, a13, a21, a22, a23, a31, a32, a33, perspectI, perspectJ, perspectK;
    double BGRCoefficient;
    vector<double> coefficientsDouble;
    vector<int> imgRowData, imgColData;

private slots:
    void openImg();
    void openImgForFuse();
    void geometric();
    void wavelet();
    void fuse();
    void zeroCrossing();
    void hough();

    int pixelCheck(int pixel);
};

#endif // MAINWINDOW_H
