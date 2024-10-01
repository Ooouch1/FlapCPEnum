
# FlapCPEnum

[My doctoral thesis](https://dspace.jaist.ac.jp/dspace/bitstream/10119/16649/2/paper.pdf) implementation:
Enumeration of single vertex crease pattern with unit angle of 360/n degrees using parallel processing.
It works well on a supercomputer.

The subject is the same as my paper "Efficient enumeration of flat-foldable single vertex crease patterns",
 Koji Ouchi and Ryuhei Uehara, IEICE Transactions on Information and Systems, E102-D(3):416–422, 2019. 
 However, the algorithm is updated so that it performs better with parallel processing.

## Required environment

If you use Windows:
* Visual Studio 2022 (with Desktop development with C++ workloads, including Google Test tools)
* Microsoft MPI

If you use Linux:
* g++
* Open MPI
* Google Test (for testing)

## Run

For 22.5(=360/16) degree unit angle and using 4 processes, Run:

```
mpiexec -n 4 FlapCPEnum.exe 16 cp_exLSLparallel
```
