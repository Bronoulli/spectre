\cond NEVER
Distributed under the MIT License.
See LICENSE.txt for details.
\endcond
# Modeling coating thermal noise with the elliptic solver

\tableofcontents

## Introduction
Welcome to the beginner's guide to modeling thermal noise! This tutorial
assumes that you are familiar with building SpECTRE, and visualizing data with
ParaView. In this guide, you will learn how to build and use the Elasticity
executables in SpECTRE. This will allow you to model the elastostatic equations
for a test mass undergoing constant pressure, and use their solutions to
recover the coating thermal noise caused by that test mass.

This guide assumes that the user is familiar with compiling SpECTRE and
building executables. For more info on compiling SpECTRE and submitting jobs
on clusters, see the
[Beginners guide](https://spectre-code.org/beginners_guide.html) and
[Running and Visualizing](https://spectre-code.org/tutorial_visualization.html)
tutorials.

<!--- (when to use href vs []?) -->
**Prerequisite reading**:
<a
href="https://arxiv.org/abs/2111.06893">
High-accuracy numerical models of Brownian thermal noise in thin mirror
coatings
</a>

**Executables to build**: SolveElasticity3D, cli, all-pybindings

## Editing the input file
**An example Input file is available here**:
`spectre/tests/InputFiles/Elasticity/SingleCoatingMirror.yaml`

### **Background**
Only used to compute numerical errors from an approximate analytic solution.
The parameters here should correspond to an uncoated, fused silica test mass
with the same dimensions of the coated test mass being modeled.

### **Material**
Defines the elastic stiffness properties of the simulated test mass. You can
choose to treat each layer as an isotropic homogeneous or cubic crystalline
material. Layers begin at the surface coating (Layer0 at z=0) and extend to the
substrate at the final layer.

### **DomainCreator**
Defines the size and shape of the test mass. Current work focuses on modeling
cylindrical test masses much smaller than those used in gravitational wave
detectors. The test input file consists of a single, 6.83-micron thick coating
of cubic crystalline AlGaAs on a 12.5 mm fused silica (isotropic homogeneous)
substrate. (Just like the domain used in the
[SpECTRE paper](https://arxiv.org/pdf/2111.06893)!)

Note that the inner radius of 0.2 mm is slightly larger than the beam width of
0.17677669534 mm. Keep this in mind if you plan to modify the size of your test
mass. This will be important if you hope to compare future results with live
detector readings.

The domain creator is also where you can dictate the initial resolution of your
simulation. You can do this through the InitialRefinement and InitialGridPoints
settings (commonly referred to as h-refinement and p-refinement respectively).
Currently, the optimal configuration (found by trial and error, with room for
improvement) is as follows:

```
InitialRefinement:
      Layer0: [{{ L }}, {{ L }}, {{ L }}]
      Layer1: [{{ L }}, {{ L }}, {{ L + 2 }}]
    InitialGridPoints:
      Layer0: [{{ P + 2 }}, {{ P + 1 }}, {{ P - 2 }}]
      Layer1: [{{ P + 2 }}, {{ P + 1 }}, {{ P + 1 }}]
```
\note Here, `Layer0` is the crystalline coating, while `Layer1` is the fused
silica substrate. We resolve the coating much less than the substrate in the
Z-direction. This is possible because the coatings are marginally thin compared
to the substrate.

A typical ‘low resolution’ domain with h-refinement 0 and p-refinement 5
(typically notated as h0p5) will look like this:

```
InitialRefinement:
      Layer0: [0, 0, 0]
      Layer1: [0, 0, 2]
    InitialGridPoints:
      Layer0: [7, 6, 3]
      Layer1: [7, 6, 6]
```

`PartitioningInZ`: For the case of a single coating layer, only a single
partition in the Z-direction is necessary (placed at the interface between the
coating and substrate). However, another partition in the Z-direction will be
necessary for each coating layer added to the domain.

`RadialDistribution`: We choose a linear coordinate map for the inner radius,
and extend a logarithmic coordinate map to the outer radius.

`BoundaryConditions`: The beam width dictates the size of the pressure
distribution applied to the surface of the test mass at z=0 in the elasticity
problem that we aim to model, while the base of the mirror at `UpperZ` is fixed
in place. Meanwhile, the mantle is free to move in response to the deformation
of the test mass surface.

### **Discretization**
<!--- (Explain PenaltyParameter?) -->

### Observers: untouched

### LinearSolver
`MaxIterations`: The simulation will end after reaching the max iteration count
regardless of convergence, so be sure to increase this number for more
complicated domains. 1000-2500 iterations should be more than enough for a
single coating at low resolutions.

`LinearSolver.Gmres.ConvergenceCriteria.AbsoluteResidual`: is very low by
default. In practice, this can be much larger. Try solving down to 1.e-6 for
faster runs, but be sure to run convergence tests to ensure a proper solve.

### Multigrid
`MaxLevels`: The multigrid is currently limited to 1 level for elasticity
problems that use adaptive mesh refinement, so change this option from Auto to
1.

`PostSmoothingAtBottom`: Post smoothing currently causes a deadlock error, so
change this from True to False.

### SchwarzSmoother
Nothing to see here.

### EventsAndTriggers
If using AMR to iterate over several resolutions, change this from
`HasConverged` to `Always`.

### Amr (Adaptive Mesh Refinement)
<!--- (Might include instruction for running a convergence test with one job
submission via AMR criteria) -->
Sample of input for AMR. This example will run 9 solves for the price of one
submission, and only one reduction file will be created.
```
Amr:
  Verbosity: Verbose
  Criteria:
    - IncreaseResolution:
    - TruncationError:
        VariablesToMonitor:
          - Displacement
        AbsoluteTarget: 1.e-10
        RelativeTarget: 1.e-7
  Policies:
    EnforceTwoToOneBalanceInNormalDirection: false
    Isotropy: Anisotropic
    Limits:
      NumGridPoints: Auto
      RefinementLevel: Auto
  Iterations: 8

PhaseChangeAndTriggers:
# Run AMR in every iteration, but not on the initial guess
  - Trigger:
      EveryNIterations:
        N: 1
        Offset: 1
    PhaseChanges:
      - VisitAndReturn(EvaluateAmrCriteria)
      - VisitAndReturn(AdjustDomain)
      - VisitAndReturn(CheckDomain)
```
### PhaseChangeAndTriggers

## Start the job
Once the input file contains the desired parameters, you are ready to submit
the job. For instructions on submitting a job and visualizing your results, see
the tutorial on
[Running and Visualizing](https://spectre-code.org/tutorial_visualization.html)
.

## Analyzing data

<!---  (If AMR/convergence test instructions are included, might want to add
instructions for reproducing paper plots... or just point to the paper repo?)
-->

Upon completion, you should have a reductions file (`MirrorReductions.h5`)
containing the potential energy density stored within each layer of the test
mass.

For a single coating, the calculation for thermal noise is relatively simple.
For multiple layers, we will need to use the 'plot over line' filter in
Paraview. This will plot the elastic variables olong the z-axis, which you can
save and visualize.

Examples for both of these cases can be found in the CTN paper's repository.
<!--- CTN calculation is fairly straightforward since potential energy is given
for each layer. The user just needs to find accurate values for each material's
loss angle, and calculate a sum of the noise from every layer -->
