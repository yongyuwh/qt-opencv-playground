
#include "MyCameraWindow.h"

MyCameraWindow::MyCameraWindow(CvCapture *cam, QWidget *parent) : QWidget(parent) {
    camera = cam;
    BgTake = false;    BgON = false;
    QHBoxLayout *layout = new QHBoxLayout;
//    QVBoxLayout *layout = new QVBoxLayout;
    cvwidget = new QOpenCVWidget(this);
    cvwidget2 = new QOpenCVWidget(this);
    bgButton = new QPushButton("Take BG", this);
    connect(bgButton, SIGNAL(pressed()), this, SLOT(bgButtonClicked()) );
    layout->addWidget(cvwidget);
    layout->addWidget(cvwidget2);
    layout->addWidget(bgButton);
    setLayout(layout);
    resize(500, 400);
    CvSize size = cvSize(320,240);
    image2 = cvCreateImage(size, IPL_DEPTH_8U, 3);
    background = cvCreateImage(size, IPL_DEPTH_8U, 3);
    startTimer(100);  // 0.1-second timer
 }

void MyCameraWindow::timerEvent(QTimerEvent*) {
    IplImage *image=cvQueryFrame(camera);
    if (BgTake) {
       background=cvCloneImage(image);
       BgTake = false;
       BgON = true;
    }
    if (BgON) {
        cvAbsDiff(image, background, image2);
//        cvSub(image, background, image2);
    }
    else {
        image2=cvCloneImage(image);
        cvFlip(image2, image2);

    }
    //Insert your image process task here
    //output shall be in image2
    //input shall be in image
    //
    //
    cvwidget->putImage(image);
    cvwidget2->putImage(image2);
}

void MyCameraWindow::bgButtonClicked(){
     BgTake = true;
}
