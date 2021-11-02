#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <assert.h>
#include <gtest/gtest.h>
#include "../theme_t.h"

#include "../../lib/lib.h" // log

int main(int _, char ** argv_)
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
	int const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 0);
	ASSERT_EQ(theme.ds, 0);
}

TEST(ThemeTest, EmptyString) {
char const * text = "";
	theme_t theme;
	int const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 0);
	ASSERT_EQ(theme.ds, 0);
}

TEST(ThemeTest, Eol) {
char const * text = "00DEAD";
	theme_t theme;
	int const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 1);
	ASSERT_EQ(theme.ds, 0x00DEAD);
}

TEST(ThemeTest, IncompleteThird) {
char const * text = "123456/123/12";
	theme_t theme;
	int const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 2);
	ASSERT_EQ(theme.ds, 0x00123456);
	ASSERT_EQ(theme.ls, 0x00000123);
	ASSERT_EQ(theme.dp, 0x00000012);
}

TEST(ThemeTest, MixedSeparators) {
char const * text = "123ACE ACE,123A5C7E";
	theme_t theme;
	int const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 3);
	ASSERT_EQ(theme.ds, 0x00123ACE);
	ASSERT_EQ(theme.ls, 0x00000ACE);
	ASSERT_EQ(theme.dp, 0x123A5C7E);
}

TEST(ThemeTest, HashPrefixed) {
char const * text = "#123ACE,#ACE #123A5C7E";
	theme_t theme;
	int const count = theme.parse(text);
	ASSERT_EQ(count, theme.valid);
	ASSERT_EQ(count, 3);
	ASSERT_EQ(theme.ds, 0x00123ACE);
	ASSERT_EQ(theme.ls, 0x00000ACE);
	ASSERT_EQ(theme.dp, 0x123A5C7E);
}

}
