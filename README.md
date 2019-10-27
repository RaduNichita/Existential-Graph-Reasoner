# README

##### // Copyright 2019 Olaru Cristian & Nichita Radu, 311CA, TEMA3 STRUCTURI DE DATE

 
included:  

1. Source file (aegraph.cpp) 
2. this README 


#### Task1 ####
        The function finds all paths where the double cut action is 
		possible. The condition is equivalent to finding all nodes that have
		exactly one subgraph and 0 atoms, which is a sufficient and necessary
		condition. The function checks every subgraph of one node in a
		recursive implementation and if the condition is satisfied, then the 
		all correct path for a specific node are returned.
#### Task2
        The function applies the double cut action of a specific graph. The
		last element of the where vector, given as parameter, is the node on 
		which double cut is applied. That means, there is only one subgraph
		related to this graph, whose atoms and subgraphs will be shifted 2 
		levels upward.
#### Task3
        The function finds all paths to possible nodes, that can be erased.
		All the subgraph of the first level can be erased, as they are on an
		even level. Then, for the rest of the nodes, it is checked that they
		are on the even level and they contain at least 2 elements(atoms and
		/or subgraphs), because graphs containing only one element can't be 
		deleted. For example, the [0,0,0] path can't be taken into
		consideration, as there is only only one element in the subgraph,
		whereas we can erase [1 0 0], as it contains 2 atoms.
			  
				   0 /\ 1
                  0 /  \ 0
                 0 /    \ 0 
                  /   0 /\ 1
                 a     b   c
                    