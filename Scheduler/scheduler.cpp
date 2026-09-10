#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

const int N = 10000000;

int *a = new int[N];
int *b = new int[N];
int *c = new int[N];

void worker(int start, int end) {

    for (int i = start; i < end; i++) {
        c[i] = a[i] + b[i];
    }
}

int main() {

    for (int i = 0; i < N; i++) {
        a[i] = i;
        b[i] = i * 2;
    }

    auto start = chrono::high_resolution_clock::now();

    thread t1(worker, 0, N);
   /* thread t2(worker, N /4, N/2);
    thread t3(worker,N/2,(3*N)/4);
    thread t4(worker,(3*N)/4,N);*/

    t1.join();
    //t2.join();
    //t3.join();
    //t4.join();

    auto end = chrono::high_resolution_clock::now();

    auto time = chrono::duration_cast<chrono::microseconds>(
        end - start
    );

    cout << "Time: " << time.count() << " microseconds\n";

    cout << c[0] << endl;
    cout << c[N / 4] << endl;
    cout << c[(3*N) / 4] << endl;
    cout << c[N / 2] << endl;
    cout << c[N - 1] << endl;

    delete[] a;
    delete[] b;
    delete[] c;

    return 0;
}