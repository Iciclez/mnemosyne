#include <gtest/gtest.h>
#include "mnemosyne.hpp"

#pragma comment(lib, "mnemosyne.lib")
#pragma comment(lib, "detours.lib")

TEST(util_unittest, test_util_string_to_bytes) {
  std::vector<uint8_t> expected = {0x12, 0x34, 0x56, 0x78,
                                   0x90, 0xab, 0xcd, 0xef};
  std::vector<uint8_t> actual =
      mnemosyne::util::string_to_bytes("12 34 56 78 90 AB CD EF");
  EXPECT_EQ(expected.size(), actual.size());

  for (size_t n = 0; n < expected.size(); ++n) {
    EXPECT_EQ(expected.at(n), actual.at(n));
  }
}

TEST(util_unittest, test_util_bytes_to_string) {
  EXPECT_EQ("12 34 56 78 90 AB CD EF",
            mnemosyne::util::byte_to_string(std::vector<uint8_t>{
                0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef}));
  EXPECT_EQ(
      "\\x12\\x34\\x56\\x78\\x90\\xAB\\xCD\\xEF",
      mnemosyne::util::byte_to_string(
          std::vector<uint8_t>{0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef},
          "\\x"));
  EXPECT_EQ(
      "1234567890ABCDEF",
      mnemosyne::util::byte_to_string(
          std::vector<uint8_t>{0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef},
          ""));
  EXPECT_EQ(
      "12*34*56*78*90*AB*CD*EF",
      mnemosyne::util::byte_to_string(
          std::vector<uint8_t>{0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef},
          "*"));
}

TEST(util_unittest, test_util_to) {
  auto res = mnemosyne::util::to<int32_t>({0xab, 0xcd, 0xef, 0x12});

  EXPECT_EQ(0x12efcdab, res);
}
