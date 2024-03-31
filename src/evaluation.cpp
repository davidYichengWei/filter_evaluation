#include <iostream>
#include <chrono>

#include "filter/bloom/bloom.h"
#include "filter/vector_quotient_filter/vqf_cpp.h"

class Timer {
 public:
  Timer(const std::string &operation_name, int repeatition = 1)
    : start_time_(std::chrono::high_resolution_clock::now()),
      operation_name_(operation_name), repeatition_(repeatition) {}

  ~Timer() {
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start_time_;
    std::cout << operation_name_ << " took " << duration.count()/repeatition_ << " ms" << std::endl;
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::string operation_name_;
  int repeatition_;
};

void testBloomFilter(int num_items) {
  // Test insertions, repeat 10 times, get average
  {
    Timer timer("Bloom filter insertions", 10);
    for (int i = 1; i <= 10; i++) {
      bloomfilter::BloomFilter<int, 10, true> bloom_filter(num_items);
      for (int j = 1; j <= num_items; j++) {
        bloom_filter.Add(j);
      }
    }
  }
}

void testVQFilter(int num_items) {
  // Test insertions, repeat 10 times, get average
  {
    Timer timer("VQ filter insertions", 10);
    for (int i = 1; i <= 10; i++) {
      vqfilter::VQFilter<int> vq_filter(num_items);
      for (int j = 1; j <= num_items; j++) {
        vq_filter.Add(j);
      }
    }
  }
}

int main() {

  testBloomFilter(10000000);
  testVQFilter(10000000);

  return 0;
}