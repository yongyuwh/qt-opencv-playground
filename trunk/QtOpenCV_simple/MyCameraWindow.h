#ifndef MYCAMERAWINDOW_H_
#define MYCAMERAWINDOW_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "QOpenCVWidget.h"
#include <cv.h>
#include <highgui.h>

using namespace cv;

class MyCameraWindow : public QWidget
{
    Q_OBJECT
    private:
        QOpenCVWidget *cvwidget;
        QOpenCVWidget *cvwidget2;
        CvCapture *camera;
        IplImage *image2;
        IplImage *background;
        QPushButton *bgButton;
        bool BgTake, BgON;
        
    public:
        MyCameraWindow(CvCapture *cam, QWidget *parent=0);

    public slots:
        void bgButtonClicked();

    protected:
        void timerEvent(QTimerEvent*);        
};


#endif /*MYCAMERAWINDOW_H_*/
