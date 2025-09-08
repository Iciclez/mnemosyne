#include "../mnemosyne.h"

#include <gtest/gtest.h>

#ifdef _WIN64
#pragma comment(lib, "mnemosyne.lib")
#pragma comment(lib, "detours64.lib")
#elif _WIN32
#pragma comment(lib, "mnemosyne32.lib")
#pragma comment(lib, "detours.lib")
#endif

TEST(memory_edit_unittest, test_memory_patch_edit) {
  uint32_t n = 0xdeadbeef;
  mnemosyne::memory_patch patch(&n,
                                std::vector<uint8_t>{0x78, 0x56, 0x34, 0x12});

  patch.edit();
  EXPECT_EQ(0x12345678, n);

  patch.revert();
  patch.edit();
  EXPECT_EQ(0x12345678, n);
}

TEST(memory_edit_unittest, test_memory_patch_revert) {
  uint32_t n = 0xdeadbeef;
  mnemosyne::memory_patch patch(&n,
                                std::vector<uint8_t>{0x78, 0x56, 0x34, 0x12});

  patch.edit();
  patch.revert();
  EXPECT_EQ(0xdeadbeef, n);

  patch.edit();
  patch.revert();
  EXPECT_EQ(0xdeadbeef, n);
}

TEST(memory_edit_unittest, test_memory_data_edit_edit) {
  uint32_t n = 0xdeadbeef;
  mnemosyne::memory_data_edit<int32_t> data_edit(&n, 0x12345678);

  data_edit.edit();
  EXPECT_EQ(0x12345678, n);

  data_edit.revert();
  data_edit.edit();
  EXPECT_EQ(0x12345678, n);
}

TEST(memory_edit_unittest, test_memory_data_edit_revert) {
  uint32_t n = 0xdeadbeef;
  mnemosyne::memory_data_edit<int32_t> data_edit(&n, 0x12345678);

  data_edit.edit();
  data_edit.revert();
  EXPECT_EQ(0xdeadbeef, n);

  data_edit.edit();
  data_edit.revert();
  EXPECT_EQ(0xdeadbeef, n);
}

TEST(memory_edit_unittest, test_memory_redirect_edit) {
  static bool variable = false;

  typedef decltype(&MessageBoxA) messageboxa_t;
  static messageboxa_t messageboxa = &MessageBoxA;

  auto to = [](_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText,
               _In_opt_ LPCSTR lpCaption, _In_ UINT uType) -> int {
    variable = true;
    // messageboxa(hWnd, lpText, lpCaption, uType);
    return TRUE;
  };

  auto redirect =
      mnemosyne::memory_redirect::from<messageboxa_t>(&messageboxa, to);

  redirect.edit();

  MessageBoxA(0, "detour test failed", "", MB_OK);
  EXPECT_TRUE(variable);

  redirect.revert();
  // MessageBoxA(0, "detour test ok", "", MB_OK);
}
