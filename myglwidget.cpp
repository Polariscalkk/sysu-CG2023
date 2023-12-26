#include "myglwidget.h"

//MyGLWidget::MyGLWidget(QWidget *parent)
//	:QOpenGLWidget(parent),
//	scene_id(0)
//{
//}
MyGLWidget::~MyGLWidget()
{

}

void MyGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
	update();
}

const GLfloat MyGLWidget::COLORS[][3] =                 //彩虹的颜色
{
     {1.0f, 0.5f, 0.5f}, { 1.0f, 0.75f, 0.5f }, { 1.0f, 1.0f, 0.5f },
     {0.75f, 1.0f, 0.5f}, { 0.5f, 1.0f, 0.5f }, { 0.5f, 1.0f, 0.75f },
     {0.5f, 1.0f, 1.0f}, { 0.5f, 0.75f, 1.0f }, { 0.5f, 0.5f, 1.0f },
     {0.75f, 0.5f, 1.0f}, { 1.0f, 0.5f, 1.0f }, { 1.0f, 0.5f, 0.75f }
};

MyGLWidget::MyGLWidget(QWidget* parent) :
     QGLWidget(parent)
{
    fullscreen = false;
    m_FileName = "C:/Users/user/桌面/fireworkPicture.png";    //应根据实际存放图片的路径进行修改
    m_Rainbow = true;
    m_Color = 0;
    m_Slowdown = 2.0f;
    m_xSpeed = 0.0f;
    m_ySpeed = 0.0f;
    m_Deep = -40.0f;

    for (int i = 0; i < MAX_PARTICLES; i++)                 //循环初始化所以粒子
    {
        m_Particles[i].active = true;                   //使所有粒子为激活状态
        m_Particles[i].life = 1.0f;                     //所有粒子生命值为最大
        //随机生成衰减速率
        m_Particles[i].fade = float(rand() % 100) / 1000.0f + 0.001;
        //粒子的颜色
        m_Particles[i].r = COLORS[int(i * (12.0f / MAX_PARTICLES))][0];
        m_Particles[i].g = COLORS[int(i * (12.0f / MAX_PARTICLES))][1];
        m_Particles[i].b = COLORS[int(i * (12.0f / MAX_PARTICLES))][2];
         //粒子的初始位置
        m_Particles[i].x = 0.0f;
        m_Particles[i].y = 0.0f;
        m_Particles[i].z = 0.0f;

        //随机生成x、y、z轴方向速度
        m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
        m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
        m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;

        m_Particles[i].xg = 0.0f;                       //设置x方向加速度为0
        m_Particles[i].yg = -0.8f;                      //设置y方向加速度为-0.8
        m_Particles[i].zg = 0.0f;                       //设置z方向加速度为0
    }

    QTimer * timer = new QTimer(this);                   //创建一个定时器
    //将定时器的计时信号与updateGL()绑定
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(10);                                   //以10ms为一个计时周期
}

void MyGLWidget::initializeGL()                         //此处开始对OpenGL进行所以设置
{
    m_Texture = bindTexture(QPixmap(m_FileName));       //载入位图并转换成纹理
    glEnable(GL_TEXTURE_2D);                            //启用纹理映射

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);               //黑色背景
    glShadeModel(GL_SMOOTH);                            //启用阴影平滑
    glClearDepth(1.0);                                  //设置深度缓存
    glDisable(GL_DEPTH_TEST);                           //禁止深度测试
    glEnable(GL_BLEND);                                 //启用融合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);                  //设置融合因子
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //告诉系统对透视进行修正
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
}

void MyGLWidget::paintGL()                              //从这里开始进行所以的绘制
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存
    glLoadIdentity();                                   //重置模型观察矩阵
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    for (int i = 0; i < MAX_PARTICLES; i++)                 //循环所有的粒子
    {
        if (m_Particles[i].active)                      //如果粒子为激活的
        {
            float x = m_Particles[i].x;                 //x轴位置
            float y = m_Particles[i].y;                 //y轴位置
            float z = m_Particles[i].z + m_Deep;        //z轴位置
            //设置粒子颜色
            glColor4f(m_Particles[i].r, m_Particles[i].g,
                      m_Particles[i].b, m_Particles[i].life);
            glBegin(GL_TRIANGLE_STRIP);                 //绘制三角形带
                glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
                glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
                glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
                glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
            glEnd();

            //更新各方向坐标及速度
            m_Particles[i].x += m_Particles[i].xi / (m_Slowdown * 1000);
            m_Particles[i].y += m_Particles[i].yi / (m_Slowdown * 1000);
            m_Particles[i].z += m_Particles[i].zi / (m_Slowdown * 1000);
            m_Particles[i].xi += m_Particles[i].xg;
            m_Particles[i].yi += m_Particles[i].yg;
            m_Particles[i].zi += m_Particles[i].zg;

            m_Particles[i].life -= m_Particles[i].fade; //减少粒子的生命值
            if (m_Particles[i].life < 0.0f)             //如果粒子生命值小于0
            {
                m_Particles[i].life = 1.0f;             //产生一个新粒子
                m_Particles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

                m_Particles[i].r = COLORS[m_Color][0];  //设置颜色
                m_Particles[i].g = COLORS[m_Color][1];
                m_Particles[i].b = COLORS[m_Color][2];

                m_Particles[i].x = 0.0f;                //粒子出现在屏幕中央
                m_Particles[i].y = 0.0f;
                m_Particles[i].z = 0.0f;

                //随机生成粒子速度
                m_Particles[i].xi = m_xSpeed + float((rand() % 60) - 32.0f);
                m_Particles[i].yi = m_ySpeed + float((rand() % 60) - 30.0f);
                m_Particles[i].zi = float((rand() % 60) - 30.0f);
            }
        }
    }

    if (m_Rainbow)                                      //如果为彩虹模式
    {
        m_Color++;                                      //进行颜色的变换
        if (m_Color > 11)
        {
            m_Color = 0;
        }
    }
}

void MyGLWidget::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_F1:                                    //F1为全屏和普通屏的切换键
            fullscreen = !fullscreen;
            if (fullscreen) {
                showFullScreen();
            }
            else {
                showNormal();
            }
            updateGL();
            break;
        case Qt::Key_Escape:                                //ESC为退出键
            close();
            break;
        case Qt::Key_Tab:                                   //Tab按下使粒子回到原点，产生爆炸
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                m_Particles[i].x = 0.0f;
                m_Particles[i].y = 0.0f;
                m_Particles[i].z = 0.0f;

                //随机生成速度
                m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
                m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
                m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;
            }
            break;
        case Qt::Key_8:                                     //按下8增加y方向加速度
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (m_Particles[i].yg < 3.0f)
                {
                    m_Particles[i].yg += 0.05f;
                }
            }
            break;
        case Qt::Key_2:                                     //按下2减少y方向加速度
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (m_Particles[i].yg > -3.0f)
                {
                    m_Particles[i].yg -= 0.05f;
                }
            }
            break;
        case Qt::Key_6:                                     //按下6增加x方向加速度
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (m_Particles[i].xg < 3.0f)
                {
                    m_Particles[i].xg += 0.05f;
                }
            }
            break;
        case Qt::Key_4:                                     //按下4减少x方向加速度
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (m_Particles[i].xg > -3.0f)
                {
                    m_Particles[i].xg -= 0.05f;
                }
            }
            break;
        case Qt::Key_Plus:                                  //+ 号按下加速粒子
            if (m_Slowdown > 1.0f)
            {
                m_Slowdown -= 0.05f;
            }
            break;
        case Qt::Key_Minus:                                 //- 号按下减速粒子
            if (m_Slowdown < 3.0f)
            {
                m_Slowdown += 0.05f;
            }
            break;
        case Qt::Key_PageUp:                                //PageUp按下使粒子靠近屏幕
            m_Deep += 0.5f;
            break;
        case Qt::Key_PageDown:                              //PageDown按下使粒子远离屏幕
            m_Deep -= 0.5f;
            break;
        case Qt::Key_Return:                                //回车键为是否彩虹模式的切换键
            m_Rainbow = !m_Rainbow;
            break;
        case Qt::Key_Space:                                 //空格键为颜色切换键
            m_Rainbow = false;
            m_Color++;
            if (m_Color > 11)
            {
                m_Color = 0;
            }
            break;
        case Qt::Key_Up:                                    //Up按下增加粒子y轴正方向的速度
            if (m_ySpeed < 400.0f)
            {
                m_ySpeed += 5.0f;
            }
            break;
        case Qt::Key_Down:                                  //Down按下减少粒子y轴正方向的速度
            if (m_ySpeed > -400.0f)
            {
                m_ySpeed -= 5.0f;
            }
            break;
        case Qt::Key_Right:                                 //Right按下增加粒子x轴正方向的速度
            if (m_xSpeed < 400.0f)
            {
                m_xSpeed += 5.0f;
            }
            break;
        case Qt::Key_Left:                                  //Left按下减少粒子x轴正方向的速度
            if (m_xSpeed > -400.0f)
            {
                m_xSpeed -= 5.0f;
            }
            break;

    }
}