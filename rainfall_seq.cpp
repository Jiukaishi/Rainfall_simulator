#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
// • Traverse over all landscape points
// ◦ 1) Receive a new raindrop (if it is still raining) for each point.
// ◦ 2) If there are raindrops on a point, absorb water into the point
// ◦ 3a) Calculate the number of raindrops that will next trickle to the lowest
// neighbor(s) • Make a second traversal over all landscape points ◦ 3b) For
// each point, use the calculated number of raindrops that will trickle to the
// lowest neighbor(s) to update the number of raindrops at each lowest neighbor,
// if applicable
class Neighbors {
 public:
  int n;
  int* neighbor_xs;
  int* neighbor_ys;
  Neighbors() : n(0), neighbor_xs(nullptr), neighbor_ys(nullptr) {}
  Neighbors(int num) : n(num) {
    int* neighbor_xs = new int[n];
    int* neighbor_ys = new int[n];
  }
  ~Neighbors() {
    delete[] neighbor_xs;
    delete[] neighbor_ys;
  }
};
void find_neighbors(Neighbors** neighbors, int dim, int** topo) {
  int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  for (int x = 0; x < dim; x++) {
    for (int y = 0; y < dim; y++) {
      int minElement = topo[x][y];
      vector<int> xs, ys;
      for (int i = 0; i < 4; i++) {
        int new_x = x + directions[i][0];
        int new_y = y + directions[i][1];
        if (new_x >= 0 && new_y >= 0 && new_x < dim && new_y < dim) {
          minElement = min(topo[new_x][new_y], minElement);
        }
      }
      if (minElement >= topo[x][y]) {
        continue;
      }
      for (int i = 0; i < 4; i++) {
        int new_x = x + directions[i][0];
        int new_y = y + directions[i][1];
        if (new_x >= 0 && new_y >= 0 && new_x < dim && new_y < dim) {
          if (topo[new_x][new_y] == minElement) {
            (neighbors[x][y].n)++;
            xs.push_back(new_x);
            ys.push_back(new_y);
          }
        }
      }
      neighbors[x][y].neighbor_xs = new int[neighbors[x][y].n];
      neighbors[x][y].neighbor_ys = new int[neighbors[x][y].n];
      for (int i = 0; i < xs.size(); i++) {
        neighbors[x][y].neighbor_xs[i] = xs[i];
        neighbors[x][y].neighbor_ys[i] = ys[i];
      }
    }
  }
}
void receive_raindrop(float** capacity, int dim) {
  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      capacity[i][j] += 1.0;
    }
  }
}
bool absorb(float** capacity, int dim, float** absorbed, float absorb_rate) {
  bool all_absorbed = true;
  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      float rate = absorb_rate;
      if (capacity[i][j] <= absorb_rate) {
        // if(capacity[i][j]>=0)
        rate = capacity[i][j];
      }
      capacity[i][j] -= rate;
      absorbed[i][j] += rate;

      if (capacity[i][j] > 0.0) {
        all_absorbed = false;
      }
    }
  }
  return all_absorbed;
}
void trickle(float** capacity, Neighbors** neighbors, int dim) {
  vector<vector<float> > trickled(dim, vector<float>(dim, 0.0));
  for (int x = 0; x < dim; x++) {
    for (int y = 0; y < dim; y++) {
      // if no neighbors lower than itself, continue.
      int n = neighbors[x][y].n;
      if (n == 0) {
        continue;
      }
      float trickle_away = 1;
      if (capacity[x][y] <= 1.0) {
        trickle_away = capacity[x][y];
      }
      capacity[x][y] -= trickle_away;

      for (int i = 0; i < n; i++) {
        trickled[neighbors[x][y].neighbor_xs[i]]
                [neighbors[x][y].neighbor_ys[i]] += trickle_away / n;
      }
    }
  }
  // second traversal
  for (int x = 0; x < dim; x++) {
    for (int y = 0; y < dim; y++) {
      capacity[x][y] += trickled[x][y];
    }
  }
}
double calc_time(struct timespec start, struct timespec end) {
  double start_sec = (double)start.tv_sec*1000000000.0 + (double)start.tv_nsec;
  double end_sec = (double)end.tv_sec*1000000000.0 + (double)end.tv_nsec;

  if (end_sec < start_sec) {
    return 0;
  } else {
    return end_sec - start_sec;
  }
}


int main(int argc, char* argv[]) {
  // check inputs
  if (argc != 6) {
    std::cout << "Usage: ./rainfall <P> <M> <A> <N> <elevation_file>"
              << std::endl;
    return 1;
  }
  // parse inputs
  struct timespec start_time, end_time;
  int thread_n = stoi(argv[1]);
  int rain_drop_steps = stoi(argv[2]);
  float absorb_rate = stof(argv[3]);
  
  int dim = stoi(argv[4]);
  // cout<<"args: "<<thread_n<< " "<<rain_drop_steps<< " "<<absorb_rate<< " "<<dim<<endl;
  string elevation_file = argv[5];
  // allocate arrays
  int** topo = new int*[dim];
  float** capacity = new float*[dim];
  float** absorbed = new float*[dim];
  int** trickle_direction = new int*[dim];
  Neighbors** neighbors = new Neighbors*[dim];
  for (int i = 0; i < dim; ++i) {
    topo[i] = new int[dim];
    capacity[i] = new float[dim];
    absorbed[i] = new float[dim];
    trickle_direction[i] = new int[dim];
    neighbors[i] = new Neighbors[dim];
  }
  // read sample file
  ifstream input(elevation_file);
  if (!input.is_open()) {
    cout << "File does not exist!" << endl;
    return 2;
  }
  for (int i = 0; i < dim; ++i) {
    for (int j = 0; j < dim; ++j) {
      input >> topo[i][j];
    }
  }
  input.close();
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  // raindrop simulation
  find_neighbors(neighbors, dim, topo);
  bool all_absorbed = false;
  int step = 0;
  while (!all_absorbed) {
    if (step < rain_drop_steps) {
      receive_raindrop(capacity, dim);
    }
    all_absorbed = absorb(capacity, dim, absorbed, absorb_rate);
    // cout<<step<<endl;
    // for (int i = 0; i < dim; i++) {
    //   for (int j = 0; j < dim; j++) {
    //     cout << capacity[i][j] << " ";
    //   }
    //   cout << endl;
    // }
    trickle(capacity, neighbors, dim);
    step++;
  }
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  // output
  cout<<"it took "<<step<<" steps to complete\n";
  double elapsed_ns = calc_time(start_time, end_time);
  printf("Runtime = %f s\n", elapsed_ns/1000000000);
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      cout << absorbed[i][j] << " ";
    }
    cout << endl;
  }
  // release resources
  for (int i = 0; i < dim; ++i) {
    delete[] topo[i];
    delete[] absorbed[i];
    delete[] capacity[i];
    delete[] trickle_direction[i];
    delete[] neighbors[i];
  }
  delete[] topo;
  delete[] absorbed;
  delete[] capacity;
  delete[] trickle_direction;
  delete[] neighbors;
  return 0;
}