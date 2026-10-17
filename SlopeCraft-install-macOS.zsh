#!/usr/bin/env zsh

if [[ $OSTYPE != darwin* ]]; then
  echo "This script can ONLY be used on macOS!"
  exit 1
fi

# Check if Homebrew is installed
which -s brew

if [[ $? != 0 ]] ; then
  # Install Homebrew
  echo "Installing Homebrew"
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
else
  echo "Updating Homebrew"
  brew update
fi

echo "Installing dependencies"
brew install install ninja libzip libpng zstd xz bzip2 boost cereal magic_enum xsimd eigen sevenzip libomp qt

echo "Configuring CMake... This step might take a while."
cmake -S . -B ./build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release -DSlopeCraft_GPU_API=None

echo "Building... This step might take a while."
cmake --build build --parallel

echo "Installing and Deploying"
cmake --install build
cd build/install

echo "Get your app files at $(pwd)!"

