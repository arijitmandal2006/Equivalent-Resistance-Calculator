<p align="center">
  <img width="1024" height="301" alt="image" src="https://github.com/user-attachments/assets/15deb9ee-01a0-42a0-b883-1174ff51c621" />

</p>

<p align="center">
  <img src="https://img.shields.io/badge/Language-C-blue.svg">
  <img src="https://img.shields.io/badge/Platform-Linux%2FWindows-lightgrey.svg">
  <img src="https://img.shields.io/badge/Build-GCC%20%7C%20Clang-brightgreen.svg">
  <img src="https://img.shields.io/badge/Accuracy-100%25-success.svg">
  <img src="https://img.shields.io/badge/Algorithm-Gaussian%20Elimination-orange.svg">
  <img src="https://img.shields.io/badge/Circuit-Nodal%20Analysis-yellow.svg">
  <img src="https://img.shields.io/badge/Interactive-Yes-blueviolet.svg">
  <img src="https://img.shields.io/badge/License-MIT-red.svg">
</p>



**Equivalent Resistance Calculator (C Program)**
================================================

A fully interactive, menu-driven C application that computes the **equivalent resistance between any two nodes** of an **arbitrary resistor network**.\
Supports unlimited branches, node-based circuit creation, colored terminal interface, and a robust numerical solver based on **Nodal Analysis + Gaussian Elimination**.

* * * * *

📌 **Features**
============

### ⭐ **1\. Interactive Network Builder**

-   Add branches (u, v, R) between any two nodes

-   Automatically stores values in a dynamic BranchList

-   Prevents invalid input and ensures correctness

### ⭐ **2\. Professional Menu System**

-   Add branch

-   View all branches in a clean formatted table

-   Compute equivalent resistance

-   Remove one branch / remove a range / clear all

-   Restart network with new nodes

-   Exit safely

### ⭐ **3\. Accurate Electrical Computation**

Uses **nodal conductance matrix** + **Gaussian elimination with partial pivoting** to compute equivalent resistance for *ANY* circuit topology:

-   Series

-   Parallel

-   Mesh / bridge networks

-   Arbitrary node interconnections

### ⭐ **4\. Colorful, User-Friendly CLI**

-   ANSI color-coded messages

-   Clear prompts

-   Error-proof input handling

### ⭐ **5\. Handles Special Cases**

-   Automatically detects open circuits

-   If only 2 nodes exist → uses direct formula:\
    `R_eq = 1 / g_AB`

* * * * *

📘 **How It Works (Short Explanation)**
---------------------------------------

The program follows these steps:

1.  User enters the **number of nodes**.

2.  User builds a network by adding **branches (resistors)** between nodes.

3.  When "Compute R_eq" is selected:

    -   A **conductance matrix G** (n×n) is created using each resistor:\
        `g = 1 / R`

    -   A 1-volt test is applied between two chosen nodes A and B\
        (`V_A = 1`, `V_B = 0`)

    -   A reduced system of equations **M-V = b** is formed

    -   **Gaussian elimination** solves node voltages

    -   Total current leaving node A is computed

    -   Equivalent resistance is:\
        `R_eq = 1 / I`

This gives **100% correct** results even for complicated resistor networks.

* * * * *

 ### 🧮 **Mathematical Model**

The program constructs the **Admittance Matrix (G)** for the network. For a network with `N` nodes, the system solves the linear equation:

$$[G] \cdot [V] = [I]$$

Where:
* **[G]** is the system conductance matrix ($N \times N$).
* **[V]** is the vector of unknown node voltages.
* **[I]** is the current vector.

To find the equivalent resistance between Node A and Node B:
1.  We set **Node B as the reference (Ground, 0V)**.
2.  We inject a test voltage at **Node A (1V)**.
3.  We remove the rows/cols for fixed nodes and solve for remaining voltages using **Gaussian Elimination**.
4.  We calculate total current $I_{total}$ leaving Node A.
5.  Result: $R_{eq} = \frac{V_{test}}{I_{total}}$

* * * * *
 🔢**Flowchart**
---------
<img width="1448" height="2115" alt="image" src="https://github.com/user-attachments/assets/5210102c-856a-47ed-b38e-1ecfc25eecfd" />

****************

🚀 **Why This Tool Is Useful**
------------------------------

-   Helps engineering students understand nodal analysis

-   Solves resistor networks that are too complex for simple series/parallel rules

-   Demonstrates numerical analysis, data structures, and matrix solving in C

-   Useful for **simulation, circuit analysis, and academic projects**

-   Helps visualize circuit structure with a clean branch table

    
-   Dynamic memory management (`malloc`, `realloc`, `free`)

-   Structured programming

-   Algorithm implementation

-   Robust user interface handling
-------------------------------------
🧑‍💻 **How to Use**
--------------------

### **Step 1 --- Compile**

`gcc your file name .c -o Reqv`

*********************

### **Step 2 --- Run**

`./Reqv`
*********************
### **Step 3 --- Build the Network**

<img width="602" height="446" alt="image" src="https://github.com/user-attachments/assets/acdcde0f-3233-4e77-981f-e07615195c58" />

-   Enter number of nodes:
  -Use the menu to add branches
  - Enter initial node(u):
  - Enter final node(v):
  - resistance between final and initial node:
-   View table anytime to confirm connections
  
<img width="590" height="150" alt="image" src="https://github.com/user-attachments/assets/c4b21b25-998b-432d-92fa-f090ece962d3" />

********

### **Step 4 --- Compute Equivalent Resistance**

-   Choose option **3**

-   Enter terminal nodes A and B

-   Program prints the **exact equivalent resistance**
   <img width="727" height="416" alt="image" src="https://github.com/user-attachments/assets/47b807af-0dce-48b2-bdfa-aab1f65c6d79" />

*********************

### **Step 5 --- Manage Branches**

Use option **4** to:

-   Remove a single branch

-   Remove multiple branches

-   Clear all branches

<img width="623" height="565" alt="image" src="https://github.com/user-attachments/assets/753208f1-6851-40e4-a8f0-f149567f6ed9" />

***********************************************

 **🔮Future Improvements**

------------------------------

-   [ ]  Support for Ideal Voltage and Current Sources.
-   [ ]  Graphical Visualization of the circuit (using Python/Matplotlib integration).
-   [ ]  AC Analysis (Impedance with Capacitors/Inductors).
-   [ ]  File I/O: Save and Load circuit configurations.

****************************

**⚠️Troubleshooting**
--------------
-   "Singular Matrix / Error": This usually means the circuit is either:
    -   Disconnected: There is no path from Node A to Node B.
    -   Short Circuited: Node A and Node B are directly connected with 0 resistance.
-   "Open Circuit": The calculated current is effectively zero, meaning infinite resistance.
