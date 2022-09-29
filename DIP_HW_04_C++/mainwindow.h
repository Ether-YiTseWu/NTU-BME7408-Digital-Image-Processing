#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

# include <QMainWindow>
# include <QFileDialog>
# include <iostream>
# include <time.h>
# include <math.h>
using namespace std;

# include <opencv2/highgui/highgui.hpp>
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

private slots:
    void openImg();
    void fftImg();
    void idealLowpass();
    void idealHighpass();

private:
    int imgCols, imgRows, fourierImgRows, fourierImgCols, R, centerX, centerY;
    double timeStart, timeEnd;
    QImage QImgIn, QImgFFT, QImgOut;
    Mat MatImgIn, MatImgInGray;
    Mat fourierPadded, fourierImg, InvFourierImg, fourierSpectrum, fourierSpectrumClone, fourierAngle;

    Mat QImage2Mat(const QImage& src);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
