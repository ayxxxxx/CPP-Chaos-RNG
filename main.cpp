#include <iostream>
#include <cstdint>
#include <vector>
#include <sstream>
#include <string>
#include <chrono>
#include <random> 

inline uint64_t rotateLeft(uint64_t x, int r) noexcept {
    return (x << r) | (x >> (64 - r));
}

uint64_t chaotic_noise(uint64_t seed, uint64_t index) noexcept {
    uint64_t x = seed ^ 0xA5A5A5A5A5A5A5A5ULL;
    x ^= index * 0x27BB2EE687B0B0FDULL;
    x = rotateLeft(x, 17);
    x *= 0xC6BC279692B5C323ULL;
    x ^= x >> 33;
    x *= 0x9E3779B97F4A7C15ULL;
    x ^= x >> 29;

    x ^= rotateLeft(x, 13);
    x ^= x >> 7;
    x *= 0x2545F4914F6CDD1DULL;
    x ^= x >> 32;
    return x;
}

int custom_random(uint64_t seed, uint64_t index, int min, int max) noexcept {
    return static_cast<int>(min + (chaotic_noise(seed, index) % (max - min + 1)));
}

std::string colorize(const std::string& text, const std::string& color) {
    const std::string RESET = "\033[0m";
    if (color == "red")    return "\033[31m" + text + RESET;
    if (color == "green")  return "\033[32m" + text + RESET;
    if (color == "yellow") return "\033[33m" + text + RESET;
    if (color == "blue")   return "\033[34m" + text + RESET;
    if (color == "magenta")return "\033[35m" + text + RESET;
    if (color == "cyan")   return "\033[36m" + text + RESET;
    return text;
}

int main() {
    std::string seedInput;
    uint64_t seed;

    std::cout << colorize("seed (leeg voor random): ", "cyan");
    std::getline(std::cin, seedInput);

    if (seedInput.empty()) {
        std::random_device rd;
        seed = static_cast<uint64_t>(rd()) << 32 | rd();
        std::cout << colorize("Using random seed: " + std::to_string(seed), "yellow") << "\n";
    }
    else {
        try {
            seed = std::stoull(seedInput);
        }
        catch (...) {
            std::cout << colorize("[X] geen geldige seed, random in plaats.", "red") << "\n";
            std::random_device rd;
            seed = static_cast<uint64_t>(rd()) << 32 | rd();
        }
    }

    int range, amount;
    std::cout << colorize("range: ", "cyan");
    std::cin >> range;
    std::cout << colorize("amount: ", "cyan");
    std::cin >> amount;

    std::cin.ignore();

    auto gen_start = std::chrono::high_resolution_clock::now();

    std::vector<int> numbers;
    numbers.reserve(amount);
    std::ostringstream output;

    for (int i = 0; i < amount; ++i) {
        const int num = custom_random(seed, i, 0, range);
        numbers.emplace_back(num);
        output << colorize(std::to_string(num), "green") << ((i + 1) % 6 ? "\t" : "\n");
    }

    auto gen_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> gen_duration = gen_end - gen_start;

    std::cout << output.str()
        << colorize("\n\genereren duurt: ", "yellow")
        << colorize(std::to_string(std::round(gen_duration.count() * 100) / 100), "green")
        << colorize(" seconden\n", "yellow")
        << colorize("Tellen hoe vaak ze voorkomen? (y/n): ", "yellow");

    char choice;
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        auto count_start = std::chrono::high_resolution_clock::now();

        std::vector<int> counts(range + 1);
        for (int num : numbers) ++counts[num];

        auto count_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> count_duration = count_end - count_start;

        std::cout << colorize("\Nummer\tTelling", "magenta") << "\n";
        for (int i = 0; i <= range; ++i) {
            if (counts[i]) std::cout << colorize(std::to_string(i), "blue") << "\t"
                << colorize(std::to_string(counts[i]), "green") << "\n";
        }

        std::cout << colorize("\Tellen duurde: ", "yellow")
            << colorize(std::to_string(std::round(count_duration.count() * 100) / 100), "green")
            << colorize(" seconden\n", "yellow");
    }
}
