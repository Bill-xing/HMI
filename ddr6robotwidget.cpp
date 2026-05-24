#include "ddr6robotwidget.h"
#include <QDebug>
#include <QDir>
#include "runtime_paths.h"

// DDR6RobotWidget类：用于渲染和控制6轴机器人的3D模型显示
// 继承自RRGLWidget，实现OpenGL渲染功能

namespace {
QString stlPath(const QString &fileName)
{
    return RuntimePaths::resourcePath("res/binary/" + fileName);
}
}

DDR6RobotWidget::DDR6RobotWidget(QWidget *parent)
        : RRGLWidget(parent) {
    // 初始化视角参数
    z_zoom = -500;    // 观察距离，负值表示向后移动视点
    xRot = 30 * 16;    // 绕X轴旋转角度（Qt中角度值需要乘16）
    yRot = 45 * 16;    // 绕Y轴旋转角度
    xTran = 0;         // X轴平移量
    yTran = -200;         // Y轴平移量
    loadRobotModelSTLFile();
    configureModelParams();
}

DDR6RobotWidget::~DDR6RobotWidget() {

}

void DDR6RobotWidget::loadRobotModelSTLFile() {
    // 加载机器人各个连杆的STL模型文件
    // 基座使用1000的缩放比例（米转毫米）
    // 其他连杆使用1的缩放比例（已经是毫米单位）
    mRobotModel.link0 = new STLFileLoader(stlPath("base_link.STL"), 1);  // 基座
    mRobotModel.link1 = new STLFileLoader(stlPath("link_1.STL"), 1);     // 第1关节
    mRobotModel.link2 = new STLFileLoader(stlPath("link_2.STL"), 1);
    mRobotModel.link3 = new STLFileLoader(stlPath("link_3.STL"), 1);
    mRobotModel.link4 = new STLFileLoader(stlPath("link_4.STL"), 1);
    mRobotModel.link5 = new STLFileLoader(stlPath("link_5.STL"), 1);
}

void DDR6RobotWidget::configureModelParams() {
    //设置DH参数
    //d: 连杆偏距(mm) - 相邻连杆坐标系原点沿z轴的距离
    //JVars: 关节角度(度) - 相邻连杆坐标系x轴绕z轴的夹角
    //a: 连杆长度(mm) - 相邻连杆坐标系z轴沿x轴的距离
    //alpha: 扭转角(度) - 相邻连杆坐标系z轴绕x轴的夹角
    mRobotConfig.d = {0, 95.0, 0.00, 0.00, 0.00, 59.28, 0.00};
    mRobotConfig.JVars = {0, 0, 90, 0, -90, 0, 0};
    mRobotConfig.a = {0, 0, -9.8, 104, 88.47, 0, 0};
    mRobotConfig.alpha = {0, 0, 90, 0, 0, -90, 0};



//    mRobotConfig.d = {0, 127.00, -122.00, -101.00, -1.0, 0.00, 0.00};
//    mRobotConfig.JVars = {0, 0, 0, 0, 0, 0, 0};
//    mRobotConfig.a = {0, 0, 0, 0, 0, 0, 0};
//    mRobotConfig.alpha = {0, 0, 180.00, 0, 0, 0, 0};
    // 默认显示配置：只开启网格显示
    mGlobalConfig = {true, false, false, false, false, false, false, false, false};
}

void DDR6RobotWidget::drawGL() {
    // OpenGL主绘制函数，使用矩阵栈实现机器人运动学变换
    glPushMatrix();  // 保存当前矩阵状态，用于后续恢复

    if (mGlobalConfig.isDrawGrid) drawGrid();            // 绘制网格
    if (mGlobalConfig.isDrawWorldCoord) drawCoordinates(); // 绘制世界坐标系

    // 绘制基座（link0）
    setupColor(20, 126, 60);  // 设置绿色
    mRobotModel.link0->draw();

    // 第一关节（link1）变换和绘制
    if (mGlobalConfig.isDrawJoint1Coord) {
        drawSTLCoordinates(255, 0, 0);  // 显示关节1坐标系（红色）
    }
    setupColor(169, 169, 169);  // 设置灰色
    // 应用DH参数变换

    glTranslatef(mRobotConfig.a[1], 0.0, 0.0);         // a: x轴偏移
    glRotatef(mRobotConfig.alpha[1], 1.0, 0.0, 0.0);   // α: 绕x轴旋转
    glTranslatef(0.0, 0.0, mRobotConfig.d[1]);         // d: z轴偏移
    glRotatef(mRobotConfig.JVars[1], 0.0, 0.0, 1.0);   // θ: 绕z轴旋转
    mRobotModel.link1->draw();

    /*// 调整坐标系
    glTranslatef(0, -2.3, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glTranslatef(-9.8, 0.0, 0.0);    */              // z轴方向平移
    glTranslatef(0.0,-2.2, 0.0);                  // z轴方向平移

    // 二关节  修改2关节的Z轴
    if (mGlobalConfig.isDrawJoint2Coord) {
        drawSTLCoordinates(0, 255, 0);
    }
    setupColor(20, 126, 60);

    glTranslatef(mRobotConfig.a[2], 0.0, 0.0);                  // x轴方向平移
    glRotatef(mRobotConfig.alpha[2], 1.0, 0.0, 0.0);            // 绕x轴旋转
    glTranslatef(0.0, 0.0, mRobotConfig.d[2]);                  // z轴方向平移
    glRotatef(mRobotConfig.JVars[2], 0.0, 0.0, 1.0);       // 绕z轴旋转
    mRobotModel.link2->draw();

    // 调整坐标系
//    glTranslatef(300, 0.0, 0.0);

    // 三关节
    if (mGlobalConfig.isDrawJoint3Coord) {
        drawSTLCoordinates(0, 0, 255);
    }
    setupColor(169, 169, 169);
    glTranslatef(0.0,0.0, -3.5);                  // z轴方向平移

    glTranslatef(mRobotConfig.a[3], 0.0, 0.0);                  // x轴方向平移
    glRotatef(mRobotConfig.alpha[3], 1.0, 0.0, 0.0);            // 绕x轴旋转
    glTranslatef(0.0, 0.0, mRobotConfig.d[3]);                  // z轴方向平移
    glRotatef(mRobotConfig.JVars[3], 0.0, 0.0, 1.0);            // 绕z轴旋转
    mRobotModel.link3->draw();

//    // 调整坐标系
//    glTranslatef(260, 0.0, 0.0);
//    glRotatef(-90, 0.0, 0.0, 1.0);                              // 绕x轴旋转

    // 四关节
    if (mGlobalConfig.isDrawJoint4Coord) {
        drawSTLCoordinates(255, 255, 0);
    }
    setupColor(20, 126, 60);

    glTranslatef(mRobotConfig.a[4], 0.0, 0.0);                  // x轴方向平移
    glRotatef(mRobotConfig.alpha[4], 1.0, 0.0, 0.0);            // 绕x轴旋转
    glTranslatef(0.0, 0.0, mRobotConfig.d[4]);                  // z轴方向平移
    glRotatef(mRobotConfig.JVars[4], 0.0, 0.0, 1.0);            // 绕z轴旋转
    mRobotModel.link4->draw();

//    // 调整坐标系
//    glTranslatef(0.0, 0.0, 110.0);
//    glRotatef(-90, 1.0, 0.0, 0.0);                              // 绕x轴旋转

    // 五关节
    if (mGlobalConfig.isDrawJoint5Coord) {
        drawSTLCoordinates(0, 255, 255);
    }
    setupColor(169, 169, 169);
    glTranslatef(0.0, 0.0, -3.5);

    glTranslatef(mRobotConfig.a[5], 0.0, 0.0);                  // x轴方向平移
    glRotatef(mRobotConfig.alpha[5], 1.0, 0.0, 0.0);            // 绕x轴旋转
    glTranslatef(0.0, 0.0, mRobotConfig.d[5]);                  // z轴方向平移
    glRotatef(mRobotConfig.JVars[5], 0.0, 0.0, 1.0);            // 绕z轴旋转
    mRobotModel.link5->draw();

    // 调整坐标系
    glTranslatef(0.0, 0.0, 110.0);
    glRotatef(90, 1.0, 0.0, 0.0);                               // 绕x轴逆时针旋转90°

//    // 六关节
//    if (mGlobalConfig.isDrawJoint6Coord) {
//        drawSTLCoordinates(255, 0, 255);
//    }

//    setupColor(20, 126, 60);
//    glTranslatef(0.0, 0.0, mRobotConfig.d[6]);                  // z轴方向平移
//    glRotatef(mRobotConfig.JVars[6], 0.0, 0.0, 1.0);            // 绕z轴旋转
//    glTranslatef(mRobotConfig.a[6], 0.0, 0.0);                  // x轴方向平移
//    glRotatef(mRobotConfig.alpha[6], 1.0, 0.0, 0.0);            // 绕x轴旋转
//    mRobotModel.link6->draw();

    glPopMatrix();
}

//用来绘制OpenGL的窗口，只要有更新发生，这个函数就会被调用
void DDR6RobotWidget::paintGL() {
    // OpenGL渲染循环函数，在需要更新显示时调用
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 清除颜色和深度缓冲

    glPushMatrix();  // 保存当前矩阵状态
    
    // 应用视角变换
    glTranslated(0, 0, z_zoom);        // 调整观察距离
    glTranslated(xTran, yTran, 0);     // 平移视角
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);  // X轴旋转（Qt角度需要除16）
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);  // Y轴旋转
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);  // Z轴旋转
    glRotated(+90.0, 1.0, 0.0, 0.0);   // 调整初始方向为垂直向上

    glRotated(180.0, 1.0, 0.0, 0.0);  // 绕 x 轴旋转180度，实现 z 轴翻转

    
    drawGL();  // 执行实际的绘制操作
    
    glPopMatrix();  // 恢复矩阵状态
}
