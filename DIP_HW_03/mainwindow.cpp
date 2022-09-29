#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Image Viewer");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openImg()
{
    // Open Image
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Images(*.bmp *.jpeg *.png *.jpg)"));                  // Get path
    MatImgIn = imread(filePath.toStdString());
    if (!MatImgIn.empty())
    {    
        imgCols = MatImgIn.cols;
        imgRows = MatImgIn.rows;

        // Set QImgIn
        QImgIn = QImage(imgCols, imgRows, QImage::Format_RGB32);
        for (int i = 0 ; i < imgCols ; i++)
        {
            for (int j = 0 ; j < imgRows ; j++)
            {
                int B = MatImgIn.at<Vec3b>(j,i)[0], G = MatImgIn.at<Vec3b>(j,i)[1], R = MatImgIn.at<Vec3b>(j,i)[2];
                QImgIn.setPixel(i, j, qRgb(R, G, B));
            }
        }

        // Show Image
        ui->showImgIn->setPixmap(QPixmap::fromImage(QImgIn.scaled(ui->showImgIn->width(),ui->showImgIn->height(),Qt::KeepAspectRatio)));   // Scale
    }
}

void MainWindow::convImg()
{
    // Set up variables
    maskSize = ui->maskSize->text().toInt();
    QImgOut = QImage(imgCols-maskSize+1, imgRows-maskSize+1, QImage::Format_RGB32);
    int index;

    // Set mask size
    coefficientsString = ui->maskCoefficients->text().toStdString();
    stringstream input(coefficientsString);
    coefficientsDouble.clear();
    while(input>>splitResult)   // Split coefficientsString and transform it to double
        coefficientsDouble.push_back(atof(splitResult.c_str()));

    // There are four "for" loops. The outside two "for" loops have the ability that find the central element of a mask
    for (int i = (maskSize+1)/2-1 ; i < imgCols-(maskSize-1)/2 ; i++)
    {
        for (int j = (maskSize+1)/2-1 ; j < imgRows-(maskSize-1)/2 ; j++)
        {
            // Regulate the variables
            pixelB = 0;
            pixelG = 0;
            pixelR = 0;
            index = maskSize*maskSize - 1;

            // The inside two "for" loops insert the mask elements in vector and do the convolution
            for (int x = i-(maskSize-1)/2 ; x < i-(maskSize-1)/2+maskSize ; x++)
            {
                for (int y = j-(maskSize-1)/2 ; y < j-(maskSize-1)/2+maskSize ; y++)
                {
                    double B = MatImgIn.at<Vec3b>(y,x)[0], G = MatImgIn.at<Vec3b>(y,x)[1], R = MatImgIn.at<Vec3b>(y,x)[2];  // Get pixels
                    BGRCoefficient = coefficientsDouble.at(index);

                    pixelB += (B * BGRCoefficient);
                    pixelG += (G * BGRCoefficient);
                    pixelR += (R * BGRCoefficient);
                    index --;
                }
            }
            // Check pixel
            pixelB = pixelCheck(pixelB);
            pixelG = pixelCheck(pixelG);
            pixelR = pixelCheck(pixelR);

            // Set pixel
            QImgOut.setPixel(i-(maskSize+1)/2+1, j-(maskSize+1)/2+1, qRgb(pixelR, pixelG, pixelB));
        }
    }
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));
}

void MainWindow::LoGImg()
{
    // Set up variables
    maskSize = 9;
    QImgOut = QImage(imgCols-maskSize+1, imgRows-maskSize+1, QImage::Format_RGB32);
    double log3DMask[9][9] = {{0, 0, 3, 2, 2, 2, 3, 0, 0},
                              {0, 2, 3, 5, 5, 5, 3, 2, 0},
                              {3, 3, 5, 3, 0, 3, 5, 3, 3},
                              {2, 5, 3, -12, -23, -12, 3, 5, 2},
                              {2, 5, 0, -23, -40, -23, 0, 5, 2},
                              {2, 5, 3, -12, -23, -12, 3, 5, 2},
                              {3, 3, 5, 3, 0, 3, 5, 3, 3},
                              {0, 2, 3, 5, 5, 5, 3, 2, 0},
                              {0, 0, 3, 2, 2, 2, 3, 0, 0}};

    // Set mask size
    coefficientsDouble.clear();
    for (int i = 0 ; i < maskSize ; i ++)
        for (int j = 0 ; j < maskSize ; j++)
            coefficientsDouble.push_back(log3DMask[i][j]);
    int index;

    // There are four "for" loops. The outside two "for" loops have the ability that find the central element of a mask
    for (int i = (maskSize+1)/2-1 ; i < imgCols-(maskSize-1)/2 ; i++)
    {
        for (int j = (maskSize+1)/2-1 ; j < imgRows-(maskSize-1)/2 ; j++)
        {
            // Regulate the variables
            pixelB = 0;
            pixelG = 0;
            pixelR = 0;
            index = maskSize*maskSize - 1;

            // The inside two "for" loops insert the mask elements in vector and do the convolution
            for (int x = i-(maskSize-1)/2 ; x < i-(maskSize-1)/2+maskSize ; x++)
            {
                for (int y = j-(maskSize-1)/2 ; y < j-(maskSize-1)/2+maskSize ; y++)
                {
                    double B = MatImgIn.at<Vec3b>(y,x)[0], G = MatImgIn.at<Vec3b>(y,x)[1], R = MatImgIn.at<Vec3b>(y,x)[2];  // Get pixels
                    BGRCoefficient = coefficientsDouble.at(index);

                    pixelB += (B * BGRCoefficient);
                    pixelG += (G * BGRCoefficient);
                    pixelR += (R * BGRCoefficient);
                    index --;
                }
            }
            // Check pixel
            pixelB = pixelCheck(pixelB);
            pixelG = pixelCheck(pixelG);
            pixelR = pixelCheck(pixelR);
            pixel = (pixelB + pixelG + pixelR)/3;

            QImgOut.setPixel(i-(maskSize+1)/2+1, j-(maskSize+1)/2+1, qRgb(pixel, pixel, pixel));
        }
    }
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));
}

void MainWindow::MarrHildrethImg()
{
    // Set up variables
    maskSize = 9;
    QImgOut = QImage(imgCols-maskSize+1, imgRows-maskSize+1, QImage::Format_RGB32);
    MatForZero = Mat(imgCols-maskSize+1, imgRows-maskSize+1, CV_64FC1);
    double log3DMask[9][9] = {{0, 0, 3, 2, 2, 2, 3, 0, 0},
                              {0, 2, 3, 5, 5, 5, 3, 2, 0},
                              {3, 3, 5, 3, 0, 3, 5, 3, 3},
                              {2, 5, 3, -12, -23, -12, 3, 5, 2},
                              {2, 5, 0, -23, -40, -23, 0, 5, 2},
                              {2, 5, 3, -12, -23, -12, 3, 5, 2},
                              {3, 3, 5, 3, 0, 3, 5, 3, 3},
                              {0, 2, 3, 5, 5, 5, 3, 2, 0},
                              {0, 0, 3, 2, 2, 2, 3, 0, 0}};

    // Set mask size
    coefficientsDouble.clear();
    for (int i = 0 ; i < maskSize ; i ++)
        for (int j = 0 ; j < maskSize ; j++)
            coefficientsDouble.push_back(log3DMask[i][j]);
    int index;

    // There are four "for" loops. The outside two "for" loops have the ability that find the central element of a mask
    for (int i = (maskSize+1)/2-1 ; i < imgCols-(maskSize-1)/2 ; i++)
    {
        for (int j = (maskSize+1)/2-1 ; j < imgRows-(maskSize-1)/2 ; j++)
        {
            // Regulate the variables
            pixelB = 0;
            pixelG = 0;
            pixelR = 0;
            index = maskSize*maskSize - 1;

            // The inside two "for" loops insert the mask elements in vector and do the convolution
            for (int x = i-(maskSize-1)/2 ; x < i-(maskSize-1)/2+maskSize ; x++)
            {
                for (int y = j-(maskSize-1)/2 ; y < j-(maskSize-1)/2+maskSize ; y++)
                {
                    double B = MatImgIn.at<Vec3b>(y,x)[0], G = MatImgIn.at<Vec3b>(y,x)[1], R = MatImgIn.at<Vec3b>(y,x)[2];  // Get pixels
                    BGRCoefficient = coefficientsDouble.at(index);

                    pixelB += (B * BGRCoefficient);
                    pixelG += (G * BGRCoefficient);
                    pixelR += (R * BGRCoefficient);
                    index --;
                }
            }
            pixelB = pixelCheck(pixelB);
            pixelG = pixelCheck(pixelG);
            pixelR = pixelCheck(pixelR);
            pixel = (pixelB + pixelG + pixelR)/3;

            MatForZero.at<double>(i-(maskSize+1)/2+1, j-(maskSize+1)/2+1) = pixel;
        }
    }

    // Find zero-crossing
    for (int i = 1 ; i < imgCols-maskSize ; i++)
    {
        for (int j = 1 ; j < imgRows-maskSize ; j++)
        {
            if ((MatForZero.at<double>(i - 1, j) * MatForZero.at<double>(i + 1, j) <= 0) || (MatForZero.at<double>(i, j + 1) * MatForZero.at<double>(i, j - 1) <= 0) || (MatForZero.at<double>(i + 1, j - 1) * MatForZero.at<double>(i - 1, j + 1) <= 0) || (MatForZero.at<double>(i - 1, j - 1) * MatForZero.at <double> (i + 1, j + 1) <= 0))
                QImgOut.setPixel(i, j, qRgb(255, 255, 255));
            else
                QImgOut.setPixel(i, j, qRgb(0, 0, 0));
        }
    }

    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));
}

void MainWindow::medianImg()
{
    // Set up variables
    orderMaskSize = ui->maskSizeOrder->text().toInt();
    QImgOut = QImage(imgCols-orderMaskSize+1, imgRows-orderMaskSize+1, QImage::Format_RGB32);

    // There are four "for" loops. The outside two "for" loops have the ability that find the central element of a mask
    for (int i = (orderMaskSize+1)/2-1 ; i < imgCols-(orderMaskSize-1)/2 ; i++)
    {
        for (int j = (orderMaskSize+1)/2-1 ; j < imgRows-(orderMaskSize-1)/2 ; j++)
        {
            // The inside two "for" loops insert the mask elements in vector
            for (int x = i-(orderMaskSize-1)/2 ; x < i-(orderMaskSize-1)/2+orderMaskSize ; x++)
            {
                for (int y = j-(orderMaskSize-1)/2 ; y < j-(orderMaskSize-1)/2+orderMaskSize ; y++)
                {
                    int B = MatImgIn.at<Vec3b>(y,x)[0], G = MatImgIn.at<Vec3b>(y,x)[1], R = MatImgIn.at<Vec3b>(y,x)[2];  // Get pixels
                    pixelSeriesB.push_back(B);
                    pixelSeriesG.push_back(G);
                    pixelSeriesR.push_back(R);
                }
            }
            // Sorting
            sort(pixelSeriesR.begin(), pixelSeriesR.end());
            sort(pixelSeriesG.begin(), pixelSeriesG.end());
            sort(pixelSeriesB.begin(), pixelSeriesB.end());

            // Set pixel
            pixelB = pixelSeriesB.at((pixelSeriesB.size()+1)/2);
            pixelG = pixelSeriesG.at((pixelSeriesG.size()+1)/2);
            pixelR = pixelSeriesR.at((pixelSeriesR.size()+1)/2);
            QImgOut.setPixel(i-(orderMaskSize+1)/2+1, j-(orderMaskSize+1)/2+1, qRgb(pixelR, pixelG, pixelB));

            // Clear Vector
            pixelSeriesB.clear();
            pixelSeriesG.clear();
            pixelSeriesR.clear();
        }
    }
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));
}

void MainWindow::minImg()
{
    // Set up variables
    orderMaskSize = ui->maskSizeOrder->text().toInt();
    QImgOut = QImage(imgCols-orderMaskSize+1, imgRows-orderMaskSize+1, QImage::Format_RGB32);

    // There are four "for" loops. The outside two "for" loops have the ability that find the central element of a mask
    for (int i = (orderMaskSize+1)/2-1 ; i < imgCols-(orderMaskSize-1)/2 ; i++)
    {
        for (int j = (orderMaskSize+1)/2-1 ; j < imgRows-(orderMaskSize-1)/2 ; j++)
        {
            // The inside two "for" loops insert the mask elements in vector
            for (int x = i-(orderMaskSize-1)/2 ; x < i-(orderMaskSize-1)/2+orderMaskSize ; x++)
            {
                for (int y = j-(orderMaskSize-1)/2 ; y < j-(orderMaskSize-1)/2+orderMaskSize ; y++)
                {
                    int B = MatImgIn.at<Vec3b>(y,x)[0], G = MatImgIn.at<Vec3b>(y,x)[1], R = MatImgIn.at<Vec3b>(y,x)[2];  // Get pixels
                    pixelSeriesB.push_back(B);
                    pixelSeriesG.push_back(G);
                    pixelSeriesR.push_back(R);
                }
            }
            // Sorting
            sort(pixelSeriesR.begin(), pixelSeriesR.end());
            sort(pixelSeriesG.begin(), pixelSeriesG.end());
            sort(pixelSeriesB.begin(), pixelSeriesB.end());

            // Set pixel
            pixelB = pixelSeriesB.at(0);
            pixelG = pixelSeriesG.at(0);
            pixelR = pixelSeriesR.at(0);
            QImgOut.setPixel(i-(orderMaskSize+1)/2+1, j-(orderMaskSize+1)/2+1, qRgb(pixelR, pixelG, pixelB));

            // Clear Vector
            pixelSeriesB.clear();
            pixelSeriesG.clear();
            pixelSeriesR.clear();
        }
    }
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));
}

void MainWindow::maxImg()
{
    // Set up variables
    orderMaskSize = ui->maskSizeOrder->text().toInt();
    QImgOut = QImage(imgCols-orderMaskSize+1, imgRows-orderMaskSize+1, QImage::Format_RGB32);

    // There are four "for" loops. The outside two "for" loops have the ability that find the central element of a mask
    for (int i = (orderMaskSize+1)/2-1 ; i < imgCols-(orderMaskSize-1)/2 ; i++)
    {
        for (int j = (orderMaskSize+1)/2-1 ; j < imgRows-(orderMaskSize-1)/2 ; j++)
        {
            // The inside two "for" loops insert the mask elements in vector
            for (int x = i-(orderMaskSize-1)/2 ; x < i-(orderMaskSize-1)/2+orderMaskSize ; x++)
            {
                for (int y = j-(orderMaskSize-1)/2 ; y < j-(orderMaskSize-1)/2+orderMaskSize ; y++)
                {
                    int B = MatImgIn.at<Vec3b>(y,x)[0], G = MatImgIn.at<Vec3b>(y,x)[1], R = MatImgIn.at<Vec3b>(y,x)[2];  // Get pixels
                    pixelSeriesB.push_back(B);
                    pixelSeriesG.push_back(G);
                    pixelSeriesR.push_back(R);
                }
            }
            // Sorting
            sort(pixelSeriesR.begin(), pixelSeriesR.end());
            sort(pixelSeriesG.begin(), pixelSeriesG.end());
            sort(pixelSeriesB.begin(), pixelSeriesB.end());

            // Set pixel
            pixelB = pixelSeriesB.at(pixelSeriesB.size()-1);
            pixelG = pixelSeriesG.at(pixelSeriesG.size()-1);
            pixelR = pixelSeriesR.at(pixelSeriesR.size()-1);
            QImgOut.setPixel(i-(orderMaskSize+1)/2+1, j-(orderMaskSize+1)/2+1, qRgb(pixelR, pixelG, pixelB));

            // Clear Vector
            pixelSeriesB.clear();
            pixelSeriesG.clear();
            pixelSeriesR.clear();
        }
    }
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));
}

int MainWindow::pixelCheck(int index)
{
    if (index > 255)
        index = 255;
    else if (index < 0)
        index = 0;

    return index;
}
