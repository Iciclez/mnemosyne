#include "../mnemosyne.h"

#include <gtest/gtest.h>

#ifdef _WIN64
#pragma comment(lib, "mnemosyne.lib")
#pragma comment(lib, "detours64.lib")
#elif _WIN32
#pragma comment(lib, "mnemosyne32.lib")
#pragma comment(lib, "detours.lib")
#endif

TEST(address_unittest, test_address_as_ptr) {
  EXPECT_EQ(reinterpret_cast<void*>(0xab), mnemosyne::address(0xab).as_ptr());
}

TEST(address_unittest, test_address_as_int) {
  EXPECT_EQ(static_cast<uintptr_t>(0xab), mnemosyne::address(0xab).as_int());
}

TEST(address_unittest, test_address_read_memory) {
  uint32_t n = 0x12345678;

  std::vector<uint8_t> actual = mnemosyne::address(&n).read_memory(sizeof(n));

  EXPECT_EQ(sizeof(n), actual.size());
  EXPECT_EQ(0x78, actual.at(0));
  EXPECT_EQ(0x56, actual.at(1));
  EXPECT_EQ(0x34, actual.at(2));
  EXPECT_EQ(0x12, actual.at(3));
}

TEST(address_unittest, test_address_write_memory) {
  uint32_t n = 0xdeadbeef;

  EXPECT_TRUE(mnemosyne::address(&n).write_memory({0x78, 0x56, 0x34, 0x12}));
  EXPECT_EQ(0x12345678, n);
}

TEST(address_unittest, test_address_copy_memory) {
  uint32_t n = 0xdeadbeef;

  EXPECT_TRUE(
      mnemosyne::address(&n).copy_memory("\x78\x56\x34\x12", sizeof(n)));
  EXPECT_EQ(0x12345678, n);
}

TEST(address_unittest, test_address_fill_memory) {
  uint32_t n = 0xdeadbeef;

  EXPECT_TRUE(mnemosyne::address(&n).fill_memory(0x90, 3));
  EXPECT_EQ(0xde909090, n);
}

TEST(address_unittest, test_address_write) {
  uint64_t n = 0x12345678deadbeef;

  EXPECT_TRUE(mnemosyne::address(&n).write<uint8_t>(0x90));
  EXPECT_EQ(0x12345678deadbe90, n);

  EXPECT_TRUE(mnemosyne::address(&n).write<uint16_t>(0xbaad));
  EXPECT_EQ(0x12345678deadbaad, n);

  EXPECT_TRUE(mnemosyne::address(&n).write<uint32_t>(0xdeadbeef));
  EXPECT_EQ(0x12345678deadbeef, n);

  EXPECT_TRUE(mnemosyne::address(&n).write<uint64_t>(0xdeadbeef12345678));
  EXPECT_EQ(0xdeadbeef12345678, n);
}

TEST(address_unittest, test_address_read) {
  uint64_t n = 0x12345678deadbeef;

  EXPECT_EQ(0xef, mnemosyne::address(&n).read<uint8_t>());
  EXPECT_EQ(0xbeef,
            mnemosyne::address(&n).read<uint16_t>());  // ok
  EXPECT_EQ(0xdeadbeef, mnemosyne::address(&n).read<uint32_t>());
  EXPECT_EQ(0x12345678deadbeef, mnemosyne::address(&n).read<uint64_t>());
}

TEST(address_unittest, test_address_write_ptr_val) {
  struct test_struct {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint64_t d;
  };

  EXPECT_EQ(0, offsetof(struct test_struct, a));
  EXPECT_EQ(2, offsetof(struct test_struct, b));
  EXPECT_EQ(4, offsetof(struct test_struct, c));
  EXPECT_EQ(8, offsetof(struct test_struct, d));

  test_struct obj = {0x33, 0x9090, 0xbaadf00d, 0xdeadbeefdeadbeef};
  test_struct* ptr = &obj;

  EXPECT_TRUE(mnemosyne::address(&ptr).write_ptr_val<uint8_t>(
      offsetof(struct test_struct, a), 0x88));
  EXPECT_EQ(0x88, ptr->a);

  EXPECT_TRUE(mnemosyne::address(&ptr).write_ptr_val<uint16_t>(
      offsetof(struct test_struct, b), 0xefef));
  EXPECT_EQ(0xefef, ptr->b);

  EXPECT_TRUE(mnemosyne::address(&ptr).write_ptr_val<uint32_t>(
      offsetof(struct test_struct, c), 0x45454545));
  EXPECT_EQ(0x45454545, ptr->c);

  EXPECT_TRUE(mnemosyne::address(&ptr).write_ptr_val<uint64_t>(
      offsetof(struct test_struct, d), 0x1234567887654321));
  EXPECT_EQ(0x1234567887654321, ptr->d);
}

TEST(address_unittest, test_address_read_ptr_val) {
  struct test_struct {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint64_t d;
  };

  EXPECT_EQ(0, offsetof(struct test_struct, a));
  EXPECT_EQ(2, offsetof(struct test_struct, b));
  EXPECT_EQ(4, offsetof(struct test_struct, c));
  EXPECT_EQ(8, offsetof(struct test_struct, d));

  test_struct obj = {0x33, 0x9090, 0xbaadf00d, 0xdeadbeefdeadbeef};
  test_struct* ptr = &obj;

  EXPECT_EQ(ptr->a, mnemosyne::address(&ptr).read_ptr_val<uint8_t>(
                        offsetof(struct test_struct, a)));
  EXPECT_EQ(ptr->b, mnemosyne::address(&ptr).read_ptr_val<uint16_t>(
                        offsetof(struct test_struct, b)));
  EXPECT_EQ(ptr->c, mnemosyne::address(&ptr).read_ptr_val<uint32_t>(
                        offsetof(struct test_struct, c)));
  EXPECT_EQ(ptr->d, mnemosyne::address(&ptr).read_ptr_val<uint64_t>(
                        offsetof(struct test_struct, d)));
}

TEST(address_unittest, test_address_write_multilevel_ptr_val) {
  struct test_struct_inner {
    uint32_t x;
    uint32_t* y;
    uint32_t z;
  };

  struct test_struct {
    uint8_t a;
    uint16_t b;
    uint32_t* c;
    test_struct_inner* d;
    uint64_t e;
  };

#ifdef _WIN64
  EXPECT_EQ(0, offsetof(struct test_struct_inner, x));
  EXPECT_EQ(8, offsetof(struct test_struct_inner, y));
  EXPECT_EQ(16, offsetof(struct test_struct_inner, z));

  EXPECT_EQ(0, offsetof(struct test_struct, a));
  EXPECT_EQ(2, offsetof(struct test_struct, b));
  EXPECT_EQ(8, offsetof(struct test_struct, c));
  EXPECT_EQ(16, offsetof(struct test_struct, d));
  EXPECT_EQ(24, offsetof(struct test_struct, e));
#elif _WIN32
  EXPECT_EQ(0, offsetof(struct test_struct_inner, x));
  EXPECT_EQ(4, offsetof(struct test_struct_inner, y));
  EXPECT_EQ(8, offsetof(struct test_struct_inner, z));

  EXPECT_EQ(0, offsetof(struct test_struct, a));
  EXPECT_EQ(2, offsetof(struct test_struct, b));
  EXPECT_EQ(4, offsetof(struct test_struct, c));
  EXPECT_EQ(8, offsetof(struct test_struct, d));
  EXPECT_EQ(16, offsetof(struct test_struct, e));
#endif
  uint32_t v1 = 0xc0cac0ca;
  uint32_t v2 = 0xbaadf00d;

  test_struct_inner inner_obj = {0x11223344, &v1, 0x56565656};

  test_struct obj = {0x33, 0x9090, &v2, &inner_obj, 0xdeadbeefdeadbeef};
  test_struct* ptr = &obj;

  EXPECT_TRUE(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint8_t>(
      std::queue<size_t>({offsetof(struct test_struct, a)}), 0x88));
  EXPECT_EQ(0x88, ptr->a);

  EXPECT_TRUE(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint16_t>(
      std::queue<size_t>({offsetof(struct test_struct, b)}), 0xefef));
  EXPECT_EQ(0xefef, ptr->b);

  EXPECT_TRUE(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint32_t>(
      std::queue<size_t>({offsetof(struct test_struct, c), 0}), 0x45454545));
  EXPECT_EQ(0x45454545, *ptr->c);

  EXPECT_TRUE(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint32_t>(
      std::queue<size_t>({offsetof(struct test_struct, d),
                          offsetof(struct test_struct_inner, x)}),
      0x11111111));
  EXPECT_EQ(0x11111111, ptr->d->x);
  EXPECT_TRUE(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint32_t>(
      std::queue<size_t>({offsetof(struct test_struct, d),
                          offsetof(struct test_struct_inner, y), 0}),
      0x77777777));
  EXPECT_EQ(0x77777777, *ptr->d->y);
  EXPECT_TRUE(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint32_t>(
      std::queue<size_t>({offsetof(struct test_struct, d),
                          offsetof(struct test_struct_inner, z)}),
      0x66666666));
  EXPECT_EQ(0x66666666, ptr->d->z);

  EXPECT_TRUE(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint64_t>(
      std::queue<size_t>({offsetof(struct test_struct, e)}),
      0x1234567887654321));
  EXPECT_EQ(0x1234567887654321, ptr->e);
}

TEST(address_unittest, test_address_read_multilevel_ptr_val) {
  struct test_struct_inner {
    uint32_t x;
    uint32_t* y;
    uint32_t z;
  };

  struct test_struct {
    uint8_t a;
    uint16_t b;
    uint32_t* c;
    test_struct_inner* d;
    uint64_t e;
  };

#ifdef _WIN64
  EXPECT_EQ(0, offsetof(struct test_struct_inner, x));
  EXPECT_EQ(8, offsetof(struct test_struct_inner, y));
  EXPECT_EQ(16, offsetof(struct test_struct_inner, z));

  EXPECT_EQ(0, offsetof(struct test_struct, a));
  EXPECT_EQ(2, offsetof(struct test_struct, b));
  EXPECT_EQ(8, offsetof(struct test_struct, c));
  EXPECT_EQ(16, offsetof(struct test_struct, d));
  EXPECT_EQ(24, offsetof(struct test_struct, e));
#elif _WIN32
  EXPECT_EQ(0, offsetof(struct test_struct_inner, x));
  EXPECT_EQ(4, offsetof(struct test_struct_inner, y));
  EXPECT_EQ(8, offsetof(struct test_struct_inner, z));

  EXPECT_EQ(0, offsetof(struct test_struct, a));
  EXPECT_EQ(2, offsetof(struct test_struct, b));
  EXPECT_EQ(4, offsetof(struct test_struct, c));
  EXPECT_EQ(8, offsetof(struct test_struct, d));
  EXPECT_EQ(16, offsetof(struct test_struct, e));
#endif
  uint32_t v1 = 0xc0cac0ca;
  uint32_t v2 = 0xbaadf00d;

  test_struct_inner inner_obj = {0x11223344, &v1, 0x56565656};

  test_struct obj = {0x33, 0x9090, &v2, &inner_obj, 0xdeadbeefdeadbeef};
  test_struct* ptr = &obj;

  EXPECT_EQ(ptr->a, mnemosyne::address(&ptr).read_multilevel_ptr_val<uint8_t>(
                        std::queue<size_t>({offsetof(struct test_struct, a)})));
  EXPECT_EQ(ptr->b, mnemosyne::address(&ptr).read_multilevel_ptr_val<uint16_t>(
                        std::queue<size_t>({offsetof(struct test_struct, b)})));
  EXPECT_EQ(*ptr->c,
            mnemosyne::address(&ptr).read_multilevel_ptr_val<uint32_t>(
                std::queue<size_t>({offsetof(struct test_struct, c), 0})));

  EXPECT_EQ(ptr->d->x,
            mnemosyne::address(&ptr).read_multilevel_ptr_val<uint32_t>(
                std::queue<size_t>({offsetof(struct test_struct, d),
                                    offsetof(struct test_struct_inner, x)})));
  EXPECT_EQ(
      *ptr->d->y,
      mnemosyne::address(&ptr).read_multilevel_ptr_val<uint32_t>(
          std::queue<size_t>({offsetof(struct test_struct, d),
                              offsetof(struct test_struct_inner, y), 0})));
  EXPECT_EQ(ptr->d->z,
            mnemosyne::address(&ptr).read_multilevel_ptr_val<uint32_t>(
                std::queue<size_t>({offsetof(struct test_struct, d),
                                    offsetof(struct test_struct_inner, z)})));

  EXPECT_EQ(ptr->e, mnemosyne::address(&ptr).read_multilevel_ptr_val<uint64_t>(
                        std::queue<size_t>({offsetof(struct test_struct, e)})));
}
