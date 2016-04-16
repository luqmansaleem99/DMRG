//
//  main.cpp
//  DMRG-Hubbard
//
//  Created by Andrew Shen on 16/3/28.
//  Copyright © 2016 Andrew Shen. All rights reserved.
//

//#define EIGEN_NO_AUTOMATIC_RESIZING
#define EIGEN_INITIALIZE_MATRICES_BY_NAN

#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <unsupported/Eigen/KroneckerProduct>

#include "Class_DMRGSystem.hpp"

using namespace Eigen;
using namespace std;


int main()
{
    int nsites, n_sweeps, n_states_to_keep, max_lanczos_iter;
    double hubbard_u, particles;
    double rel_err, truncation_error;

    // DMRG Parameters
    nsites = 30;
    n_sweeps = 3;

    n_states_to_keep = 500;
    max_lanczos_iter = 100;
    truncation_error = 1e-8;
    rel_err = 1e-9;
    
    // Model Paramater
    hubbard_u = 1;
    particles = nsites;
    
    
    // Initialization
    DMRGSystem S(nsites, max_lanczos_iter, rel_err, hubbard_u);
    // Warmup
    for (int n = 1; n < nsites/2; n++) {
        cout << "=== Warmup Iteration " << n << endl;

        S.BuildBlockLeft(n);
        S.BuildBlockRight(n);
        
        cout << "Total particle number " << 2 * n + 2 << endl;
        S.GroundState(2 * n + 2, false);
        
        S.Truncate(BlockPosition::LEFT, n_states_to_keep, truncation_error);
        S.Truncate(BlockPosition::RIGHT, n_states_to_keep, truncation_error);
    }
    
    // Sweep
    cout << "=== Start sweeps" << endl;
    int first_iter = 0.5 * nsites;
    for (int sweep = 1; sweep <= n_sweeps; sweep++) {
        // this is ugly
        if (first_iter == 1) {
            S.left_size = 0;
            S.right_size = nsites - 2;
        }
        for (int iter = first_iter; iter < nsites - 2; iter++) { // why must n-2 to predict wavefunction
            cout << "=== Left-to-right Iteration " << iter << endl;
            S.BuildSeed(particles, SweepDirection::L2R);
            S.BuildBlockLeft(iter);

            S.BuildBlockRight(nsites - iter - 2);

            S.GroundState(particles, false);
            S.Truncate(BlockPosition::LEFT, n_states_to_keep, truncation_error);

        }
        first_iter = 1;
        for (int iter = first_iter; iter < nsites - 2; iter++) {
            cout << "=== Right-to-left Iteration " << iter << endl;
            S.BuildBlockLeft(nsites - iter - 2);
            S.BuildBlockRight(iter);
            //S.BuildSeed(R2L);
            S.GroundState(particles, false);
            /*
            if (iter == 0.5 * nsites) {
                S.Measure();
            }
             */
            S.Truncate(BlockPosition::RIGHT, n_states_to_keep, truncation_error);

        }
        
    }

    cout << "Sweep completed. " << endl;
    
    
    return 0;
}