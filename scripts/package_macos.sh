#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd -P)"

if [[ -z "${PYTHON_HOME:-}" && -f "${ROOT_DIR}/scripts/hmi_local_env.sh" ]]; then
  # shellcheck disable=SC1091
  . "${ROOT_DIR}/scripts/hmi_local_env.sh"
fi

PYTHON_HOME="${PYTHON_HOME:-}"
if [[ -z "${PYTHON_HOME}" || ! -x "${PYTHON_HOME}/bin/python" ]]; then
  echo "PYTHON_HOME must point to the Python environment used by HMI." >&2
  exit 1
fi

PYTHON_VERSION="${PYTHON_VERSION:-$("${PYTHON_HOME}/bin/python" - <<'PY'
import sys
print(f"{sys.version_info.major}.{sys.version_info.minor}")
PY
)}"

QMAKE_PATH="${QMAKE_PATH:-}"
if [[ -z "${QMAKE_PATH}" ]]; then
  if command -v qmake >/dev/null 2>&1; then
    QMAKE_PATH="$(command -v qmake)"
  elif [[ -x /opt/homebrew/opt/qt@5/bin/qmake ]]; then
    QMAKE_PATH="/opt/homebrew/opt/qt@5/bin/qmake"
  else
    echo "qmake was not found. Set QMAKE_PATH=/path/to/qmake." >&2
    exit 1
  fi
fi

QT_BIN_DIR="$(cd "$(dirname "${QMAKE_PATH}")" && pwd -P)"
MACDEPLOYQT="${MACDEPLOYQT:-${QT_BIN_DIR}/macdeployqt}"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build-HMI-package-macos}"
DIST_DIR="${DIST_DIR:-${ROOT_DIR}/dist/macos}"
APP_SOURCE="${BUILD_DIR}/HMI.app"
APP_DIR="${DIST_DIR}/HMI.app"
APP_BIN="${APP_DIR}/Contents/MacOS/HMI"
RUNTIME_DIR="${APP_DIR}/Contents/Resources/HMI"
PYTHON_RUNTIME="${APP_DIR}/Contents/Resources/python"
ARCH_NAME="$(uname -m)"
ZIP_PATH="${DIST_DIR}/HMI-macos-${ARCH_NAME}.zip"
CONDA_PACK_BIN="$(command -v conda-pack || true)"
if [[ -z "${CONDA_PACK_BIN}" && -x "${PYTHON_HOME}/bin/conda-pack" ]]; then
  CONDA_PACK_BIN="${PYTHON_HOME}/bin/conda-pack"
fi

mkdir -p "${BUILD_DIR}" "${DIST_DIR}"

(
  cd "${BUILD_DIR}"
  PYTHON_HOME="${PYTHON_HOME}" PYTHON_VERSION="${PYTHON_VERSION}" "${QMAKE_PATH}" CONFIG+=release "${ROOT_DIR}/HMI.pro"
  make -j"$(sysctl -n hw.ncpu)"
)

rm -rf "${APP_DIR}" "${ZIP_PATH}"
ditto "${APP_SOURCE}" "${APP_DIR}"

if [[ -x "${MACDEPLOYQT}" ]]; then
  "${MACDEPLOYQT}" "${APP_DIR}" -verbose=1
else
  echo "warning: macdeployqt not found at ${MACDEPLOYQT}; Qt frameworks were not bundled." >&2
fi

rm -rf "${RUNTIME_DIR}"
mkdir -p "${RUNTIME_DIR}"

copy_file() {
  local source="$1"
  install -m 0644 "${ROOT_DIR}/${source}" "${RUNTIME_DIR}/${source}"
}

copy_dir() {
  local source="$1"
  mkdir -p "$(dirname "${RUNTIME_DIR}/${source}")"
  rsync -a --delete --exclude='__pycache__' --exclude='*.pyc' "${ROOT_DIR}/${source}/" "${RUNTIME_DIR}/${source}/"
}

copy_res_dir() {
  mkdir -p "${RUNTIME_DIR}/res"
  local exclude_args=(--exclude='__pycache__' --exclude='*.pyc')
  if [[ "${HMI_INCLUDE_LOCAL_STL_ASSETS:-0}" != "1" ]]; then
    exclude_args+=(--exclude='binary/*.STL' --exclude='binary/*.stl')
  fi
  rsync -a --delete "${exclude_args[@]}" "${ROOT_DIR}/res/" "${RUNTIME_DIR}/res/"
}

copy_file "predict.py"
copy_file "unet.py"
copy_file "segmentation.py"
copy_file "requirements.txt"
copy_dir "nets"
copy_dir "utils"
copy_res_dir

if [[ "${HMI_INCLUDE_LOCAL_STL_ASSETS:-0}" != "1" ]]; then
  echo "note: local STL assets were excluded from this public package."
  echo "      Set HMI_INCLUDE_LOCAL_STL_ASSETS=1 only for private local packages."
fi

mkdir -p "${RUNTIME_DIR}/model_data"
install -m 0644 "${ROOT_DIR}/model_data/README.md" "${RUNTIME_DIR}/model_data/README.md"
if [[ -f "${ROOT_DIR}/model_data/seam_unet.pth" ]]; then
  install -m 0644 "${ROOT_DIR}/model_data/seam_unet.pth" "${RUNTIME_DIR}/model_data/seam_unet.pth"
else
  echo "warning: model_data/seam_unet.pth was not found; inference will not work in the package." >&2
fi

rm -rf "${PYTHON_RUNTIME}"
if [[ -n "${CONDA_PACK_BIN}" ]]; then
  TMP_DIR="$(mktemp -d)"
  trap 'rm -rf "${TMP_DIR}"' EXIT
  "${CONDA_PACK_BIN}" -p "${PYTHON_HOME}" -o "${TMP_DIR}/python.tar.gz" --force --ignore-missing-files
  mkdir -p "${PYTHON_RUNTIME}"
  tar -xzf "${TMP_DIR}/python.tar.gz" -C "${PYTHON_RUNTIME}"
  if [[ -x "${PYTHON_RUNTIME}/bin/conda-unpack" ]]; then
    "${PYTHON_RUNTIME}/bin/conda-unpack" || true
  fi
else
  echo "warning: conda-pack not found; copying PYTHON_HOME directly. Install conda-pack for a more relocatable package." >&2
  rsync -a --copy-links \
    --exclude='__pycache__' \
    --exclude='*.pyc' \
    --exclude='pkgs' \
    "${PYTHON_HOME}/" "${PYTHON_RUNTIME}/"
fi

if command -v install_name_tool >/dev/null 2>&1; then
  install_name_tool -add_rpath "@executable_path/../Resources/python/lib" "${APP_BIN}" 2>/dev/null || true
fi

if command -v codesign >/dev/null 2>&1; then
  codesign --force --deep --sign - "${APP_DIR}" >/dev/null 2>&1 || true
fi

(
  cd "${DIST_DIR}"
  ditto -c -k --sequesterRsrc --keepParent "HMI.app" "$(basename "${ZIP_PATH}")"
)

echo "Created:"
echo "  ${APP_DIR}"
echo "  ${ZIP_PATH}"
