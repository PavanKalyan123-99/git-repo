#include <iostream>
#include <vector>
#include <chrono>
#include <windows.h>
#include <random>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace std::chrono;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0);
uniform_real_distribution<> delay_dis(0.2, 0.5);

double selective_repeat_arq(int totalFrames, int windowSize, double errorProb, double timeout = 1.0) {
    vector<double> sentTime(totalFrames, 0.0);
    vector<bool> ackReceived(totalFrames, false);
    int base = 0, nextSeq = 0, successfulFrames = 0;

    auto startTime = high_resolution_clock::now();

    cout << "\n=== Selective Repeat ARQ Simulation (Window=" << windowSize
         << ", Error Prob=" << errorProb << ") ===\n\n";

    while (successfulFrames < totalFrames) {
        while (nextSeq < base + windowSize && nextSeq < totalFrames) {
            if (!ackReceived[nextSeq]) {
                cout << "Sender: Sending Frame " << nextSeq << endl;
                Sleep(int(delay_dis(gen) * 1000));
                sentTime[nextSeq] = duration<double>(high_resolution_clock::now() - startTime).count();

                if (dis(gen) < errorProb) {
                    cout << "!!! Frame " << nextSeq << " lost during transmission !!!\n";
                } else {
                    cout << "Receiver: Frame " << nextSeq << " received successfully.\n";
                    if (dis(gen) < errorProb) {
                        cout << "!!! ACK for Frame " << nextSeq << " lost !!!\n";
                    } else {
                        cout << "Receiver: Sending ACK for Frame " << nextSeq << endl;
                        ackReceived[nextSeq] = true;
                        successfulFrames++;
                        cout << "Sender: ACK for Frame " << nextSeq << " received.\n";
                    }
                }
            }
            nextSeq++;
        }

        bool timedOut = false;
        auto currentTime = high_resolution_clock::now();
        for (int i = base; i < min(base + windowSize, totalFrames); i++) {
            if (!ackReceived[i] && sentTime[i] != 0 &&
                duration<double>(currentTime - startTime).count() - sentTime[i] > timeout) {
                cout << "Timeout for Frame " << i << " --> Retransmitting...\n";
                nextSeq = i;
                timedOut = true;
                break;
            }
        }
        if (timedOut) continue;

        while (base < totalFrames && ackReceived[base]) base++;
    }

    auto endTime = high_resolution_clock::now();
    double totalTime = duration<double>(endTime - startTime).count();
    double throughput = successfulFrames / totalTime;

    cout << "=== Simulation Completed ===\n";
    cout << "Total Frames: " << totalFrames << "\n";
    cout << "Successful Frames: " << successfulFrames << "\n";
    cout << "Total Time: " << fixed << setprecision(2) << totalTime << " seconds\n";
    cout << "Throughput: " << fixed << setprecision(2) << throughput << " frames/second\n\n";

    return throughput;
}

double go_back_n_arq(int totalFrames, int windowSize, double errorProb, double timeout = 1.0) {
    int base = 0, nextSeq = 0, successfulFrames = 0;
    auto startTime = high_resolution_clock::now();

    while (base < totalFrames) {
        while (nextSeq < base + windowSize && nextSeq < totalFrames) {
            Sleep(int(delay_dis(gen) * 1000));
            if (dis(gen) >= errorProb) successfulFrames++;
            nextSeq++;
        }

        if (dis(gen) < errorProb) {
            Sleep(int(timeout * 1000));
            nextSeq = base;
            continue;
        }

        int ackNum = base + rand() % (nextSeq - base);
        base = ackNum + 1;
    }

    auto endTime = high_resolution_clock::now();
    return successfulFrames / duration<double>(endTime - startTime).count();
}

int main() {
    vector<double> errorProbs = {0.0, 0.1, 0.2};
    vector<int> windowSizes = {3, 5};
    int totalFrames = 5;

    cout << "\n--- Comparison: Go-Back-N vs Selective Repeat (5 Frames) ---\n";

    for (int w : windowSizes) {
        vector<double> srThroughput;
        vector<double> gbnThroughput;
        for (double p : errorProbs) {
            cout << "\nTesting Window=" << w << ", Error Prob=" << p << "\n";
            srThroughput.push_back(selective_repeat_arq(totalFrames, w, p));
            gbnThroughput.push_back(go_back_n_arq(totalFrames, w, p));
        }

        cout << "\nWindow = " << w << "\nSelective Repeat: ";
        for (double t : srThroughput) cout << fixed << setprecision(2) << t << " ";
        cout << "\nGo-Back-N:       ";
        for (double t : gbnThroughput) cout << fixed << setprecision(2) << t << " ";
        cout << "\n";
    }

    return 0;
}



