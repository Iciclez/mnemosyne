#include "CppUnitTest.h"
#include "pch.h"

#include "mnemosyne.hpp"

#pragma comment(lib, "mnemosyne.lib")
#pragma comment(lib, "detours.lib")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mnemosyne_test {
TEST_CLASS(test_address){public : TEST_METHOD(test_address_as_ptr){
    Assert::AreEqual(reinterpret_cast<void*>(0xab),
                     mnemosyne::address(0xab).as_ptr());
}

TEST_METHOD(test_address_as_int) {
  Assert::AreEqual(static_cast<uintptr_t>(0xab),
                   mnemosyne::address(0xab).as_int());
}
TEST_METHOD(test_address_read_memory) {
  uint32_t n = 0x12345678;

  std::vector<uint8_t> actual = mnemosyne::address(&n).read_memory(sizeof(n));

  Assert::AreEqual(sizeof(n), actual.size());
  Assert::AreEqual<uint8_t>(0x78, actual.at(0));
  Assert::AreEqual<uint8_t>(0x56, actual.at(1));
  Assert::AreEqual<uint8_t>(0x34, actual.at(2));
  Assert::AreEqual<uint8_t>(0x12, actual.at(3));
}

TEST_METHOD(test_address_write_memory) {
  uint32_t n = 0xdeadbeef;

  Assert::IsTrue(mnemosyne::address(&n).write_memory({0x78, 0x56, 0x34, 0x12}));
  Assert::AreEqual<uint32_t>(0x12345678, n);
}

TEST_METHOD(test_address_copy_memory) {
  uint32_t n = 0xdeadbeef;

  Assert::IsTrue(
      mnemosyne::address(&n).copy_memory("\x78\x56\x34\x12", sizeof(n)));
  Assert::AreEqual<uint32_t>(0x12345678, n);
}

TEST_METHOD(test_address_fill_memory) {
  uint32_t n = 0xdeadbeef;

  Assert::IsTrue(mnemosyne::address(&n).fill_memory(0x90, 3));
  Assert::AreEqual<uint32_t>(0xde909090, n);
}

TEST_METHOD(test_address_write) {
  uint64_t n = 0x12345678deadbeef;

  Assert::IsTrue(mnemosyne::address(&n).write<uint8_t>(0x90));
  Assert::AreEqual<uint64_t>(0x12345678deadbe90, n);

  Assert::IsTrue(mnemosyne::address(&n).write<uint16_t>(0xbaad));
  Assert::AreEqual<uint64_t>(0x12345678deadbaad, n);

  Assert::IsTrue(mnemosyne::address(&n).write<uint32_t>(0xdeadbeef));
  Assert::AreEqual<uint64_t>(0x12345678deadbeef, n);

  Assert::IsTrue(mnemosyne::address(&n).write<uint64_t>(0xdeadbeef12345678));
  Assert::AreEqual<uint64_t>(0xdeadbeef12345678, n);
}

TEST_METHOD(test_address_read) {
  uint64_t n = 0x12345678deadbeef;

  Assert::AreEqual<uint8_t>(0xef, mnemosyne::address(&n).read<uint8_t>());
  Assert::AreEqual<uint32_t>(0xbeef,
                             mnemosyne::address(&n).read<uint16_t>());  // ok
  Assert::AreEqual<uint32_t>(0xdeadbeef,
                             mnemosyne::address(&n).read<uint32_t>());
  Assert::AreEqual<uint64_t>(0x12345678deadbeef,
                             mnemosyne::address(&n).read<uint64_t>());
}

TEST_METHOD(test_address_write_ptr_val) {
  struct test_struct {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint64_t d;
  };

  Assert::AreEqual<size_t>(0, offsetof(struct test_struct, a));
  Assert::AreEqual<size_t>(2, offsetof(struct test_struct, b));
  Assert::AreEqual<size_t>(4, offsetof(struct test_struct, c));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct, d));

  test_struct obj = {0x33, 0x9090, 0xbaadf00d, 0xdeadbeefdeadbeef};
  test_struct* ptr = &obj;

  Assert::IsTrue(mnemosyne::address(&ptr).write_ptr_val<uint8_t>(
      offsetof(struct test_struct, a), 0x88));
  Assert::AreEqual<uint8_t>(0x88, ptr->a);

  Assert::IsTrue(mnemosyne::address(&ptr).write_ptr_val<uint16_t>(
      offsetof(struct test_struct, b), 0xefef));
  Assert::AreEqual<uint32_t>(0xefef, ptr->b);

  Assert::IsTrue(mnemosyne::address(&ptr).write_ptr_val<uint32_t>(
      offsetof(struct test_struct, c), 0x45454545));
  Assert::AreEqual<uint32_t>(0x45454545, ptr->c);

  Assert::IsTrue(mnemosyne::address(&ptr).write_ptr_val<uint64_t>(
      offsetof(struct test_struct, d), 0x1234567887654321));
  Assert::AreEqual<uint64_t>(0x1234567887654321, ptr->d);
}

TEST_METHOD(test_address_read_ptr_val) {
  struct test_struct {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint64_t d;
  };

  Assert::AreEqual<size_t>(0, offsetof(struct test_struct, a));
  Assert::AreEqual<size_t>(2, offsetof(struct test_struct, b));
  Assert::AreEqual<size_t>(4, offsetof(struct test_struct, c));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct, d));

  test_struct obj = {0x33, 0x9090, 0xbaadf00d, 0xdeadbeefdeadbeef};
  test_struct* ptr = &obj;

  Assert::AreEqual<uint8_t>(ptr->a,
                            mnemosyne::address(&ptr).read_ptr_val<uint8_t>(
                                offsetof(struct test_struct, a)));
  Assert::AreEqual<uint32_t>(ptr->b,
                             mnemosyne::address(&ptr).read_ptr_val<uint16_t>(
                                 offsetof(struct test_struct, b)));
  Assert::AreEqual<uint32_t>(ptr->c,
                             mnemosyne::address(&ptr).read_ptr_val<uint32_t>(
                                 offsetof(struct test_struct, c)));
  Assert::AreEqual<uint64_t>(ptr->d,
                             mnemosyne::address(&ptr).read_ptr_val<uint64_t>(
                                 offsetof(struct test_struct, d)));
}

TEST_METHOD(test_address_write_multilevel_ptr_val) {
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
  Assert::AreEqual<size_t>(0, offsetof(struct test_struct_inner, x));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct_inner, y));
  Assert::AreEqual<size_t>(16, offsetof(struct test_struct_inner, z));

  Assert::AreEqual<size_t>(0, offsetof(struct test_struct, a));
  Assert::AreEqual<size_t>(2, offsetof(struct test_struct, b));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct, c));
  Assert::AreEqual<size_t>(16, offsetof(struct test_struct, d));
  Assert::AreEqual<size_t>(24, offsetof(struct test_struct, e));
#elif _WIN32
  Assert::AreEqual<size_t>(0, offsetof(struct test_struct_inner, x));
  Assert::AreEqual<size_t>(4, offsetof(struct test_struct_inner, y));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct_inner, z));

  Assert::AreEqual<size_t>(0, offsetof(struct test_struct, a));
  Assert::AreEqual<size_t>(2, offsetof(struct test_struct, b));
  Assert::AreEqual<size_t>(4, offsetof(struct test_struct, c));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct, d));
  Assert::AreEqual<size_t>(16, offsetof(struct test_struct, e));
#endif
  uint32_t v1 = 0xc0cac0ca;
  uint32_t v2 = 0xbaadf00d;

  test_struct_inner inner_obj = {0x11223344, &v1, 0x56565656};

  test_struct obj = {0x33, 0x9090, &v2, &inner_obj, 0xdeadbeefdeadbeef};
  test_struct* ptr = &obj;

  Assert::IsTrue(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint8_t>(
      std::queue<size_t>({offsetof(struct test_struct, a)}), 0x88));
  Assert::AreEqual<uint8_t>(0x88, ptr->a);

  Assert::IsTrue(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint16_t>(
      std::queue<size_t>({offsetof(struct test_struct, b)}), 0xefef));
  Assert::AreEqual<uint32_t>(0xefef, ptr->b);

  Assert::IsTrue(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint32_t>(
      std::queue<size_t>({offsetof(struct test_struct, c), 0}), 0x45454545));
  Assert::AreEqual<uint32_t>(0x45454545, *ptr->c);

  Assert::IsTrue(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint32_t>(
      std::queue<size_t>({offsetof(struct test_struct, d),
                          offsetof(struct test_struct_inner, x)}),
      0x11111111));
  Assert::AreEqual<uint32_t>(0x11111111, ptr->d->x);
  Assert::IsTrue(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint32_t>(
      std::queue<size_t>({offsetof(struct test_struct, d),
                          offsetof(struct test_struct_inner, y), 0}),
      0x77777777));
  Assert::AreEqual<uint32_t>(0x77777777, *ptr->d->y);
  Assert::IsTrue(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint32_t>(
      std::queue<size_t>({offsetof(struct test_struct, d),
                          offsetof(struct test_struct_inner, z)}),
      0x66666666));
  Assert::AreEqual<uint32_t>(0x66666666, ptr->d->z);

  Assert::IsTrue(mnemosyne::address(&ptr).write_multilevel_ptr_val<uint64_t>(
      std::queue<size_t>({offsetof(struct test_struct, e)}),
      0x1234567887654321));
  Assert::AreEqual<uint64_t>(0x1234567887654321, ptr->e);
}

TEST_METHOD(test_address_read_multilevel_ptr_val) {
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
  Assert::AreEqual<size_t>(0, offsetof(struct test_struct_inner, x));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct_inner, y));
  Assert::AreEqual<size_t>(16, offsetof(struct test_struct_inner, z));

  Assert::AreEqual<size_t>(0, offsetof(struct test_struct, a));
  Assert::AreEqual<size_t>(2, offsetof(struct test_struct, b));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct, c));
  Assert::AreEqual<size_t>(16, offsetof(struct test_struct, d));
  Assert::AreEqual<size_t>(24, offsetof(struct test_struct, e));
#elif _WIN32
  Assert::AreEqual<size_t>(0, offsetof(struct test_struct_inner, x));
  Assert::AreEqual<size_t>(4, offsetof(struct test_struct_inner, y));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct_inner, z));

  Assert::AreEqual<size_t>(0, offsetof(struct test_struct, a));
  Assert::AreEqual<size_t>(2, offsetof(struct test_struct, b));
  Assert::AreEqual<size_t>(4, offsetof(struct test_struct, c));
  Assert::AreEqual<size_t>(8, offsetof(struct test_struct, d));
  Assert::AreEqual<size_t>(16, offsetof(struct test_struct, e));
#endif
  uint32_t v1 = 0xc0cac0ca;
  uint32_t v2 = 0xbaadf00d;

  test_struct_inner inner_obj = {0x11223344, &v1, 0x56565656};

  test_struct obj = {0x33, 0x9090, &v2, &inner_obj, 0xdeadbeefdeadbeef};
  test_struct* ptr = &obj;

  Assert::AreEqual<uint8_t>(
      ptr->a, mnemosyne::address(&ptr).read_multilevel_ptr_val<uint8_t>(
                  std::queue<size_t>({offsetof(struct test_struct, a)})));
  Assert::AreEqual<uint32_t>(
      ptr->b, mnemosyne::address(&ptr).read_multilevel_ptr_val<uint16_t>(
                  std::queue<size_t>({offsetof(struct test_struct, b)})));
  Assert::AreEqual<uint32_t>(
      *ptr->c, mnemosyne::address(&ptr).read_multilevel_ptr_val<uint32_t>(
                   std::queue<size_t>({offsetof(struct test_struct, c), 0})));

  Assert::AreEqual<uint32_t>(
      ptr->d->x,
      mnemosyne::address(&ptr).read_multilevel_ptr_val<uint32_t>(
          std::queue<size_t>({offsetof(struct test_struct, d),
                              offsetof(struct test_struct_inner, x)})));
  Assert::AreEqual<uint32_t>(
      *ptr->d->y,
      mnemosyne::address(&ptr).read_multilevel_ptr_val<uint32_t>(
          std::queue<size_t>({offsetof(struct test_struct, d),
                              offsetof(struct test_struct_inner, y), 0})));
  Assert::AreEqual<uint32_t>(
      ptr->d->z,
      mnemosyne::address(&ptr).read_multilevel_ptr_val<uint32_t>(
          std::queue<size_t>({offsetof(struct test_struct, d),
                              offsetof(struct test_struct_inner, z)})));

  Assert::AreEqual<uint64_t>(
      ptr->e, mnemosyne::address(&ptr).read_multilevel_ptr_val<uint64_t>(
                  std::queue<size_t>({offsetof(struct test_struct, e)})));
}
}
;
}
