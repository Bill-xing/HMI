# HMI 打包

[English](packaging.en.md)

桌面应用包包含两个运行时部分：

- Qt/C++ 应用和 Qt frameworks；
- Python 分割运行时，包括 `predict.py`、模型代码、Python 包，以及本地存在时的 `model_data/seam_unet.pth`。

STL 文件默认排除，因为本仓库不分发 LeArm 模型文件。

## macOS

在 macOS 上使用 Qt 5 和准备好的 Python 环境运行：

```bash
cd /path/to/HMI
cp scripts/hmi_local_env.example.sh scripts/hmi_local_env.sh
. scripts/hmi_local_env.sh
scripts/package_macos.sh
```

输出：

```text
dist/macos/HMI.app
dist/macos/HMI-macos-<arch>.zip
```

脚本会运行 `macdeployqt`，将 HMI 运行时文件复制到 `HMI.app/Contents/Resources/HMI`，将 Python 环境复制到 `HMI.app/Contents/Resources/python`，并创建 zip 压缩包。

如果安装了 `conda-pack`，脚本会使用它生成更可迁移的 Python 环境。没有 `conda-pack` 时，脚本会直接复制 `PYTHON_HOME`。

## Windows

在 Windows 上使用 Qt 5、匹配的编译工具链和准备好的 Python 环境运行：

```powershell
cd C:\path\to\HMI
$env:PYTHON_HOME = "C:\path\to\python-env"
$env:PYTHON_VERSION = "3.9"
$env:QT_BIN = "C:\Qt\5.15.2\msvc2019_64\bin"
powershell -ExecutionPolicy Bypass -File .\scripts\package_windows.ps1
```

输出：

```text
dist\windows\HMI\
dist\windows\HMI-windows.zip
```

Windows 包必须在 Windows 或 Windows CI runner 上构建。当前 macOS 机器无法在没有独立 Windows 交叉编译工具链的情况下生成原生 Windows `.exe`。

## 私有 STL 打包

仅在本地私有构建时，在运行打包脚本前设置：

```bash
export HMI_INCLUDE_LOCAL_STL_ASSETS=1
```

```powershell
$env:HMI_INCLUDE_LOCAL_STL_ASSETS = "1"
```

除非已经确认 STL 文件的二次分发授权，否则不要将该模式用于公开 GitHub Release。

## 备注

- 打包产物较大，因为可能包含 PyTorch 和模型 checkpoint。
- `model_data/seam_unet.pth` 被 Git 有意忽略；如需分发，请通过 Release 或外部存储发布。
- 脚本未处理 macOS Gatekeeper notarization 和 Windows 代码签名。
