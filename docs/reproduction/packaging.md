# Packaging HMI

The desktop package contains two runtime parts:

- the Qt/C++ application and Qt frameworks;
- the Python segmentation runtime, including `predict.py`, model code, Python packages, and `model_data/seam_unet.pth` when available locally.

STL files are excluded by default because the LeArm model files are not redistributed by this repository.

## macOS

Run on macOS with Qt 5 and the prepared Python environment:

```bash
cd /path/to/HMI
cp scripts/hmi_local_env.example.sh scripts/hmi_local_env.sh
. scripts/hmi_local_env.sh
scripts/package_macos.sh
```

Outputs:

```text
dist/macos/HMI.app
dist/macos/HMI-macos-<arch>.zip
```

The script runs `macdeployqt`, copies the HMI runtime files to `HMI.app/Contents/Resources/HMI`, copies the Python environment to `HMI.app/Contents/Resources/python`, and creates a zip archive.

If `conda-pack` is installed, the script uses it to make the Python environment relocatable. Without `conda-pack`, it falls back to copying `PYTHON_HOME` directly.

## Windows

Run on Windows with Qt 5, a matching compiler toolchain, and the prepared Python environment:

```powershell
cd C:\path\to\HMI
$env:PYTHON_HOME = "C:\path\to\python-env"
$env:PYTHON_VERSION = "3.9"
$env:QT_BIN = "C:\Qt\5.15.2\msvc2019_64\bin"
powershell -ExecutionPolicy Bypass -File .\scripts\package_windows.ps1
```

Outputs:

```text
dist\windows\HMI\
dist\windows\HMI-windows.zip
```

The Windows package must be built on Windows or on a Windows CI runner. This repository cannot produce a native Windows `.exe` from macOS without a separate Windows cross-build toolchain.

## Private STL Packaging

For a private local build only, set this variable before running the package script:

```bash
export HMI_INCLUDE_LOCAL_STL_ASSETS=1
```

```powershell
$env:HMI_INCLUDE_LOCAL_STL_ASSETS = "1"
```

Do not use this mode for public GitHub Releases unless you have confirmed redistribution rights for the STL files.

## Notes

- Packaged artifacts are large because PyTorch and the model checkpoint may be included.
- `model_data/seam_unet.pth` is intentionally ignored by Git; publish it through Releases or external storage if you choose to distribute it.
- macOS Gatekeeper notarization and Windows code signing are not handled by these scripts.
