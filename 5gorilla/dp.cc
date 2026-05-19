#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

const int GAP = -4;

int main() {
    string line;

    // Read characters from first line
    getline(cin, line);

    vector<char> chars;
    for (char c : line) {
        if (c != ' ') {
            chars.push_back(c);
        }
    }

    int k = chars.size();

    unordered_map<char, int> index;
    for (int i = 0; i < k; i++) {
        index[chars[i]] = i;
    }

    // Read scoring matrix
    vector<vector<int>> score(k, vector<int>(k));

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            cin >> score[i][j];
        }
    }

    int Q;
    cin >> Q;

    while (Q--) {
        string a, b;
        cin >> a >> b;

        int n = a.size();
        int m = b.size();

        vector<vector<int>> dp(n + 1, vector<int>(m + 1));

        // Base cases:
        // aligning a prefix with an empty string means gaps only
        for (int i = 0; i <= n; i++) {
            dp[i][0] = i * GAP;
        }

        for (int j = 0; j <= m; j++) {
            dp[0][j] = j * GAP;
        }

        // Fill DP table
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                int alignLetters =
                    dp[i - 1][j - 1] + score[index[a[i - 1]]][index[b[j - 1]]];

                int gapInB =
                    dp[i - 1][j] + GAP;

                int gapInA =
                    dp[i][j - 1] + GAP;

                dp[i][j] = max({alignLetters, gapInB, gapInA});
            }
        }

        // Backtrack to reconstruct alignment
        string alignedA;
        string alignedB;

        int i = n;
        int j = m;

        while (i > 0 || j > 0) {
            if (i > 0 && j > 0) {
                int alignLetters =
                    dp[i - 1][j - 1] + score[index[a[i - 1]]][index[b[j - 1]]];

                if (dp[i][j] == alignLetters) {
                    alignedA.push_back(a[i - 1]);
                    alignedB.push_back(b[j - 1]);
                    i--;
                    j--;
                    continue;
                }
            }

            if (i > 0) {
                int gapInB = dp[i - 1][j] + GAP;

                if (dp[i][j] == gapInB) {
                    alignedA.push_back(a[i - 1]);
                    alignedB.push_back('*');
                    i--;
                    continue;
                }
            }

            if (j > 0) {
                alignedA.push_back('*');
                alignedB.push_back(b[j - 1]);
                j--;
            }
        }

        // Backtracking creates the strings backwards
        reverse(alignedA.begin(), alignedA.end());
        reverse(alignedB.begin(), alignedB.end());

        cout << alignedA << " " << alignedB << '\n';
    }

    return 0;
}