#include "myglwidget.h"

MyGLWidget::~MyGLWidget()
{

}

// �����ڴ�С�仯
void MyGLWidget::resizeGL(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(w) / static_cast<double>(h), 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
    m_FileName = "E:/fireworkPicture.png";    //Ӧ����ʵ�ʴ��ͼƬ��·�������޸�
    m_Rainbow = true;
    m_Color = 0;
    m_Slowdown = 2.0f;
    m_xSpeed = 0.0f;
    m_ySpeed = 0.0f;
    m_Deep = -40.0f;

    m_Particles[MAX_PARTICLES].active = true;                   //ʹԴ����Ϊ����״̬
    m_Particles[MAX_PARTICLES].life = 1.0f;                     //Դ��������ֵΪ���
    //�������˥������
    m_Particles[MAX_PARTICLES].fade = float(rand() % 100) / 1000.0f + 0.001;
    //���ӵ���ɫ
    m_Particles[MAX_PARTICLES].r = COLORS[int(MAX_PARTICLES * (12.0f / MAX_PARTICLES))][0];
    m_Particles[MAX_PARTICLES].g = COLORS[int(MAX_PARTICLES * (12.0f / MAX_PARTICLES))][1];
    m_Particles[MAX_PARTICLES].b = COLORS[int(MAX_PARTICLES * (12.0f / MAX_PARTICLES))][2];

    //���ӵĳ�ʼλ��
    m_Particles[MAX_PARTICLES].x = 0.0f;
    m_Particles[MAX_PARTICLES].y = -20.0f;  //�ӵײ��������
    m_Particles[MAX_PARTICLES].z = 0.0f;

    //�������x��y��z�᷽���ٶ�
    m_Particles[MAX_PARTICLES].xi = float((rand() % 50) - 26.0f) * 2.0f;
    m_Particles[MAX_PARTICLES].yi = float((rand() % 50) + 25.0f) * 10.0f;
    m_Particles[MAX_PARTICLES].zi = float((rand() % 50) - 25.0f) * 2.0f;

    m_Particles[MAX_PARTICLES].xg = 0.0f;                       //����x������ٶ�Ϊ0
    m_Particles[MAX_PARTICLES].yg = -0.8f;                      //����y������ٶ�Ϊ-0.8
    m_Particles[MAX_PARTICLES].zg = 0.0f;                       //����z������ٶ�Ϊ0

    for (int i = 0; i < MAX_PARTICLES; i++)                 //ѭ����ʼ����������
    {
        m_Particles[i].active = false;                   //ʹ��������Ϊ�Ǽ���״̬
        m_Particles[i].life = 1.0f;                     //������������ֵΪ���
        //�������˥������
        m_Particles[i].fade = float(rand() % 100) / 1000.0f + 0.001;
        //���ӵ���ɫ
        m_Particles[i].r = COLORS[int(i * (12.0f / MAX_PARTICLES))][0];
        m_Particles[i].g = COLORS[int(i * (12.0f / MAX_PARTICLES))][1];
        m_Particles[i].b = COLORS[int(i * (12.0f / MAX_PARTICLES))][2];

        m_Particles[i].xg = -0.1f;                       //����x������ٶ�Ϊ-0.1
        m_Particles[i].yg = -0.1f;                      //����y������ٶ�Ϊ-0.1
        m_Particles[i].zg = -0.1f;                       //����z������ٶ�Ϊ-0.1
    }

    QTimer * timer = new QTimer(this);                   //����һ����ʱ��
    //����ʱ���ļ�ʱ�ź���updateGL()��
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(10);                                   //��10msΪһ����ʱ����
}

void MyGLWidget::initializeGL()
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

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�����Ļ����Ȼ���
    glLoadIdentity();                                   //����ģ�͹۲����
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    if (m_Particles[MAX_PARTICLES].active)                      //�������Ϊ�����
    {
        //���¸��������꼰�ٶ�
        m_Particles[MAX_PARTICLES].x += m_Particles[MAX_PARTICLES].xi / (m_Slowdown * 1000);
        m_Particles[MAX_PARTICLES].y += m_Particles[MAX_PARTICLES].yi / (m_Slowdown * 1000);
        m_Particles[MAX_PARTICLES].z += m_Particles[MAX_PARTICLES].zi / (m_Slowdown * 1000);

        float x = m_Particles[MAX_PARTICLES].x;
        float y = m_Particles[MAX_PARTICLES].y;
        float z = m_Particles[MAX_PARTICLES].z + m_Deep;

        // ����������ɫ
        glColor4f(m_Particles[MAX_PARTICLES].r, m_Particles[MAX_PARTICLES].g, m_Particles[MAX_PARTICLES].b, m_Particles[MAX_PARTICLES].life);

        glBegin(GL_TRIANGLE_STRIP); // ���������δ�
        glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
        glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
        glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
        glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
        glEnd();

        // ���ӱ�ը���жϺʹ���
        if (m_Particles[MAX_PARTICLES].y > 10.0f) // ��ը�����������Ӵﵽĳ���߶�
        {
            m_Particles[MAX_PARTICLES].xi = 0.0f;
            m_Particles[MAX_PARTICLES].yi = 0.0f;
            m_Particles[MAX_PARTICLES].zi = 0.0f;

            float n_x = m_Particles[MAX_PARTICLES].x;
            float n_y = m_Particles[MAX_PARTICLES].y;
            float n_z = m_Particles[MAX_PARTICLES].z;

            m_Particles[MAX_PARTICLES].life -= m_Particles[MAX_PARTICLES].fade; //�������ӵ�����ֵ
            /*if (m_Particles[MAX_PARTICLES].life < 0.0f) {
                m_Particles[MAX_PARTICLES].active = false;
            }*/

            for (int i = 0; i < MAX_PARTICLES; i++)                 //ѭ�����е�����
            {
                if (!m_Particles[i].active)                      //�������Ϊ�Ǽ����
                {
                    m_Particles[i].active = true;
                    m_Particles[i].life = 1.0f;

                    //���ӵĳ�ʼλ��
                    m_Particles[i].x = n_x;
                    m_Particles[i].y = n_y;
                    m_Particles[i].z = n_z;

                    //�������x��y��z�᷽���ٶ�
                    m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
                    m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
                    m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;
                }
            }

        }

        for (int i = 0; i < MAX_PARTICLES; i++)                 //ѭ�����е�����
        {
            if (m_Particles[i].active)                      //�������Ϊ�����
            {
                //���¸��������꼰�ٶ�
                m_Particles[i].x += m_Particles[i].xi / (m_Slowdown * 1000);
                m_Particles[i].y += m_Particles[i].yi / (m_Slowdown * 1000);
                m_Particles[i].z += m_Particles[i].zi / (m_Slowdown * 1000);

                float x = m_Particles[i].x;
                float y = m_Particles[i].y;
                float z = m_Particles[i].z + m_Deep;

                // ����������ɫ
                glColor4f(m_Particles[i].r, m_Particles[i].g, m_Particles[i].b, m_Particles[i].life);

                glBegin(GL_TRIANGLE_STRIP); // ���������δ�
                glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z);
                glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z);
                glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z);
                glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z);
                glEnd();

                m_Particles[i].life -= m_Particles[i].fade; //�������ӵ�����ֵ
                if (m_Particles[i].life < 0.0f) {
                    m_Particles[i].active = false;
                }
            }
        }
    }

    if (m_Rainbow)                                      //���Ϊ�ʺ�ģʽ
    {
        m_Color++;                                      //������ɫ�ı任
        if (m_Color > 11) {
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
                m_Particles[i].y = -10.0f;
                m_Particles[i].z = 0.0f;

                //��������ٶ�
                m_Particles[i].xi = float((rand() % 50) - 26.0f) * 10.0f;
                m_Particles[i].yi = float((rand() % 50) - 25.0f) * 10.0f;
                m_Particles[i].zi = float((rand() % 50) - 25.0f) * 10.0f;
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