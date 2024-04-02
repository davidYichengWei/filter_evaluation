#include <iostream>
#include <chrono>
#include <pqxx/pqxx>

#include "filter/bloom/bloom.h"
#include "filter/vector_quotient_filter/vqf_cpp.h"

std::string conn_str = "dbname=test user=ubuntu hostaddr=127.0.0.1 port=5432 password=newpassword";

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

void testBloomFilterThreshold(int num_items) {
  // Test insertions, repeat 10 times, get average
  {
    Timer timer("Bloom filter insertions", 5);
    for (int i = 1; i <= 5; i++) {
      bloomfilter::BloomFilter<int, 10, true> bloom_filter(num_items);
      for (int j = 1; j <= num_items; j++) {
        bloom_filter.Add(j);
      }
    }
  }

  bloomfilter::BloomFilter<int, 10, true> bloom_filter(num_items);
  std::cout << bloom_filter.Info() << std::endl;
  for (int i = 1; i <= num_items; i++) {
    bloom_filter.Add(i);
  }

  // Connect to the test database using pqxx
  try {
    pqxx::connection conn(conn_str);
    pqxx::nontransaction ntxn(conn);

    for (double neg_percent = 0; neg_percent <= 20; neg_percent += 0.5) {
      std::cout << "negative query: " << neg_percent << "%" << std::endl;
      int negative_query_count = 100000 * neg_percent / 100;
      int positive_query_count = 100000 - negative_query_count;
      // std::cout << "positive query count: " << positive_query_count << std::endl;
      // std::cout << "negative query count: " << negative_query_count << std::endl;
      // no filter
      {
        Timer timer("no bloom filter, negative query: " + std::to_string(neg_percent) + "%", 10);
        for (int k = 0; k < 10; k++) {
          for (int i = 1; i <= positive_query_count; i++) {
            pqxx::result res = ntxn.exec("SELECT * FROM t1 WHERE id = " + std::to_string(i));
          }
          for (int i = 1; i <= negative_query_count; i++) {
            int j = num_items + i;
            pqxx::result res = ntxn.exec("SELECT * FROM t1 WHERE id = " + std::to_string(j));
          }
        }

      }

      // with filter
      {
        Timer timer("bloom filter, negative query: " + std::to_string(neg_percent) + "%", 10);
        for (int k = 0; k < 10; k++) {
          for (int i = 1; i <= positive_query_count; i++) {
            if (bloom_filter.Contain(i) == bloomfilter::Status::NotFound) continue;
            pqxx::result res = ntxn.exec("SELECT * FROM t1 WHERE id = " + std::to_string(i));
          }
          // int skip_count = 0;
          for (int i = 1; i <= negative_query_count; i++) {
            int j = num_items + i;
            if (bloom_filter.Contain(j) == bloomfilter::Status::NotFound) {
              // skip_count++;
              continue;
            }
            pqxx::result res = ntxn.exec("SELECT * FROM t1 WHERE id = " + std::to_string(j));
          }
          // std::cout << "skip count: " << skip_count << std::endl;
        }
      }
    }

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

}

void testVQFilterThreshold(int num_items) {
  // Test insertions, repeat 10 times, get average
  {
    Timer timer("VQF insertions", 5);
    for (int i = 1; i <= 5; i++) {
      vqfilter::VQFilter<int> vq_filter(num_items);
      for (int j = 1; j <= num_items; j++) {
        vq_filter.Add(j);
      }
    }
  }

  vqfilter::VQFilter<int> vq_filter(num_items);
  std::cout << vq_filter.Info() << std::endl;
  for (int i = 1; i <= num_items; i++) {
    vq_filter.Add(i);
  }

  // Connect to the test database using pqxx
  try {
    pqxx::connection conn(conn_str);
    pqxx::nontransaction ntxn(conn);

    // repeat for 0% to 10% negative query
    for (double neg_percent = 0; neg_percent <= 20; neg_percent += 0.5) {
      std::cout << "negative query: " << neg_percent << "%" << std::endl;
      int negative_query_count = 100000 * neg_percent / 100;
      int positive_query_count = 100000 - negative_query_count;
      // std::cout << "positive query count: " << positive_query_count << std::endl;
      // std::cout << "negative query count: " << negative_query_count << std::endl;
      // no filter
      {
        Timer timer("no vq filter, negative query: " + std::to_string(neg_percent) + "%", 10);
        for (int k = 0; k < 10; k++) {
          for (int i = 1; i <= positive_query_count; i++) {
            pqxx::result res = ntxn.exec("SELECT * FROM t1 WHERE id = " + std::to_string(i));
          }
          for (int i = 1; i <= negative_query_count; i++) {
            int j = num_items + i;
            pqxx::result res = ntxn.exec("SELECT * FROM t1 WHERE id = " + std::to_string(j));
          }
        }

      }

      // with filter
      {
        Timer timer("vq filter, negative query: " + std::to_string(neg_percent) + "%", 10);
        for (int k = 0; k < 10; k++) {
          for (int i = 1; i <= positive_query_count; i++) {
            if (vq_filter.Contain(i) == vqfilter::Status::NotFound) continue;
            pqxx::result res = ntxn.exec("SELECT * FROM t1 WHERE id = " + std::to_string(i));
          }
          // int skip_count = 0;
          for (int i = 1; i <= negative_query_count; i++) {
            int j = num_items + i;
            if (vq_filter.Contain(j) == vqfilter::Status::NotFound) {
              // skip_count++;
              continue;
            }
            pqxx::result res = ntxn.exec("SELECT * FROM t1 WHERE id = " + std::to_string(j));
          }
          // std::cout << "skip count: " << skip_count << std::endl;
        }
      }
    }

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

}

void testBloomQueryPerformanceAtLoadFactor(int num_items, double load_factor) {
  std::cout << "Testing Bloom filter query performance at load factor " << load_factor << std::endl;
  bloomfilter::BloomFilter<int, 2, true> bloom_filter(num_items);

  int num_item_to_insert = num_items * load_factor;
  for (int i = 1; i <= num_item_to_insert; i++) {
    bloom_filter.Add(i);
  }

  // Test query performance
  {
    Timer timer("Query", 10);
    for (int k = 0; k < 10; k++) {
      for (int i = 1; i <= num_items; i++) {
        bloom_filter.Contain(i);
      }
    }
  }

  // Test false positive rate
  int false_positive_count = 0;
  for (int i = 0; i < num_item_to_insert; i++) {
    int j = num_item_to_insert + i;
    if (bloom_filter.Contain(j) == bloomfilter::Status::Ok)
      false_positive_count++;
  }
  // calculate false positive rate
  double false_positive_rate = (double)false_positive_count / num_item_to_insert;
  std::cout << "False positive rate: " << false_positive_rate << std::endl;

  // Test insert
  int new_insert_count = num_items * 0.1;
  {
    Timer timer("Insert");
    for (int i = num_item_to_insert + 1; i <= num_item_to_insert + new_insert_count; i++) {
      bloom_filter.Add(i);
    }
  }
}

void testVQFilterPerformanceAtLoadFactor(int num_items, double load_factor) {
  std::cout << "Testing VQF query performance at load factor " << load_factor << std::endl;
  vqfilter::VQFilter<int> vq_filter(num_items);

  int num_item_to_insert = num_items * load_factor;
  for (int i = 1; i <= num_item_to_insert; i++) {
    vq_filter.Add(i);
  }

  // Test query performance
  {
    Timer timer("Query", 10);
    for (int k = 0; k < 10; k++) {
      for (int i = 1; i <= num_items; i++) {
        vq_filter.Contain(i);
      }
    }
  }

  // Test false positive rate
  int false_positive_count = 0;
  for (int i = 0; i < num_item_to_insert; i++) {
    int j = num_item_to_insert + i;
    if (vq_filter.Contain(j) == vqfilter::Status::Ok)
      false_positive_count++;
  }
  // calculate false positive rate
  double false_positive_rate = (double)false_positive_count / num_item_to_insert;
  std::cout << "False positive rate: " << false_positive_rate << std::endl;

  // Test insert
  int new_insert_count = num_items * 0.1;
  {
    Timer timer("Insert");
    for (int i = num_item_to_insert + 1; i <= num_item_to_insert + new_insert_count; i++) {
      vq_filter.Add(i);
    }
  }
}

int main() {

  // testVQFilterThreshold(10000000);
  testBloomFilterThreshold(10000000);

  // for (double i = 0.1; i <= 2; i += 0.1) {
  //   // testVQFilterPerformanceAtLoadFactor(10000000, i);
  //   testBloomQueryPerformanceAtLoadFactor(10000000, i);
  // }


  return 0;
}