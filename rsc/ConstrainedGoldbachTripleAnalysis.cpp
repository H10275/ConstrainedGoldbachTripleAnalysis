#include <iostream>
#include <vector>
#include <unordered_set>
#include <cmath>
#include <iomanip>
#include <algorithm>

using namespace std;

vector<int> generateBasePrimes(int limit) {
    int root = static_cast<int>(sqrt(limit)) + 1;
    vector<bool> isPrime(root + 1, true);
    isPrime[0] = isPrime[1] = false;
    for (int i = 2; i * i <= root; ++i)
        if (isPrime[i])
            for (int j = i * i; j <= root; j += i)
                isPrime[j] = false;
    vector<int> basePrimes;
    for (int i = 2; i <= root; ++i)
        if (isPrime[i]) basePrimes.push_back(i);
    return basePrimes;
}

vector<long long> segmentedSieve(long long low, long long high, const vector<int>& basePrimes) {
    vector<bool> isPrime(high - low + 1, true);
    for (int p : basePrimes) {
        long long start = max(static_cast<long long>(p) * p, ((low + p - 1) / p) * p);
        for (long long j = start; j <= high; j += p)
            isPrime[j - low] = false;
    }
    vector<long long> primes;
    for (long long i = low; i <= high; ++i)
        if (isPrime[i - low] && i >= 2)
            primes.push_back(i);
    return primes;
}

int countRepresentations(long long n, int c, const unordered_set<long long>& primeSet) {
    long long target = n - c;
    int count = 0;
    for (long long p : primeSet) {
        if (p > target / 2) break;
        if (primeSet.count(target - p)) ++count;
    }
    return count;
}

struct Stats {
    long long count = 0;
    long long total = 0;
    double ratioToR5Sum = 0.0;
    long long ratioCount = 0;
};

void updateExclusive(Stats& stats, int rc, int r5) {
    stats.count++;
    stats.total += rc;
    if (r5 > 0) {
        stats.ratioToR5Sum += static_cast<double>(rc) / r5;
        stats.ratioCount++;
    }
}

void updateTotal(Stats& stats, int rc, int r5) {
    if (rc > 0) stats.count++;
    stats.total += rc;
    if (r5 > 0) {
        stats.ratioToR5Sum += static_cast<double>(rc) / r5;
        stats.ratioCount++;
    }
}

void printTable(const string& title, const Stats& s3, const Stats& s5, const Stats& s7, const Stats& s11) {
    cout << "\n--- " << title << " ---\n";
    cout << "| c     | Count | Avg r_c | Avg r_c / r_5 |\n";
    cout << "|-------|--------|----------|----------------|\n";
    auto printRow = [](int c, const Stats& s, double ref) {
        cout << "| " << setw(5) << c
            << " | " << setw(6) << s.count
            << " | " << setw(8) << fixed << setprecision(4) << (s.count > 0 ? static_cast<double>(s.total) / s.count : 0.0)
            << " | " << setw(14) << fixed << setprecision(4) << (s.ratioCount > 0 ? s.ratioToR5Sum / s.ratioCount : 0.0)
            << " |\n";
        };
    printRow(3, s3, s5.total);
    printRow(5, s5, s5.total);
    printRow(7, s7, s5.total);
    printRow(11, s11, s5.total);
}

void analyzeRange(const vector<long long>& numbers, const unordered_set<long long>& primeSet) {
    Stats ex3, ex5, ex7, ex11;
    Stats tot3, tot5, tot7, tot11;

    for (long long n : numbers) {
        int r3 = countRepresentations(n, 3, primeSet);
        int r5 = countRepresentations(n, 5, primeSet);
        int r7 = countRepresentations(n, 7, primeSet);
        int r11 = countRepresentations(n, 11, primeSet);

        // Exclusive first-hit
        if (r3 > 0) updateExclusive(ex3, r3, r5);
        else if (r5 > 0) updateExclusive(ex5, r5, r5);
        else if (r7 > 0) updateExclusive(ex7, r7, r5);
        else if (r11 > 0) updateExclusive(ex11, r11, r5);

        // Total density
        updateTotal(tot3, r3, r5);
        updateTotal(tot5, r5, r5);
        updateTotal(tot7, r7, r5);
        updateTotal(tot11, r11, r5);
    }

    printTable("Exclusive First-Hit Representation Table", ex3, ex5, ex7, ex11);
    printTable("Total Density Representation Table", tot3, tot5, tot7, tot11);
}

int main() {
    int mode;
    cout << "Select analysis mode:\n1 - Development mode (choose one case)\n2 - Final mode (show all cases)\nYour choice: ";
    cin >> mode;

    vector<int> basePrimes = generateBasePrimes(1000000);
    vector<long long> allPrimes = segmentedSieve(9, 1000000, basePrimes);
    unordered_set<long long> primeSet(allPrimes.begin(), allPrimes.end());

    if (mode == 1) {
        int choice;
        cout << "Select number type:\n1 - Odd primes only\n2 - Odd composites only\n3 - All odd numbers\nYour choice: ";
        cin >> choice;

        vector<long long> selected;
        for (long long n = 9; n <= 1000000; n += 2) {
            bool isPrime = primeSet.count(n);
            if ((choice == 1 && isPrime) || (choice == 2 && !isPrime) || (choice == 3)) {
                selected.push_back(n);
            }
        }
        analyzeRange(selected, primeSet);
    }
    else {
        vector<long long> primesOnly, compositesOnly, allNumbers;
        for (long long n = 9; n <= 1000000; n += 2) {
            bool isPrime = primeSet.count(n);
            allNumbers.push_back(n);
            if (isPrime) primesOnly.push_back(n);
            else compositesOnly.push_back(n);
        }

        cout << "\n=== Case: Odd Prime Numbers ===";
        analyzeRange(primesOnly, primeSet);

        cout << "\n=== Case: Odd Composite Numbers ===";
        analyzeRange(compositesOnly, primeSet);

        cout << "\n=== Case: All Odd Numbers ===";
        analyzeRange(allNumbers, primeSet);
    }

    return 0;

}
