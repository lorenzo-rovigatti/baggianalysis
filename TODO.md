## Quality-of-life-changes

This is a list of things that will have to be changed as soon as I have some time to spare

* Use getters and setters to incapsulate members

## Architectural changes

* As of now, we can handle cuboid boxes with PBC only. We need to add a class hierarchy to support additional box types.
* A particle cannot be bonded to another one more than once. This makes it impossible to handle self-interactions.

## Features to be added

* Angles, dihedrals etc. (impropers?) are not managed
* Add a mass member to particles
