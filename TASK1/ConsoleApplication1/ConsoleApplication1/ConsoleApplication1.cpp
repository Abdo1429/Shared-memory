/*
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <condition_variable>

using namespace std;

struct DataItem {
    string str;
    int integer;
    double dbl;
};

class DataProcessor {
public:
    DataProcessor(int num_threads) : num_threads(num_threads) {}

    void Start() {
        // Create and start worker threads
        vector<thread> workerThreads;
        for (int i = 0; i < num_threads; ++i) {
            workerThreads.emplace_back(&DataProcessor::WorkerFunction, this, i);
        }

        // Read data from the data file and insert into the data vector
        ifstream dataFile("IFU_AhmedA_L1_dat_1.txt");
        DataItem item;

        while (dataFile >> item.str >> item.integer >> item.dbl) {
            unique_lock<mutex> lock(dataMutex);
            data.push_back(item);
        }

        // Wait for all worker threads to finish
        for (auto& thread : workerThreads) {
            thread.join();
        }

        // Filter and process data, then write results to the output file
        ProcessDataAndWriteResults();
    }

private:
    int num_threads;
    vector<DataItem> data;
    mutex dataMutex;

    void WorkerFunction(int worker_id) {
        // Worker threads process data here
        for (DataItem& item : data) {
            item.integer += 1;
            if (item.integer % 2 == 0) {
                // Process the item and insert it into results
                ProcessItem(item);
            }
        }
    }

    void ProcessItem(const DataItem& item) {
        // Process the item here
    }

    void ProcessDataAndWriteResults() {
        ofstream resultFile("IFU_AhmedA_L1_rez.txt");
/*        if (!resultFile.is_open()) {
            cerr << "Error: Unable to open result file." << endl;
            return;
        }
        *//*
        resultFile << "String\tInteger\tDouble\n";
        for (const DataItem& result : data) {
            if (result.integer % 2 == 0) {
                resultFile << result.str << '\t' << result.integer << '\t' << result.dbl << '\n';
            }
        }
    }
};

int main() {
    const int num_threads = 4;

    DataProcessor dataProcessor(num_threads);
    dataProcessor.Start();

    return 0;

}
*/
/*--------------------------------------------------------------------------------------------------------------------------------------------*/


#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>

using namespace std;

struct DataItem {
    string str;
    int integer;
    double dbl;
};

class DataMonitor {
public:

    DataMonitor(int max_size) : max_size(max_size) {}

    void Insert(const DataItem& item) {
        unique_lock<mutex> lock(mutex_);
        cv_full.wait(lock, [this] { return data_.size() < max_size; });
        data_.push_back(item);
        cv_empty.notify_all();
    }

    DataItem Remove() {
        unique_lock<mutex> lock(mutex_);
        cv_empty.wait(lock, [this] { return !data_.empty(); });// check if m
        DataItem item = data_.back();
        data_.pop_back();
        cv_full.notify_all();
        return item;
    }


private:
    int max_size;
    vector<DataItem> data_;
    mutex mutex_;
    condition_variable cv_empty;
    condition_variable cv_full;
};

class ResultMonitor {
public:
    void InsertSorted(const DataItem& result) {
        unique_lock<mutex> lock(mutex_);
        results_.insert(lower_bound(results_.begin(), results_.end(), result,
            [](const DataItem& a, const DataItem& b) {
                return a.integer < b.integer;
            }), result);
    }

    vector<DataItem> GetResults() {
        unique_lock<mutex> lock(mutex_);
        vector<DataItem> resultsCopy = results_;
        return resultsCopy;
    }

private:
    vector<DataItem> results_;
    mutex mutex_;
};

void WorkerFunction(DataMonitor& dataMonitor, ResultMonitor& resultMonitor) {
    while (true) {
        DataItem item = dataMonitor.Remove();

        if (item.str.empty()) {
            break; // Terminate the worker thread when encountering the termination item
        }

        // Simulate a non-trivial computation
        item.integer += 1;

        if (item.integer % 2 == 0) {
            resultMonitor.InsertSorted(item);
        }
    }
}

int main() {
    const int max_data_monitor_size = 25;
    const int num_worker_threads = 4;

    DataMonitor dataMonitor(max_data_monitor_size);
    ResultMonitor resultMonitor;

    ifstream dataFile("IFU_AhmedA_L1_dat_3.txt");
    DataItem item;

    if (!dataFile.is_open()) {
        cerr << "Error: Unable to open data file." << endl;
        return 1;
    }

    int dataItemCount = 0; // Track the number of data items read

    while (dataFile >> item.str >> item.integer >> item.dbl) {
        dataMonitor.Insert(item);
        dataItemCount++;
    }

    // Insert a termination item for each worker thread
    for (int i = 0; i < num_worker_threads; ++i) {
        DataItem terminationItem;
        terminationItem.str = ""; // Set an empty string as a termination signal
        dataMonitor.Insert(terminationItem);
    }

    vector<thread> workerThreads;
    for (int i = 0; i < num_worker_threads; ++i) {
        workerThreads.emplace_back(WorkerFunction, ref(dataMonitor), ref(resultMonitor));
    }

    for (auto& thread : workerThreads) {
        thread.join();
    }

    vector<DataItem> results = resultMonitor.GetResults();

    ofstream resultFile("IFU_AhmedA_L1_rez.txt");
    resultFile << "String\tInteger\tDouble\n";
    for (const DataItem& result : results) {
        resultFile << result.str << '\t' << result.integer << '\t' << result.dbl << '\n';
    }

    cout << "Processed " << dataItemCount << " data items." << endl;

    return 0;
}

 