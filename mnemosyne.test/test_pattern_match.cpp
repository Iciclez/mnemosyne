#include "pch.h"
#include "CppUnitTest.h"

#include "mnemosyne.hpp"

#pragma comment(lib, "mnemosyne.lib")
#pragma comment(lib, "detours.lib")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mnemosyne_test
{
	TEST_CLASS(test_patern_match)
	{
	public:
		TEST_METHOD(test_pattern_match_find_address)
		{
			std::vector<uint8_t> haystack = {
				0xf1, 0x80, 0xd7, 0x50, 0x1a, 0x7b, 0x69, 0x57, 0x07, 0x80, 0xbc, 0x27, 0xc7, 0x5e, 0x88, 0x0c,
				0xac, 0x7f, 0xd8, 0xe0, 0x13, 0x7d, 0xf4, 0xfb, 0xf4, 0x91, 0x0b, 0x07, 0xa6, 0xe1, 0x54, 0x22
			};


			Assert::AreEqual<uintptr_t>(reinterpret_cast<uintptr_t>(haystack.data()) + 5, mnemosyne::pattern_match("7b ?? 57 07 ?? bc ?? c7", haystack.data(), haystack.size()).find_address());
			Assert::AreEqual<uintptr_t>(0, mnemosyne::pattern_match("7b ?? 57 33 07 ?? bc ?? c7", haystack.data(), haystack.size()).find_address());
		}

		TEST_METHOD(test_pattern_match_find_next_address)
		{
			std::vector<uint8_t> haystack = {
				0xf1, 0x80, 0xd7, 0x50, 0x1a, 0x7b, 0x69, 0x57, 0x07, 0x80, 0xbc, 0x27, 0xc7, 0x5e, 0x88, 0x0c,
				0xac, 0x7f, 0xd8, 0xe0, 0x13, 0x7d, 0xf4, 0xfb, 0xf4, 0x91, 0x0b, 0x07, 0xa6, 0xe1, 0x54, 0x22,
				0x7b, 0xa0, 0x57, 0x07, 0x2b, 0xbc, 0xdd, 0xc7, 0x24, 0x53, 0xb3, 0x3f, 0xf1, 0xd5, 0x67, 0x23
			};

			mnemosyne::pattern_match match("7b ?? 57 07 ?? bc ?? c7", haystack.data(), haystack.size());

			// next result
			Assert::AreEqual<uintptr_t>(reinterpret_cast<uintptr_t>(haystack.data()) + 5, match.find_address());
			Assert::AreEqual<uintptr_t>(reinterpret_cast<uintptr_t>(haystack.data()) + 32, match.find_next_address());
		}

	};
}
