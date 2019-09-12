#include "dopenglwidget.h"
#include <QOpenGLShader>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QDebug>

DOpenGLWidget::DOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    this->resize(800, 600);
    translate_z = -6.0;
    translate_x = 0.0;
    translate_y = 0.0;
    xRot = zRot = 0.0;
    yRot = -30.0;
    m_mouseLeftPress = false;

    m_cameraPos   = QVector3D(0.0f, 0.0f,  3.0f);
    m_cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
    m_cameraUp    = QVector3D(0.0f, 1.0f,  0.0f);
}

DOpenGLWidget::~DOpenGLWidget()
{
    m_vbo.destroy();
    if(m_pTexture)
    {
        for(int i = 0; i < Texture_Num; ++i)
        {
            delete m_pTexture[i];
            m_pTexture[i] = nullptr;
        }
    }
}

void DOpenGLWidget::initializeGL()
{
    // 为当前环境初始化OpenGL函数
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    QOpenGLShader* vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    bool success = vertexShader->compileSourceFile(":/glsl/vertex.glsl");
    if(!success)
    {
        qDebug() << vertexShader->log();
    }
    QOpenGLShader* fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    success = fragmentShader->compileSourceFile(":/glsl/fragment.glsl");
    if(!success)
    {
        qDebug() << fragmentShader->log();
    }

    m_pShaderProgram = new QOpenGLShaderProgram(this);
    m_pShaderProgram->addShader(vertexShader);
    m_pShaderProgram->addShader(fragmentShader);
    m_pShaderProgram->link();
    m_pShaderProgram->bind();
    initTexture();

    m_pShaderProgram->setUniformValue("ourTexture1", 0);
    m_pShaderProgram->setUniformValue("ourTexture2", 1);

    // 顶点位置
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // world space positions of our cubes
    QVector3D cubePositions[] = {
        QVector3D( 0.0f,  0.0f,  0.0f),
        QVector3D( 2.0f,  5.0f, -15.0f),
        QVector3D(-1.5f, -2.2f, -2.5f),
        QVector3D(-3.8f, -2.0f, -12.3f),
        QVector3D( 2.4f, -0.4f, -3.5f),
        QVector3D(-1.7f,  3.0f, -7.5f),
        QVector3D( 1.3f, -2.0f, -2.5f),
        QVector3D( 1.5f,  2.0f, -2.5f),
        QVector3D( 1.5f,  0.2f, -1.5f),
        QVector3D(-1.3f,  1.0f, -1.5f)
    };

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));

    m_pShaderProgram->enableAttributeArray(0);
    m_pShaderProgram->enableAttributeArray(1);

    m_pShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0 , 3, 5 * sizeof(float));
    m_pShaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

}

void DOpenGLWidget::paintGL()
{
    QElapsedTimer eTime;
    eTime.start();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 绘制
    m_pTexture[0]->bind(0);
    m_pTexture[1]->bind(1);

    QMatrix4x4 viewMat;

    viewMat.lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    QMatrix4x4 projectionMat;
    projectionMat.perspective(45.0f, (GLfloat)width()/(GLfloat)height(), 0.1f, 100.0f);

    m_pShaderProgram->setUniformValue("view", viewMat);
    m_pShaderProgram->setUniformValue("projection", projectionMat);

    QMatrix4x4 modelMat;
    modelMat.translate(translate_x, translate_y, translate_z);
    modelMat.rotate(xRot, 1.0, 0.0, 0.0);
    modelMat.rotate(yRot, 0.0, 1.0, 0.0);
    modelMat.rotate(zRot, 0.0, 0.0, 1.0);

    m_pShaderProgram->setUniformValue("model", modelMat);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_vbo.release();

    m_renderTime = (1.0 * eTime.nsecsElapsed()) / pow(10, 9);

}

void DOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void DOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    float cameraSpeed = 1000 *  m_renderTime;// adjust accordingly
    qDebug() << "cameraSpeed: " << cameraSpeed;
    switch (event->key()) {
    case Qt::Key_X:
        xRot += 10;
        break;
    case Qt::Key_Y:
        yRot += 10;
        break;
    case Qt::Key_Z:
        zRot += 10;
        break;
    case Qt::Key_PageUp:
        translate_z += 1;
        break;
    case Qt::Key_PageDown:
        translate_z -= 1;
        break;
    case Qt::Key_A:
        m_cameraPos += QVector3D::crossProduct(m_cameraFront, m_cameraUp).normalized() * cameraSpeed;
        break;
    case Qt::Key_D:
        m_cameraPos -= QVector3D::crossProduct(m_cameraFront, m_cameraUp).normalized() * cameraSpeed;
        break;
    case Qt::Key_S:
        m_cameraPos += m_cameraUp * cameraSpeed;
        break;
    case Qt::Key_W:
        m_cameraPos -= m_cameraUp * cameraSpeed;
        break;
    default:
        break;
    }
    update();
}

void DOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mouseLeftPress = true;
     m_startPos = event->pos();
}

void DOpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(QEvent::MouseButtonRelease != event->type())
    {
        return;
    }
    m_mouseLeftPress = false;
}

void DOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_mouseLeftPress)
    {
        QPoint diff = event->pos() - m_startPos;
        qDebug() << diff.x() << ", " << diff.y();
        m_startPos = event->pos();
        xRot += diff.y();
        yRot += diff.x();
        update();
    }
}

void DOpenGLWidget::wheelEvent(QWheelEvent *event)
{
    float zt = ((event->angleDelta().y()) * 1.0) / (120.0);
    translate_z += zt;
    this->update();
}

void DOpenGLWidget::initTexture()
{
    m_pTexture[0] = new QOpenGLTexture(QImage(":/image/container.jpg").mirrored());
    m_pTexture[1] = new QOpenGLTexture(QImage(":/image/awesomeface.png").mirrored());
    for(int i = 0; i < Texture_Num; ++i)
    {
        m_pTexture[i]->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
        m_pTexture[i]->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
        m_pTexture[i]->setMinificationFilter(QOpenGLTexture::Linear);
        m_pTexture[i]->setMagnificationFilter(QOpenGLTexture::Linear);
    }

}
