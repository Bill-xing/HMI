#include "rrglwidget.h"
//#include <GL/glu.h>

#include <QDebug>
#include <QMouseEvent>
#include <QtOpenGL>

// RRGLWidget类：自定义的OpenGL窗口部件，用于绘制3D图形
// 继承自QGLWidget，提供OpenGL绘制功能

RRGLWidget::RRGLWidget(QWidget *parent) : QGLWidget(parent) {
    // 构造函数，初始化OpenGL窗口
}

RRGLWidget::~RRGLWidget() {
    // 析构函数，清理资源（如果有需要）
}

// 绘制网格函数
void RRGLWidget::drawGrid() {
    glPushMatrix();  // 保存当前坐标系状态
    GLfloat color[] = {8.0f / 255, 108.0f / 255, 162.0f / 255};  // 设置网格颜色

    // 设置材质的环境光和漫射光反射率
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

    int step = 50;  // 网格线间隔
    int num = 15;   // 网格线数量
    for (int i = -num; i < num + 1; i++) {
        glBegin(GL_LINES);  // 开始绘制线段
        // 绘制垂直线
        glVertex3f(i * step, -num * step, 0);
        glVertex3f(i * step, num * step, 0);
        // 绘制水平线
        glVertex3f(-num * step, i * step, 0);
        glVertex3f(num * step, i * step, 0);
        glEnd();  // 结束绘制线段
    }
    glPopMatrix();  // 恢复之前保存的坐标系状态
}

// 绘制坐标系函数
void RRGLWidget::drawCoordinates() {
    glPushMatrix();  // 保存当前坐标系状态
    glLineWidth(2.0f);  // 设置线宽
    setupColor(255, 255, 255);  // 设置坐标轴颜色为白色
    glBegin(GL_LINES);  // 开始绘制线段

    // 绘制X轴
    glVertex3f(-900, 0, 0);
    glVertex3f(900, 0, 0);
    // 绘制Y轴
    glVertex3f(0, -900, 0);
    glVertex3f(0, 900, 0);
    // 绘制Z轴
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 700);
    glEnd();  // 结束绘制线段

    // 绘制坐标轴标签
    qglColor(QColor::fromRgbF(1, 0, 0));  // 设置X轴标签颜色为红色
    renderText(-900, 0, 0, "-X", QFont("helvetica", 12, QFont::Bold, true));  // 绘制-X标签
    renderText(900, 0, 0, "+X", QFont("helvetica", 12, QFont::Bold, true));  // 绘制+X标签
    qglColor(QColor::fromRgbF(0, 1, 0));  // 设置Y轴标签颜色为绿色
    renderText(0, -900, 0, "-Y", QFont("helvetica", 12, QFont::Bold, true));  // 绘制-Y标签
    renderText(0, 900, 0, "+Y", QFont("helvetica", 12, QFont::Bold, true));  // 绘制+Y标签
    qglColor(QColor::fromRgbF(0, 0, 1));  // 设置Z轴标签颜色为蓝色
    renderText(0, 0, 700, "+Z", QFont("helvetica", 12, QFont::Bold, true));  // 绘制+Z标签
    glLineWidth(1.0f);  // 恢复线宽
    glPopMatrix();  // 恢复之前保存的坐标系状态
}

// 绘制STL坐标系函数
void RRGLWidget::drawSTLCoordinates(int r, int g, int b) {
    glPushMatrix();  // 保存当前坐标系状态
    glLineWidth(1.5f);  // 设置线宽
    setupColor(r, g, b);  // 设置坐标系颜色

    glBegin(GL_LINES);  // 开始绘制线段
    // 绘制X轴
    glVertex3f(-300, 0, 0);
    glVertex3f(300, 0, 0);
    // 绘制Y轴
    glVertex3f(0, -300, 0);
    glVertex3f(0, 300, 0);
    // 绘制Z轴
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 500);
    glEnd();  // 结束绘制线段

    // 绘制坐标轴标签
    qglColor(QColor::fromRgbF(1, 0, 0));  // 设置X轴标签颜色为红色
    renderText(-300, 0, 0, "-X", QFont("helvetica", 12, QFont::Bold, true));  // 绘制-X标签
    renderText(300, 0, 0, "+X", QFont("helvetica", 12, QFont::Bold, true));  // 绘制+X标签
    qglColor(QColor::fromRgbF(0, 1, 0));  // 设置Y轴标签颜色为绿色
    renderText(0, -300, 0, "-Y", QFont("helvetica", 12, QFont::Bold, true));  // 绘制-Y标签
    renderText(0, 300, 0, "+Y", QFont("helvetica", 12, QFont::Bold, true));  // 绘制+Y标签
    qglColor(QColor::fromRgbF(0, 0, 1));  // 设置Z轴标签颜色为蓝色
    renderText(0, 0, 500, "+Z", QFont("helvetica", 12, QFont::Bold, true));  // 绘制+Z标签
    glLineWidth(1.0f);  // 恢复线宽
    glPopMatrix();  // 恢复之前保存的坐标系状态
}

// 绘制OpenGL内容的主函数
void RRGLWidget::drawGL() {
    // 具体绘制内容的实现
}

// 设置颜色的辅助函数
void RRGLWidget::setupColor(int r, int g, int b) {
    GLfloat color[] = {static_cast<GLfloat>(r / 255.0), static_cast<GLfloat>(g / 255.0),
                       static_cast<GLfloat>(b / 255.0)};  // 将RGB值转换为0-1范围
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);  // 设置材质的环境光和漫射光反射率
}

// 设置X轴旋转的函数
void RRGLWidget::setXRotation(int angle) {
    int tangle = angle;  // 规范化角度
    if (tangle != xRot) {  // 如果角度发生变化
        xRot = tangle;  // 更新X轴旋转角度
        emit xRotationChanged(angle);  // 发射信号，通知角度变化
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清除颜色和深度缓冲
        updateGL();  // 更新OpenGL窗口
    }
}

// 设置Y轴旋转的函数
void RRGLWidget::setYRotation(int angle) {
    int tangle = angle;  // 规范化角度
    if (tangle != yRot) {  // 如果角度发生变化
        yRot = tangle;  // 更新Y轴旋转角度
        emit yRotationChanged(angle);  // 发射信号，通知角度变化
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清除颜色和深度缓冲
    }
}

// 设置X轴和Y轴平移的函数
void RRGLWidget::setXYTranslate(int dx, int dy) {
    xTran += 3.0 * dx;  // 更新X轴平移量
    yTran -= 3.0 * dy;  // 更新Y轴平移量
    updateGL();  // 更新OpenGL窗口
}

// 规范化角度的函数
int RRGLWidget::normalizeAngle(int angle) {
    int tangle = angle;  // 规范化角度
    while (tangle < 0) {
        tangle += 360 * 16;  // 确保角度为正
    }
    while (tangle > 360 * 16) {
        tangle -= 360 * 16;  // 确保角度在360度范围内
    }
    return tangle;  // 返回规范化后的角度
}

// 设置缩放的函数
void RRGLWidget::setZoom(int zoom) {
    z_zoom = zoom;  // 更新缩放值
    updateGL();  // 更新OpenGL窗口
}

// 初始化OpenGL设置的函数
void RRGLWidget::initializeGL() {
    // 设置光源的环境光、散射光和镜面反射光强度
    GLfloat ambientLight[] = {0.7f, 0.7f, 0.7f, 1.0f};      // 环境光强度
    GLfloat diffuseLight[] = {0.7f, 0.8f, 0.8f, 1.0f};      // 散射光强度
    GLfloat specularLight[] = {0.4f, 0.4f, 0.4f, 1.0f};     // 镜面反射光强度
    GLfloat positionLight[] = {20.0f, 20.0f, 20.0f, 0.0f};  // 光源位置

    // 设置光源属性
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);     // 设置0号光源的环境光属性
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);     // 设置0号光源的散射光属性
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);   // 设置0号光源的镜面反射光属性
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);        // 设置照明模型参数
    glLightfv(GL_LIGHT0, GL_POSITION, positionLight);   // 设置0号光源的位置属性

    glEnable(GL_LIGHTING);   // 启用光照
    glEnable(GL_LIGHT0);     // 打开光源
    // glEnable(GL_DEPTH_TEST); // 启用深度测试（可选）

    glClearDepth(1.0);       // 设置深度缓存
    glEnable(GL_DEPTH_TEST); // 启用深度测试
    glDepthFunc(GL_LEQUAL);  // 设置深度测试的类型
    glEnable(GL_NORMALIZE);  // 启用法向量归一化

    glClearColor(0.0, 0.0, 0.0, 1.0);  // 设置背景颜色为黑色
}

// 用于绘制OpenGL内容的主函数
void RRGLWidget::paintGL() {
    // 具体绘制内容的实现
}

// 处理窗口大小变化的函数
void RRGLWidget::resizeGL(int w, int h) {
    if (w < 0 || h < 0) {
        return;  // 确保宽高有效
    }
    glViewport(0, 0, w, h);  // 设置视口大小
    glMatrixMode(GL_PROJECTION);  // 切换到投影矩阵模式
    glLoadIdentity();  // 重置投影矩阵

    GLfloat zNear = 1.0;           // 近裁剪面
    GLfloat zFar = 20000.0;        // 远裁剪面
    GLfloat aspect = (GLfloat) w / (GLfloat) h;  // 计算宽高比
    GLfloat fH = tan(GLfloat(70.0 / 360.0 * 3.14159)) * zNear;  // 计算视野高度
    GLfloat fW = fH * aspect;      // 计算视野宽度
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);  // 设置透视投影矩阵

    glMatrixMode(GL_MODELVIEW);  // 切换到模型视图矩阵模式
    glLoadIdentity();  // 重置模型视图矩阵
    glTranslated(0.0, 0.0, -40.0);  // 设置初始观察位置
}

// 鼠标按下事件处理
void RRGLWidget::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();  // 记录鼠标按下的位置
}

// 鼠标移动事件处理
void RRGLWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - lastPos.x();  // 计算X轴移动距离
    int dy = event->y() - lastPos.y();  // 计算Y轴移动距离
    // 这里必须使用buttons()
    if (event->buttons() & Qt::LeftButton) {  // 如果左键按下
        setXRotation(xRot + 4 * dy);  // 更新X轴旋转
        setYRotation(yRot - 4 * dx);  // 更新Y轴旋转
    } else if (event->buttons() & Qt::RightButton) {  // 如果右键按下
        setZoom(z_zoom + 5.0 * dy);  // 更新缩放
    } else if (event->buttons() & Qt::MidButton) {  // 如果中键按下
        setXYTranslate(dx, dy);  // 更新平移
    }
    lastPos = event->pos();  // 更新最后位置
}
