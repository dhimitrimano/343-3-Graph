# Graph

Graph class with several graph algorithms including depth-first search, 
breadth-first search, dijkstra's shortest path, minimum spanning tree


## cpp files

- graph.h, graph.cpp: Graph class
- graphtest.cpp: Test functions
- main.cpp: A generic main file to call testAll() to run all tests

## Warnings
- 'graph.cpp:319:37: warning: unused parameter ‘startLabel’ [-Wunused-parameter]': This warning in mstKruskal exists because professor Pisan accidentally made the function signature the same as that of mstPrim. We were told not to change the function, so I did not.
- 'graph.cpp:319:37: error: parameter 'startLabel' is unused [misc-unused-parameters,-warnings-as-errors]': This warning is also a biproduct of that accident.

## Unexecuted Code
All code that was never executed is related to nonexistent files being passed in.

Credits:<br>
Shell scripts, text files, and starter code provided by Yusuf Pisan<br>
Graph class implementation by Dhimitri Mano
