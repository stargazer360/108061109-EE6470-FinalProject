# 108061109-EE6470-FinalProject
In the final project, a synthesible PE for finding upper tangent of two fixed size (=16 points) upper convex hull is implemented. Base on the PE, a software for finding the upper convex hull for given points on a multi-core/multi-PE RISCV-VP platform with DMA is implemented as well.
# HLS
` $ cd Stratus/stratus`

` $ make sim_all`

# Build and run convex hull searcher on riscv-vp
` $ cp -r riscv-vp/vp/src/platform/* $EE6470/riscv-vp/vp/src/platform`

` $ cp -r riscv-vp/sw/* $EE6470/riscv-vp/sw`

` $ cd $EE6470/riscv-vp/vp/build`

` $ cmake..`

` $ make install`

Build Software
` $ cd $EE6470/riscv-vp/sw`

` $ cd convex-hull`

` $ make`

` $ make sim`

