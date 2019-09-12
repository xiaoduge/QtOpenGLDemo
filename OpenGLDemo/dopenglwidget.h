#ifndef DOPENGLWIDGET_H
#define DOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QVector3D>


namespace Ui {
class DOpenGLWidget;
}



class QOpenGLShaderProgram;

class DOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    enum EnumTexture
    {
        Texture_1,
        Texture_2,
        Texture_Num
    };

public:
    explicit DOpenGLWidget(QWidget *parent = 0);
    ~DOpenGLWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void keyPressEvent(QKeyEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void wheelEvent(QWheelEvent* event);

private:
    void initTexture();

private:
    QOpenGLShaderProgram* m_pShaderProgram;
    QOpenGLBuffer m_vbo;

    QOpenGLTexture* m_pTexture[Texture_Num];

    GLfloat translate_x, translate_y, translate_z, xRot, yRot, zRot;

    bool m_mouseLeftPress;
    QPoint m_startPos;

    QVector3D m_cameraPos;
    QVector3D m_cameraFront;
    QVector3D m_cameraUp;

    double m_renderTime;
};

#endif // DOPENGLWIDGET_H
