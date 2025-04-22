/*
 * math.cpp
 *
 *  Created on: Jun 21, 2023
 *      Author: lorenzo
 */

#include "math.h"

#include <stdexcept>

namespace ba {

namespace utils {

constexpr int MAX_ITER = 50;
constexpr double EPSILON = 1e-10;

void jacobi_rotate(mat3 &A, mat3 &V, int p, int q) {
    if (A[p][q] == 0.0) return;

    double diff = A[q][q] - A[p][p];
    double t;
    if(std::abs(diff) < EPSILON) {
        t = A[p][q] / diff;
    } 
    else {
        double phi = 0.5 * diff / A[p][q];
        t = 1.0 / (std::abs(phi) + std::sqrt(phi * phi + 1.0));
        if (phi < 0.0) t = -t;
    }

    double c = 1.0 / std::sqrt(t * t + 1.0);
    double s = t * c;
    double tau = s / (1.0 + c);

    double app = A[p][p];
    double aqq = A[q][q];
    double apq = A[p][q];

    A[p][p] = app - t * apq;
    A[q][q] = aqq + t * apq;
    A[p][q] = A[q][p] = 0.0;

    for(int r = 0; r < 3; ++r) {
        if(r != p && r != q) {
            double arp = A[r][p];
            double arq = A[r][q];
            A[r][p] = A[p][r] = arp - s * (arq + tau * arp);
            A[r][q] = A[q][r] = arq + s * (arp - tau * arq);
        }

        double vrp = V[r][p];
        double vrq = V[r][q];
        V[r][p] = vrp - s * (vrq + tau * vrp);
        V[r][q] = vrq + s * (vrp - tau * vrq);
    }
}

void sort_eigenpairs(vec3 &eigenvalues, mat3 &eigenvectors) {
    for(int i = 0; i < 2; ++i) {
        int min_index = i;
        double min_value = eigenvalues[i];

        for(int j = i + 1; j < 3; ++j) {
            if(eigenvalues[j] < min_value) {
                min_index = j;
                min_value = eigenvalues[j];
            }
        }

        if(min_index != i) {
            // Swap eigenvalues
            std::swap(eigenvalues[i], eigenvalues[min_index]);

            // Swap corresponding eigenvector columns
            for(int k = 0; k < 3; ++k) {
                std::swap(eigenvectors[k][i], eigenvectors[k][min_index]);
            }
        }
    }
}

void diagonalize_3x3(mat3 &A, vec3 &eigenvalues, mat3 &eigenvectors) {
    eigenvectors = mat3(1.0); // Identity matrix

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        // Off-diagonal max element
        int p = 0, q = 1;
        double max_val = std::abs(A[p][q]);
        for(int i = 0; i < 2; ++i) {
            for(int j = i + 1; j < 3; ++j) {
                if(std::abs(A[i][j]) > max_val) {
                    max_val = std::abs(A[i][j]);
                    p = i;
                    q = j;
                }
            }
        }

        if(max_val < EPSILON) break; // converged
        jacobi_rotate(A, eigenvectors, p, q);
    }

    eigenvalues[0] = A[0][0];
    eigenvalues[1] = A[1][1];
    eigenvalues[2] = A[2][2];

    sort_eigenpairs(eigenvalues, eigenvectors);
}

}

}

