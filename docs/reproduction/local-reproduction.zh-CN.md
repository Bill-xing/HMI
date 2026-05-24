# 本地复现

[English](local-reproduction.en.md)

本文档说明如何在本地从源码运行 HMI 项目。

## 前置条件

- Qt 5 和 `qmake`
- 当前 Qt kit 支持的 C++17 编译器
- 与 `requirements.txt` 依赖兼容的 Python 环境
- 训练好的权重文件 `model_data/seam_unet.pth`
- 如需 3D 机械臂视图，需要将有授权的 LeArm STL 文件放入 `res/binary/`
- 用于 `--self-test` 的本地测试图片

## 环境配置

从示例文件创建本地环境文件：

```bash
cp scripts/hmi_local_env.example.sh scripts/hmi_local_env.sh
```

按本机环境编辑 `scripts/hmi_local_env.sh`：

```bash
export HMI_PROJECT_ROOT="/absolute/path/to/HMI"
export PYTHON_EXECUTABLE="/absolute/path/to/python-env/bin/python"
export PYTHON_HOME="/absolute/path/to/python-env"
export PYTHON_VERSION="3.9"
```

加载环境：

```bash
. scripts/hmi_local_env.sh
```

真实的 `scripts/hmi_local_env.sh` 被 Git 忽略，因为它包含本机路径。

## Python 推理

安装依赖并运行单张图片预测：

```bash
pip install -r requirements.txt
python predict.py /path/to/test-image.jpg
```

输出图像会写入运行时输出路径，文件名为 `segmented_image.png`。

## Qt 构建

使用 qmake 构建：

```bash
mkdir -p build-HMI-local
cd build-HMI-local
qmake CONFIG+=release ../HMI.pro
make -j"$(sysctl -n hw.ncpu)"
```

Windows 下请使用匹配的 Qt kit 和编译工具链，或运行等价的 `qmake` 和构建命令。

## Headless 自测

必须显式设置测试图片。应用不提供硬编码 fallback 路径：

```bash
HMI_TEST_IMAGE="/path/to/test-image.jpg" \
QT_QPA_PLATFORM=offscreen \
./build-HMI-local/HMI.app/Contents/MacOS/HMI --self-test
```

自测会检查默认演示登录流程、应用初始化、焊缝分割调用和输出图像生成。离屏模式下 OpenGL 上下文创建可能受限；需要检查 3D 可视化时请使用正常 GUI 启动。

## GUI 启动

从构建后的二进制文件运行应用，或在 macOS 上打开 `.app` bundle。如果运行时无法自动定位项目文件，设置：

```bash
export HMI_PROJECT_ROOT="/absolute/path/to/HMI"
export PYTHON_EXECUTABLE="/absolute/path/to/python-env/bin/python"
```
