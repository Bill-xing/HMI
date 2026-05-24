# Local Reproduction

This guide describes how to run the HMI project from source on a local machine.

## Prerequisites

- Qt 5 with `qmake`
- A C++17 compiler supported by the selected Qt kit
- A Python environment compatible with the dependencies in `requirements.txt`
- The trained weight file `model_data/seam_unet.pth`
- Authorized LeArm STL files under `res/binary/` if the 3D robot view is required
- A local test image for `--self-test`

## Environment

Create a local environment file from the example:

```bash
cp scripts/hmi_local_env.example.sh scripts/hmi_local_env.sh
```

Edit `scripts/hmi_local_env.sh` for your machine:

```bash
export HMI_PROJECT_ROOT="/absolute/path/to/HMI"
export PYTHON_EXECUTABLE="/absolute/path/to/python-env/bin/python"
export PYTHON_HOME="/absolute/path/to/python-env"
export PYTHON_VERSION="3.9"
```

Then load it:

```bash
. scripts/hmi_local_env.sh
```

The real `scripts/hmi_local_env.sh` is ignored by Git because it contains machine-specific paths.

## Python Inference

Install dependencies and run a single-image prediction:

```bash
pip install -r requirements.txt
python predict.py /path/to/test-image.jpg
```

The output image is written to the runtime output path as `segmented_image.png`.

## Qt Build

Build with qmake:

```bash
mkdir -p build-HMI-local
cd build-HMI-local
qmake CONFIG+=release ../HMI.pro
make -j"$(sysctl -n hw.ncpu)"
```

On Windows, use the matching Qt kit and compiler toolchain from Qt Creator or run the equivalent `qmake` and build tool commands.

## Headless Self-Test

Set a test image explicitly. The app does not provide a hard-coded fallback path:

```bash
HMI_TEST_IMAGE="/path/to/test-image.jpg" \
QT_QPA_PLATFORM=offscreen \
./build-HMI-local/HMI.app/Contents/MacOS/HMI --self-test
```

The self-test checks the default demo login flow, app initialization, seam segmentation invocation, and output image generation. OpenGL context creation may be limited in offscreen mode; use a normal GUI launch to inspect the 3D visualization.

## GUI Launch

Run the app from the built binary or open the `.app` bundle on macOS. If the runtime cannot locate project files automatically, set:

```bash
export HMI_PROJECT_ROOT="/absolute/path/to/HMI"
export PYTHON_EXECUTABLE="/absolute/path/to/python-env/bin/python"
```
