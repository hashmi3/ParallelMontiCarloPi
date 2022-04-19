#include "core/utils.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <stdio.h>

#define sqr(x) ((x) * (x))
#define DEFAULT_NUMBER_OF_POINTS "12345678"

std::atomic<int> count(0);

uint c_const = (uint)RAND_MAX + (uint)1;
inline double get_random_coordinate(uint *random_seed) {
  return ((double)rand_r(random_seed)) / c_const;
}

void get_points_in_circle(uint id, uint n, uint random_seed) {
  
  uint circle_count = 0;
  double x_coord, y_coord;
  uint temp;
  srand(time (NULL)); //init seed for rand()
  temp = rand(); 
  timer serial_timer;

  serial_timer.start();
  
  double time_taken = 0.0;

  for (uint i = 0; i < n; i++) {
    x_coord = (2.0 * get_random_coordinate(&temp)) - 1.0;
    y_coord = (2.0 * get_random_coordinate(&temp)) - 1.0;
    if ((sqr(x_coord) + sqr(y_coord)) <= 1.0){
        //count.fetch_add(1);            
        circle_count++;
    }
  }
  count.fetch_add(circle_count);
  

  time_taken = serial_timer.stop();
  printf("Thrd ID: %u: ",id);
  printf("%u, %u, %u, %lf\n",id,n,circle_count,time_taken);
  //std::cout << id<< ", "<< n<< ", "<< circle_count<< ", "<<time_taken <<std::endl;
  
}

void piCalculation(uint n, uint n_workers) {
  timer serial_timer;
  double time_taken = 0.0;
  uint random_seed = 1;
  uint temp = n%n_workers;
  

  std::vector<std::thread> thrds;
  uint circle_points = 0;
  std::cout << "thread_id, points_generated, circle_points, time_taken\n";

  serial_timer.start();
  // Create threads and distribute the work across T threads
  // -------------------------------------------------------------------
  //uint circle_points = get_points_in_circle(n, random_seed);
  for(uint i=0; i<n_workers;i++ ){
    if(temp > 0 && i==0){
    thrds.push_back(std::thread(get_points_in_circle, i,(n/n_workers)+temp,random_seed ));
    temp = 0;
    }
    else
    ///Create thread get_points_in_circle(uint id, uint n, uint random_seed)  
    thrds.push_back(std::thread(get_points_in_circle, i,(n/n_workers),random_seed ));
  }  

  for (auto &i :thrds ){
    i.join();
  }
    
  circle_points = count;

  double pi_value = 4.0 * (double)circle_points / (double)n;
  // -------------------------------------------------------------------


  time_taken = serial_timer.stop();

  // Print the overall statistics
  std::cout << "Total points generated : " << n << "\n";
  std::cout << "Total points in circle : " << circle_points << "\n";
  std::cout << "Result : " << std::setprecision(VAL_PRECISION) << pi_value
            << "\n";
  std::cout << "Time taken (in seconds) : " << std::setprecision(TIME_PRECISION)
            << time_taken << "\n";
}

int main(int argc, char *argv[]) {
  // Initialize command line arguments
  cxxopts::Options options("pi_calculation",
                           "Calculate pi using serial and parallel execution");
  options.add_options(
      "custom",
      {
          {"nPoints", "Number of points",
           cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_OF_POINTS)},
          {"nWorkers", "Number of workers",
           cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_OF_WORKERS)},
      });

  auto cl_options = options.parse(argc, argv);
  uint n_points = cl_options["nPoints"].as<uint>();
  uint n_workers = cl_options["nWorkers"].as<uint>();
  std::cout << std::fixed;
  std::cout << "Number of points : " << n_points << "\n";
  std::cout << "Number of workers : " << n_workers << "\n";

  //std::cout << "Time:"<< (int) time(NULL) << std::endl;
  piCalculation(n_points, n_workers);

  return 0;
}
