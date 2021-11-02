#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <assert.h>
#include <gtest/gtest.h>
#include "../shlex_t.h"

// #include "../lib.h" // log

int main(int _, char ** argv_)
{
#if 0
	LPCWSTR const wcl = GetCommandLineW();
	wprintf(L"wcl |%S|\n", wcl);
	lib::shlex_t shlex;
	shlex.parse(wcl);
	shlex.print();
	for (size_t i=0; i<shlex.count(); ++i)
		printf("%2d : |%s|\n", i, shlex.get(i));
	return 0;
#endif
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}

namespace {

TEST(ShlexTest, Simple) {
	LPCWSTR wcl = L"hello.exe Hi ho. \"Oh, hello!\"";
	// wprintf(L"wcl |%S|\n", wcl);
	lib::shlex_t shlex;
	shlex.parse(wcl);
	// shlex.print();
	ASSERT_EQ(shlex.count(), 4);
	// for (size_t i=0; i<shlex.count(); ++i)
	// 	printf("%2d : |%s|\n", i, shlex.get(i));
	ASSERT_EQ(strcmp(shlex.get(0), "hello.exe"), 0);
	ASSERT_EQ(strcmp(shlex.get(1), "Hi"), 0);
	ASSERT_EQ(strcmp(shlex.get(2), "ho."), 0);
	ASSERT_EQ(strcmp(shlex.get(3), "Oh, hello!"), 0);
}

}
