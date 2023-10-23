#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <sstream>

using namespace std;

struct DataItem {
    string str;
    int integer;
    double dbl;
    string computedValue;  // additional computed field
};

void ProcessData(const vector<DataItem>& data, int start, int end, vector<DataItem>& results) {
    for (int i = start; i < end; ++i) {
        DataItem item = data[i];

        // Simulate a non-trivial computation
        item.integer += 0;

        // Additional computation
        item.computedValue = item.str + std::to_string(item.integer);

        if (item.integer % 2 == 0) {
            results[i] = item;
        }
    }
}

int main() {
    const int num_worker_threads = 4;
    omp_set_num_threads(num_worker_threads);

    ifstream dataFile("IFU_AhmedA_L1_dat_1.txt");
    DataItem item;
    vector<DataItem> data;

    if (!dataFile.is_open()) {
        cerr << "Error: Unable to open data file." << endl;
        return 1;
    }

    while (dataFile >> item.str >> item.integer >> item.dbl) {
        data.push_back(item);
    }

    int dataSize = data.size();
    int chunkSize = dataSize / num_worker_threads;
    int remainder = dataSize % num_worker_threads;

    vector<DataItem> results(dataSize); // initialize with input size

#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int start = thread_id * chunkSize + min(thread_id, remainder);
        int end = start + chunkSize + (thread_id < remainder);

        ProcessData(data, start, end, results);
    }

    // Compute the sums after processing the data
    int totalIntSum = 0;
    double totalDblSum = 0.0;

    for (const DataItem& result : results) {
        if (!result.computedValue.empty()) {
            totalIntSum += result.integer;
            totalDblSum += result.dbl;
        }
    }

    ofstream resultFile("IFU_AhmedA_L1_rez.txt");
    resultFile << "String\tInteger\tDouble\tComputedValue\n";  // Added ComputedValue header
    for (const DataItem& result : results) {
        if (!result.computedValue.empty()) {
            resultFile << result.str << '\t' << result.integer << '\t' << result.dbl << '\t' << result.computedValue << '\n'; // Output the computed value
        }
    }
    resultFile << "Sum of Integers: " << totalIntSum << "\n";
    resultFile << "Sum of Doubles: " << totalDblSum << "\n";

    cout << "Processed " << dataSize << " data items." << endl;

    return 0;
}
