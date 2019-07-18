## Quality-of-life-changes

This is a list of things that will have to be changed as soon as I have some time to spare

* Make it possible to use `for(auto frame : trajectory)`
* Use getters and setters to incapsulate members
* Add a method that splits a string into tokens and trims each token

## Architectural changes

* As it is now, parsers' parse method must handle both reading from files containing whole trajectories as well as single configurations. This makes things confusing and more prone to bugs.
* How do we store the particle type? Now we use integers, but what about software that handles "real" atoms such as gromacs? 
* As of now, we can handle cuboid boxes with PBC only. We need to add a class hierarchy to support additional box types.
