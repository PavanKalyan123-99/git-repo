#include <iostream>
#include <windows.h>
#include <random>
#include <vector>
#include <ctime>
#include <iomanip>

using namespace std;

double random_double(double min, double max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

double go_back_n_arq(int total_frames, int window_size, double error_prob, double min_delay=0.2, double max_delay=0.5, double timeout=1.0) {
    int base = 0;
    int next_seq = 0;
    int successful_frames = 0;
    clock_t start_time = clock();

    cout << "\n=== Go-Back-N ARQ Simulation (Window=" << window_size << ", Error Prob=" << fixed << setprecision(2) << error_prob << ") ===\n" << endl;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> delay_dis(min_delay, max_delay);
    uniform_real_distribution<> error_dis(0.0, 1.0);

    while (base < total_frames) {
        while (next_seq < base + window_size && next_seq < total_frames) {
            cout << "Sender: Sending Frame " << next_seq << endl;
            Sleep(static_cast<DWORD>(delay_dis(gen) * 1000));

            if (error_dis(gen) < error_prob) {
                cout << "!!! Frame " << next_seq << " lost during transmission !!!" << endl;
            } else {
                cout << "Receiver: Frame " << next_seq << " received successfully." << endl;
            }
            next_seq++;
        }

        if (error_dis(gen) < error_prob) {
            cout << "!!! ACK lost, sender will timeout and resend window !!!" << endl;
            Sleep(static_cast<DWORD>(timeout * 1000));
            cout << "Sender: Timeout occurred --> Resending all frames from Base = " << base << "\n" << endl;
            next_seq = base;
            continue;
        }

        uniform_int_distribution<> ack_dis(base, next_seq - 1);
        int ack_num = ack_dis(gen);
        cout << "Receiver: Sending ACK for Frame " << ack_num << endl;
        Sleep(static_cast<DWORD>(delay_dis(gen) * 1000));

        base = ack_num + 1;
        successful_frames = base;
        cout << "Sender: ACK " << ack_num << " received --> Sliding window. New base = " << base << "\n" << endl;
    }

    clock_t end_time = clock();
    double total_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    double throughput = successful_frames / total_time;

    cout << "=== Simulation Completed ===" << endl;
    cout << "Total Frames: " << total_frames << endl;
    cout << "Successful Frames: " << successful_frames << endl;
    cout << "Total Time: " << fixed << setprecision(2) << total_time << " seconds" << endl;
    cout << "Throughput: " << fixed << setprecision(2) << throughput << " frames/second\n" << endl;

    return throughput;
}

int main() {
    int total_frames = 5;
    vector<double> error_probs = {0.0, 0.1, 0.2};
    vector<int> window_sizes = {3, 5};
    vector<vector<double>> results;

    cout << "\n--- Go-Back-N ARQ Throughput Experiment (5 Frames) ---" << endl;

    for (int w : window_sizes) {
        vector<double> throughputs;
        for (double p : error_probs) {
            double th = go_back_n_arq(total_frames, w, p);
            throughputs.push_back(th);
        }
        results.push_back(throughputs);
    }

    cout << "\n=== Summary Table (Throughput in frames/sec) ===" << endl;
    cout << "Error Probability --> ";
    for (double p : error_probs) cout << fixed << setprecision(2) << p << " ";
    cout << endl;
    for (size_t i = 0; i < window_sizes.size(); i++) {
        cout << "Window " << window_sizes[i] << ": ";
        for (double t : results[i]) cout << fixed << setprecision(2) << t << " ";
        cout << endl;
    }

    return 0;
}

