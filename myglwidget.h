
#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#ifdef MAC_OS
#include <QtOpenGL/QtOpenGL>
#else
#include <GL/glew.h>
#endif
#include <QtGui>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWidget>
#include <QGLWidget>
#include <QTimer>

class MyGLWidget: public QGLWidget {
    Q_OBJECT

public:
    explicit MyGLWidget(QWidget * parent = 0);
    ~MyGLWidget();

protected:
    void initializeGL();

    //! @brief Handle window size adjustment.
    //! 
    //! @param w : New width
    //! @param h : New height
    void resizeGL(int w, int h);
    
    //! @brief Paint the scene
    //! 
    void paintGL();

    //! @brief Handle key press event
    //! 
    //! @param e : the pressed key
    void keyPressEvent(QKeyEvent * e);              //!< Key Press Event

private:
    bool fullscreen;                                //!< [true] -> is full screen
    QString m_FileName;                             //!< particle texture path
    GLuint m_Texture;                               //!< particle texture

    static const int MAX_PARTICLES = 1000;          //!< maximum number of particles
    static const GLfloat COLORS[12][3];             //!< rainbow color
    bool m_Rainbow;                                 //!< [true] -> rainbow mode (particle in rainbow color)
    GLuint m_Color;                                 //!< current color

    float m_Slowdown;                               //!< deceleration coefficient
    float m_xSpeed;                                 //!< x-axis velocity
    float m_ySpeed;                                 //!< y-axis velocity
    float m_Deep;                                   //!< deepth

    struct Particle {
        bool active;                                //!< [true] -> is active, show on screen
        float life;                                 //!< the life span of a particle
        float fade;                                 //!< fade velocity

        float r, g, b;                              //!< color
        float x, y, z;                              //!< postion
        float xi, yi, zi;                           //!< velocity
        float xg, yg, zg;                           //!< gravity
    } m_Particles[MAX_PARTICLES+1];                 //!< max-num particle with ONE trigger
};

#endif // MYGLWIDGET_H