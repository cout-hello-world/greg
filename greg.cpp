/* Copyright 2015 Henry Elliott
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* This program links against the LGPL library GMP <https://gmplib.org/>
 * It is itself licensed under the permissive license above.
 */
/* I compile this on my 64-bit Debian box with
 * g++ --std=c++11 -pedantic -wall -wextra -werror greg.cpp -o greg -lgmp -pthread -O2
 * 
 * if you have trouble with that you could try removing
 * the warning flags:
 * g++ --std=c++11 greg.cpp -o greg -lgmp -pthread -O2
 * 
 * Note that you will need to have gmp installed with the
 * header files. On Debian the package is libgmp-dev.
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <gmp.h>
#include <thread>
#include <vector>
//#include <iostream>
//#include <chrono>

const int BASE = 10;
const int BAD_INPUT = 42;
const int THREAD_MULTIPLIER = 2;

void parallelize_gregory(mpq_t result, unsigned long input, unsigned long thread_count);
void compute_gregory(mpq_t sum, unsigned long begin, unsigned long end);
//void sum_results(mpq_t result, const std::vector<mpq_t> &sums, unsigned int low, unsigned int high);
unsigned long guess_threads();

int main(int argc, char **argv) {
	unsigned long input;
	unsigned long thread_count = guess_threads();
	try {
		if (argc == 2 || argc == 3) {
			input = std::stoul(*(argv + 1), nullptr, BASE);
			if (**(argv + 1) == '-') {
				throw BAD_INPUT;
			}
			if (argc == 3) {
				thread_count = std::stoul(*(argv + 2), nullptr, BASE);
				if (**(argv + 2) == '-') {
					throw BAD_INPUT;
				}
			}
		} else {
			throw BAD_INPUT;
		}
		if (input == 0 || thread_count == 0) {
			throw BAD_INPUT;
		}
	} catch (...) {
		fprintf(stderr, "usage: greg <number of terms to sum> [number of threads]\n");
		return 1;
	}
	if (input < thread_count) {
		thread_count = input;
	}
	
	mpq_t result;
	mpq_init(result);
	parallelize_gregory(result, input, thread_count); 
	
	mpq_out_str(stdout, BASE, result);
	putchar('\n');
	//printf("%.14f\n", mpq_get_d(result));
	mpq_clear(result);
	return 0;
}

void parallelize_gregory(mpq_t result, unsigned long input, unsigned long thread_count) {
	mpq_t number_four;
	mpq_init(number_four);
	mpq_set_str(number_four, "4", BASE);
	
	std::vector<mpq_t> sums(thread_count);
	for (unsigned long i = 0; i < thread_count; i++) {
		mpq_init(sums[i]);
	}
	
	std::vector<std::thread> threads(thread_count);
	for (unsigned long i = 0; i < thread_count; i++) {
		threads[i] = std::thread(compute_gregory, sums[i], i * input / thread_count + 1, (i + 1) * input / thread_count);
	}
	
	for (unsigned long i = 0; i < thread_count; i++) {
		threads[i].join();
	}
	//auto start = std::chrono::high_resolution_clock::now();
	//sum_results(result, sums, 0, sums.size() - 1);
	for (unsigned long i = 0; i < thread_count; i++) {
		mpq_add(result, result, sums[i]);
	}
	//auto end = std::chrono::high_resolution_clock::now();
	//std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

	for (unsigned long i = 0; i < thread_count; i++) {
		mpq_clear(sums[i]);
	}

	mpq_mul(result, result, number_four);
	mpq_clear(number_four);
}

// This is the recursive version.
void compute_gregory(mpq_t result, unsigned long low, unsigned long high) {
	if (low == high) {
		if ((low & 1) == 0) {
			mpq_set_si(result, -1, (low << 1) - 1);
		} else {
			mpq_set_ui(result, 1, (low << 1) - 1);
		}
	} else {
		mpq_t first, second;
		mpq_inits(first, second, NULL);
		unsigned long mid = ((high - low) >> 1) + low;
		compute_gregory(first, low, mid);
		compute_gregory(second, mid + 1, high);
		mpq_add(result, first, second);
		mpq_clears(first, second, NULL);
	}
}

/*void sum_results(mpq_t result, const std::vector<mpq_t> &sums, unsigned int low, unsigned int high) {
	if (low + 1 == high) {
		mpq_add(result, sums[low], sums[high]);
	} else {
		mpq_t first, second;
		mpq_inits(first, second, NULL);
		unsigned int mid = ((high - low) >> 1) + low;
		sum_results(first, sums, low, mid);
		sum_results(second, sums, mid + 1, high);
		mpq_add(result, first, second);
		mpq_clears(first, second);
	}
}*/

unsigned long guess_threads() {
	unsigned long thread_count = std::thread::hardware_concurrency();
	if (thread_count == 0) {
		return THREAD_MULTIPLIER;
	} else {
		return THREAD_MULTIPLIER * thread_count;
	}
}
