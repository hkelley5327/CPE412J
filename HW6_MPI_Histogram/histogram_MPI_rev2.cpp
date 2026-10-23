/**************************************************************/
/* Histogram construction Program -- MPI version              */
/* September 2023 -- B. Earl Wells -- University of Alabama   */
/* (C MPI bindings)                  in Huntsville            */
/**************************************************************/
/*
This program illustrates the basic concepts of SPMD programming using 
MPI.  The program represents a common example that is used often in 
the CPE 412/512 text, creating a histogram of binned numbers.
The program is written in such a way that it is assumed that the
sequence of numbers is first read from a central source (in this case
a data file) by a single MPI process (the root process) and then 
partitioned into equal groups with each part being distributed 
(scattered) to the local memory domains of the other MPI
processes in the system. Each MPI process then locally computes its
own histogram of the data for the data in its own domain. This is
followed by a a coordinated communication/computation operation
where the local histogram information is transferred to the
root MPI process where it is combined into the final histogram
by using element-wise addition (i.e. a vector reduce operation).
The histogram output is then placed by the root process in a file
which is named histogram.txt

The following is a simplified version of the program which you will
be asked to augment in a number of ways in future homework 
assignments. Use the dmc.asc.edu or the Jetson Cluster system for
these assignments. 
Notes: This implementation utilizes the MPI library's collective
       communication routines MPI_Bcast, MPI_Scatterv, and MPI_Reduce
       along with Init, Finalize, Comm_size, Comm_rank, and Abort.

  Compilation on dmc.asc.edu

   GNU Compiler
   mpic++ histogram_MPI_rev2.cpp -o histogram_MPI_rev2

To execute:
   
   GNU compiler
   use run_script_mpi histogram_MPI.sh  (edited to run histogram_MPI_rev2)

*/  

using namespace std;
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <random>
#include <exception>
#include <mpi.h> /* MPI Prototype Header Files */

/*
Function used by get_parameters routine to prompt the user to 
interactively enter the next parameter and to return it to the 
calling routine
*/
template <class T>
T enter_next_param(const std::string prompt) {
   T num;
   while (1) {
      string input = "";
      cout << prompt << endl;
      getline(cin, input);
      stringstream myStream(input);
      if (myStream >> num) break;
      cout << "Invalid Input" << endl << endl;
   }
   return num;
}

/*
Routine to retrieve the run time parameters which include
1) the number of numbers in the number list,
2) number of equally spaced bins to be used by the histogram,
3) maximum value in bin range, 4) minimum value in bin range.
If there is a 5th run time parameter then outputing of the numbers 
array that was generated is suppressed 
If no parameters are supplied on the command line, then the program
interactively prompts the user for the information.
*/
void get_parameters(int *list_size,int *num_bins,double *max_bin_val, 
   double *min_bin_val,bool *num_list_out, int argc,char *argv[],
   int rank,int numprocs) {

   if(argc==1) {
      // One Command Line Argument Case:
      // case where user did not enter number of numbers on command line
      // In this case, only one of the MPI processes needs to communicate
      // directly with the user. Since there will always be a MPI process
      // with rank 0 this is the one that will perform the 
      // human interface communication.
      if (rank==0) {
         *list_size=enter_next_param<int>("Enter number of numbers in list:");
         *num_bins=enter_next_param<int>("Enter number of bins in histogram:");
         *max_bin_val=enter_next_param<double>("Enter Upper Range of histogram:");
         *min_bin_val=enter_next_param<double>("Enter Lower Range of histogram:");
         char print_out=enter_next_param<char>("Suppress numbers array printing (y or n):");
         if (print_out=='y' || print_out=='Y') *num_list_out=false; 
      }
      // since only the rank=0 MPI process is communicating with the
      // user, this process must send its value to all of the
      // other MPI processes. It does this with the MPI library's
      // MPI_Bcast() collective one-to-all broadcast routine.
      // NOTE: ALL MPI PROCESSES execute the MPI_Bcast calls
      MPI_Bcast(list_size,1,MPI_INT,0,MPI_COMM_WORLD);
      MPI_Bcast(num_bins,1,MPI_INT,0,MPI_COMM_WORLD);
      MPI_Bcast(max_bin_val,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
      MPI_Bcast(min_bin_val,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
   }
   else if (argc==5 || argc==6) {
      // Two Command Line Argument case:
      // user supplied the number of numbers on the command line.
      // Each MPI process can retrieve it from there. No need to
      // broadcast it to the other processes because each have it at
      // run time.
      *list_size = atoi(argv[1]); // get list_size from command line
      *num_bins = atoi(argv[2]);  // get num_bins from command line
      *max_bin_val = atoi(argv[3]);  // get max_bin_val from command line
      *min_bin_val = atoi(argv[4]);  // get min_bin_val from command line
      if (argc==6) *num_list_out=false; // suppress output of numbers array
   }
   else {
      if (rank==0) {
         cout << "Usage: mpirun -np [num MPI processes] histogram_MPI [list size num_bins max_bin_val min_bin_val]" << endl;
      }
      MPI_Finalize(); // Terminate MPI
      exit(0); // Exit Program 
   }
}

/*
Routine that fills a 1-D array with Random Data with values
based on a normal distibution that has a mean of MEAN and a
standard deviation of STDEV
This simulates in some way what might happen if there was a 
single sequential data acquisition source that only one MPI process
could handle at a time
*/
#define MEAN  50.0f
#define STDEV 20.0f
void create_list(double *numbers,int list_size) {
   std::default_random_engine generator(123546);
   std::normal_distribution<double> distribution(MEAN,STDEV);

   for(int i=0;i<list_size;i++) {
      numbers[i]=distribution(generator);
   }
}

// Routine that outputs the numbers matrix to the screen 
void print_matrix(double *numbers,int list_size)
{
   for(int i=0;i<list_size;i++) {
      cout << numbers[i] << endl;
   }
}


// MAIN ROUTINE: parallel histogram calculation
int main(int argc, char *argv[]) {

   int numprocs,rank;

   MPI_Init(&argc,&argv); // Initalize MPI environment
   MPI_Comm_size(MPI_COMM_WORLD,&numprocs); //get total number of processes
   MPI_Comm_rank(MPI_COMM_WORLD,&rank); // get process identity number

   //get run time parameters from the user
   // list_size == number of numbers to generate on root node
   // num_bins  == number of equally-spaced bins for the histogram
   // value range (max_val,min_val) that is being recorded in the histogram
   int list_size, num_bins;
   double max_val, min_val;
   bool print_flg=true; // default is to print numbers out to screen

   get_parameters(&list_size,&num_bins,&max_val,&min_val,&print_flg,
      argc,argv,rank,numprocs);
   // compute the range associated with each bin
   const double bin_range = (max_val-min_val)/num_bins;

   double *numbers = NULL;   // only allocated on the root process
   int *histogram = NULL;    // only allocated on the root process
   //  if root MPI Process (0) then
   if(rank==0) {

      // dynamically allocate from heap the histogram array on the root process
      try {
         histogram = new int[num_bins];
      }
      catch (exception& err) { 
         cout << err.what() << " on Rank=0 MPI Process for histogram "
              << "array :(" << endl << flush;
         MPI_Abort(MPI_COMM_WORLD,1); // abort the MPI Environment
      }

      // dynamically allocate from heap the numbers array on the root process
      try {
         numbers = new double[list_size];
      }
      catch (exception& err) { 
         cout << err.what() << " on Rank=0 MPI Process for numbers "
              << "array :(" << endl << flush;
         MPI_Abort(MPI_COMM_WORLD,1); // abort the MPI Environment
      }

      // initialize numbers array with random data
      create_list(numbers,list_size);

      // print the array if printing is enabled
      if (print_flg) { 
         cout << "numbers matrix =" << endl; 
         print_matrix(numbers,list_size);
         cout << endl;
      }
   }

   // dynamically allocate from heap the numbers_local array on each
   // of the MPI Processes
   // The number of elements assigned to this MPI process is
   // list_size/numprocs, plus one extra element if this process is one of
   // the first list_size%numprocs ranks (these take up the remainder).
   // This never exceeds ceil(list_size/numprocs), and the local array is
   // allocated at exactly this size so no memory is wasted.
   const int local_list_size = list_size/numprocs + 
                               (rank < list_size%numprocs ? 1 : 0);
   const int numbers_local_array_sz = local_list_size; 
   double *numbers_local = NULL;
   try {
      numbers_local = new double[numbers_local_array_sz];
   }
   catch (exception& err) { 
      cout << err.what() << " on Rank=" << rank << 
          " MPI Process for numbers_local array :(" << endl << flush;
      MPI_Abort(MPI_COMM_WORLD,1); // abort the MPI Environment
   }
 
   // dynamically allocate from heap the histogram_local array on each
   // of the MPI Processes
   int *histogram_local = NULL;
   try {
      histogram_local = new int[num_bins];
   }
   catch (exception& err) { 
      cout << err.what() << " on Rank=" << rank << 
          " MPI Process for histogram_local array :(" << endl << flush;
      MPI_Abort(MPI_COMM_WORLD,1); // abort the MPI Environment
   }

   // scatter the numbers array to all MPI processes in the system using
   // the MPI library's MPI_Scatterv() collective routine. Scatterv is
   // used instead of MPI_Scatter because the list size is not necessarily
   // evenly divisible by the number of MPI processes. The root process
   // builds two arrays (significant only on the root): sendcounts[i] is
   // the number of elements sent to process i and displs[i] is the offset
   // in the numbers array where process i's elements begin. Each process
   // gets list_size/numprocs elements, with the first list_size%numprocs
   // processes getting one extra, so none gets more than
   // ceil(list_size/numprocs) elements.
   int *sendcounts = NULL;
   int *displs = NULL;
   if (rank==0) {
      try {
         sendcounts = new int[numprocs];
      }
      catch (exception& err) { 
         cout << err.what() << " on Rank=0 MPI Process for sendcounts "
              << "array :(" << endl << flush;
         MPI_Abort(MPI_COMM_WORLD,1); // abort the MPI Environment
      }
      try {
         displs = new int[numprocs];
      }
      catch (exception& err) { 
         cout << err.what() << " on Rank=0 MPI Process for displs "
              << "array :(" << endl << flush;
         MPI_Abort(MPI_COMM_WORLD,1); // abort the MPI Environment
      }
      int begin_element=0;
      for (int mpiproc=0;mpiproc<numprocs;mpiproc++) {
         sendcounts[mpiproc] = list_size/numprocs + 
                               (mpiproc < list_size%numprocs ? 1 : 0);
         displs[mpiproc] = begin_element;
         begin_element += sendcounts[mpiproc];
      }
   }
   MPI_Scatterv(numbers,sendcounts,displs,MPI_DOUBLE,
                numbers_local,local_list_size,MPI_DOUBLE,
                0,MPI_COMM_WORLD);

   // create local histogram of the set of numbers associated 
   // with the current MPI process

   // (local_list_size, the number of elements assigned to this MPI
   //  process, was computed above when numbers_local was allocated)

   // clear out local histogram array, histogram_local
   for (int i=0;i<num_bins;i++) histogram_local[i]=0.0f;
 
   // compute the local histogram for the local set of numbers
   for (int i=0;i<local_list_size;i++) {
      double num;
      num = numbers_local[i];
      if (num>=min_val && num < max_val) {
         num -= min_val;
         histogram_local[(int) (num/bin_range)]++;
      }
   }
   // obtain final histogram on root MPI process by element-wise
   // summation of each of the local histograms present on each 
   // MPI process using the MPI library's MPI_Reduce() collective routine
   MPI_Reduce(histogram_local,histogram,num_bins,MPI_INT,MPI_SUM,
              0,MPI_COMM_WORLD);

   // output sum from root MPI process
   if (rank==0) {
       cout << "Histogram Information" << endl;
       for (int i=0;i<num_bins;i++) {
          cout << setprecision(8) << min_val+bin_range/2+bin_range*i <<
             "  " << histogram[i] << endl;
       }
       cout << numprocs << " MPI Process OpenMPI Implementation"
            << endl << flush;
   }

   // reclaim dynamiclly allocated memory
   if (rank==0) {
      delete [] histogram;
      delete [] numbers;
      delete [] sendcounts;
      delete [] displs;
   }
   delete [] histogram_local;
   delete [] numbers_local;

   // Terminate MPI Program -- perform necessary MPI housekeeping
   // clear out all buffers, remove handlers, etc.
   MPI_Finalize(); // Remove MPI Environment

}
