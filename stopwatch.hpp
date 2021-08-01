#include <chrono>

template <typename T = std::chrono::milliseconds>
class stopwatch {
public:
    explicit stopwatch() : start_time_(std::chrono::high_resolution_clock::now())
    {
    }

    ~stopwatch() {
        std::cout << std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - start_time_);
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
};

// usage:
int main() {
    {
        stopwatch<std::chrono::microseconds> sw;
        // do stuff
    }
    std::cout << " microseconds\n";
    return 0;
}
