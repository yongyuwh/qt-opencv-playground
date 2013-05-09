/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "glwidget.h"
#include "window.h"

Window::Window()
{
    glWidget = new GLWidget;
    light = FALSE;
    blend = FALSE;
    glDisable(GL_LIGHTING);
    /*xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();

    connect(xSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setXRotation(int)));
    connect(glWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setYRotation(int)));
    connect(glWidget, SIGNAL(yRotationChanged(int)), ySlider, SLOT(setValue(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZRotation(int)));
    connect(glWidget, SIGNAL(zRotationChanged(int)), zSlider, SLOT(setValue(int)));
*/
    QTimer *timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), glWidget, SLOT(updateFrame()));
    connect(timer, SIGNAL(timeout()), this, SLOT(rotateOneStep()));
    timer->start(20);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    //mainLayout->addWidget(xSlider);
    //mainLayout->addWidget(ySlider);
    //mainLayout->addWidget(zSlider);
    setLayout(mainLayout);

    //xSlider->setValue(15 * 16);
    //ySlider->setValue(345 * 16);
    //zSlider->setValue(0 * 16);
    setWindowTitle(tr("Hello GL"));
}

QSlider *Window::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 1);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 1);
    slider->setTickInterval(15 * 1);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void Window::rotateOneStep()
{
    glWidget->rotateBy();
}

void Window::keyPressEvent(QKeyEvent *e)
{
    float stepChange = 0.05f;
    if (e->key() == Qt::Key_Escape)
        close();
    else if(e->key() == Qt::Key_Up)
        glWidget->changeXSpeed(-stepChange);
    else if(e->key() == Qt::Key_Down)
        glWidget->changeXSpeed(stepChange);
    else if(e->key() == Qt::Key_Right)
        glWidget->changeYSpeed(stepChange);
    else if(e->key() == Qt::Key_Left)
        glWidget->changeYSpeed(-stepChange);
    else if(e->key() == Qt::Key_PageUp)
        glWidget->zoom(0.02f);
    else if(e->key() == Qt::Key_PageDown)
        glWidget->zoom(-0.02f);
    else if(e->key() == Qt::Key_L)
    {
        light=!light;
        if (!light)
            glDisable(GL_LIGHTING);
        else
            glEnable(GL_LIGHTING);
    }
    else if(e->key() == Qt::Key_F)
        glWidget->changeFilter(1);
    else if(e->key() == Qt::Key_B)
    {
        blend = !blend;
        if(blend)
        {
            glEnable(GL_BLEND);			// Turn Blending On
            glDisable(GL_DEPTH_TEST);       // Turn Depth Testing Off
        }
        else
        {
            glDisable(GL_BLEND);		// Turn Blending Off
            glEnable(GL_DEPTH_TEST);        // Turn Depth Testing On
        }
    }
    else
    {
        QWidget::keyPressEvent(e);
    }
}


