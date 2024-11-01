# ZCAT
Implementation of the scalable multi-objective test problems proposed in:

S. Zapotecas-Martínez, C.A. Coello Coello, H.E. Aguirre & K. Tanaka (2023). Challenging test problems for multi-and many-objective optimization. Swarm and Evolutionary Computation, Volume 81, 101350, ISSN 2210-6502. https://doi.org/10.1016/j.swevo.2023.101350

This source code presents the implementation of the ZCAT multi-objective test 
problems according to the description given in the paper:
"Challenging test problems for multi- and many-objective optimization".

The "main.c" file contains three examples of the use of this implementation.

Example 1 shows how to generate (in the allowed decision space) and evaluate 
a single random solution to the ZCAT1 problem.

Example 2 shows how to generate random optimal solutions and use the ZCAT 
functions to evaluate any solution defined in the allowed decision space.

Example 3 automatically generates a given number of optimal solutions for all
ZCAT problems.

NOTE: In this implementation, the generation of Pareto optimal solutions 
(for problems with disconnected Pareto fronts) is limited to 15 objectives. 
Contact me if you need Pareto optimal solutions for more than 15 objectives 
in these types of problems.

Slightly modified ZCAT suite of problems, so that the user can use one file for the flags, one file for the inputs and the output (objectives) goes to one file. The idea is to support professor Denis E. C. Vargas from CEFET-MG and any other researchers that need a middleware solution to using ZCAT in MATLAB. (I plan on doing an implementation for PlatEMO in the near future, so that this maneuver is not needed anymore.) 
