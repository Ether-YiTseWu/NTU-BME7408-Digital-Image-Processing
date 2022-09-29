#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
                double B = MatImgIn.at<Vec3b>(j,i)[0], G = MatImgIn.at<Vec3b>(j,i)[1], R = MatImgIn.at<Vec3b>(j,i)[2];
                QImgIn.setPixel(i, j, qRgb(int(R), int(G), int(B)));
            }
        }
        // Show Image
        ui->showImgIn->setPixmap(QPixmap::fromImage(QImgIn.scaled(ui->showImgIn->width(),ui->showImgIn->height(),Qt::KeepAspectRatio)));   // Scale
    }
}

void MainWindow::fftImg()
{
    // Gray and add image padding
    cvtColor(MatImgIn, MatImgInGray, COLOR_RGB2GRAY);
    timeStart = clock();    // Start count
    fourierImgRows = getOptimalDFTSize(MatImgInGray.rows);
    fourierImgCols = getOptimalDFTSize(MatImgInGray.cols);    // on the border add zero values
    copyMakeBorder(MatImgInGray, fourierPadded, 0, fourierImgRows - MatImgInGray.rows, 0, fourierImgCols - MatImgInGray.cols, BORDER_CONSTANT, Scalar::all(0));
    Mat planes[] = {Mat_<float>(fourierPadded), Mat::zeros(fourierPadded.size(), CV_32F)};
    merge(planes, 2, fourierImg);                  // Add to the expanded another plane with zeros

    // DFT
    dft(fourierImg, fourierImg);                         // this way the result may fit in the source matrix
    split(fourierImg, planes);                           // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], fourierSpectrum);    // planes[0] = magnitude

    // Get specturm
    fourierSpectrum += Scalar::all(1);             // switch to logarithmic scale
    log(fourierSpectrum, fourierSpectrum);
    fourierSpectrum = fourierSpectrum(Rect(0, 0, fourierSpectrum.cols & -2, fourierSpectrum.rows & -2));    // Crop the spectrum, if it has an odd number of rows or columns

    // Rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = fourierSpectrum.cols/2;
    int cy = fourierSpectrum.rows/2;
    Mat q0(fourierSpectrum, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(fourierSpectrum, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(fourierSpectrum, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(fourierSpectrum, Rect(cx, cy, cx, cy)); // Bottom-Right
    Mat tmp;                                       // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);                                // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
    normalize(fourierSpectrum, fourierSpectrum, 0, 1, NORM_MINMAX); // Transform the matrix with float values into a, viewable image form (float between values 0 and 1).

    // Get angle
    phase(planes[0], planes[1], fourierAngle);
    log(fourierAngle, fourierAngle);

    // IDFT
    idft(fourierImg, InvFourierImg, DFT_SCALE | DFT_REAL_OUTPUT);
    Mat planesi[] = {Mat_<float>(InvFourierImg), Mat::zeros(InvFourierImg.size(), CV_32F)};
    split(InvFourierImg, planesi);
    InvFourierImg.convertTo(InvFourierImg, CV_8U);

    // Count and show spend time
    timeEnd = clock();
    QString timeSpend = "The OpenCV FFT and IFFT algorithm spend " + QString::number((timeEnd - timeStart)/CLOCKS_PER_SEC) +
                        " seconds to process the " + QString::number(imgCols) + " * " + QString::number(imgRows) + " image";
    ui -> showData -> setFontPointSize(14);
    ui -> showData -> append(timeSpend.toStdString().data());

    // Show QImage Out
    QImgOut = QImage((const unsigned char*) (InvFourierImg.data), InvFourierImg.cols, InvFourierImg.rows, QImage::Format_Grayscale8);
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));   // Scale

    // Show Image
    destroyAllWindows();
    imshow("Grayscale Original Image", MatImgInGray);
    imshow("Spectrum Magnitude of Original Image", fourierSpectrum);
    imshow("Phase Angle of Original Image", fourierAngle);
    //waitKey();
}

void MainWindow::idealLowpass()
{
    // Set up variables
    destroyAllWindows();
    R = 70;
    centerX = fourierImgCols*2/3;
    centerY = fourierImgRows/2;
    fourierSpectrumClone = fourierSpectrum.clone();

    // Do ideal low pass
    for (int i = 0; i < fourierImgCols*4/3 ; i++)
    {
        for (int j = 0; j < fourierImgRows ; j++)
        {
            if((i-centerX)*(i-centerX)+(j-centerY)*(j-centerY) <= R*R);
            else
            {
                fourierSpectrumClone.at<Vec3b>(j,i)[0] = 0;
                fourierSpectrumClone.at<Vec3b>(j,i)[1] = 0;
                fourierSpectrumClone.at<Vec3b>(j,i)[2] = 0;
            }
            cout << fourierSpectrumClone.at<Vec3b>(j,i)[0] << endl;
        }
    }
    imshow("Spectrum Magnitude after Ideal Lowpass", fourierSpectrumClone);

    // Rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = fourierSpectrumClone.cols/2;
    int cy = fourierSpectrumClone.rows/2;
    Mat q0(fourierSpectrumClone, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(fourierSpectrumClone, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(fourierSpectrumClone, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(fourierSpectrumClone, Rect(cx, cy, cx, cy)); // Bottom-Right
    Mat tmp;                                       // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);                                // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
    normalize(fourierSpectrum, fourierSpectrum, 0, 1, NORM_MINMAX);
    imshow("IFFT Shift Spectrum Magnitude after Ideal Lowpass", fourierSpectrumClone);

    // IDFT
    InvFourierImg.release();
    idft(fourierSpectrumClone, InvFourierImg, DFT_REAL_OUTPUT);
    Mat planesi[] = {Mat_<float>(InvFourierImg), Mat::zeros(InvFourierImg.size(), CV_32F)};
    split(InvFourierImg, planesi);
    InvFourierImg.convertTo(InvFourierImg, CV_8U);

    // Show Image
    QImgOut = QImage((const unsigned char*) (InvFourierImg.data), InvFourierImg.cols, InvFourierImg.rows, QImage::Format_Grayscale8);
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));   // Scale
}

void MainWindow::idealHighpass()
{
    // Set up variables
    destroyAllWindows();
    R = 70;
    centerX = fourierImgCols*2/3;
    centerY = fourierImgRows/2;
    fourierSpectrumClone = fourierSpectrum.clone();

    // Do ideal High pass
    for (int i = centerX - R; i < centerX + R ; i++)
    {
        for (int j = centerY - R; j < centerY + R ; j++)
        {
            if((i-centerX)*(i-centerX)+(j-centerY)*(j-centerY) <= R*R)
            {
                fourierSpectrumClone.at<Vec3b>(j,i)[0] = 0;
                fourierSpectrumClone.at<Vec3b>(j,i)[1] = 0;
                fourierSpectrumClone.at<Vec3b>(j,i)[2] = 0;
            }
        }
    }
    imshow("Spectrum Magnitude after Ideal Lowpass", fourierSpectrumClone);

    // Rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = fourierSpectrumClone.cols/2;
    int cy = fourierSpectrumClone.rows/2;
    Mat q0(fourierSpectrumClone, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(fourierSpectrumClone, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(fourierSpectrumClone, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(fourierSpectrumClone, Rect(cx, cy, cx, cy)); // Bottom-Right
    Mat tmp;                                       // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);                                // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
    normalize(fourierSpectrum, fourierSpectrum, 0, 1, NORM_MINMAX);
    imshow("IFFT Shift Spectrum Magnitude after Ideal Lowpass", fourierSpectrumClone);

    // IDFT
    InvFourierImg.release();
    idft(fourierSpectrumClone, InvFourierImg, DFT_REAL_OUTPUT);
    Mat planesi[] = {Mat_<float>(InvFourierImg), Mat::zeros(InvFourierImg.size(), CV_32F)};
    split(InvFourierImg, planesi);
    InvFourierImg.convertTo(InvFourierImg, CV_8U);

    // Show Image
    QImgOut = QImage((const unsigned char*) (InvFourierImg.data), InvFourierImg.cols, InvFourierImg.rows, QImage::Format_Grayscale8);
    ui->showImgOut->setPixmap(QPixmap::fromImage(QImgOut.scaled(ui->showImgOut->width(),ui->showImgOut->height(),Qt::KeepAspectRatio)));   // Scale
}
