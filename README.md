
# FlapCPEnum

My doctoral thesis implementation: Enumeration of single vertex crease pattern with unit angle of 360/n degrees
using parallel processing.
It works well on a supercomputer.

#Required environment

If you use Windows:
* Visual Studio 2022 (with Desktop development with C++ workloads, including Google Test tools)
* Microsoft MPI

If you use Linux:
* g++
* Open MPI
* Google Test (for testing)

#Run

For 22.5(=360/16) degree unit angle and using 4 processes, Run:

```
mpiexec -n 4 flapCPEnum.exe 16 cp_exLSLparallel
```
