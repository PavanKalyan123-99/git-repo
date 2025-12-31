#include <iostream>
#include <windows.h>
#include <random>
#include <ctime>

using namespace std;
double random_double(double min, double max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

void stop_and_wait(int total_frames, double min_delay, double max_delay) {
    int sequence = 0;
    int acknowledged_frames = 0;
    clock_t start_time = clock();

    for (int frame = 1; frame <= total_frames; frame++) {
        cout << "Sender: Sending Frame " << frame << " (Seq=" << sequence << ")" << endl;
        double delay = random_double(min_delay, max_delay);
        Sleep(static_cast<DWORD>(delay * 1000));

        bool lost = random_double(0.0, 1.0) < 0.1;

        if (lost) {
            cout << "Receiver: Frame " << frame << " ACK not received!" << endl;
            cout << "Sender: Timeout! Waiting... (WAIT STATE)" << endl;
            Sleep(1000);
            cout << "Sender: Resending the frame\n" << endl;
            frame--;
            continue;
        }

        cout << "Receiver: Frame " << frame << " received successfully." << endl;
        cout << "Receiver: Sending ACK " << sequence << endl;
        Sleep(static_cast<DWORD>(delay * 1000));
        cout << "Sender: ACK " << sequence << " received. Sender exits WAIT STATE.\n" << endl;

        sequence = 1 - sequence;
        acknowledged_frames++;
    }

    clock_t end_time = clock();
    double total_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    double throughput = acknowledged_frames / total_time;

    cout << "=== Simulation Completed ===" << endl;
    cout << "Total Frames Sent: " << total_frames << endl;
    cout << "Total Acknowledged Frames: " << acknowledged_frames << endl;
    cout << "Total Time Taken: " << total_time << " seconds" << endl;
    cout << "Throughput: " << throughput << " frames/second\n" << endl;
}

int main() {
    cout << "\n--- Case 1: Low Propagation Delay (0.2 to 0.5s) ---" << endl;
    stop_and_wait(5, 0.2, 0.5);

    cout << "\n--- Case 2: Medium Propagation Delay (0.5 to 1.0s) ---" << endl;
    stop_and_wait(5, 0.5, 1.0);

    cout << "\n--- Case 3: High Propagation Delay (1.0 to 2.0s) ---" << endl;
    stop_and_wait(5, 1.0, 2.0);

    return 0;
}





