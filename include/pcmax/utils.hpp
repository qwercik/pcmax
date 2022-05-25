#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <functional>
#include <chrono>

double logarithm(double value, double base) {
	return std::log(value) / std::log(base);
}

template <typename T>
void print_vec(const std::vector<T>& vec) {
	for (const auto& e : vec) {
		std::cout << e << " ";
	}
	std::cout << "\n";
}

double measureTime(std::function<void()> func) {
	auto start = std::chrono::high_resolution_clock::now();
    func();
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> diff = end - start;
	double total_time = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / std::pow(10, 9);
    return total_time;
}

