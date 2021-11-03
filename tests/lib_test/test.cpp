#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <assert.h>
#include <gtest/gtest.h>

// #include <lib/out.h> // log
#include <lib/lib.h>

int main(int, char **)
{
#if 0
	return 0;
#endif
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}

namespace {

TEST(LibTest, Rgb) {
	ASSERT_EQ(HTML_RED(0x112233), 0x11);
	ASSERT_EQ(HTML_GREEN(0x112233), 0x22);
	ASSERT_EQ(HTML_BLUE(0x112233), 0x33);
}

TEST(LibTest, RgbS) {
	ASSERT_EQ(HTML_RED(0xF1F2F3), 0xF1);
	ASSERT_EQ(HTML_GREEN(0xF1F2F3), 0xF2);
	ASSERT_EQ(HTML_BLUE(0xF1F2F3), 0xF3);
}

TEST(LibTest, Bgr) {
	ASSERT_EQ(COLORREF_RED(RGB(11,22,33)), 11);
	ASSERT_EQ(COLORREF_GREEN(RGB(11,22,33)), 22);
	ASSERT_EQ(COLORREF_BLUE(RGB(11,22,33)), 33);
}

TEST(LibTest, BgrS) {
	ASSERT_EQ(COLORREF_RED(RGB(0xF1,0xF2,0xF3)), 0xF1);
	ASSERT_EQ(COLORREF_GREEN(RGB(0xF1,0xF2,0xF3)), 0xF2);
	ASSERT_EQ(COLORREF_BLUE(RGB(0xF1,0xF2,0xF3)), 0xF3);
}

TEST(LibTest, Bgr2) {
	ASSERT_EQ(RGB(0xF1,0xF2,0xF3), 0xF3F2F1);
	ASSERT_EQ(RGB(0xF2,0xF3,0xF1), 0xF1F3F2);
	ASSERT_EQ(RGB(0xF3,0xF1,0xF2), 0xF2F1F3);
}

}
