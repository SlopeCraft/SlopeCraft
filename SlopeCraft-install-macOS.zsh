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
brew install git
brew install llvm

if [[ $CPUTYPE == arm64 ]]; then
  echo "Configuring LLVM Clang for Apple Silicon Macs"
  alias clang=/opt/homebrew/opt/llvm/bin/clang
  alias clang++=/opt/homebrew/opt/llvm/bin/clang++
else
  echo "Configuring LLVM Clang for Intel Macs"
  alias clang=/usr/local/opt/llvm/bin/clang
  alias clang++=/usr/local/opt/llvm/bin/clang++
fi

brew install cmake ninja
brew install qt
brew install zlib libpng libzip xsimd

echo "Cloning git repo"
cd ~
git clone https://github.com/SlopeCraft/SlopeCraft.git && cd SlopeCraft

echo "Configuring CMake... This step might take a while."
cmake -S . -B ./build -G "Ninja" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=./build/install -DCMAKE_BUILD_TYPE=Release

echo "Building... This step might take a while."
cd build
cmake --build . --parallel

echo "Installing and Deploying"
cmake --install .
cd install
macdeployqt *.app

echo "Get your app files at $(pwd)!"

