#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>

struct Point {
    long long x;
    long long y;
};

long long distanceSquared(const Point& a, const Point& b) {
    long long dx = a.x - b.x;
    long long dy = a.y - b.y;
    return dx * dx + dy * dy;
}

 long bruteForce(const std::vector<Point>& points, int left, int right) {
    long long best = std::numeric_limits<long long>::max();

    for (int i = left; i < right; i++) {
        for (int j = i + 1; j < right; j++) {
            best = std::min(best, distanceSquared(points[i], points[j]));
        }
    }

    return best;
}

long long closestPairRecursive(std::vector<Point>& points, int left, int right) {
    int n = right - left;

    if (n <= 3) {
        return bruteForce(points, left, right);
    }

    int mid = left + n / 2;
    long long midX = points[mid].x;

    long long bestLeft = closestPairRecursive(points, left, mid);
    long long bestRight = closestPairRecursive(points, mid, right);

    long long best = std::min(bestLeft, bestRight);

    std::vector<Point> strip;

    for (int i = left; i < right; i++) {
        long long dx = points[i].x - midX;
        if (dx * dx < best) {
            strip.push_back(points[i]);
        }
    }

    std::sort(strip.begin(), strip.end(), [](const Point& a, const Point& b) {
        return a.y < b.y;
    });

    for (int i = 0; i < strip.size(); i++) {
        for (int j = i + 1; j < strip.size(); j++) {
            long long dy = strip[j].y - strip[i].y;

            if (dy * dy >= best) {
                break;
            }

            best = std::min(best, distanceSquared(strip[i], strip[j]));
        }
    }

    return best;
}

int main() {
    int n;
    std::cin >> n;

    std::vector<Point> points(n);

    for (int i = 0; i < n; i++) {
        std::cin >> points[i].x >> points[i].y;
    }

    std::sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        if (a.x == b.x) {
            return a.y < b.y;
        }
        return a.x < b.x;
    });

    long long answerSquared = closestPairRecursive(points, 0, n);
    double answer = std::sqrt(static_cast<double>(answerSquared));

    std::cout << std::fixed << std::setprecision(6) << answer << "\n";

    return 0;
}