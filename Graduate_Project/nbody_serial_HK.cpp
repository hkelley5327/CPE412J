// ============================================================================
// File: nbody_serial_HK.cpp
// Assignment: 412J graduate project
// Purpose: solve the n-body problem SERIALLY
//          serves as the benchmark for the later parallel version
// Name: Hannah Kelley
// Due Date: December 13, 2026
// ============================================================================

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <string>

const double G = 6.67430e-11;     // gravitational constant
const double EPSILON = 1e-3;   // Softening factor squared (to prevent div-by-zero)
const double DT = 0.01;           // simulation time step in seconds

struct ParticleSystem {
    std::vector<double> x, y, z;       // 3D position in meters
    std::vector<double> vx, vy, vz;    // 3D velocity in meters/sec
    std::vector<double> fx, fy, fz;    // force accumulators in Newtons
    std::vector<double> mass;          // mass in kg

    explicit ParticleSystem(int N)
        : x(N), y(N), z(N),
          vx(N, 0.0), vy(N, 0.0), vz(N, 0.0),
          fx(N, 0.0), fy(N, 0.0), fz(N, 0.0),
          mass(N, 1.0) {}
};

// initialization
void initializeParticles(ParticleSystem& p, int N) {
    for (int i = 0; i < N; i++) {
        p.x[i] = static_cast<double>(i);
        p.y[i] = (i % 2 == 0) ? 0.5 : -0.5;
        p.z[i] = 0.0;

        p.vx[i] = 0.0;
        p.vy[i] = 0.0;
        p.vz[i] = 0.0;

        p.mass[i] = 1.0e10; // scaled mass in kg
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_particles> <timesteps>\n";
        return 1;
    }

    int N = std::atoi(argv[1]);
    int timesteps = std::atoi(argv[2]);

    ParticleSystem p(N);
    initializeParticles(p, N);

    // start timer
    auto start = std::chrono::high_resolution_clock::now();

    // main loop
    for (int step = 0; step < timesteps; step++) {

        // reset force accumulators to zero
        std::fill(p.fx.begin(), p.fx.end(), 0.0);
        std::fill(p.fy.begin(), p.fy.end(), 0.0);
        std::fill(p.fz.begin(), p.fz.end(), 0.0);

        // gravitation force calcs
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (i == j) continue;

                double dx = p.x[j] - p.x[i];
                double dy = p.y[j] - p.y[i];
                double dz = p.z[j] - p.z[i];

                double distSqr = dx * dx + dy * dy + dz * dz + EPSILON;
                double invDist = 1.0 / std::sqrt(distSqr);
                double invDist3 = invDist * invDist * invDist;

                double forceMagnitude = G * p.mass[i] * p.mass[j] * invDist3;

                p.fx[i] += forceMagnitude * dx;
                p.fy[i] += forceMagnitude * dy;
                p.fz[i] += forceMagnitude * dz;
            }
        }

        // update velocities and position
        // results calculated using integration
        for (int i = 0; i < N; i++) {
            double ax = p.fx[i] / p.mass[i];
            double ay = p.fy[i] / p.mass[i];
            double az = p.fz[i] / p.mass[i];

            p.vx[i] += ax * DT;
            p.vy[i] += ay * DT;
            p.vz[i] += az * DT;

            p.x[i] += p.vx[i] * DT;
            p.y[i] += p.vy[i] * DT;
            p.z[i] += p.vz[i] * DT;
        }
    }

    // stop timer and calculate execution time
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();

    // open .txt output file to write both execution metrics and final positions
    std::string filename = "serial_ref_N" + std::to_string(N)
                     + "_T" + std::to_string(timesteps) + ".txt";
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file " << filename << "\n";
        return 1;
    }

    outFile << std::fixed << std::setprecision(6);

    // write performance summary to output file
    outFile << "==========================================\n";
    outFile << "--- Serial Baseline Execution ---\n";
    outFile << "Particles (N): " << N << "\n";
    outFile << "Timesteps:     " << timesteps << "\n";
    outFile << "Runtime:       " << elapsed << " seconds\n";
    outFile << "==========================================\n\n";

    // write final coords to output file
    outFile << "--- Final Particle Positions ---\n";
    for (int i = 0; i < N; i++) {
        outFile << i << " " << p.x[i] << " " << p.y[i] << " " << p.z[i] << "\n";
    }

    outFile.close();

    return 0;
}
