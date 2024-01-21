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

const GLfloat MyGLWidget::COLORS[][3] =  //烟花粒子颜色
{
     {1.0f, 0.5f, 0.5f}, { 1.0f, 0.75f, 0.5f }, { 1.0f, 1.0f, 0.5f },
     {0.75f, 1.0f, 0.5f}, { 0.5f, 1.0f, 0.5f }, { 0.5f, 1.0f, 0.75f },
     {0.5f, 1.0f, 1.0f}, { 0.5f, 0.75f, 1.0f }, { 0.5f, 0.5f, 1.0f },
     {0.75f, 0.5f, 1.0f}, { 1.0f, 0.5f, 1.0f }, { 1.0f, 0.5f, 0.75f }
};


MyGLWidget::MyGLWidget(QWidget* parent) :
     QGLWidget(parent)
{
    m_FileName = "E:/fireworkPicture.png"; //烟花粒子纹理
    m_Rainbow = false;
    m_Color = 0;
    m_Slowdown = 2.0f;
    m_xSpeed = 0.0f;
    m_ySpeed = 0.0f;
    m_Deep = -40.0f;

    m_Particles[MAX_PARTICLES].active = true; //使触发器粒子为激活状态
    m_Particles[MAX_PARTICLES].life = 1.0f; //触发器粒子生命值为最大
    m_Particles[MAX_PARTICLES].fade = float(rand() % 100) / 1000.0f + 0.001; //随机生成触发器粒子的衰减速率

    //随机生成触发器粒子的颜色
    m_Particles[MAX_PARTICLES].r = COLORS[m_Rainbow ? rand() % 12 : m_Color][0];
    m_Particles[MAX_PARTICLES].g = COLORS[m_Rainbow ? rand() % 12 : m_Color][1];
    m_Particles[MAX_PARTICLES].b = COLORS[m_Rainbow ? rand() % 12 : m_Color][2];

    //触发器粒子的初始位置
    m_Particles[MAX_PARTICLES].x = 0.0f;
    m_Particles[MAX_PARTICLES].y = -20.0f; //从底部向上喷出
    m_Particles[MAX_PARTICLES].z = 0.0f;

    //随机生成触发器粒子在x、y、z轴方向上的速度
    m_Particles[MAX_PARTICLES].xi = float((rand() % 50) - 26.0f) * 2.0f;
    m_Particles[MAX_PARTICLES].yi = float((rand() % 50) + 25.0f) * 10.0f;
    m_Particles[MAX_PARTICLES].zi = float((rand() % 50) - 25.0f) * 2.0f;

    //设置触发器粒子在x、y、z轴方向上的加速度
    m_Particles[MAX_PARTICLES].xg = 0.0f;
    m_Particles[MAX_PARTICLES].yg = -0.8f;
    m_Particles[MAX_PARTICLES].zg = 0.0f;

    for (int i = 0; i < MAX_PARTICLES; i++) //循环初始化所有粒子
    {
        m_Particles[i].active = false; //使所有粒子为非激活状态
        m_Particles[i].life = 1.0f; //所有粒子生命值为最大
        m_Particles[i].fade = float(rand() % 100) / 1000.0f + 0.001; //随机生成衰减速率

        //粒子的颜色
        m_Particles[i].r = COLORS[m_Rainbow ? rand() % 12 : m_Color][0];
        m_Particles[i].g = COLORS[m_Rainbow ? rand() % 12 : m_Color][1];
        m_Particles[i].b = COLORS[m_Rainbow ? rand() % 12 : m_Color][2];

        //粒子在每个方向上的加速度
        m_Particles[i].xg = -0.1f;
        m_Particles[i].yg = -0.1f;
        m_Particles[i].zg = -0.1f;
    }

    //创建一个定时器
    QTimer * timer = new QTimer(this);
    //将定时器的计时信号与updateGL()绑定
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    //以10ms为一个计时周期
    timer->start(10);
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
    showFullScreen();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕和深度缓存
    glLoadIdentity(); //重置模型观察矩阵
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    if (m_Particles[MAX_PARTICLES].active) //如果粒子为激活的
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
        if (m_Particles[MAX_PARTICLES].y > 10.0f && m_Particles[MAX_PARTICLES].life > 0) // 爆炸条件，当粒子达到某个高度
        {
            m_Particles[MAX_PARTICLES].xi = 0.0f;
            m_Particles[MAX_PARTICLES].yi = 0.0f;
            m_Particles[MAX_PARTICLES].zi = 0.0f;

            float n_x = m_Particles[MAX_PARTICLES].x;
            float n_y = m_Particles[MAX_PARTICLES].y;
            float n_z = m_Particles[MAX_PARTICLES].z;

            //减少粒子的生命值
            m_Particles[MAX_PARTICLES].life -= m_Particles[MAX_PARTICLES].fade;

            for (int i = 0; i < MAX_PARTICLES; i++)  //循环所有的粒子
            {
                if (!m_Particles[i].active) //如果粒子为非激活的
                {
                    m_Particles[i].active = true;
                    m_Particles[i].life = 1.0f;

                    //粒子的初始位置
                    m_Particles[i].x = n_x;
                    m_Particles[i].y = n_y;
                    m_Particles[i].z = n_z;

                    //粒子的颜色
                    m_Particles[i].r = COLORS[m_Rainbow ? rand() % 12 : m_Color][0];
                    m_Particles[i].g = COLORS[m_Rainbow ? rand() % 12 : m_Color][1];
                    m_Particles[i].b = COLORS[m_Rainbow ? rand() % 12 : m_Color][2];

                    //随机生成x、y、z轴方向速度
                    m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
                    m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
                    m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;
                }
            }

        }
        else if (m_Particles[MAX_PARTICLES].y > 10.0f && m_Particles[MAX_PARTICLES].life <= 0) // 重复条件，当粒子达到某个高度，并且不再燃放
        {
            m_Particles[MAX_PARTICLES].active = true; //使触发器粒子为激活状态
            m_Particles[MAX_PARTICLES].life = 1.0f; //触发器粒子生命值为最大
            m_Particles[MAX_PARTICLES].fade = float(rand() % 100) / 1000.0f + 0.001; //随机生成触发器粒子的衰减速率

            //随机生成触发器粒子的颜色
            m_Particles[MAX_PARTICLES].r = COLORS[m_Rainbow ? rand() % 12 : m_Color][0];
            m_Particles[MAX_PARTICLES].g = COLORS[m_Rainbow ? rand() % 12 : m_Color][1];
            m_Particles[MAX_PARTICLES].b = COLORS[m_Rainbow ? rand() % 12 : m_Color][2];

            //触发器粒子的初始位置
            m_Particles[MAX_PARTICLES].x = 0.0f;
            m_Particles[MAX_PARTICLES].y = -20.0f; //从底部向上喷出
            m_Particles[MAX_PARTICLES].z = 0.0f;

            //随机生成触发器粒子在x、y、z轴方向上的速度
            m_Particles[MAX_PARTICLES].xi = float((rand() % 50) - 26.0f) * 2.0f;
            m_Particles[MAX_PARTICLES].yi = float((rand() % 50) + 25.0f) * 10.0f;
            m_Particles[MAX_PARTICLES].zi = float((rand() % 50) - 25.0f) * 2.0f;

            //设置触发器粒子在x、y、z轴方向上的加速度
            m_Particles[MAX_PARTICLES].xg = 0.0f;
            m_Particles[MAX_PARTICLES].yg = -0.8f;
            m_Particles[MAX_PARTICLES].zg = 0.0f;
        }

        for (int i = 0; i < MAX_PARTICLES; i++) //循环所有的粒子
        {
            if (m_Particles[i].active) //如果粒子为激活的
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

                //减少粒子的生命值
                m_Particles[i].life -= m_Particles[i].fade;
                if (m_Particles[i].life < 0.0f) {
                    m_Particles[i].active = false;
                }
            }
        }
    }
}

void MyGLWidget::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_Escape:                                //ESC为退出键
            close();
            break;
        case Qt::Key_PageUp:                                //PageUp按下靠近粒子
            m_Deep += 0.5f;
            break;
        case Qt::Key_PageDown:                              //PageDown按下远离粒子
            m_Deep -= 0.5f;
            break;
        case Qt::Key_Return:                                //切换彩虹模式
            m_Rainbow = !m_Rainbow;
            break;
        case Qt::Key_Space:                                 //切换烟花粒子颜色
            m_Rainbow = false;
            m_Color++;
            if (m_Color > 11)
            {
                m_Color = 0;
            }
            break;
        case Qt::Key_Up:                                    //增加粒子y轴正方向的速度
            if (m_ySpeed < 400.0f)
            {
                m_ySpeed += 5.0f;
            }
            break;
        case Qt::Key_Down:                                  //减少粒子y轴正方向的速度
            if (m_ySpeed > -400.0f)
            {
                m_ySpeed -= 5.0f;
            }
            break;
        case Qt::Key_Right:                                 //增加粒子x轴正方向的速度
            if (m_xSpeed < 400.0f)
            {
                m_xSpeed += 5.0f;
            }
            break;
        case Qt::Key_Left:                                  //减少粒子x轴正方向的速度
            if (m_xSpeed > -400.0f)
            {
                m_xSpeed -= 5.0f;
            }
            break;
    }
}