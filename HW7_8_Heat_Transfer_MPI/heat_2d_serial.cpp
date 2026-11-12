// 2-D temperature Example  -- Single-Threaded Program
/*
To compile on the Picocluster System
   GNU Compiler
      g++ heat_2d_serial.cpp -o heat_2d_serial -Ofast

To execute on the Picocluster System
   GNU Compiler
      run_script heat_2d_serial.sh
      where heat_2d_serial.sh is a script file that contains
         #!/bin/bash
         ./heat_2d_serial 10 100 
         # execute a 10 x 10 point 2d-heat transfer problem 
         # for 100 iterations 
*/

using namespace std;
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sys/time.h>

#define TIMER_CLEAR  (tv1.tv_sec = tv1.tv_usec = tv2.tv_sec = tv2.tv_usec = 0)
#define TIMER_START     gettimeofday(&tv1, (struct timezone*)0)
#define TIMER_ELAPSED   (double) (tv2.tv_usec- tv1.tv_usec)/1000000.0+(tv2.tv_sec-tv1.tv_sec)
#define TIMER_STOP      gettimeofday(&tv2, (struct timezone*)0)
struct timeval tv1,tv2;


// Global Constants
const int ROOM_TEMP=20;       // temperature everywhere except the fireplace
const int FIREPLACE_TEMP=100; // temperature at upper/middle boundary

int n;                // number of non boundary condition rows in problem 
int num_iterations;   // number of successive iterations before terminating

int active_rows;     // number of non boundary condition rows

int total_rows;      // total number of rows including boundary point rows

int total_cols;      // total number of columns including
                     // boundary condition columns

double *temp;        // pointer local temperature array row-ordered storage

// Old style macro to give the illusion of 2D memory
#define Temp(x,y) temp[(x)*total_cols+y] 

// routine to initialize the temperature vector and the temperature at the
// boundary
// initialization occurs separately on each MPI process for the region
// that the processes is assigned as well as additional boundary condition/
// ghost point rows
void init_temp(void) {
    const int fireplace_start = 0.3 * (double) n;
    const int fireplace_end = 0.7 * (double) n;

    // Set leftmost boundary condition on column 0
    for (int row=0;row < total_rows; row++) {
        for (int col=0;col<total_cols;col++) {
            if (row == 0) {
                if (col<=fireplace_start || col > fireplace_end) {
                    Temp(row,col) = ROOM_TEMP; // temp[row*total_cols+col];
                }
                else {
                    Temp(row,col) = FIREPLACE_TEMP;
                }
            }
            else {
                Temp(row,col) = ROOM_TEMP;
            }
        }
    }
}
// main compute/communication region -- allows for temperature diffusion 
// to occur one iteration at a time until the specified number of 
// iterations has been performed
//
// Section 1: Nearest-neighbor ghost point reconciliation
//
// Section 2: Local Computation Phase -- stenciled computation
//                                       using ghost points
// 
void compute_temp() {
    #define Temp_buf(x,y) temp_buf[(x)*total_cols+y] // *(temp_buf+x*total_cols+y)
    double *temp_buf = new double[total_rows*total_cols];

    for (int i=0;i<num_iterations;i++) {

      // Start local stenciled computation 
        for (int j=1;j<=active_rows;j++) {
            for (int k=1;k<=total_cols-2;k++) {
                Temp_buf(j,k)=0.25*(Temp(j-1,k)+Temp(j+1,k)+
                        Temp(j,k-1)+Temp(j,k+1));
            }
        }
        for (int j=1;j<=active_rows;j++) {
            for (int k=1;k<=total_cols-2;k++) { 
                Temp (j,k)=Temp_buf(j,k);
            }
        }
      // End of iternation 
    }
    delete temp_buf;
}
// routine to display temperature values at each point including the 
// boundary points
void print_temp(void) {
    cout << "Temperature Matrix Including Boundary Points" << endl;
    for (int row=0;row<total_rows;row++) {
        for (int col=0;col<total_cols;col++) {
            cout << setw(5) << Temp(row,col) << " ";
        }
        cout << endl << flush;
    }
}

// Routine that performs a simple 64 integer checksum
// of the binary contents of the final Temp array
// This is used to perform a quick comparison of the
// results to insure that modifications to the original
// program did not affect the accuracy of the computation
unsigned long long int checksum(void) {
    void *ptr;
    unsigned long long int sum = 0;
    for (int row=0;row<total_rows;row++) {
        for (int col=0;col<total_cols;col++) {
            ptr=(void *) &Temp(row,col);
            sum += *(unsigned long long int *) ptr;
        }
    }
    return sum;
}

void print_usage_instructions(char *command) {
    cout << "Usage: " << command <<
        " [Dim n = num row/cols square matrices] [Number Iterations] " <<
        "<x>" << endl <<
        "   where optional argument x = " << endl <<
        "       H suppress output -- print n,runtime and" << endl <<
        "                            checksum in human readable format" << endl <<
        "       S suppress output -- print checkSum" << endl <<
        "       C suppress output -- print n,runtime" << endl <<
        "                            in CSV format" << endl <<
        "       G suppress output -- print n,runtime" << endl <<
        "                            in gnuplot format" << endl;

        exit(1);
}

int main (int argc, char *argv[]){

    if (argc!=3 && argc!=4) {
        print_usage_instructions(argv[0]);
        exit(0);
    }

    // get total number of points not counting boundary points
    // from first command line argument 
    // Warning No Error Checking 
    n = atoi(argv[1]);

    // get total number of iterations to run simulation
    // Warning No Error Checking
    num_iterations = atoi(argv[2]);

    // define the number of rows that are to be updated each iteration
    active_rows = n;

    // set total rows processed including boundary points
    total_rows = n+2;

    // set total columns processed including boundary points
    total_cols = n+2; 

    // dynamically allocate memory to store the MPI process
    // local temp array
    temp = new double [total_rows*total_cols]; 

    // initialize MPI temperature matrix
    init_temp();

    // take start timer reading
    TIMER_CLEAR;
    TIMER_START;

    // compute temperatures returning after specified number
    // of iterations
    compute_temp(); 

    // take end timer reading
    TIMER_STOP;


    // print temp array output
    // print out the results if there is no suppress output argument
    if (argc!=4) {
        print_temp();
        unsigned long long int ck_sum=checksum();
        // print time in normal human readable format
        std::cout << "Execution Time = " << TIMER_ELAPSED << " Seconds"
            << std::endl;
        std::cout << "64 bit Checksum = " << ck_sum << std::endl;
    }
    // if there exists a 4th argument, then suppress the output
    if (argc==4) {
        // print time in gnuplot format
        if (*argv[3]=='G') {
            std::cout << n << " " << TIMER_ELAPSED << std::endl;
        }
        // print time in CSV format
        else if (*argv[3]=='C') {
            std::cout << n << "," << TIMER_ELAPSED << std::endl;
        }
        // print 64 bit checksum
        else if (*argv[3]=='S') {
            // compute 64 bit data checksum
            unsigned long long int ck_sum=checksum();
            std::cout << "64 bit Checksum = " << ck_sum << std::endl;
        }
        else if (*argv[3]=='H') {
            unsigned long long int ck_sum=checksum();
            // print time and Checksum in normal human readable format
            std::cout << "Number of active data points =" << n << std::endl;
            std::cout << "Execution Time = " << TIMER_ELAPSED << " Seconds"
                      << std::endl;
            std::cout << "64 bit Checksum = " << ck_sum << std::endl;
        }
    }

    delete temp;

}
