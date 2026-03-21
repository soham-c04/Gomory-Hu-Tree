# Gomory-Hu Tree
**Muti-Terminal Network Flow** problem can be solved trivially by doing $\binom{n}{2}$ flow computations. **Gomory-Hu Tree** reduces that to $n-1$ flow computations. Thus, improving **Time Complexity** by factor of $\mathcal{O(n)}$.

A Gomory-Hu Tree (1961) is an undirected, weighted tree on the vertices of a given graph such
that each edge in the tree represents a minimum separating cut in the underlying graph
with respect to its incident vertices.<br>
**Property:**
1. Min-Cut between two vertices on the tree also corresponds to a Min-Cut between those vertices in the underlyng graph.
2. Min-Cut value between two vertices is equal cheapeast tree edge on the unique path
between the vertices (Corollary to 1).

**Dynamic Gomory-Hu Tree** (Tanja Hartmann and Dorothea Wagner, 2013) aims to <u>re-use</u> previously computed cuts to prevent complete re-construction of the whole tree when an edge-weights changes in the underlying graph. This, further improves the Execution Time for a dynamic graph.
<br><br>

## How to Run (Scripts)
- Use Ubuntu/Linux/WSL Terminal.
- **Argument 1** is compulsory and path is relative to "Implementation/" directory.
- Paths to [brute.cpp] and [gen.cpp] are relative to "Implementation/Test/" directory.
- Default for [number_of_iterations = 100] [brute = brute.cpp] [gen = gen.cpp].
- Keep same input and output format for <code.cpp> [brute.cpp] and use that for Testcase generator.
- [**Benchmarks**](https://github.com/soham-c04/Gomory-Hu-Tree/tree/main/benchmarks) - `./bench.sh <code.cpp> [number_of_iterations] [brute.cpp] [gen.cpp]`
    - Output is appended to "benchmark-\<code\>-\<brute\>-\<gen\>.csv".
    - If all iterations are done then Choose (y/n) for logarithmic plotting of that .csv file.
    - Use [Plotter.py](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/benchmarks/Plotter.py) to plot (takes relative path to .csv file as input).
- [**Stress Test**](https://github.com/soham-c04/Gomory-Hu-Tree/tree/main/Stress%20Test) - `./bash.sh <code.cpp> [brute.cpp] [gen.cpp]`
    - Any Testcase for which output of both codes is different is printed.
    - **Ctrl+C** to stop script.
<br><br>

## Performance
### Static Gomory-Hu Tree
<img src="benchmarks/benchmark-Static_GomoryHu_Tree-brute-gen_plot.png" width="50%">

### Dynamic Gomory-Hu Tree
<table align="center">
  <tr>
    <td align="center">
      <img src="benchmarks/benchmark-Dynamic_GomoryHu_Tree-reusing_Static_GomoryHu_Tree-gen_plot.png" width="450"><br>
      <b>Single Update</b>
    </td>
    <td align="center">
      <img src="benchmarks/benchmark-Dynamic_GomoryHu_Tree_MU-gen_MU-MU_plot.png" width="450"><br>
      <b>Multiple Updates</b>
    </td>
  </tr>
</table>
<br><br>

## Implementation
- [**My Report**](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Report/Report.pdf)
- [Static_GomoryHu_Tree.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Static_GomoryHu_Tree.cpp) Based on [Very Simple Methods for All Pairs Network Flow Analysis](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Research%20Papers/1990%20-%20gusfield.Gomory-Hu%20Tree.pdf) ([Website](https://epubs.siam.org/doi/10.1137/0219009)).
- [Equivalent_Flow_Tree_gusfield.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Equivalent_Flow_Tree_gusfield.cpp) Based on [Very Simple Methods for All Pairs Network Flow Analysis](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Research%20Papers/1990%20-%20gusfield.Gomory-Hu%20Tree.pdf) ([Website](https://epubs.siam.org/doi/10.1137/0219009)).
- [Dynamic_GomoryHu_Tree.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Dynamic_GomoryHu_Tree.cpp) Based on [Dynamic Gomory-Hu Tree Construction -- fast and simple](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Research%20Papers/2013%20-%20Dynamic%20Gomory-Hu%20Tree%20construction%20-%20fast%20and%20simple%20-%20B.pdf) ([Website](https://arxiv.org/abs/1310.0178)).
<br><br>

## Test Codes
- [Dinic.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Dinic.cpp) - Algorithm used to compute maxflow (treated as a blackbox).
- [brute.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Test/brute.cpp) - Runs maxflow algorithm for each $\binom{n}{2}$ pairs of vertices.
- [reusing_Static_GomoryHu_Tree.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Test/reusing_Static_GomoryHu_Tree.cpp) - Reconstructs Gomory-Hu Tree from scratch for each edge update. 
- [gen.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Test/gen.cpp) - Generates Testcases (Random Graphs) for Static and SingleUpdate Dynamic Gomory-Hu Tree.
- **Multiple Updates (MU):**
    - Testing Dynamic Gomory-Hu Tree against Multiple Updates per Graph.
    - [benchMU.sh](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/benchmarks/benchMU.sh) - `./benchMU.sh [code.cpp] [number_of_iterations] [gen.cpp]`
    - [Dynamic_GomoryHu_Tree_MU.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Test/Dynamic_GomoryHu_Tree_MU.cpp) - Prints the time requried for Each Update.
    - [gen_MU.cpp](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Test/gen_MU.cpp) - Generates Random Graph with Multiple Update Testcases.
    - [PlotterMU.py](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/benchmarks/PlotterMU.py) - Plots mean %age of time required for each subsequent update w.r.t Initial Tree construction time.<br>
- **Example:**<br><img src="benchmarks/example_benchMU.png" width="400">
<br><br>

## Application

### [<u>Image Segmentation</u>](https://github.com/soham-c04/Gomory-Hu-Tree/tree/main/Implementation/Image%20Segmentation)
**<u>How to Run:</u>**
- Go to UCRT64 terminal(or whichever compiler is being used) and install **opencv** using - `pacman -S mingw-w64-ucrt-x86_64-opencv`
- Add the following commands when calling the compiler - `-std=c++20 -O3 -Wl,--stack,1073741824`
- Add the following commands when calling the linker - `-lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc`
- Include the folder **include/opencv4** in *C++ includes*.

Image segmentation is done based on an Unsupervised-Flow-based-Hierarchical-Clustering-Algorithm as mentioned in [this paper](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Research%20Papers/1993%20-%20Image%20Segmentation%20via%20Clustering%20-%20A%2B.pdf).

The [given code](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Image%20Segmentation/FlowbasedHierarchicalClustering.cpp) is an implementation of the algorithm mentioned in the above paper.

In the [images](https://github.com/soham-c04/Gomory-Hu-Tree/tree/main/Implementation/Image%20Segmentation/images) folder are the segmented images produced after running the algorithm.

| [Image](https://github.com/soham-c04/Gomory-Hu-Tree/tree/main/Implementation/Image%20Segmentation/images) | Pixels | Execution Time (in sec) |
|:----------:|:----------:|:----------:|
| [basic3.png](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Image%20Segmentation/images/basic3.png)  | $100 \times 100 = 10^4$ | 0 |
| [login.png](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Image%20Segmentation/images/login.png)  | $170 \times 94 = 1.6 \times 10^4$ | 1 |
| [paint.png](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Image%20Segmentation/images/paint.png)  | $465 \times 287 = 1.3 \times 10^5$ | 32 |
| [paint1.png](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Image%20Segmentation/images/paint1.png)  | $617 \times 317 = 2 \times 10^5$ | 267 |
| [paint2.png](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Image%20Segmentation/images/paint2.png)  | $480 \times 331 = 1.6 \times 10^5$ | 175 |
| [sample2.png](https://github.com/soham-c04/Gomory-Hu-Tree/blob/main/Implementation/Image%20Segmentation/images/sample2.png)  | $400 \times 788 = 3.1 \times 10^5$ | 109 |
<br>

**NOTE:**  HyperParameters - *sigma*, *cMAX*, *FACTOR*, *WINDOW* affect both final clusters and Execution Time (Time required for Clustering), whereas *MIN_SIZE* (minimum no. of pixels that should be present in a cluster) affects only the final cluster.