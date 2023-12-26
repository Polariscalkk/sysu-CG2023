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

const GLfloat MyGLWidget::COLORS[][3] =                 //�ʺ����ɫ
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
    m_FileName = "C:/Users/user/����/fireworkPicture.png";    //Ӧ����ʵ�ʴ��ͼƬ��·�������޸�
    m_Rainbow = true;
    m_Color = 0;
    m_Slowdown = 2.0f;
    m_xSpeed = 0.0f;
    m_ySpeed = 0.0f;
    m_Deep = -40.0f;

    for (int i = 0; i < MAX_PARTICLES; i++)                 //ѭ����ʼ����������
    {
        m_Particles[i].active = true;                   //ʹ��������Ϊ����״̬
        m_Particles[i].life = 1.0f;                     //������������ֵΪ���
        //�������˥������
        m_Particles[i].fade = float(rand() % 100) / 1000.0f + 0.001;
        //���ӵ���ɫ
        m_Particles[i].r = COLORS[int(i * (12.0f / MAX_PARTICLES))][0];
        m_Particles[i].g = COLORS[int(i * (12.0f / MAX_PARTICLES))][1];
        m_Particles[i].b = COLORS[int(i * (12.0f / MAX_PARTICLES))][2];
         //���ӵĳ�ʼλ��
        m_Particles[i].x = 0.0f;
        m_Particles[i].y = 0.0f;
        m_Particles[i].z = 0.0f;

        //�������x��y��z�᷽���ٶ�
        m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
        m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
        m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;

        m_Particles[i].xg = 0.0f;                       //����x������ٶ�Ϊ0
        m_Particles[i].yg = -0.8f;                      //����y������ٶ�Ϊ-0.8
        m_Particles[i].zg = 0.0f;                       //����z������ٶ�Ϊ0
    }

    QTimer * timer = new QTimer(this);                   //����һ����ʱ��
    //����ʱ���ļ�ʱ�ź���updateGL()��
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(10);                                   //��10msΪһ����ʱ����
}

void MyGLWidget::initializeGL()                         //�˴���ʼ��OpenGL������������
{
    m_Texture = bindTexture(QPixmap(m_FileName));       //����λͼ��ת��������
    glEnable(GL_TEXTURE_2D);                            //��������ӳ��

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);               //��ɫ����
    glShadeModel(GL_SMOOTH);                            //������Ӱƽ��
    glClearDepth(1.0);                                  //������Ȼ���
    glDisable(GL_DEPTH_TEST);                           //��ֹ��Ȳ���
    glEnable(GL_BLEND);                                 //�����ں�
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);                  //�����ں�����
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //����ϵͳ��͸�ӽ�������
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
}

void MyGLWidget::paintGL()                              //�����￪ʼ�������ԵĻ���
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�����Ļ����Ȼ���
    glLoadIdentity();                                   //����ģ�͹۲����
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    for (int i = 0; i < MAX_PARTICLES; i++)                 //ѭ�����е�����
    {
        if (m_Particles[i].active)                      //�������Ϊ�����
        {
            float x = m_Particles[i].x;                 //x��λ��
            float y = m_Particles[i].y;                 //y��λ��
            float z = m_Particles[i].z + m_Deep;        //z��λ��
            //����������ɫ
            glColor4f(m_Particles[i].r, m_Particles[i].g,
                      m_Particles[i].b, m_Particles[i].life);
            glBegin(GL_TRIANGLE_STRIP);                 //���������δ�
                glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
                glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
                glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
                glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
            glEnd();

            //���¸��������꼰�ٶ�
            m_Particles[i].x += m_Particles[i].xi / (m_Slowdown * 1000);
            m_Particles[i].y += m_Particles[i].yi / (m_Slowdown * 1000);
            m_Particles[i].z += m_Particles[i].zi / (m_Slowdown * 1000);
            m_Particles[i].xi += m_Particles[i].xg;
            m_Particles[i].yi += m_Particles[i].yg;
            m_Particles[i].zi += m_Particles[i].zg;

            m_Particles[i].life -= m_Particles[i].fade; //�������ӵ�����ֵ
            if (m_Particles[i].life < 0.0f)             //�����������ֵС��0
            {
                m_Particles[i].life = 1.0f;             //����һ��������
                m_Particles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

                m_Particles[i].r = COLORS[m_Color][0];  //������ɫ
                m_Particles[i].g = COLORS[m_Color][1];
                m_Particles[i].b = COLORS[m_Color][2];

                m_Particles[i].x = 0.0f;                //���ӳ�������Ļ����
                m_Particles[i].y = 0.0f;
                m_Particles[i].z = 0.0f;

                //������������ٶ�
                m_Particles[i].xi = m_xSpeed + float((rand() % 60) - 32.0f);
                m_Particles[i].yi = m_ySpeed + float((rand() % 60) - 30.0f);
                m_Particles[i].zi = float((rand() % 60) - 30.0f);
            }
        }
    }

    if (m_Rainbow)                                      //���Ϊ�ʺ�ģʽ
    {
        m_Color++;                                      //������ɫ�ı任
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
        case Qt::Key_F1:                                    //F1Ϊȫ������ͨ�����л���
            fullscreen = !fullscreen;
            if (fullscreen) {
                showFullScreen();
            }
            else {
                showNormal();
            }
            updateGL();
            break;
        case Qt::Key_Escape:                                //ESCΪ�˳���
            close();
            break;
        case Qt::Key_Tab:                                   //Tab����ʹ���ӻص�ԭ�㣬������ը
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                m_Particles[i].x = 0.0f;
                m_Particles[i].y = 0.0f;
                m_Particles[i].z = 0.0f;

                //��������ٶ�
                m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
                m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
                m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;
            }
            break;
        case Qt::Key_8:                                     //����8����y������ٶ�
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (m_Particles[i].yg < 3.0f)
                {
                    m_Particles[i].yg += 0.05f;
                }
            }
            break;
        case Qt::Key_2:                                     //����2����y������ٶ�
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (m_Particles[i].yg > -3.0f)
                {
                    m_Particles[i].yg -= 0.05f;
                }
            }
            break;
        case Qt::Key_6:                                     //����6����x������ٶ�
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (m_Particles[i].xg < 3.0f)
                {
                    m_Particles[i].xg += 0.05f;
                }
            }
            break;
        case Qt::Key_4:                                     //����4����x������ٶ�
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                if (m_Particles[i].xg > -3.0f)
                {
                    m_Particles[i].xg -= 0.05f;
                }
            }
            break;
        case Qt::Key_Plus:                                  //+ �Ű��¼�������
            if (m_Slowdown > 1.0f)
            {
                m_Slowdown -= 0.05f;
            }
            break;
        case Qt::Key_Minus:                                 //- �Ű��¼�������
            if (m_Slowdown < 3.0f)
            {
                m_Slowdown += 0.05f;
            }
            break;
        case Qt::Key_PageUp:                                //PageUp����ʹ���ӿ�����Ļ
            m_Deep += 0.5f;
            break;
        case Qt::Key_PageDown:                              //PageDown����ʹ����Զ����Ļ
            m_Deep -= 0.5f;
            break;
        case Qt::Key_Return:                                //�س���Ϊ�Ƿ�ʺ�ģʽ���л���
            m_Rainbow = !m_Rainbow;
            break;
        case Qt::Key_Space:                                 //�ո��Ϊ��ɫ�л���
            m_Rainbow = false;
            m_Color++;
            if (m_Color > 11)
            {
                m_Color = 0;
            }
            break;
        case Qt::Key_Up:                                    //Up������������y����������ٶ�
            if (m_ySpeed < 400.0f)
            {
                m_ySpeed += 5.0f;
            }
            break;
        case Qt::Key_Down:                                  //Down���¼�������y����������ٶ�
            if (m_ySpeed > -400.0f)
            {
                m_ySpeed -= 5.0f;
            }
            break;
        case Qt::Key_Right:                                 //Right������������x����������ٶ�
            if (m_xSpeed < 400.0f)
            {
                m_xSpeed += 5.0f;
            }
            break;
        case Qt::Key_Left:                                  //Left���¼�������x����������ٶ�
            if (m_xSpeed > -400.0f)
            {
                m_xSpeed -= 5.0f;
            }
            break;

    }
}