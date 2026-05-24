#include "stlfileloader.h"
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <QDebug>
#include <QFile>
#include <fstream>
#include <iostream>
#include <QtOpenGL>

// STLFileLoader类：用于加载和渲染STL格式的3D模型文件
// STL（立体光刻）是一种广泛使用的3D模型文件格式

STLFileLoader::STLFileLoader(QString filename, float ratio) {
    mRatio = ratio;  // 设置缩放比例，用于调整模型大小
    model.clear();   // 清空之前的模型数据
    loadStl(filename);  // 加载指定的STL文件
}

STLFileLoader::~STLFileLoader() {
    // 析构函数，清理资源（如果有需要）
}

void STLFileLoader::loadStl(const QString &filename) {
    QFile file(filename);  // 创建文件对象以读取STL文件
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {  // 尝试以只读模式打开文件
        QByteArray arr;
        arr = file.read(5);  // 读取前5个字节以判断文件格式
        file.close();  // 关闭文件
        // 这里可以根据文件格式选择加载方式（ASCII或二进制）
        qDebug() << "正在加载模型:" << filename;  // 输出加载信息
        qDebug() << "文件大小:" << file.size();   // 输出文件大小
        loadBinaryStl(filename);  // 直接加载二进制格式STL文件
    } else {
        qDebug() << filename << u8"不存在";  // 输出错误信息，表示文件不存在
    }
}

// 加载ASCII格式STL文件
void STLFileLoader::loadTextStl(const QString &filename) {
    qDebug() << "load text file:" << filename;  // 输出加载的文件名
    model.clear(); // 清除之前的模型数据
    QList <QVector3D> triangle;  // 存储三角形的顶点
    STLTriangle tSTLTriangle;     // 创建三角形对象
    QFile file(filename);          // 创建文件对象

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {  // 打开文件
        while (!file.atEnd()) {  // 逐行读取文件
            QString line = file.readLine().trimmed();  // 读取并去除空白
            QStringList words = line.split(' ', QString::SkipEmptyParts);  // 分割单词

            if (words[0] == "facet") {  // 处理facet行
                triangle.clear();  // 清空三角形数据
                tSTLTriangle.reset();  // 重置三角形对象
                tSTLTriangle.setNormal(words[2].toFloat(), words[3].toFloat(), words[4].toFloat());  // 设置法向量
            } else if (words[0] == "vertex") {  // 处理顶点行
                triangle.append(QVector3D(words[1].toFloat(), words[2].toFloat(), words[3].toFloat()));  // 添加顶点
            } else if (words[0] == "endloop") {  // 处理endloop行
                if (triangle.length() == 3) {  // 确保有三个顶点
                    for (int i = 0; i < 3; ++i) {
                        tSTLTriangle.setVertex(i, triangle[i]);  // 设置三角形的顶点
                    }
                    model.append(tSTLTriangle);  // 将三角形添加到模型中
                }
            }
        }
        file.close();  // 关闭文件
    }
}

// 加载二进制格式STL文件，速度更快
void STLFileLoader::loadBinaryStl(const QString &filename) {
    qDebug() << "load Binary file:" << filename;  // 输出加载的文件名
    model.clear();  // 清除之前的模型数据
    QList <QVector3D> triangle;  // 存储三角形的顶点
    STLTriangle tSTLTriangle;     // 创建三角形对象

    QFile STL_file(filename);  // 创建文件对象

    int fileSize = STL_file.size();  // 获取文件大小
    char *buf = (char *) malloc(sizeof(char) * fileSize);  // 分配内存以存储文件内容

    bool isOk = STL_file.open(QIODevice::ReadOnly);  // 打开文件
    if (!isOk) return;  // 如果打开失败，返回

    QDataStream stream(&STL_file);  // 创建数据流
    stream.readRawData(buf, fileSize);  // 读取文件数据
    STL_file.close();  // 关闭文件

    const char *p = buf;  // 指向数据缓冲区
    char name[80];  // 文件名的前80个字节
    int triangle_num;  // 三角形数量
    float n1, n2, n3;  // 法向量
    float v1, v2, v3;  // 顶点坐标

    memcpy(name, p, 80);  // 记录文件名
    p += 80;  // 跳过文件名

    memcpy(&triangle_num, p, 4);  // 读取三角形数量
    p += 4;  // 跳过数量标识

    for (int i = 0; i < triangle_num; i++) {  // 读取每个三角形
        memcpy(&n1, p, 4);  // 读取法向量
        p += 4;
        memcpy(&n2, p, 4);
        p += 4;
        memcpy(&n3, p, 4);
        p += 4;

        triangle.clear();  // 清空三角形数据
        tSTLTriangle.reset();  // 重置三角形对象
        tSTLTriangle.setNormal(n1, n2, n3);  // 设置法向量

        for (int j = 0; j < 3; j++) {  // 读取顶点信息
            memcpy(&v1, p, 4);
            p += 4;
            memcpy(&v2, p, 4);
            p += 4;
            memcpy(&v3, p, 4);
            p += 4;
            triangle.append(QVector3D(v1, v2, v3));  // 添加顶点

            // 在读取顶点时直接更新模型尺寸
            mModelSize.minX = qMin(mModelSize.minX, v1 * mRatio);  // 更新最小X
            mModelSize.maxX = qMax(mModelSize.maxX, v1 * mRatio);  // 更新最大X
            mModelSize.minY = qMin(mModelSize.minY, v2 * mRatio);  // 更新最小Y
            mModelSize.maxY = qMax(mModelSize.maxY, v2 * mRatio);  // 更新最大Y
            mModelSize.minZ = qMin(mModelSize.minZ, v3 * mRatio);  // 更新最小Z
            mModelSize.maxZ = qMax(mModelSize.maxZ, v3 * mRatio);  // 更新最大Z
        }
        if (triangle.length() == 3) {  // 确保有三个顶点
            for (int i = 0; i < 3; ++i) {
                tSTLTriangle.setVertex(i, triangle[i]);  // 设置三角形的顶点
            }
            model.append(tSTLTriangle);  // 将三角形添加到模型中
        }
        p += 2;  // 跳过尾部标志（两个字节）
    }

    // 打印模型信息
    qDebug() << "模型" << filename << "尺寸:";
    qDebug() << "宽度:" << mModelSize.getWidth();  // 输出宽度
    qDebug() << "高度:" << mModelSize.getHeight();  // 输出高度
    qDebug() << "深度:" << mModelSize.getDepth();    // 输出深度
    qDebug() << "最大尺寸:" << mModelSize.getMaxSize();  // 输出最大尺寸

    free(buf);  // 释放分配的内存
}

// 绘制模型
void STLFileLoader::draw() {
    QList <STLTriangle> triangles = model;  // 获取模型中的三角形
    QVector3D normal;  // 法向量
    QVector3D vertex;  // 顶点坐标
    glBegin(GL_TRIANGLES); // 开始绘制三角形
    foreach(STLTriangle tri, triangles) {  // 遍历每个三角形
        normal = tri.getNormal();  // 获取法向量
        glNormal3f(mRatio * normal.x(), mRatio * normal.y(), mRatio * normal.z());  // 设置法向量
        for (int j = 0; j < 3; ++j) {
            vertex = tri.getVertex(j);  // 获取顶点
            glVertex3f(mRatio * vertex.x(), mRatio * vertex.y(), mRatio * vertex.z());  // 绘制顶点
        }
    }
    qDebug() << "绘制三角形数量:" << model.size();  // 输出绘制的三角形数量
    glEnd();  // 结束绘制
}

// STLTriangle类：表示单个三角形
STLTriangle::STLTriangle() {
    reset();  // 初始化三角形
}

// 设置三角形的顶点
void STLTriangle::setVertex(int index, QVector3D point3D) {
    if (!checkVertexIndex(index)) {
        return;  // 检查索引有效性
    }
    vector_vertex[index] = point3D;  // 设置顶点
}

// 获取三角形的顶点
QVector3D STLTriangle::getVertex(int index) {
    if (!checkVertexIndex(index)) {
        return QVector3D();  // 检查索引有效性
    }
    return vector_vertex[index];  // 返回顶点
}

// 设置法向量
void STLTriangle::setNormal(float nx, float ny, float nz) {
    vector_normal = QVector3D(nx, ny, nz);  // 设置法向量
}

// 获取法向量
QVector3D STLTriangle::getNormal() {
    return vector_normal;  // 返回法向量
}

// 重置三角形数据
void STLTriangle::reset() {
    vector_normal = QVector3D(0.f, 0.f, 0.f);  // 初始化法向量
    for (int i = 0; i < 3; ++i) {
        vector_vertex[i] = QVector3D(0.f, 0.f, 0.f);  // 初始化顶点
    }
}

// 检查顶点索引有效性
bool STLTriangle::checkVertexIndex(int index) {
    if (index < 0 || index > 2) {
        qDebug() << "CRITICAL: invalid index provided to STLTriangle::SetVertex()!";  // 输出错误信息
        return false;  // 返回无效
    }
    return true;  // 返回有效
}
