#include <random>

std::random_device rd;
std::mt19937_64 mt(rd());
std::uniform_int_distribution<uint_fast64_t> distrib(0, (static_cast<uint_fast64_t>(1) << 63)-1);

unsigned long rand64() {
    return distrib(mt);
}