# HiPOP

High Performance Optimal Path module


## Install Hipop

HiPOP is the C++ graph library used by [MnMS](https://github.com/EMob-Lab/MnMS.git).


### Install prebuilt distribution

This is the recommended (and most simple!) method to install HiPOP:
a prebuilt distribution adapted to your Python version and OS / system architecture
is downloaded from [PyPI](https://pypi.org/project/scisoftdev-tmp-HiPOP).

```shell
pip install scisoftdev-tmp-HiPOP # TODO change package name (and links in the whole page)
```

This installation method requires your OS / system architecture to be one of these:
- Windows on x86_64 architecture (i.e. Intel/AMD CPU)
- Linux on x86_64 architecture (i.e. Intel/AMD CPU)
- MacOS (≥ 14 Sonoma) on ARM64 architecture (i.e. Apple M* CPU)

The exact list of prebuilt distributions is available on
https://pypi.org/project/scisoftdev-tmp-HiPOP/#files.
If none of them matches your environment, please contact to the package maintainers,
or try the local build installation method.


### Install from local build

This method requires the following components to be available on your system:

- C++ compiler that supports C++17 and [OpenMP](https://www.openmp.org/)
- [CMake](https://cmake.org/)

Then, here is the local build install procedure:

1. Download the latest HiPOP source from https://github.com/EMob-Lab/HiPOP/archive/refs/heads/main.zip
or by cloning the git repository:

```shell
git clone https://github.com/EMob-Lab/HiPOP.git
```

2. From the root directory of the HiPOP sources:

```shell
pip install .

# Optionally, to run the tests:
pip install --group dev
pytest
```

Remarks:

1. The `--group` option requires pip ≥ 25.1. If you have an older version,
the install command above will fail with an error message such as `no such option: --group`.
In this case, try to upgrade pip beforehand with:
```shell
pip install --upgrade pip
```
Alternatively, install the test runner `pytest` manually (or skip the optional test running step).

2. TODO: editable installs

3. If you don't have the prerequisite C++/OpenMP/CMake components available on you system,
but you have [Conda](https://docs.conda.io/) available, a Conda environment is provided
with these components. Install and activate it with:
```shell
conda create -f conda/env.yaml
conda activate hipop-dev
```
Please note that the use of this Conda environment is optional. HiPOP can be used with regular
(non-Conda) set-ups.

4. If you are using MacOS and the Apple Clang C++ compiler, please note that OpenMP
may not be available by default. You may install it with:
```shell
brew install libomp
OpenMP_ROOT=$(brew --prefix libomp)
```
Alternatively, you may also use the Conda environment (cf. the previous remark).


### C++ only

Inside your conda environment go to the cpp folder, and install the code using cmake:

```shell
cd cpp
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=<PREFIX>\
         -DCMAKE_INSTALL_PREFIX=<PREFIX>\
         -DCMAKE_BUILD_TYPE=Release\
         -DBUILD_TESTS=ON
cmake --build . --target install --config Release
```
Where `<PREFIX>` is the path to your prefix.

If you used conda to install the dependencies, replace it by `$CONDA_PREFIX`.

If you used venv to install the dependencies, replace it by the path to your venv.

You can then lauch the tests in the `build` directory:

```shell
ctest --output-on-failure
```


### Python

To install C++ code use the script `install_cpp.py`:

```shell
python python/install_cpp.py
```

Then install the python lib:
```shell
python -m pip install python/
```
