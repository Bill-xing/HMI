#!/usr/bin/env bash
set -euo pipefail

# Copy this file to scripts/hmi_local_env.sh and adjust the paths for your
# machine. The real hmi_local_env.sh is ignored by Git.

export HMI_PROJECT_ROOT="/absolute/path/to/HMI"
export PYTHON_EXECUTABLE="/absolute/path/to/python-env/bin/python"
export PYTHON_HOME="/absolute/path/to/python-env"
export PYTHON_VERSION="3.9"

if [[ -d "${PYTHON_HOME}/lib" ]]; then
  export DYLD_LIBRARY_PATH="${PYTHON_HOME}/lib:${DYLD_LIBRARY_PATH:-}"
fi
