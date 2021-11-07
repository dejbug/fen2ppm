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

TEST(LibTest, ColorMacroRgb) {
	ASSERT_EQ(HTML_RED(0x112233), 0x11);
	ASSERT_EQ(HTML_GREEN(0x112233), 0x22);
	ASSERT_EQ(HTML_BLUE(0x112233), 0x33);
}

TEST(LibTest, ColorMacroRgbS) {
	ASSERT_EQ(HTML_RED(0xF1F2F3), 0xF1);
	ASSERT_EQ(HTML_GREEN(0xF1F2F3), 0xF2);
	ASSERT_EQ(HTML_BLUE(0xF1F2F3), 0xF3);
}

TEST(LibTest, ColorMacroBgr) {
	ASSERT_EQ(COLORREF_RED(RGB(11,22,33)), 11);
	ASSERT_EQ(COLORREF_GREEN(RGB(11,22,33)), 22);
	ASSERT_EQ(COLORREF_BLUE(RGB(11,22,33)), 33);
}

TEST(LibTest, ColorMacroBgrS) {
	ASSERT_EQ(COLORREF_RED(RGB(0xF1,0xF2,0xF3)), 0xF1);
	ASSERT_EQ(COLORREF_GREEN(RGB(0xF1,0xF2,0xF3)), 0xF2);
	ASSERT_EQ(COLORREF_BLUE(RGB(0xF1,0xF2,0xF3)), 0xF3);
}

TEST(LibTest, ColorMacroBgr2) {
	ASSERT_EQ(RGB(0xF1,0xF2,0xF3), 0xF3F2F1);
	ASSERT_EQ(RGB(0xF2,0xF3,0xF1), 0xF1F3F2);
	ASSERT_EQ(RGB(0xF3,0xF1,0xF2), 0xF2F1F3);
}

TEST(LibTest, NotDir) {
	ASSERT_EQ(lib::notdir(nullptr), nullptr);
	ASSERT_EQ(strcmp(lib::notdir(""), ""), 0);
	ASSERT_EQ(strcmp(lib::notdir("N"), "N"), 0);
	ASSERT_EQ(strcmp(lib::notdir("P\\N"), "N"), 0);
	ASSERT_EQ(strcmp(lib::notdir("P//N"), "N"), 0);
	ASSERT_EQ(strcmp(lib::notdir("D:\\P//N"), "N"), 0);
	ASSERT_EQ(strcmp(lib::notdir("D://P\\N"), "N"), 0);
	ASSERT_EQ(strcmp(lib::notdir("D:"), "D:"), 0);
}

TEST(LibTest, SplitPath) {
	lib::split_path_t x;

	x.parse("");
	x.print();
	ASSERT_EQ(x.path, nullptr);
	ASSERT_EQ(strcmp(x.name, ""), 0);
	ASSERT_EQ(x.ext, nullptr);

	
	x.parse("Hello");
	x.print();
	ASSERT_EQ(x.path, nullptr);
	ASSERT_EQ(strcmp(x.name, "Hello"), 0);
	ASSERT_EQ(x.ext, nullptr);

	x.parse("Hi\\Ho/Hello");
	x.print();
	ASSERT_EQ(strcmp(x.path, "Hi\\Ho"), 0);
	ASSERT_EQ(strcmp(x.name, "Hello"), 0);
	ASSERT_EQ(x.ext, nullptr);

	x.parse("Hi/Ho\\Hello");
	x.print();
	ASSERT_EQ(strcmp(x.path, "Hi/Ho"), 0);
	ASSERT_EQ(strcmp(x.name, "Hello"), 0);
	ASSERT_EQ(x.ext, nullptr);

	x.parse("Hi\\Ho\\Hello.txt");
	x.print();
	ASSERT_EQ(strcmp(x.path, "Hi\\Ho"), 0);
	ASSERT_EQ(strcmp(x.name, "Hello"), 0);
	ASSERT_EQ(strcmp(x.ext, "txt"), 0);

	x.parse("Hi//Ho//Hello.txt");
	x.print();
	ASSERT_EQ(strcmp(x.path, "Hi//Ho/"), 0);
	ASSERT_EQ(strcmp(x.name, "Hello"), 0);
	ASSERT_EQ(strcmp(x.ext, "txt"), 0);

	x.parse("/Hello.txt");
	x.print();
	ASSERT_EQ(strcmp(x.path, ""), 0);
	ASSERT_EQ(strcmp(x.name, "Hello"), 0);
	ASSERT_EQ(strcmp(x.ext, "txt"), 0);

	x.parse("Hello.txt");
	x.print();
	ASSERT_EQ(x.path, nullptr);
	ASSERT_EQ(strcmp(x.name, "Hello"), 0);
	ASSERT_EQ(strcmp(x.ext, "txt"), 0);

	x.parse(".txt");
	x.print();
	ASSERT_EQ(x.path, nullptr);
	ASSERT_EQ(strcmp(x.name, ""), 0);
	ASSERT_EQ(strcmp(x.ext, "txt"), 0);
}

}
