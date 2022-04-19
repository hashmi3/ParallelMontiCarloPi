#include "core/graph.h"
#include "core/utils.h"
#include <future>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <stdio.h>

std::atomic<int> tri_count(0);

uintV countTriangles(uintV *array1, uintE len1, uintV *array2, uintE len2,
                     uintV u, uintV v) {

  uintE i = 0, j = 0; // indexes for array1 and array2
  uintV count = 0;

  if (u == v)
    return count;

  while ((i < len1) && (j < len2)) {
    if (array1[i] == array2[j]) {
      if ((array1[i] != u) && (array1[i] != v)) {
        count++;
      }
      i++;
      j++;
    } else if (array1[i] < array2[j]) {
      i++;
    } else {
      j++;
    }
  }
  return count;
}

void getCount(uint id, uint n, Graph &g ){

  long triangle_count = 0;  
  // Process each edge <u,v>
  printf("Inside thread: %u\n",id);
  for (uintV u = 0; u < n; u++) {
    // For each outNeighbor v, find the intersection of inNeighbor(u) and
    // outNeighbor(v)
    uintE out_degree = g.vertices_[u].getOutDegree();
    for (uintE i = 0; i < out_degree; i++) {
      uintV v = g.vertices_[u].getOutNeighbor(i);
      triangle_count += countTriangles(g.vertices_[u].getInNeighbors(),
                                       g.vertices_[u].getInDegree(),
                                       g.vertices_[v].getOutNeighbors(),
                                       g.vertices_[v].getOutDegree(), u, v);
    }
  }
  printf("Thrd:%u done, count:%ld\n",id,triangle_count);
  tri_count.fetch_add(triangle_count);
}

void triangleCountSerial(Graph &g, uint nWorkers) {
  uintV n = g.n_;
  long triangle_count = 0;
  double time_taken = 0.0;
  timer t1;

  //prep thrd sequence
  int * thrd_seq = (int*)malloc(sizeof(int)*n);
  int * thrd_group = (int*) malloc(sizeof(int)* nWorkers);

  for(int i=0; i< nWorkers; ++i)    //create group of threads
       thrd_group[i] = i;

  for(int i=0; i<n; ++i)          //create thrd sequence
       thrd_seq[i] = thrd_group[i%nWorkers];

  //std::cout <<"Workers: "<<nWorkers<<" \n"<< "Vertices: "<<n<< std::endl;
  
  //for(int i=0; i<n; ++i)
  //    std::cout<< thrd_seq[i]<<", ";

  // The outNghs and inNghs for a given vertex are already sorted

  // Create threads and distribute the work across T threads
  // -------------------------------------------------------------------
  t1.start();
  uint i = 0;
                        //void getCount(uint id, uint n, Graph &g ){
  while(i<n){
      std::vector<std::thread> thrds;
      for(uint j=0; j<nWorkers; j++){
  std::cout <<"Thrd: "<<j<<", "<< "Vertex: "<<i<< std::endl;
          thrds.push_back(std::thread(getCount, j,n,std::ref(g)));
          i++;
      }
      for(auto &k: thrds){
          k.join();
      }
  }

  time_taken = t1.stop();
  // -------------------------------------------------------------------

  // Print the overall statistics
  std::cout << "Number of triangles : " << triangle_count << "\n";
  std::cout << "Number of unique triangles : " << triangle_count / 3 << "\n";
  std::cout << "Time taken (in seconds) : " << std::setprecision(TIME_PRECISION)
            << time_taken << "\n";
}

int main(int argc, char *argv[]) {
  cxxopts::Options options(
      "triangle_counting_serial",
      "Count the number of triangles using serial and parallel execution");
  options.add_options(
      "custom",
      {
          {"nWorkers", "Number of workers",
           cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_OF_WORKERS)},
          {"inputFile", "Input graph file path",
           cxxopts::value<std::string>()->default_value(
               "/scratch/assignment1/input_graphs/roadNet-CA")},
      });

  auto cl_options = options.parse(argc, argv);
  uint n_workers = cl_options["nWorkers"].as<uint>();
  std::string input_file_path = cl_options["inputFile"].as<std::string>();
  std::cout << std::fixed;
  std::cout << "Number of workers : " << n_workers << "\n";

  Graph g;
  std::cout << "Reading graph\n";
  g.readGraphFromBinary<int>(input_file_path);
  std::cout << "Created graph\n";

  //triangleCountSerial(g, n_workers);
  triangleCountSerial(g, 4);

  return 0;
}
