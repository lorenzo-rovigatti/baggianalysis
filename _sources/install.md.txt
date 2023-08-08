# Installation

## Requirements

* CMake >= 3.12
* make
* A c++14-compliant compiler (tested with GCC >= 5.4)
* If Python bindings are enabled, the `pip`, `setuptools` and `setuptools-scm` packages, as well as Python 3's header file are required (`python3-dev`)
* The pore size observable requires the [NLopt](https://nlopt.readthedocs.io/en/latest/) library (`libnlopt-dev`)
* The bond-order parameter observable requires the [GNU Scientific Library](https://www.gnu.org/software/gsl/) and [Boost's math](https://github.com/boostorg/math) libraries (`libgsl-dev` and `libboost-math-dev`)
* [Sphinx](https://www.sphinx-doc.org/en/master), [sphinx_rtd_theme](https://github.com/readthedocs/sphinx_rtd_theme) and [recommonmark](https://recommonmark.readthedocs.io/en/latest/) are required to generate the Python bindings' documentation. Those can all be installed by using `pip` (for instance with the command `pip3 install --user sphinx sphinx_rtd_theme recommonmark`)

## Install with `pip`

If all the requirements are met, the `baggianalysis` Python package can be compiled & installed by cloning the repo and using `pip` as follows:

```bash
$ git clone https://github.com/lorenzo-rovigatti/baggianalysis.git
$ PIP_COMMAND install ./baggianalysis
```

where `PIP_COMMAND` should be `pip`, `pip3`, `python -m pip` or `python3 -m pip`, depending on your local configuration. Use `-v` for verbose output.

## Manual install

The code can be compiled with the following commands:

	cd baggianalysis   # enter the baggianalysis folder
	mkdir build        # create a new build folder. It is good practice to compile out-of-source
	cd build
	cmake ..           # here you can specify additional options, see next section
	make -j4           # compile baggianalysis. The -jX make option makes it compile the code in parallel by using X threads.

At the end of the compilation a test executable used for development (`test_feature`) will be placed in the build/bin folder. If Python bindings are enabled, `make install` will install the package through `pip` to make the library accessible by the user.

### cmake options

Here is a list of options that can be passed to cmake during the pre-compilation stage:

* `-DPython=On|Off` enables|disables Python bindings (defaults to `On`)
* `-DDebug=On|Off` compiles with|without debug symbols and without|with optimisation flags (defaults to `Off`)
* `-DG=On|Off` compiles with|without debug symbols and optimisation flags (defaults to `Off`)
* `-DCOMPILER_EXTENSIONS=On|Off` enables|disables compiler (most likely GNU) C++ extensions (defaults to `On`). Disable it to compiler with older compilers (*e.g.* g++6).

If `cmake` cannot find some of the libraries required, add their paths to the `CMAKE_FIND_ROOT_PATH` set command which can be found close to the top of the `CMakeLists.txt` file.

### Common compilation issues

* Sometimes the compilation fails with the following error `fatal error: Python.h: No such file or directory`: this either means that the Python header files are not installed (`python3-dev` on Ubuntu) or that there is a mismatch between the version of the Python headers and shared library detected by CMake. Make sure that the two match before re-running CMake.

## Test suite

`baggianalysis` contains a very simple testing suite that checks whether the Python bindings and core functionalities work. The test suite is run by using the `make test_quick` command. 
