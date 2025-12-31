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

double stop_and_wait_arq(int total_frames, double min_delay, double max_delay, double error_prob, double timeout = 1.0) {
    int seq_num = 0;
    int successful_frames = 0;
    clock_t start_time = clock();

    cout << "\n=== Stop-and-Wait ARQ Simulation (Error Probability = " << fixed << setprecision(2) << error_prob << ") ===\n" << endl;

    for (int frame = 1; frame <= total_frames; frame++) {
        bool ack_received = false;

        while (!ack_received) {
            cout << "Sender: Sending Frame " << frame << " (Seq=" << seq_num << ")" << endl;
            double propagation_delay = random_double(min_delay, max_delay);
            Sleep(static_cast<DWORD>(propagation_delay * 1000));

            if (random_double(0.0, 1.0) < error_prob) {
                cout << "!!! Frame " << frame << " lost during transmission !!!" << endl;
                cout << "Sender: Timeout waiting for ACK... (WAIT STATE)" << endl;
                Sleep(static_cast<DWORD>(timeout * 1000));
                cout << "Sender: Retransmitting frame...\n" << endl;
                continue;
            }

            cout << "Receiver: Frame " << frame << " received successfully." << endl;

            if (random_double(0.0, 1.0) < error_prob) {
                cout << "!!! ACK for Frame " << frame << " lost !!!" << endl;
                cout << "Sender: Timeout waiting for ACK... (WAIT STATE)" << endl;
                Sleep(static_cast<DWORD>(timeout * 1000));
                cout << "Sender: Retransmitting frame...\n" << endl;
                continue;
            }

            cout << "Receiver: Sending ACK " << seq_num << endl;
            Sleep(static_cast<DWORD>(propagation_delay * 1000));
            cout << "Sender: ACK " << seq_num << " received. Sender exits WAIT STATE.\n" << endl;

            ack_received = true;
            successful_frames++;
            seq_num = 1 - seq_num;
        }
    }

    clock_t end_time = clock();
    double total_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    double throughput = successful_frames / total_time;

    cout << "=== Simulation Completed ===" << endl;
    cout << "Total Frames Sent: " << total_frames << endl;
    cout << "Total Successful Frames: " << successful_frames << endl;
    cout << "Total Time: " << fixed << setprecision(2) << total_time << " seconds" << endl;
    cout << "Throughput: " << fixed << setprecision(2) << throughput << " frames/second\n" << endl;

    return throughput;
}

int main() {
    int total_frames = 10;
    double min_delay = 0.3;
    double max_delay = 0.6;
    vector<double> error_probs = {0.0, 0.05, 0.1, 0.2, 0.3};
    vector<double> throughputs;

    cout << "\n--- Performance Experiment: Varying Error Probabilities ---" << endl;

    for (double p : error_probs) {
        double th = stop_and_wait_arq(total_frames, min_delay, max_delay, p);
        throughputs.push_back(th);
    }

    cout << "\nError Probability vs Throughput:\n";
    cout << "Error Prob\tThroughput (frames/sec)" << endl;
    for (size_t i = 0; i < error_probs.size(); i++) {
        cout << fixed << setprecision(2) << error_probs[i] << "\t\t" << throughputs[i] << endl;
    }

    return 0;
}


