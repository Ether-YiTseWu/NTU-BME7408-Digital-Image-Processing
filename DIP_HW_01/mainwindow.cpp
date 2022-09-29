#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    img01 = QImage(64, 64, QImage::Format_RGB32);
    img02 = QImage(64, 64, QImage::Format_RGB32);
    imgAfter = QImage(64, 64, QImage::Format_RGB32);
    imgNew = QImage(64, 64, QImage::Format_RGB32);
    ui->setupUi(this);
    this->setWindowTitle("Image Viewer");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openImg01()
{
    // Set up variables
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Images(*.64)"));
    QFile fileRead(filePath);
    int pixelCount[32];
    for (int i = 0 ; i < 32 ; i++)
        pixelCount[i] = 0;

    // Use text file to open .64 images
    if(fileRead.open(QIODevice::ReadOnly))
    {
        QTextStream input(&fileRead);
        input.setCodec("UTF-8");
        int i = 0;
        while(!input.atEnd())
        {
            string line = input.readLine().toStdString();
            for(int j = 0 ; j < 64 ; j++)
            {
                // Cast char to int
                int pixel = (int)line[j];

                // Based on ascii, make the digits and letters to 32 bits
                if (pixel <= '9')
                    pixel -= '0';
                else
                    (pixel -= 'A') += 10;

                // Count 0~31 pixel sum (Histogram)
                for (int k = 0 ; k < 32 ; k++)
                {
                    if (k == pixel)
                    {
                        pixelCount[k] += 1;
                        break;
                    }
                }
                // Put pixel in imgArr. ImgArr will be used in on_addPixel_clicked() function
                img01Array[i][j] = pixel;
                // Set pixel in img, and make the difference between pixels bigger
                img01.setPixel(j, i, qRgb(pixel*8, pixel*8, pixel*8));
            }
            i++;
            if (i >= 64)
                break;
        }
        fileRead.close();
    }

    // Show .64 Image
    ui->showImg01->setPixmap(QPixmap::fromImage(img01.scaled(ui->showImg01->width(),ui->showImg01->height(),Qt::KeepAspectRatio)));

    // Show Histogram
    QBarSet *set = new QBarSet("Image Histogram");
    int yMax = 0;
    for (int i = 0 ; i < 32 ; i++)
    {
        set->append(pixelCount[i]);
        if(yMax < pixelCount[i])
            yMax = pixelCount[i];
        set->setColor(QColor::Rgb); // Make it black
    }

    QValueAxis *axisY = new QValueAxis; // Set y axis
    axisY->setLabelFormat("%d");
    axisY->setRange(0,yMax);

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    ui->showHis01->setChart(chart);
}

void MainWindow::openImg02()
{
    // Set up variables
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Images(*.64)"));
    QFile fileRead(filePath);
    int pixelCount[32];
    for (int i = 0 ; i < 32 ; i++)
        pixelCount[i] = 0;

    // Use text file to open .64 images
    if(fileRead.open(QIODevice::ReadOnly))
    {
        QTextStream input(&fileRead);
        input.setCodec("UTF-8");
        int i = 0;
        while(!input.atEnd())
        {
            string line = input.readLine().toStdString();
            for(int j = 0 ; j < 64 ; j++)
            {
                // Cast char to int
                int pixel = (int)line[j];

                // Based on ascii, make the digits and letters to 32 bits
                if (pixel <= '9')
                    pixel -= '0';
                else
                    (pixel -= 'A') += 10;

                // Count 0~31 pixel sum
                for (int k = 0 ; k < 32 ; k++)
                {
                    if (k == pixel)
                    {
                        pixelCount[k] += 1;
                        break;
                    }
                }
                // Put pixel in imgArr. ImgArr will be used in on_addPixel_clicked() function
                img02Array[i][j] = pixel;
                // Set pixel in img, and make the difference between pixels bigger
                img02.setPixel(j, i, qRgb(pixel*8, pixel*8, pixel*8));
            }
            i++;
            if (i >= 64)
                break;
        }
        fileRead.close();
    }

    // Show .64 Image
    ui->showImg02->setPixmap(QPixmap::fromImage(img02.scaled(ui->showImg02->width(),ui->showImg02->height(),Qt::KeepAspectRatio)));

    // Show Histogram
    QBarSet *set = new QBarSet("Image Histogram");
    int yMax = 0;
    for (int i = 0 ; i < 32 ; i++)
    {
        set->append(pixelCount[i]);
        if(yMax < pixelCount[i])
            yMax = pixelCount[i];
        set->setColor(QColor::Rgb); // Make it black
    }

    QValueAxis *axisY = new QValueAxis; // Set y axis
    axisY->setLabelFormat("%d");
    axisY->setRange(0,yMax);

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    ui->showHis02->setChart(chart);
}

void MainWindow::addPixel()
{
    // Set up variables
    int pixelCount[32];
    for (int i = 0 ; i < 32 ; i++)
        pixelCount[i] = 0;

    for (int i = 0 ; i < 64 ; i++)
    {
        for (int j = 0 ; j < 64 ; j++)
        {
            img02Array[i][j] += 1; // Add one
            int pixel = img02Array[i][j];
            pixel = checkPixel(pixel);

            // Count 0~31 pixel sum
            for (int k = 0 ; k < 32 ; k++)
            {
                if (k == pixel)
                {
                    pixelCount[k] += 1;
                    break;
                }
            }
            imgAfter.setPixel(j, i, qRgb(pixel*8, pixel*8, pixel*8));
        }
    }
    // Show .64 Image after add pixel
    ui->showImgAfter->setPixmap(QPixmap::fromImage(imgAfter.scaled(ui->showImgAfter->width(),ui->showImgAfter->height(),Qt::KeepAspectRatio)));

    // Show Histogram
    QBarSet *set = new QBarSet("Image Histogram");
    int yMax = 0;
    for (int i = 0 ; i < 32 ; i++)
    {
        set->append(pixelCount[i]);
        if(yMax < pixelCount[i])
            yMax = pixelCount[i];
        set->setColor(QColor::Rgb); // Make it black
    }

    QValueAxis *axisY = new QValueAxis; // Set y axis
    axisY->setLabelFormat("%d");
    axisY->setRange(0,yMax);

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    ui->showHisAfter->setChart(chart);
}

void MainWindow::substractPixel()
{
    // Set up variables
    int pixelCount[32];
    for (int i = 0 ; i < 32 ; i++)
        pixelCount[i] = 0;

    for (int i = 0 ; i < 64 ; i++)
    {
        for (int j = 0 ; j < 64 ; j++)
        {
            img02Array[i][j] -= 1;  // Substract one
            int pixel = img02Array[i][j];
            pixel = checkPixel(pixel);

            // Count 0~31 pixel sum
            for (int k = 0 ; k < 32 ; k++)
            {
                if (k == pixel)
                {
                    pixelCount[k] += 1;
                    break;
                }
            }
            imgAfter.setPixel(j, i, qRgb(pixel*8, pixel*8, pixel*8));
        }
    }
    // Show .64 Image after add pixel
    ui->showImgAfter->setPixmap(QPixmap::fromImage(imgAfter.scaled(ui->showImgAfter->width(),ui->showImgAfter->height(),Qt::KeepAspectRatio)));

    // Show Histogram
    QBarSet *set = new QBarSet("Image Histogram");
    int yMax = 0;
    for (int i = 0 ; i < 32 ; i++)
    {
        set->append(pixelCount[i]);
        if(yMax < pixelCount[i])
            yMax = pixelCount[i];
        set->setColor(QColor::Rgb); // Make it black
    }

    QValueAxis *axisY = new QValueAxis; // Set y axis
    axisY->setLabelFormat("%d");
    axisY->setRange(0,yMax);

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    ui->showHisAfter->setChart(chart);
}

void MainWindow::multiplyPixel()
{
    // Set up variables
    int pixelCount[32];
    for (int i = 0 ; i < 32 ; i++)
        pixelCount[i] = 0;

    for (int i = 0 ; i < 64 ; i++)
    {
        for (int j = 0 ; j < 64 ; j++)
        {
            img02Array[i][j] *= 1.2; // Multiply one point two
            int pixel = img02Array[i][j];
            pixel = checkPixel(pixel);

            // Count 0~31 pixel sum
            for (int k = 0 ; k < 32 ; k++)
            {
                if (k == pixel)
                {
                    pixelCount[k] += 1;
                    break;
                }
            }
            imgAfter.setPixel(j, i, qRgb(pixel*8, pixel*8, pixel*8));
        }
    }
    // Show .64 Image after add pixel
    ui->showImgAfter->setPixmap(QPixmap::fromImage(imgAfter.scaled(ui->showImgAfter->width(),ui->showImgAfter->height(),Qt::KeepAspectRatio)));

    // Show Histogram
    QBarSet *set = new QBarSet("Image Histogram");
    int yMax = 0;
    for (int i = 0 ; i < 32 ; i++)
    {
        set->append(pixelCount[i]);
        if(yMax < pixelCount[i])
            yMax = pixelCount[i];
        set->setColor(QColor::Rgb); // Make it black
    }

    QValueAxis *axisY = new QValueAxis; // Set y axis
    axisY->setLabelFormat("%d");
    axisY->setRange(0,yMax);

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    ui->showHisAfter->setChart(chart);
}

void MainWindow::dividePixel()
{
    // Set up variables
    int pixelCount[32];
    for (int i = 0 ; i < 32 ; i++)
        pixelCount[i] = 0;

    for (int i = 0 ; i < 64 ; i++)
    {
        for (int j = 0 ; j < 64 ; j++)
        {
            img02Array[i][j] /= 1.2; // Divide one point two
            int pixel = img02Array[i][j];
            pixel = checkPixel(pixel);

            // Count 0~31 pixel sum
            for (int k = 0 ; k < 32 ; k++)
            {
                if (k == pixel)
                {
                    pixelCount[k] += 1;
                    break;
                }
            }
            imgAfter.setPixel(j, i, qRgb(pixel*8, pixel*8, pixel*8));
        }
    }
    // Show .64 Image after add pixel
    ui->showImgAfter->setPixmap(QPixmap::fromImage(imgAfter.scaled(ui->showImgAfter->width(),ui->showImgAfter->height(),Qt::KeepAspectRatio)));

    // Show Histogram
    QBarSet *set = new QBarSet("Image Histogram");
    int yMax = 0;
    for (int i = 0 ; i < 32 ; i++)
    {
        set->append(pixelCount[i]);
        if(yMax < pixelCount[i])
            yMax = pixelCount[i];
        set->setColor(QColor::Rgb); // Make it black
    }

    QValueAxis *axisY = new QValueAxis; // Set y axis
    axisY->setLabelFormat("%d");
    axisY->setRange(0,yMax);

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    ui->showHisAfter->setChart(chart);
}

void MainWindow::averageImg()
{
    // Set up variables
    int pixelCount[32];
    for (int i = 0 ; i < 32 ; i++)
        pixelCount[i] = 0;

    for (int i = 0 ; i < 64 ; i++)
    {
        for (int j = 0 ; j < 64 ; j++)
        {
            int pixel = (checkPixel(img01Array[i][j]) + checkPixel(img02Array[i][j]))/2;
            pixel = checkPixel(pixel);

            // Count 0~31 pixel sum
            for (int k = 0 ; k < 32 ; k++)
            {
                if (k == pixel)
                {
                    pixelCount[k] += 1;
                    break;
                }
            }
            imgNew.setPixel(j, i, qRgb(pixel*8, pixel*8, pixel*8));
        }
    }
    // Show .64 Image after add pixel
    ui->showImgNew->setPixmap(QPixmap::fromImage(imgNew.scaled(ui->showImgNew->width(),ui->showImgNew->height(),Qt::KeepAspectRatio)));

    // Show Histogram
    QBarSet *set = new QBarSet("Image Histogram");
    int yMax = 0;
    for (int i = 0 ; i < 32 ; i++)
    {
        set->append(pixelCount[i]);
        if(yMax < pixelCount[i])
            yMax = pixelCount[i];
        set->setColor(QColor::Rgb); // Make it black
    }

    QValueAxis *axisY = new QValueAxis; // Set y axis
    axisY->setLabelFormat("%d");
    axisY->setRange(0,yMax);

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    ui->showHisNew->setChart(chart);
}

void MainWindow::specialImg()
{
    // Set up variables
    int pixelCount[32];
    for (int i = 0 ; i < 32 ; i++)
        pixelCount[i] = 0;

    for (int i = 1 ; i < 64 ; i++)
    {
        for (int j = 0 ; j < 64 ; j++)
        {
            int pixel = checkPixel(img02Array[i][j]) - checkPixel(img02Array[i-1][j]);
            pixel = checkPixel(pixel);

            // Count 0~31 pixel sum
            for (int k = 0 ; k < 32 ; k++)
            {
                if (k == pixel)
                {
                    pixelCount[k] += 1;
                    break;
                }
            }
            imgNew.setPixel(j, i, qRgb(pixel*8, pixel*8, pixel*8));
        }
    }
    // Show .64 Image after add pixel
    ui->showImgNew->setPixmap(QPixmap::fromImage(imgNew.scaled(ui->showImgNew->width(),ui->showImgNew->height(),Qt::KeepAspectRatio)));

    // Show Histogram
    QBarSet *set = new QBarSet("Image Histogram");
    int yMax = 0;
    for (int i = 0 ; i < 32 ; i++)
    {
        set->append(pixelCount[i]);
        if(yMax < pixelCount[i])
            yMax = pixelCount[i];
        set->setColor(QColor::Rgb); // Make it black
    }

    QValueAxis *axisY = new QValueAxis; // Set y axis
    axisY->setLabelFormat("%d");
    axisY->setRange(0,yMax);

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);

    ui->showHisNew->setChart(chart);
}

int MainWindow::checkPixel(int pixel)
{
    if (pixel > 31)
        pixel = 31;
    else if (pixel < 0)
        pixel = 0;
    return pixel;
}
