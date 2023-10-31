# GUI gamma cipher
## Summary
This project provides a graphical interface for performing encryption
and decryption operations using the gamma cipher.
## Features
* Encryption and decryption
* Conversion to binary notation
* Selects the encryption mode:
  *  Normal encryption
  *  Ciphertext block chaining mode
* Generation of cryptographic strong gamma
## Requirements
* **Qt** 6.3
* **C++20**
* **CMake** 3.9
## Build and install
1. Configuration of the build system:
```console
cmake -S path-to-project-dir -B path-to-build-dir -DCMAKE_PREFIX_PATH=path-to-qt-cmake
```
   * Example:
```console
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=C:/Users/xxx/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/231.8109.222/bin/ninja/win/x64/ninja.exe -DCMAKE_PREFIX_PATH="C:/Program/Qt/6.5.2/mingw_64/lib/cmake" -DCMAKE_CXX_FLAGS="-Wall -Wextra -O3 -target x86_64-w64-mingw32"
```
2. Program build:
```console
cmake --build path-to-build-dir --target name-target
```
   * Example:
```console
cmake --build build --target CipherXor
```
3. Program install:
```console
cmake --install path-to-build-dir --prefix path-to-install-dir
```
   * Example:
```console
cmake --install build --prefix "G:\Projects\CipherXor"
```