# Final Project Report by 108061109 Chou Hsing Hang
In the final project, a synthesible PE for finding upper tangent of two fixed size (=16 points) upper convex hull is implemented. Base on the PE, a software for finding the upper convex hull for given points on a multi-core/multi-PE RISCV-VP platform with DMA is implemented as well.
## Repositories
https://github.com/stargazer360/108061109-EE6470-FinalProject.git
## Implementation
For synthesible PE, the module takes two convex hulls as input and output the indices of the tangent points of two hulls. The module is comopse with two submodule for calculating the block containing the tangent points in parellel and one submodule for finding the indices in serial. Thus, module level parellel and pipeline is implemented.

Several ways of optimization like loop unrolling and pipelining are applied for comparison.

For multi-core/multi-PE RISCV-VP, there are 5 pairs of core and PE, one DMA, one memory, one client, one system call handler, and one PLIC.For simplicity, the latency of the PE from synthesible results is not applied to the TLM version of it, and therefore the interrupt routine for DMA is not applied as well.

For software, there are 64 points as input, and the goal is to find the convex hull of these points. We merge each two convex hulls in the inputs and store them to hald the total amount of convex hulls, then repeated this process until we have only one convex hull. 

Each program in each core take portion of the pair of convex hulls and pass them to PE for finding the tangent points and merge these two hulls. Since each software exclusive access to the portion of shared array, the only resource with race condition is the access to the DMA.

Therefore, the sofware use a lock for atomic access to the DMA and a a barrier to ensure each cores are in the same iteration.

## Experimental result
There are two layer of loops in each submodule, we comparing different optimization in synthesis as following.
All results beside primitive one are applied with DPA. 
|       | Primitive | DPA     | Unroll inner | Unroll Outer | Pipeline Outer |
|-------|-----------|---------|--------------|--------------|----------------|
| Cycle | 206       | 132     | 109          | 109          | 97             |
| Area  | 72812.1   | 75822.2 | 108522.9     | 106097.2     | 126667.6       |

As the table shows, using DPA helps reduce the total cycle of latency with slightly increasing in area. And overall, using Pipeline gives out the lowest latency with trade off of area. Comparing to the midterm project with one model for finding the direction of the tangent and make rest of the computation on CPU, which takes at least 99 cycles and at most 187 cycles in addition of CPU processing time for computing the same sized of input, the fixed size model gives out a better results with only 97 cycles and no CPU participant.

Following are comparison of RISCV_VP simulation results in different number of cores and PEs.
| # of Cores and PEs | Simulation Time(ns) | Minimum # of Instructions | Maximum # of Instruction | Simulation time comparing   to singal core |
|--------------------|---------------------|---------------------------|--------------------------|--------------------------------------------|
| 1                  | 6316180             | 184907                    | 315793                   | 1                                          |
| 2                  | 5195320             | 151682                    | 259750                   | 0.822541                                   |
| 3                  | 4980850             | 145304                    | 249026                   | 0.788586                                   |
| 4                  | 4688420             | 136748                    | 234405                   | 0.742287                                   |
| 5                  | 4691660             | 136826                    | 234567                   | 0.742800                                   |

As the table shows, the simulation time drops as we increase the number of cores and PEs. However, for 5 cores and PEs, the simulation time increases instead comparing to the previous result. This might coming from the fact that each core is idling too long for the sharing resources (i.e. DMA). It mgiht come from the partition issue for idling must of the core when process in higher depth.
## Conclusion
- Using fixed size model can help reduce the total latency and minimize the involvement of CPU. 
- The synchronization implementation and number of availible hardware (like DMA) bottleneck the scalability of the system, as each core need to idle for the sharing resources rather than running in fully parellel. 
 
