# baggianalysis

### Requirements

* CMake >= 2.8
* make
* A c++14-compliant compiler (tested with GCC 5.4)
* A few Boost modules:
    * Boost Log (libboost-log-dev) 
    * Boost Signals2 (libboost-signals-dev)
    * Boost Filesystem (libboost-filesystem-dev)

### Compilation

Download the baggianalysis archive or clone it via `git`. If you do the latter, don't forget to use the `--recursive` option to download the associated submodules! 

The code can be compiled with the following commands:

	cd baggianalysis	# enter the ashell folder
	mkdir build		# create a new build folder. It is good practice to compile out-of-source
	cd build
	cmake ..			# here you can specify additional options, see next section
	make -j4			# compile baggianalysis. The -jX make option makes it compile the code in parallel by using X threads.

At the end of the compilation the final executable (baggianalysis) will be placed in the build/bin folder.

### cmake options

Here is a list of options that can be passed to cmake during the pre-compilation stage:

* `-DPython=On`			Enables Python bindings
* `-DDebug=ON`				Compiles with debug symbols and without optimisation flags
* `-DG=ON`					Compiles with debug symbols + optimisation flags

## Features

* Support many configuration types
* Filters: after being parsed, configurations can be modified by the so-called *filters*. Some available filters are:
	* select particles of certain types only (FilterByType)
	* remove the centre-of-mass position and velocity (SubtractCOM)
	* reduce the configuration so that it contains only a single particle with position and velocity given by the centre-of-mass position and velocity (FilterByReducingToCOM)

## Notes

* The timestep associated to a configuration **must** be an integer number. If your preferred format stores it as a floating-precision number, your parser will have to find a way of converting that to an integer. This is *by design*, as the time of a configuration is used as a key in several maps around the code, and floating-point numbers are not good at that. Moreover, integer numbers can be stored without losing any precision, in contrast with floats.

## Acknowledgements

* Eigen 3.3.4 is included in the source tree
* Natural sorting of files is carried out with the [NaturalSort](https://github.com/scopeInfinity/NaturalSort) library
