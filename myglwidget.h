
//struct Particle
//{
//    QVector3D position;
//    QVector3D velocity;
//    QVector4D color;
//    float lifespan;
//};
//
//class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
//{
//    Q_OBJECT
//
//public:
//    MyGLWidget(QWidget *parent = nullptr);
//    ~MyGLWidget();
//
//protected:
//    void initializeGL();
//    void paintGL();
//    void resizeGL(int width, int height);
//    void keyPressEvent(QKeyEvent *e);
//    void updateParticles();
//    void renderParticles();
//
//private:
//    std::vector<Particle> particles;
//    int scene_id;
//    void scene_0();
//};
//#endif // MYGLWIDGET_H

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

class MyGLWidget : public QGLWidget
{
     Q_OBJECT

public:
    explicit MyGLWidget(QWidget * parent = 0);
    ~MyGLWidget();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void keyPressEvent(QKeyEvent * event);           //������̰����¼�

private:
    bool fullscreen;                                //�Ƿ�ȫ����ʾ
    QString m_FileName;                             //ͼƬ��·�����ļ���
    GLuint m_Texture;                               //����һ������

    static const int MAX_PARTICLES = 1000;          //���������
    static const GLfloat COLORS[12][3];             //�ʺ����ɫ
    bool m_Rainbow;                                 //�Ƿ�Ϊ�ʺ�ģʽ
    GLuint m_Color;                                 //��ǰ����ɫ

    float m_Slowdown;                               //��������
    float m_xSpeed;                                 //x������ٶ�
    float m_ySpeed;                                 //y������ٶ�
    float m_Deep;                                   //������Ļ�ľ���

    struct Particle                                 //�������ӽṹ��
    {
        bool active;                                //�Ƿ񼤻�
        float life;                                 //��������
        float fade;                                 //˥���ٶ�

        float r, g, b;                              //������ɫ
        float x, y, z;                              //λ������
        float xi, yi, zi;                           //�������ٶ�
        float xg, yg, zg;                           //��������ٶ�
    } m_Particles[MAX_PARTICLES];                   //���1000�����ӵ�����
};

#endif // MYGLWIDGET_H