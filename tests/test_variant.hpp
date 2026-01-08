#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "nstd/variant.hpp" 

namespace tests {
	namespace variant {
		// ==========================================
		// Helper: LifeCycleTracker
		// Tracks if destructors/constructors are called correctly
		// ==========================================
		struct Tracker {
			static int alive_count;
			int id;

			Tracker(int val) : id(val) {
				alive_count++;
				std::cout << "  [Ctor] Tracker(" << id << ")\n";
			}

			~Tracker() {
				alive_count--;
				std::cout << "  [Dtor] Tracker(" << id << ")\n";
			}

			Tracker(const Tracker& other) : id(other.id) {
				alive_count++;
				std::cout << "  [Copy] Tracker(" << id << ")\n";
			}

			Tracker(Tracker&& other) noexcept : id(other.id) {
				alive_count++; // Move creates a new object (old one still dies later)
				std::cout << "  [Move] Tracker(" << id << ")\n";
				other.id = -1; // "Steal"
			}

			Tracker& operator=(const Tracker& other) {
				std::cout << "  [Copy Assign] " << id << " = " << other.id << "\n";
				id = other.id;
				return *this;
			}

			Tracker& operator=(Tracker&& other) noexcept {
				std::cout << "  [Move Assign] " << id << " = " << other.id << "\n";
				id = other.id;
				other.id = -1;
				return *this;
			}
		};
		int Tracker::alive_count = 0;

		// ==========================================
		// Test Cases
		// ==========================================

		void test_primitives() {
			std::cout << "\n--- Test 1: Primitives & Switching ---\n";

			nstd::variant<int, double> v = 10;
			assert(v.index() == 0);
			assert(v.get<int>() == 10);
			std::cout << "Holds Int: " << v.get<int>() << " (PASS)\n";

			v = 3.14;
			assert(v.index() == 1);
			assert(v.get<double>() == 3.14);
			std::cout << "Switched to Double: " << v.get<double>() << " (PASS)\n";
		}

		void test_complex_lifecycle() {
			std::cout << "\n--- Test 2: Destructor V-Table Logic ---\n";

			{
				nstd::variant<int, Tracker> v = Tracker(1);
				assert(Tracker::alive_count == 1);

				std::cout << "Switching to int (Tracker should die)...\n";
				v = 999;

				// Tracker destructor should have fired immediately upon switch
				assert(Tracker::alive_count == 0);
			}
			std::cout << "Destructor Logic: (PASS)\n";
		}

		void test_copy_move() {
			std::cout << "\n--- Test 3: Copy & Move V-Tables ---\n";

			nstd::variant<int, std::string> v1 = std::string("Hello World");

			// Test Copy
			nstd::variant<int, std::string> v2 = v1;
			assert(v2.get<std::string>() == "Hello World");
			assert(v1.get<std::string>() == "Hello World"); // v1 still valid
			std::cout << "Copy: (PASS)\n";

			// Test Move
			nstd::variant<int, std::string> v3 = std::move(v1);
			assert(v3.get<std::string>() == "Hello World");
			// v1 is now in "moved-from" state (valid but contents unspecified)
			std::cout << "Move: (PASS)\n";
		}

		void test_visit() {
			std::cout << "\n--- Test 4: Visit (The Lambda Test) ---\n";

			nstd::variant<int, double, std::string> v;

			// Case A: Int
			v = 42;
			v.visit([](auto&& arg) {
				std::cout << "Visiting: " << arg << "\n";
				});

			// Case B: String
			v = std::string("nstd::variant is cool");
			v.visit([](auto&& arg) {
				std::cout << "Visiting: " << arg << "\n";
				});

			// Case C: Double
			v = 99.99;
			bool visited_double = false;
			v.visit([&](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, double>) {
					visited_double = true;
				}
				});
			assert(visited_double);
			std::cout << "Visit Dispatch: (PASS)\n";
		}

		void test_exceptions() {
			std::cout << "\n--- Test 5: Error Handling ---\n";

			nstd::variant<int, float> v = 10;
			try {
				float f = v.get<float>(); // Should fail
				(void)f;
				std::cerr << "FAIL: Did not throw exception on wrong type access!\n";
				exit(1);
			}
			catch (const std::exception& e) {
				std::cout << "Caught expected error: " << e.what() << " (PASS)\n";
			}
		}

		void run_all_tests() {
			test_primitives();
			test_complex_lifecycle();
			test_copy_move();
			test_visit();
			test_exceptions();
		}
	}
}