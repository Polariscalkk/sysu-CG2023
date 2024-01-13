#include "myglwidget.h"

MyGLWidget::~MyGLWidget()
{

}

// 处理窗口大小变化
void MyGLWidget::resizeGL(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(w) / static_cast<double>(h), 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
    m_FileName = "E:/fireworkPicture.png";    //应根据实际存放图片的路径进行修改
    m_Rainbow = true;
    m_Color = 0;
    m_Slowdown = 2.0f;
    m_xSpeed = 0.0f;
    m_ySpeed = 0.0f;
    m_Deep = -40.0f;

    m_Particles[MAX_PARTICLES].active = true;                   //使源粒子为激活状态
    m_Particles[MAX_PARTICLES].life = 1.0f;                     //源粒子生命值为最大
    //随机生成衰减速率
    m_Particles[MAX_PARTICLES].fade = float(rand() % 100) / 1000.0f + 0.001;
    //粒子的颜色
    m_Particles[MAX_PARTICLES].r = COLORS[int(MAX_PARTICLES * (12.0f / MAX_PARTICLES))][0];
    m_Particles[MAX_PARTICLES].g = COLORS[int(MAX_PARTICLES * (12.0f / MAX_PARTICLES))][1];
    m_Particles[MAX_PARTICLES].b = COLORS[int(MAX_PARTICLES * (12.0f / MAX_PARTICLES))][2];

    //粒子的初始位置
    m_Particles[MAX_PARTICLES].x = 0.0f;
    m_Particles[MAX_PARTICLES].y = -20.0f;  //从底部向上喷出
    m_Particles[MAX_PARTICLES].z = 0.0f;

    //随机生成x、y、z轴方向速度
    m_Particles[MAX_PARTICLES].xi = float((rand() % 50) - 26.0f) * 2.0f;
    m_Particles[MAX_PARTICLES].yi = float((rand() % 50) + 25.0f) * 10.0f;
    m_Particles[MAX_PARTICLES].zi = float((rand() % 50) - 25.0f) * 2.0f;

    m_Particles[MAX_PARTICLES].xg = 0.0f;                       //设置x方向加速度为0
    m_Particles[MAX_PARTICLES].yg = -0.8f;                      //设置y方向加速度为-0.8
    m_Particles[MAX_PARTICLES].zg = 0.0f;                       //设置z方向加速度为0

    for (int i = 0; i < MAX_PARTICLES; i++)                 //循环初始化所有粒子
    {
        m_Particles[i].active = false;                   //使所有粒子为非激活状态
        m_Particles[i].life = 1.0f;                     //所有粒子生命值为最大
        //随机生成衰减速率
        m_Particles[i].fade = float(rand() % 100) / 1000.0f + 0.001;
        //粒子的颜色
        m_Particles[i].r = COLORS[int(i * (12.0f / MAX_PARTICLES))][0];
        m_Particles[i].g = COLORS[int(i * (12.0f / MAX_PARTICLES))][1];
        m_Particles[i].b = COLORS[int(i * (12.0f / MAX_PARTICLES))][2];

        m_Particles[i].xg = -0.1f;                       //设置x方向加速度为-0.1
        m_Particles[i].yg = -0.1f;                      //设置y方向加速度为-0.1
        m_Particles[i].zg = -0.1f;                       //设置z方向加速度为-0.1
    }

    QTimer * timer = new QTimer(this);                   //创建一个定时器
    //将定时器的计时信号与updateGL()绑定
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(10);                                   //以10ms为一个计时周期
}

void MyGLWidget::initializeGL()
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

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存
    glLoadIdentity();                                   //重置模型观察矩阵
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    if (m_Particles[MAX_PARTICLES].active)                      //如果粒子为激活的
    {
        //更新各方向坐标及速度
        m_Particles[MAX_PARTICLES].x += m_Particles[MAX_PARTICLES].xi / (m_Slowdown * 1000);
        m_Particles[MAX_PARTICLES].y += m_Particles[MAX_PARTICLES].yi / (m_Slowdown * 1000);
        m_Particles[MAX_PARTICLES].z += m_Particles[MAX_PARTICLES].zi / (m_Slowdown * 1000);

        float x = m_Particles[MAX_PARTICLES].x;
        float y = m_Particles[MAX_PARTICLES].y;
        float z = m_Particles[MAX_PARTICLES].z + m_Deep;

        // 设置粒子颜色
        glColor4f(m_Particles[MAX_PARTICLES].r, m_Particles[MAX_PARTICLES].g, m_Particles[MAX_PARTICLES].b, m_Particles[MAX_PARTICLES].life);

        glBegin(GL_TRIANGLE_STRIP); // 绘制三角形带
        glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
        glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
        glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
        glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
        glEnd();

        // 粒子爆炸的判断和处理
        if (m_Particles[MAX_PARTICLES].y > 10.0f) // 爆炸条件，当粒子达到某个高度
        {
            m_Particles[MAX_PARTICLES].xi = 0.0f;
            m_Particles[MAX_PARTICLES].yi = 0.0f;
            m_Particles[MAX_PARTICLES].zi = 0.0f;

            float n_x = m_Particles[MAX_PARTICLES].x;
            float n_y = m_Particles[MAX_PARTICLES].y;
            float n_z = m_Particles[MAX_PARTICLES].z;

            m_Particles[MAX_PARTICLES].life -= m_Particles[MAX_PARTICLES].fade; //减少粒子的生命值
            /*if (m_Particles[MAX_PARTICLES].life < 0.0f) {
                m_Particles[MAX_PARTICLES].active = false;
            }*/

            for (int i = 0; i < MAX_PARTICLES; i++)                 //循环所有的粒子
            {
                if (!m_Particles[i].active)                      //如果粒子为非激活的
                {
                    m_Particles[i].active = true;
                    m_Particles[i].life = 1.0f;

                    //粒子的初始位置
                    m_Particles[i].x = n_x;
                    m_Particles[i].y = n_y;
                    m_Particles[i].z = n_z;

                    //随机生成x、y、z轴方向速度
                    m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
                    m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
                    m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;
                }
            }

        }

        for (int i = 0; i < MAX_PARTICLES; i++)                 //循环所有的粒子
        {
            if (m_Particles[i].active)                      //如果粒子为激活的
            {
                //更新各方向坐标及速度
                m_Particles[i].x += m_Particles[i].xi / (m_Slowdown * 1000);
                m_Particles[i].y += m_Particles[i].yi / (m_Slowdown * 1000);
                m_Particles[i].z += m_Particles[i].zi / (m_Slowdown * 1000);

                float x = m_Particles[i].x;
                float y = m_Particles[i].y;
                float z = m_Particles[i].z + m_Deep;

                // 设置粒子颜色
                glColor4f(m_Particles[i].r, m_Particles[i].g, m_Particles[i].b, m_Particles[i].life);

                glBegin(GL_TRIANGLE_STRIP); // 绘制三角形带
                glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
                glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
                glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
                glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
                glEnd();

                m_Particles[i].life -= m_Particles[i].fade; //减少粒子的生命值
                if (m_Particles[i].life < 0.0f) {
                    m_Particles[i].active = false;
                }
            }
        }
    }

    if (m_Rainbow)                                      //如果为彩虹模式
    {
        m_Color++;                                      //进行颜色的变换
        if (m_Color > 11) {
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
                m_Particles[i].y = -10.0f;
                m_Particles[i].z = 0.0f;

                //随机生成速度
                m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
                m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
                m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;
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