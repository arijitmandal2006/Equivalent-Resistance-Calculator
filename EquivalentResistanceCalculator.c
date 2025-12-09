/*All header file */
#include <stdio.h> //standard header file
#include <stdlib.h>//for Dynamic Memory Aloocation operation like malloc(),realloc,free etc..
#include <string.h>
#include <math.h>
#include <ctype.h> // handle (y/n) input

/* ANSI color helpers */
#define COL_RESET  "\x1b[0m"
#define COL_RED    "\x1b[31m"
#define COL_GREEN  "\x1b[32m"
#define COL_YELLOW "\x1b[33m"
#define COL_BLUE   "\x1b[34m"
#define COL_CYAN   "\x1b[36m"
#define COL_MAG    "\x1b[35m"
#define COL_BOLD   "\x1b[1m"

#define EPS 1e-12 /* Gaussian elimination
detect near-zero pivots and avoid division by (near) zero.*/


/* Simple dynamic array for branches */
typedef struct {
    int u, v;     /* zero-based node indices */
    double r;     /* resistance in ohms */
} Branch;


/* hold the list of branches dynamically.. 
help to add/remove branches 
single word it is dynamic array*/
typedef struct {
    Branch *arr;
    int size; //current number of branches
    int cap; //allocated capacity.
} BranchList;


/*Allocates initial array*/
void bl_init(BranchList *b) {
    b->size = 0; b->cap = 8;
    b->arr = malloc(b->cap * sizeof(Branch));
    if (!b->arr) { perror("malloc"); exit(1); }
}


/* help to add branch using realooc()... */
void bl_push(BranchList *b, int u, int v, double r) {
    if (b->size >= b->cap) {
        b->cap *= 2;
        b->arr = realloc(b->arr, b->cap * sizeof(Branch));
        if (!b->arr) { perror("realloc"); exit(1); }
    }
    b->arr[b->size].u = u;
    b->arr[b->size].v = v;
    b->arr[b->size].r = r;
    b->size++;
}


/*simple and sufficient to remove a branch from a index ,shifting next elemeent toward left*/
void bl_remove_at(BranchList *b, int idx) {
    if (idx < 0 || idx >= b->size) return;
    for (int i = idx; i + 1 < b->size; ++i) b->arr[i] = b->arr[i+1];
    b->size--;
}


/*efficiently clear branch without deallocation*/
void bl_clear(BranchList *b) {
    b->size = 0;
}


/*Frees allocated memory exit or restart time to  prevent memory leak...*/
void bl_free(BranchList *b) { free(b->arr); b->arr = NULL; b->size = b->cap = 0; }


/*alloccate 2D 'double' matrix  with contiguous dynamically memory allocation to store data and return pointer-pointer to get index*/
double **alloc_matrix(int n, int m) {
    if (n <= 0 || m <= 0) return NULL;
    double **a = malloc(n * sizeof(double*));
    if (!a) { perror("malloc"); exit(1); } // malloc()for row pointers
    a[0] = calloc(n * m, sizeof(double));//calloc() for data (no garbage value)
    if (!a[0]) { perror("calloc"); exit(1); }
    for (int i = 1; i < n; ++i) a[i] = a[0] + i * m;
    return a;
}

/*Frees the memory allocated by alloc matrix...Prevents memory leak*/
void free_matrix(double **a) {
    if (!a) return;
    free(a[0]);
    free(a);
}


/*  Solves the linear system A·x = b using Gaussian elimination with partial pivoting.
 Input:  n = matrix size, A = coefficient matrix, b = RHS vector...
 Output: b[] will contain the solution x[], and A is modified during elimination.  
 Returns 0 on success, -1 if the matrix is singular (cannot solve). */
int gauss_solve(int n, double **A, double *b) {
    for (int i = 0; i < n; ++i) {
         // Select pivot row: start ,current row i is pivot
        int pivot = i;
        double maxv = fabs(A[i][i]);

        //find that row which has largest absolute pivot value
        for (int r = i+1; r < n; ++r) {
            double v = fabs(A[r][i]);
            if (v > maxv) { maxv = v; pivot = r; }
        }
        if (maxv < EPS) return -1; /* pivot is very small or singular matrix ... cannot solve */
       
        /*if current row is not best pivot then swap rows in both A and b */
        if (pivot != i) {
            double *tmp = A[i]; A[i] = A[pivot]; A[pivot] = tmp;
            double tmpb = b[i]; b[i] = b[pivot]; b[pivot] = tmpb;
        }

      // Eliminate entries below pivot (make column i below row i all zeros)

        for (int r = i+1; r < n; ++r) {
            double factor = A[r][i] / A[i][i];
            b[r] -= factor * b[i];
            for (int c = i; c < n; ++c) A[r][c] -= factor * A[i][c];   // update row
        }
    }
      // Solve from last to first row
    for (int i = n-1; i >= 0; --i) {
        double s = b[i];
        for (int c = i+1; c < n; ++c) s -= A[i][c] * b[c];
        b[i] = s / A[i][i];
    }
    return 0;
}

/* Build conductance matrix G from branch list */
double **build_G(int n_nodes, BranchList *bl) {
    double **G = alloc_matrix(n_nodes, n_nodes);
     // Loop through all branches 
    for (int k = 0; k < bl->size; ++k) {
        int u = bl->arr[k].u;
        int v = bl->arr[k].v;
        double r = bl->arr[k].r;
        if (u < 0 || u >= n_nodes || v < 0 || v >= n_nodes || r <= 0.0) continue;//Skip invalid branches

        if (u == v) continue;//resistor from a node to itself

        double g = 1.0 / r; // Convert resistance to conductance

        G[u][u] += g;  //self-conductance at node u

        G[v][v] += g; //self-conductance at node v

        G[u][v] -= g; //mutual conductance between u and v

        G[v][u] -= g; //symmertry 
    }
    return G; //return conductance matrix
}


/* Print header */
void print_header(void) {
    printf(COL_CYAN COL_BOLD "\n=== Equivalent Resistance Calculator ===\n" COL_RESET);
    }

/* Read integer with prompt and validation */
int read_int_range(const char *prompt, int minv, int maxv) {
    int x;
    while (1) {
        printf(COL_BLUE "%s" COL_RESET, prompt);
        if (scanf("%d", &x) != 1) {
            while (getchar() != '\n'); /* flush */
            printf(COL_RED "Please enter a valid integer.\n" COL_RESET);
            continue;
        }
        if (x < minv || x > maxv) {
            printf(COL_RED "Value must be between %d and %d. Try again.\n" COL_RESET, minv, maxv);
            continue;
        }
        return x;
    }
}

/* Read double > 0 */
double read_positive_double(const char *prompt) {
    double x;
    while (1) {
        printf(COL_BLUE "%s" COL_RESET, prompt);
        if (scanf("%lf", &x) != 1) {
            while (getchar() != '\n');
            printf(COL_RED "Please enter a valid number.\n" COL_RESET);
            continue;
        }
        if (!(x > 0.0)) {
            printf(COL_RED "Value must be positive. Try again.\n" COL_RESET);
            continue;
        }
        return x;
    }
}

/* Read yes/no
 Shows the given prompt with (y/n) */
int read_yesno(const char *prompt) {
    char s[8]; // // temporary buffer to read user text
    while (1) {
        printf(COL_BLUE "%s (y/n): " COL_RESET, prompt);
        if (scanf("%7s", s) != 1) { while (getchar() != '\n'); continue; }// clear input buffer
        char c = tolower(s[0]);
        if (c == 'y') return 1;
        if (c == 'n') return 0;
        printf(COL_RED "Please enter 'y' or 'n'.\n" COL_RESET);
    }
}

/*Table displays branch number, node U, node V, and resistance value. */
void print_branches(BranchList *bl) {
    //  // If the list is empty, inform the user and exit 
    if (bl->size == 0) {
        printf(COL_YELLOW "No branches entered yet.\n" COL_RESET);
        return;
    }
    //tabular format shows data in terminal
    printf(COL_MAG "\nCurrent branches:\n" COL_RESET);
    printf(COL_BOLD "%4s | %6s | %6s | %10s\n" COL_RESET, "No.", "NodeU", "NodeV", "Resistance");
    printf("------------------------------------------------\n");
    //Loop through all stored branches and print 
    for (int i = 0; i < bl->size; ++i) {
        printf("%4d | %6d | %6d | %10.6g ohm\n",
               i+1, bl->arr[i].u + 1, bl->arr[i].v + 1, bl->arr[i].r);
    }
}

/* Compute equivalent resistance between terminals A and B.
   Returns:
     >0 : resistance in ohms
     0  : error (singular)
     -1 : open circuit (infinite)
*/
double compute_req(int n_nodes, BranchList *bl, int A, int B) {
    if (A == B) return 0.0;
    double **G = build_G(n_nodes, bl);

    /* Quick direct two-node case */
    if (n_nodes == 2) {
        double gAB = -G[A][B];
        free_matrix(G);
        if (gAB <= 0.0) return -1.0;
        return 1.0 / gAB;
    }

    /* Build unknown index mapping for nodes except A and B */
    int unknown_count = n_nodes - 2;
    int *idx = malloc(n_nodes * sizeof(int));
    int cnt = 0;
    for (int i = 0; i < n_nodes; ++i) {
        if (i == A || i == B) idx[i] = -1;
        else idx[i] = cnt++;
    }

    /*  Allocate the reduced matrix M
    RHS vector bvec for the nodal analysis equations M * V = b  */
    double **M = alloc_matrix(unknown_count, unknown_count);
    double *bvec = calloc(unknown_count, sizeof(double));
    if (!bvec) { perror("calloc"); free_matrix(G); free(idx); free_matrix(M); return 0.0; }

    // Voltage test source values: V_A = 1 volt, V_B = 0 volt... R_eq = ( v(=1) / I_total) 
    double VA = 1.0, VB = 0.0;
// Build the reduced nodal matrix M and RHS vector b.
    for (int i = 0; i < n_nodes; ++i) {
        if (idx[i] == -1) continue;
        int ri = idx[i];
        for (int j = 0; j < n_nodes; ++j) {
            if (idx[j] == -1) continue;
            int cj = idx[j];
            M[ri][cj] = G[i][j]; //copy corresponding G entry(conductance)
        }
        bvec[ri] = - (G[i][A]*VA + G[i][B]*VB);
    }

    /* Copy because gauss modifies matrices */
    double **Mc = alloc_matrix(unknown_count, unknown_count);
    for (int i = 0; i < unknown_count; ++i)
        for (int j = 0; j < unknown_count; ++j)
            Mc[i][j] = M[i][j];
    double *bc = malloc(unknown_count * sizeof(double));//Copy RHS vector bvec for the same reason
    for (int i = 0; i < unknown_count; ++i) bc[i] = bvec[i];

    int ok = gauss_solve(unknown_count, Mc, bc);
    if (ok != 0) {
        //mat is singular so has no unique soln... 
        free_matrix(G); free_matrix(M); free_matrix(Mc);
        free(bvec); free(bc); free(idx);
        return 0.0; /* singular */
    }

    //alocate volatage vector for all nodes avaiable
    double *V = malloc(n_nodes * sizeof(double));
    for (int i = 0; i < n_nodes; ++i) V[i] = 0.0; //voltage are initialized -0
    V[A] = VA;     //1v
     V[B] = VB;     //0v
// allocate unknown node voltages from solved bc[] vector
    for (int i = 0; i < n_nodes; ++i)
     if (idx[i] != -1)
      V[i] = bc[idx[i]];
//I = Σincomig+outgoing
    double Itotal = 0.0;
    for (int j = 0; j < n_nodes; ++j) {
        if (j == A)
         continue; //  skip self branch
        double gij = -G[A][j]; //conductance between A and j - saved in conduc mat G
        if (gij <= 0.0) 
        continue;  // no direct branch available
        Itotal += gij * (V[A] - V[j]);
    }
// free memory to prevent leak
    free_matrix(G); free_matrix(M); free_matrix(Mc);
    free(bvec); free(bc); free(idx); free(V);

    // If no current flows → open circuit → infinite resistance
    
// Equivalent resistance R_eq = V_test / I_total, v_test=0
    if (Itotal <= 0.0) return -1.0;
    return 1.0 / Itotal;
}

/* Submenu for clearing branches */
void clear_branches_menu(BranchList *bl) {
    if (bl->size == 0) {
        printf(COL_YELLOW "No branches to clear.\n" COL_RESET);
        return;
    }

    print_branches(bl);

    printf(COL_BOLD "\nClear options:\n" COL_RESET);
    printf(" 1) Remove a single branch by number\n");
    printf(" 2) Remove a range of branches (from..to)\n");
    printf(" 3) Clear ALL branches\n");
    printf(" 4) Cancel\n");

    int opt = read_int_range("Choose option (1-4): ", 1, 4);
    if (opt == 1) {
        int idx = read_int_range(" Enter branch number to remove: ", 1, bl->size) - 1;
        printf(COL_YELLOW " Removing branch %d: %d <-> %d : %.6g ohm\n" COL_RESET,
               idx+1, bl->arr[idx].u + 1, bl->arr[idx].v + 1, bl->arr[idx].r);
        bl_remove_at(bl, idx);
        printf(COL_GREEN " Branch removed. Remaining branches: %d\n" COL_RESET, bl->size);
    } else if (opt == 2) {
        int a = read_int_range(" Enter start branch number: ", 1, bl->size);
        int b = read_int_range(" Enter end branch number: ", a, bl->size);
        int count = b - a + 1;
        for (int i = 0; i < count; ++i) bl_remove_at(bl, a - 1); 
        /* remove at same index repeatedly */
        printf(COL_GREEN " Removed branches %d .. %d. Remaining branches: %d\n" COL_RESET, a, b, bl->size);
    } else if (opt == 3) {
        int conf = read_yesno(" Are you sure you want to CLEAR ALL branches?");
        if (conf) {
            bl_clear(bl);
            printf(COL_GREEN "All branches cleared.\n" COL_RESET);
        } else {
            printf(COL_YELLOW "Canceled clearing all branches.\n" COL_RESET);
        }
    } else {
        printf(COL_YELLOW "Canceling. No changes made.\n" COL_RESET);
    }
}

/* Main interactive loop */
int main(void) {
    print_header();

    while (1) {
        int n_nodes = read_int_range("Enter total number of nodes (>=2): ", 2, 10000);
        BranchList bl; bl_init(&bl);

        printf(COL_GREEN "Nodes are numbered in the system 1 to %d\n" COL_RESET, n_nodes);

        /* interactive branch input */
        while (1) {
            printf("\n");
            printf(COL_BOLD "Menu:\n" COL_RESET);
            printf(" 1) Add a branch (u v R)\n");
            printf(" 2) Show current branches\n");
            printf(" 3) Compute equivalent resistance between two nodes\n");
            printf(" 4) Clear branches (single / range / all)\n");
            printf(" 5) Restart network (new node count)\n");
            printf(" 6) Exit program\n");

            int choice = read_int_range("Choose an option (1-6): ", 1, 6);
            if (choice == 1) {
                int u = read_int_range("  Enter node u (1..n): ", 1, n_nodes);
                int v = read_int_range("  Enter node v (1..n, != u): ", 1, n_nodes);
                if (u == v) {
                    printf(COL_RED "  Error: u and v must be different nodes. Branch ignored.\n" COL_RESET);
                    continue;
                }
                double r = read_positive_double("  Enter resistance R (ohm, >0): ");
                bl_push(&bl, u - 1, v - 1, r);
                printf(COL_GREEN "  Branch added Successfully: %d <-> %d : %.6g ohm\n" COL_RESET, u, v, r);
            } else if (choice == 2) {
                print_branches(&bl);
            } else if (choice == 3) {
                if (bl.size == 0) {
                    printf(COL_RED "No branches defined. Add at least one branch first.\n" COL_RESET);
                    continue;
                }
                print_branches(&bl);
                int A = read_int_range("Enter terminal node A: ", 1, n_nodes);
                int B = read_int_range("Enter terminal node B: ", 1, n_nodes);
                if (A == B) {
                    printf(COL_RED "Terminals must be different.\n" COL_RESET);
                    continue;
                }
                printf(COL_YELLOW "Computing equivalent resistance between %d and %d...\n" COL_RESET, A, B);
                double Req = compute_req(n_nodes, &bl, A - 1, B - 1);
                if (Req == 0.0) {
                    printf(COL_RED "Error: linear system singular or numerical failure.\n" COL_RESET);
                } else if (Req < 0.0) {
                    printf(COL_RED COL_BOLD "Result: open circuit between %d and %d (infinite resistance).\n" COL_RESET, A, B);
                } else {
                    printf(COL_GREEN COL_BOLD "Equivalent resistance R_eq between %d and %d = %.12g ohm\n" COL_RESET, A, B, Req);
                }
            } else if (choice == 4) {
                clear_branches_menu(&bl);
            } else if (choice == 5) {
                int conf = read_yesno("Restarting will discard current network. Continue?");
                if (conf) {
                    bl_free(&bl);
                    break; /* outer while will prompt for new n_nodes */
                } else {
                    printf(COL_YELLOW "Restart canceled. Keeping current network.\n" COL_RESET);
                }
            } else if (choice == 6) {
                int ex = read_yesno("Do you really want to exit the program?");
                if (ex) {
                    bl_free(&bl);
                    printf(COL_CYAN "Thank you! Stay curious :) \n" COL_RESET);
                    return 0;
                }
            }
        } /* end input loop */
    } /* outer restart loop */

    return 0;
}
