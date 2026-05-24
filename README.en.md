# HMI

[中文](README.zh-CN.md)

HMI is an unofficial desktop control application for the Hiwonder LeArm robotic arm platform. It is mainly used for weld seam recognition, UNet image segmentation inference, and digital-twin joint control demos.

This project was developed by studying, adapting, and integrating two public projects: the Qt/OpenGL robot visualization and interaction work references [eagleqq/Robot3D](https://github.com/eagleqq/Robot3D), and the UNet training, inference, and mIoU evaluation workflow is based on [bubbliiiing/unet-pytorch](https://github.com/bubbliiiing/unet-pytorch). On top of that, this project adapts the LeArm model, integrates the Qt HMI UI, calls weld seam segmentation inference, supports slider-based joint simulation control, and documents local reproduction and desktop packaging.

This repository is not an official Hiwonder project and is not an official fork of the referenced projects. It is primarily intended as a portfolio, learning, research, and local demonstration project.

## Scope

- Code: Qt/C++ HMI, OpenGL robot visualization, and Python UNet training/inference code.
- Dataset: the weld seam segmentation dataset was self-collected by the project author and is not committed to Git.
- Weights: the weld seam segmentation weight was trained by the project author and is not committed to Git.
- STL: LeArm STL/structure model files are not redistributed by this repository. Users must provide model files they are authorized to use.
- Executable packages: public packages exclude STL files by default. Private local packages can explicitly enable STL copying.

## Licenses

This repository does not declare one unified license for all content. The boundaries are:

| Content | License / Status |
| --- | --- |
| Weld seam segmentation dataset | [CC BY 4.0](docs/licenses/dataset-cc-by-4.0.en.md) |
| Self-trained UNet weights | [Apache License 2.0](docs/licenses/model-weights-apache-2.0.en.md) |
| LeArm STL/structure model files | Not redistributed; no redistribution rights granted |
| Third-party references | See [THIRD_PARTY_NOTICES.en.md](THIRD_PARTY_NOTICES.en.md) |
| Repository code as a whole | No unified open-source license is declared |

## Features

- Qt desktop HMI
- LeArm STL model loading and OpenGL visualization
- Slider-based independent joint control and digital-twin demo
- Serial-port and robot control UI
- Weld seam image selection, segmentation, and result display
- UNet training, prediction, and mIoU evaluation scripts
- Local reproduction scripts and macOS/Windows packaging scripts

## Demo Videos

| Weld seam recognition | Independent joint slider control |
| --- | --- |
| [![Weld seam recognition demo](docs/media/seam-recognition.gif)](https://youtu.be/_f371G87iBs?si=kt9yl1vsSmSuFXPl) | [![Independent joint slider control demo](docs/media/joint-slider-control.gif)](https://youtu.be/tFxSIknCsfQ?si=BHSCSx6OC0wbLYiz) |

Original videos:

- [Weld seam recognition demo](https://youtu.be/_f371G87iBs?si=kt9yl1vsSmSuFXPl)
- [Independent joint slider control demo](https://youtu.be/tFxSIknCsfQ?si=BHSCSx6OC0wbLYiz)

## Repository Layout

```text
.
├── HMI.pro                 # Qt qmake project file
├── *.cpp, *.h, *.ui        # Qt/C++ HMI source and UI files
├── *.py                    # Segmentation training, prediction, and data scripts
├── nets/                   # UNet network code
├── utils/                  # Training, dataloader, and evaluation helpers
├── res/
│   ├── binary/             # STL placeholder notes; model files are not redistributed
│   └── image/              # Qt image resources
├── model_data/             # Model-weight placeholder directory; .pth files are ignored
├── docs/                   # Reproduction, packaging, dataset, and weight docs
└── scripts/                # Local environment example and packaging scripts
```

## Environment

- Qt 5.13 or a compatible version
- C++17
- Python environment
- PyTorch and dependencies from `requirements.txt`

Install Python dependencies:

```bash
pip install -r requirements.txt
```

`requirements.txt` records the older dependency set used for reproduction. If you migrate to newer Python or PyTorch versions, revalidate training, inference, and Qt integration.

## Local Configuration

Copy the example environment file and edit it for your machine:

```bash
cp scripts/hmi_local_env.example.sh scripts/hmi_local_env.sh
. scripts/hmi_local_env.sh
```

The real `scripts/hmi_local_env.sh` is ignored by Git to avoid committing machine-specific paths.

Before building Qt, set at least:

```bash
export PYTHON_HOME=/path/to/python/env
export PYTHON_VERSION=3.9
```

Windows example:

```bat
set PYTHON_HOME=C:/path/to/python-env
set PYTHON_VERSION=3.9
```

If the runtime cannot locate `predict.py` or locally provided `res/binary/*.STL`, set:

```bash
export HMI_PROJECT_ROOT=/absolute/path/to/HMI
```

The image segmentation button launches `python predict.py <image>`. To specify the Python executable:

```bash
export PYTHON_EXECUTABLE=/path/to/python
```

## Model Weight

Local weld seam recognition requires a `.pth` weight file:

```text
model_data/seam_unet.pth
```

`.pth` files are ignored by `.gitignore`. Distribute model files through GitHub Releases or external storage instead of writing large binaries into Git history.

## Dataset and Weight Release Packages

The weld seam segmentation dataset was self-collected by the project author, and the model weight was trained by the project author on that dataset. To keep large files out of Git history, the dataset and weights are not committed directly.

Suggested public asset names:

```text
hmi-weld-seam-voc-dataset.zip
hmi-weld-seam-unet-weights.zip
```

The dataset package keeps a Pascal VOC-compatible structure:

```text
hmi-weld-seam-voc-dataset/
└── VOC2007/
    ├── JPEGImages/
    ├── SegmentationClass/
    └── ImageSets/
        └── Segmentation/
```

See [docs/datasets/hmi-weld-seam-voc-dataset.en.md](docs/datasets/hmi-weld-seam-voc-dataset.en.md) for the dataset and [docs/models/hmi-weld-seam-unet-weights.en.md](docs/models/hmi-weld-seam-unet-weights.en.md) for the weight package. The dataset uses CC BY 4.0; the model weights use Apache License 2.0.

## LeArm STL Model

The digital-twin robot view requires LeArm STL files, but this repository does not provide those files.

To run locally, prepare authorized Hiwonder LeArm model files or files from another authorized source and place them in:

```text
res/binary/
```

The code expects these filenames by default:

```text
base_link.STL
link_1.STL
link_2.STL
link_3.STL
link_4.STL
link_5.STL
```

These STL files are ignored by `.gitignore` and are intended only for local/private use. Public GitHub Releases should not include them unless redistribution rights have been confirmed.

## Default Login

The demo version uses the default account:

```text
username: admin
password: admin
```

This login flow is only used for demo screen transitions and should not be treated as production-grade authentication.

## Local Reproduction

See [docs/reproduction/local-reproduction.en.md](docs/reproduction/local-reproduction.en.md).

Minimal flow:

```bash
cp scripts/hmi_local_env.example.sh scripts/hmi_local_env.sh
. scripts/hmi_local_env.sh
pip install -r requirements.txt
python predict.py /path/to/test-image.jpg
```

Qt build:

```bash
mkdir -p build-HMI-local
cd build-HMI-local
qmake CONFIG+=release ../HMI.pro
make -j"$(sysctl -n hw.ncpu)"
```

## Packaging

The project provides macOS and Windows packaging scripts for creating redistributable directories or archives containing Qt dependencies, project resources, model weights, and the Python runtime:

```bash
scripts/package_macos.sh
```

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package_windows.ps1
```

The packaging scripts exclude `res/binary/*.STL` by default. For private local demo packages only, and only when you are authorized to use the STL files, enable:

```bash
export HMI_INCLUDE_LOCAL_STL_ASSETS=1
```

```powershell
$env:HMI_INCLUDE_LOCAL_STL_ASSETS = "1"
```

See [docs/reproduction/packaging.en.md](docs/reproduction/packaging.en.md).

## References and Licensing Boundaries

Third-party sources and notices are documented in [THIRD_PARTY_NOTICES.en.md](THIRD_PARTY_NOTICES.en.md).

- [bubbliiiing/unet-pytorch](https://github.com/bubbliiiing/unet-pytorch): main reference for UNet training, inference, and mIoU evaluation. The upstream project uses the MIT License.
- [eagleqq/Robot3D](https://github.com/eagleqq/Robot3D): main reference for Qt/OpenGL robot visualization, STL loading, and joint control.
- LeArm STL/structure model files: this repository only documents the adaptation and expected filenames; it does not provide STL files.
- Model weight: `model_data/seam_unet.pth` is trained for this project scenario and is not committed to Git by default.

This repository does not declare one unified open-source license for all content. Before public release, code, STL resources, and third-party dependency license conditions should still be confirmed. The dataset and model-weight licenses are stated above.
