#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <assert.h>
#include <gtest/gtest.h>

#include <lib/lib.h> // log
#include <app/theme_t.h>

int main(int, char **)
{
#if 0
	char const * text = "123456/123/12";
	lib::log("text = |%s|\n", text);
	theme_t theme;
	int const count = theme.parse(text);
	lib::log("%d COLORS matched\n", count);
	theme.print();
	return 0;
#endif
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}

namespace {

TEST(ThemeTest, NullString) {
char const * text = nullptr;
	theme_t theme;
	size_t const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 0u);
	ASSERT_EQ(theme.has(0), false);
	ASSERT_EQ(theme.get(0), 0u);
}

TEST(ThemeTest, EmptyString) {
char const * text = "";
	theme_t theme;
	size_t const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 0u);
	ASSERT_EQ(theme.has(0), false);
	ASSERT_EQ(theme.get(0), 0u);
}

TEST(ThemeTest, Eol) {
char const * text = "00DEAD";
	theme_t theme;
	size_t const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 1u);
	ASSERT_EQ(theme.has(0), true);
	ASSERT_EQ(theme.has(1), false);
	ASSERT_EQ(theme.get(0), 0x00DEADu);
}

TEST(ThemeTest, StretchedColors) {
char const * text = "#123.ACE-#DED:#A0E";
	theme_t theme;
	size_t const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 4u);
	ASSERT_EQ(theme.has(4), false);
	ASSERT_EQ(theme.get(0), 0x00112233u);
	ASSERT_EQ(theme.get(1), 0x00AACCEEu);
	ASSERT_EQ(theme.get(2), 0x00DDEEDDu);
	ASSERT_EQ(theme.get(3), 0x00AA00EEu);
}

TEST(ThemeTest, IncompleteThird) {
char const * text = "123456/123/12";
	theme_t theme;
	size_t const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 2u);
	ASSERT_EQ(theme.get(0), 0x00123456u);
	ASSERT_EQ(theme.get(1), 0x00112233u);
	ASSERT_EQ(theme.get(2), 0x00000012u);
}

TEST(ThemeTest, MixedSeparators) {
char const * text = "123ACE ACE,123A5C7E";
	theme_t theme;
	size_t const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 3u);
	ASSERT_EQ(theme.has(3), false);
	ASSERT_EQ(theme.get(0), 0x00123ACEu);
	ASSERT_EQ(theme.get(1), 0x00AACCEEu);
	ASSERT_EQ(theme.get(2), 0x123A5C7Eu);
}

TEST(ThemeTest, HashPrefixed) {
char const * text = "#123ACE,#ACE #123A5C7E";
	theme_t theme;
	size_t const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 3u);
	ASSERT_EQ(theme.has(3), false);
	ASSERT_EQ(theme.get(0), 0x00123ACEu);
	ASSERT_EQ(theme.get(1), 0x00AACCEEu);
	ASSERT_EQ(theme.get(2), 0x123A5C7Eu);
}

}
