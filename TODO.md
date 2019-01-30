## Quality-of-life-changes

This is a list of things that will have to be changed as soon as I have some time to spare

* Use getters and setters to incapsulate members
* Add a method that split a string into tokens and trim each token

## Architectural changes

* As it is now, parsers' parse method must handle both reading from files containing whole trajectories as well as single configurations. This makes things confusing and more prone to bugs.
