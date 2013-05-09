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
#include <QtOpenGL>
#include "opencv2/opencv.hpp"
#include <math.h>
#include <gl/glu.h>
#include "glwidget.h"
#include "MatToQImage.h"

using namespace cv;

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    z = -5.0f;
    LightAmbient[0] = 0.5f; LightAmbient[1] = 0.5f; LightAmbient[2] = 0.5f; LightAmbient[3] = 1.0f;
    LightDiffuse[0] = 1.0f; LightDiffuse[1] = 1.0f; LightDiffuse[2] = 1.0f; LightDiffuse[3] = 1.0f;
    LightPosition[0]= 0.0f; LightPosition[1] = 0.0f; LightPosition[2] = 2.0f; LightPosition[3] = 1.0f;
    cap.open(0); // open the default camera
//    if(!cap.isOpened())  // check if we succeeded
//        return -1;
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
     cap.release();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xrot) {
        xrot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yrot) {
        yrot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zrot) {
        zrot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::rotateBy()
{
    xrot += xspeed;
    //qDebug("xRot = %d\n",xRot);
    //qNormalizeAngle(xrot);
    yrot += yspeed;
    //qDebug("%.2f",yrot);
    //qNormalizeAngle(yrot);
    //zrot += zAngle;
    //qNormalizeAngle(zrot);
    updateFrame();
    updateGL();
    emit xRotationChanged(xrot);
    emit yRotationChanged(yrot);
    emit zRotationChanged(zrot);
}

void GLWidget::changeXSpeed(float value)
{
    xspeed += value;
}

void GLWidget::changeYSpeed(float value)
{
    yspeed += value;
}

void GLWidget::updateFrame()
{
   if(cap.isOpened())  // check if we succeeded
       cap >> frame;
// if(!cap.isOpened())  // check if we succeeded
//     cap.open(0); // open the default camera
// else  cap >> frame;
}

bool GLWidget::LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
    int Status = FALSE;
    QImage MyImage;
    QImage T;

    // Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
    if (MyImage.load(":/image/Crate.bmp"))
    {
        T = convertToGLFormat(MyImage);
        Status=TRUE;							// Set The Status To TRUE

        glGenTextures(3, &texture[0]);					// Create Three Textures

        // Create Nearest Filtered Texture
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, T.width(), T.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, T.bits());

        // Create Linear Filtered Texture
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, T.width(), T.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, T.bits());

        // Create MipMapped Texture
        glBindTexture(GL_TEXTURE_2D, texture[2]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, T.width(), T.height(), GL_RGBA, GL_UNSIGNED_BYTE, T.bits());
    }

    return Status;
}

void GLWidget::initializeGL()
{
    if (LoadGLTextures())                                       // Jump To Texture Loading Routine
    {
        glEnable(GL_TEXTURE_2D);				// Enable Texture Mapping
        glShadeModel(GL_SMOOTH);				// Enable Smooth Shading
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                   // Black Background
        glClearDepth(1.0f);					// Depth Buffer Setup
        glEnable(GL_DEPTH_TEST);				// Enables Depth Testing
        glDepthFunc(GL_LEQUAL);                                 // The Type Of Depth Testing To Do
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

        glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);         // Setup The Ambient Light
        glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);         // Setup The Diffuse Light
        glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
        glEnable(GL_LIGHT1);                                    // Enable Light One

        glColor4f(1.0f, 1.0f, 1.0f, 0.5);			// Full Brightness.  50% Alpha
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// Set The Blending Function For Translucency
    }
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
    glLoadIdentity();					// Reset The View
    glTranslatef(0.0f,0.0f,z);

    glRotatef(xrot,1.0f,0.0f,0.0f);
    glRotatef(yrot,0.0f,1.0f,0.0f);
    glRotatef(zrot,0.0f,0.0f,1.0f);
    glEnable(GL_TEXTURE_2D);
    // Convert image and depth data to OpenGL textures
    GLuint imageTex = matToTexture(frame,   GL_LINEAR_MIPMAP_LINEAR,   GL_LINEAR, GL_CLAMP);
//	GLuint depthTex = matToTexture(depthFrame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);

    // Draw the textures
    // Note: Window co-ordinates origin is top left, texture co-ordinate origin is bottom left.

    // Front facing texture
    glBindTexture(GL_TEXTURE_2D, imageTex);

//    glBindTexture(GL_TEXTURE_2D, texture[filter]);

    glBegin(GL_QUADS);
        // Front Face
        glNormal3f( 0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        // Back Face
        glNormal3f( 0.0f, 0.0f,-1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        // Top Face
        glNormal3f( 0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        // Bottom Face
        glNormal3f( 0.0f,-1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        // Right face
        glNormal3f( 1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        // Left Face
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
    glDeleteTextures(1, &imageTex);
    glDisable(GL_TEXTURE_2D);
    xrot+=xspeed;
    yrot+=yspeed;
}

void GLWidget::zoom(float z)
{
    this->z += z;
}

void GLWidget::changeFilter(int index)
{
    filter+=index;
    if (filter>2)
        filter=0;
}

void GLWidget::resizeGL(int width, int height)
{
    if (height==0)							// Prevent A Divide By Zero By
    {
        height=1;							// Making Height Equal One
    }

    glViewport(0,0,width,height);					// Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);					// Select The Projection Matrix
    glLoadIdentity();							// Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

    glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
    glLoadIdentity();							// Reset The Modelview Matrix
}

// Function turn a cv::Mat into a texture, and return the texture ID as a GLuint for use
GLuint GLWidget::matToTexture(cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter)
{
    QImage TextFrame;
    QImage Text;
    // Generate a number for our textureID's unique handle
    GLuint textureID;
    TextFrame = MatToQImage(mat);
    Text = convertToGLFormat(TextFrame);

    glGenTextures(1, &textureID);

    // Bind to our texture handle
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Catch silly-mistake texture interpolation method for magnification
    if (magFilter == GL_LINEAR_MIPMAP_LINEAR  ||
        magFilter == GL_LINEAR_MIPMAP_NEAREST ||
        magFilter == GL_NEAREST_MIPMAP_LINEAR ||
        magFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
//        cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << endl;
        magFilter = GL_LINEAR;
    }

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
//    GLenum inputColourFormat = GL_BGR;
      GLenum inputColourFormat = GL_RGBA;
    if (mat.channels() == 1)
    {
        inputColourFormat = GL_LUMINANCE;
    }

    // Create the texture
//    glTexImage2D(GL_TEXTURE_2D, 0, 3, T.width(), T.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, T.bits());
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 3,            // Internal colour format to convert to
                 Text.width(),          // Image width  i.e. 640 for Kinect in standard mode
                 Text.height(),          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 GL_RGBA, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 Text.bits());        // The actual image data itself
//    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
//                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
//                 3,            // Internal colour format to convert to
//                 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
//                 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
//                 0,                 // Border width in pixels (can either be 1 or 0)
//                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
//                 GL_UNSIGNED_BYTE,  // Image data type
//                 mat.ptr());        // The actual image data itself

    // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
//    if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
//        minFilter == GL_LINEAR_MIPMAP_NEAREST ||
//        minFilter == GL_NEAREST_MIPMAP_LINEAR ||
//        minFilter == GL_NEAREST_MIPMAP_NEAREST)
//    {
//        glGenerateMipmap(GL_TEXTURE_2D);
//    }

    return textureID;
}
