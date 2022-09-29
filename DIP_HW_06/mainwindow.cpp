#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("DIP_HW_06");
    QImgOutSizeX = 0;
    QImgOutSizeY = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openImg()
{
    // Open Image
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Images(*.bmp *.jpeg *.png *.jpg)"));   // Get path
    MatImgIn = imread(filePath.toStdString());
    if (!MatImgIn.empty())
    {
        // Set up variables
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

void MainWindow::openImgForFuse()
{
    // Open Image
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Images(*.bmp *.jpeg *.png *.jpg)"));   // Get path
    MatImgInForFuse = imread(filePath.toStdString());
    if (!MatImgInForFuse.empty())
    {
        // Show Image
        imshow("Img for fusing", MatImgInForFuse);
    }
}

void MainWindow::geometric()
{
    // Set up variables
    a11 = ui->a11->text().toDouble();
    a12 = ui->a12->text().toDouble();
    a13 = ui->a13->text().toDouble();
    a21 = ui->a21->text().toDouble();
    a22 = ui->a22->text().toDouble();
    a23 = ui->a23->text().toDouble();
    a31 = ui->a31->text().toDouble();
    a32 = ui->a32->text().toDouble();
    a33 = ui->a33->text().toDouble();

    // Determine the proper QImgOut Size
    if (a31 == 0.0 && a32 ==0.0)    // Affine Transformation
    {
        QImgOutSizeX = abs(imgCols*a11) + abs(imgRows*a12) + abs(a13);
        QImgOutSizeY = abs(imgCols*a21) + abs(imgRows*a22) + abs(a23);
        QImgOut = QImage(QImgOutSizeX, QImgOutSizeY, QImage::Format_RGB32);
    }
    else
    {
        for (int i = 0 ; i < imgCols ; i++)
        {
            for (int j = 0 ; j < imgRows ; j++)
            {
                pixelB = MatImgIn.at<Vec3b>(j,i)[0];
                pixelG = MatImgIn.at<Vec3b>(j,i)[1];
                pixelR = MatImgIn.at<Vec3b>(j,i)[2];
                perspectI = abs(i*a11) + abs(j*a12) + abs(a13);
                perspectJ = abs(i*a21) + abs(j*a22) + abs(a23);
                perspectK = abs(i*a31) + abs(j*a32) + abs(a33);
                perspectI /= perspectK;
                perspectJ /= perspectK;
                if (QImgOutSizeX < perspectI)
                    QImgOutSizeX = int(perspectI);
                if (QImgOutSizeY < perspectJ)
                    QImgOutSizeY = int(perspectJ);
            }
        }
        QImgOut = QImage(QImgOutSizeX+1, QImgOutSizeY+1, QImage::Format_RGB32);
    }

    // Do geometric transform
    for (int i = 0 ; i < imgCols ; i++)
    {
        for (int j = 0 ; j < imgRows ; j++)
        {
            pixelB = MatImgIn.at<Vec3b>(j,i)[0];
            pixelG = MatImgIn.at<Vec3b>(j,i)[1];
            pixelR = MatImgIn.at<Vec3b>(j,i)[2];
            perspectI = abs(i*a11) + abs(j*a12) + abs(a13);
            perspectJ = abs(i*a21) + abs(j*a22) + abs(a23);
            perspectK = abs(i*a31) + abs(j*a32) + abs(a33);
            perspectI /= perspectK;
            perspectJ /= perspectK;
            QImgOut.setPixel(int(perspectI), int(perspectJ), qRgb(pixelR, pixelG, pixelB));
        }
    }

    // Show image
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));   // Scale
}

void MainWindow::wavelet()
{
    // Set up variables
    QImgOut = QImage(imgCols, imgRows, QImage::Format_RGB32);

    // Do wavelet transform
    for (int i = 0 ; i < imgCols ; i++)
    {
        // Set up imgRowData
        imgRowData.clear();
        imgRowData.push_back(0);

        // Do 1-D convolution
        for (int j = 0 ; j < imgRows ; j++)
        {
            pixelB = MatImgIn.at<Vec3b>(j,i)[0];
            pixelG = MatImgIn.at<Vec3b>(j,i)[1];
            pixelR = MatImgIn.at<Vec3b>(j,i)[2];
            pixel = (pixelB + pixelG + pixelR)/3;
            imgRowData.push_back(pixel);
        }

        for (int k = 0 ; k < imgRows ; k++)
        {
            //imgRowData.at(k) + imgRowData.at(k+1);
        }
    }

    // Show image
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));   // Scale
}

void MainWindow::fuse()
{
    //MatImgInForFuse;
}

void MainWindow::zeroCrossing()
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

    // Use zero-crossing to find edge information
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

    // Show image
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));   // Scale
}

void MainWindow::hough()
{
    // Set up variables
    threshold = ui->threshold->text().toInt();
    forHough = new int[(imgCols-maskSize)*(imgRows-maskSize)];
    for (int i = 0; i < (imgCols-maskSize)*(imgRows-maskSize) ; i++)
        forHough[i] = 0;

    // Vote
    for (int i = 1 ; i < imgCols-maskSize ; i++)
    {
        for (int j = 1 ; j < imgRows-maskSize ; j++)
        {
            if ((MatForZero.at<double>(i - 1, j) * MatForZero.at<double>(i + 1, j) <= 0) || (MatForZero.at<double>(i, j + 1) * MatForZero.at<double>(i, j - 1) <= 0) || (MatForZero.at<double>(i + 1, j - 1) * MatForZero.at<double>(i - 1, j + 1) <= 0) || (MatForZero.at<double>(i - 1, j - 1) * MatForZero.at <double> (i + 1, j + 1) <= 0))
            {
                for (int k = 0 ; k < (imgCols-maskSize)*(imgRows-maskSize); k++)
                {
                    houghI = k - k/(imgCols-maskSize)*imgCols;
                    houghJ = k/(imgCols-maskSize);
                    if (houghI*i + houghJ == j)
                        forHough[k]++;
                }
            }
        }
    }

    // Hough transform : get line parameters
    int *line = new int[threshold], index = 0, max = 0;
    for (int i = 0; i < (imgCols-maskSize)*(imgRows-maskSize) ; i++)
    {
        if (forHough[i] > threshold)
        {
            cout << forHough[i] << endl;
            line[index] = i;
            index ++;
        }
        if (forHough[i] > max)
            max = forHough[i];
    }
    cout << "Max : " << max << endl;

    // Draw a line
    for (int i = 0 ; i < index ; i++)
    {
        houghI = line[i] - line[i]/(imgCols-maskSize)*imgCols;
        houghJ = line[i]/(imgCols-maskSize);
        for (int j = 0 ; j < imgCols-maskSize ; j++)
        {
            for (int k = 0 ; k < imgRows-maskSize ; k++)
            {
                if (houghI*j + houghJ == k)
                    QImgOut.setPixel(j, k, qRgb(255, 0, 0));
            }
        }
    }

    // Show image
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));   // Scale
    delete forHough;
}

int MainWindow::pixelCheck(int pixel)
{
    if (pixel > 255)
        pixel = 255;
    else if (pixel < 0)
        pixel = 0;

    return pixel;
}

