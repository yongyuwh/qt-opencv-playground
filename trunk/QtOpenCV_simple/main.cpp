#include <QApplication>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <assert.h>
#include <QWidget>
#include <QVBoxLayout>
#include "QOpenCVWidget.h"
#include "MyCameraWindow.h"

using namespace cv;

int main(int argc, char **argv) {
    CvCapture * camera = cvCaptureFromCAM(0);
//    CvCapture * camera = cvCreateCameraCapture(0);
    assert(camera);
    IplImage * image=cvQueryFrame(camera);
    assert(image);

    printf("Image depth=%i\n", image->depth);
    printf("Image nChannels=%i\n", image->nChannels);

    QApplication app(argc, argv);
    MyCameraWindow *mainWin = new MyCameraWindow(camera);
    mainWin->setWindowTitle("OpenCV --> QtImage");
    mainWin->show();
//    MyCameraWindow *mainWin2 = new MyCameraWindow(camera);
//    mainWin2->setWindowTitle("OpenCV --> QtImage2");
//    mainWin2->show();
    int retval = app.exec();
    
    cvReleaseCapture(&camera);
    
    return retval;
}

