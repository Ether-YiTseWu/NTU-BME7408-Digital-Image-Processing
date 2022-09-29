#ifndef MAINWINDOW_H
#define MAINWINDOW_H

# include <QMainWindow>
# include <QFileDialog>
# include <QtCharts>
# include <QtCharts/QChartView>
# include <QtCharts/QBarSeries>
# include <QtCharts/QBarSet>
# include <QtCharts/QLegend>
# include <QtCharts/QBarCategoryAxis>

# include <iostream>
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

private:
    Ui::MainWindow *ui;
    Mat MatImgIn;
    QImage QimgIn, QimgOut;
    QString hisTitle;
    int pixelCount[256], imgCols, imgRows, *imgOutPixel;
    double equalizationTable[256];

    void showOutput(int *pixelCount, QString title);
    int checkPixel(int pixel);
    Mat QImage2Mat(const QImage& src);

private slots:
    void openImg();
    void grayAImg();
    void grayBImg();
    void getGrayChange();
    void autoContrast();
    void thresholdImg();
    void brightnessImg();
    void contrastImg();
    void grayQuantization();
    void enlargeImg();
};

#endif // MAINWINDOW_H
