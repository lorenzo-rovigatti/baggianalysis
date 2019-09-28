# baggianalysis

### Requirements

* CMake >= 2.8
* make
* A c++14-compliant compiler (tested with GCC 5.4)
* A few Boost modules:
    * Boost Log (libboost-log-dev) 
    * Boost Signals2 (libboost-signals-dev)
    * Boost Filesystem (libboost-filesystem-dev)
    * Boost Math (libboost-math-dev)
* If Python bindings are enabled, Python 3's header file are required (python3-dev)

### Compilation

Download the baggianalysis archive or clone it via `git`. If you do the latter, don't forget to use the `--recursive` option to download the associated submodules! 

The code can be compiled with the following commands:

	cd baggianalysis   # enter the baggianalysis folder
	mkdir build        # create a new build folder. It is good practice to compile out-of-source
	cd build
	cmake ..           # here you can specify additional options, see next section
	make -j4           # compile baggianalysis. The -jX make option makes it compile the code in parallel by using X threads.

At the end of the compilation the final executable (baggianalysis) will be placed in the build/bin folder. If Python bindings are enabled, `make install` will run the `setup.py install --user` command to make the library accessible by the user.

### cmake options

Here is a list of options that can be passed to cmake during the pre-compilation stage:

* `-DPython=On|Off` enables|disables Python bindings (defaults to `On`)
* `-DDebug=On|Off` compiles with|without debug symbols and without|with optimisation flags (defaults to `Off`)
* `-DG=On|Off` compiles with|without debug symbols and optimisation flags (defaults to `Off`)

### Test suite

baggianalysis contains a very simple testing suite that checks whether the Python bindings and core functionalities work. The test suite is run by using the `make test_quick` command. 

## Features

* Support many configuration types
* Filters: after being parsed, configurations can be modified by the so-called *filters*. Some available filters are:
	* filter particles according to a custom lambda function (`FilterByFunction`)
	* select particles of certain types only (`FilterByType`)
	* remove the centre-of-mass position and velocity (`SubtractCOM`)
	* reduce the configuration so that it contains only a single particle with position and velocity given by the centre-of-mass position and velocity (`FilterByReducingToCOM`)

## Notes

* By default, the core library is compiled dynamically. However, if Python bindings are enabled, the core library is compiled statically.
* The timestep associated to a configuration **must** be an integer number. If your preferred format stores it as a floating-precision number, your parser will have to find a way of converting that to an integer. This is *by design*, as the time of a configuration is used as a key in several maps around the code, and floating-point numbers are not good at that. Moreover, integer numbers can be stored without losing any precision, in contrast with floats.
* Normal trajectories need not load all the frames at once. Trajectories that do are called "full trajectories". Many observables, in general, do not require to have access to all frames at once, which means that frames can parsed (and hence loaded) one by one when needed (lazy loading). This allows to work on big trajectories without consuming up too much memory.
* Lists of 3D vectors are copied when accessed from the Python's side. This means that their c++ counterparts (which are `std::vector`s) are not modified when `append` or similar Python methods are used.
* The `System` and `Particle` classes support dynamic attributes (that is, one can add any attributes to instances of these classes from the Python side).
* Simple Python parsers can be used to either parse single `System`s or to initialise trajectories from file lists and folders only.

## Acknowledgements

* The [glm 0.9.9](https://glm.g-truc.net/0.9.9/index.html) library is explicitly included in the source tree, while [pybind 2.2.4](https://github.com/pybind/pybind11) (commit 9a19306) is automatically downloaded as a submodule
* Natural sorting of files is carried out with the [NaturalSort](https://github.com/scopeInfinity/NaturalSort) library
