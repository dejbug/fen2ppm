#include <assert.h>
#include <stdarg.h>
#include <gtest/gtest.h>
#include "../argument_parser_t.h"

#include "../out.h" // log, print_argv

#define VERBOSE
#define MAX_ARGV 8

size_t argv_from_args(char * argv[MAX_ARGV], char * args)
{
	assert (MAX_ARGV > 0);

	size_t off = 0;
	size_t len = 0;
	size_t argc = 0;
	for (; argc<MAX_ARGV; ++argc)
	{
		len = strlen(args + off);
		argv[argc] = args + off;
		// lib::log("%d %s\n", len, args + off);
		if (!len) break;
		off += len + 1;
	}
	if (argc >= MAX_ARGV)
		argc = MAX_ARGV - 1;
	argv[argc] = 0;
	return argc;
}

int main(int _, char ** argv_)
{
#if 0
	char args[] = "TEST.EXE\0-c\0-b\0-a\0\0";
	char * argv[MAX_ARGV];// = {0};
	int const argc = argv_from_args(argv, args);

	lib::argument_parser_t p("abc:");
	p.parse(argc, argv);
	p.print();
	lib::log("c is %s\n", p.on('c') ? "ON" : "OFF");
	lib::log("c is %s\n", p.set('c') ? "SET" : "NONE");
	return 0;
#endif
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}

namespace {

TEST(SelfTest, ArgvFromArgs) {
	char args[] = "TEST.EXE\0-a\0-b\0-c\0\0";
	char * argv[MAX_ARGV];// = {0};
	int const argc = argv_from_args(argv, args);
#ifdef VERBOSE
	lib::log("%d %p\n", argc, argv);
	for (size_t i=0; i<argc+1; ++i)
		lib::log("%d |%s|\n", i, argv[i]);
	for (size_t i=0; i<MAX_ARGV; ++i)
		lib::log("%d |%p|\n", i, argv[i]);
#endif
	ASSERT_EQ(argc, 4);
	ASSERT_EQ(strcmp(argv[0], args), 0);
	ASSERT_EQ(strcmp(argv[1], "-a"), 0);
	ASSERT_EQ(strcmp(argv[2], "-b"), 0);
	ASSERT_EQ(strcmp(argv[3], "-c"), 0);
}

TEST(ParserTest, SimplePos) {
	char args[] = "TEST.EXE\0hi\0ho\0\0";
	char * argv[MAX_ARGV];// = {0};
	int const argc = argv_from_args(argv, args);

	lib::argument_parser_t p("abc:");
	p.parse(argc, argv);
#ifdef VERBOSE
	lib::print_argv(argv);
	p.print();
#endif
	ASSERT_EQ(p.on('a'), false) << "-a should be off";
	ASSERT_EQ(p.on('b'), false) << "-b should be off";
	ASSERT_EQ(p.on('c'), false) << "-c should be null";
	ASSERT_EQ(p.argscount, 2) << "argscount should be 2";
}

TEST(ParserTest, ReverseOptFlags) {
	char args[] = "TEST.EXE\0-b\0-a\0\0";
	char * argv[MAX_ARGV];// = {0};
	int const argc = argv_from_args(argv, args);

	lib::argument_parser_t p("abc:");
	p.parse(argc, argv);
#ifdef VERBOSE
	lib::print_argv(argv);
	p.print();
#endif
	ASSERT_EQ(p.on('a'), true) << "-a should be on";
	ASSERT_EQ(p.on('b'), true) << "-b should be on";
	ASSERT_EQ(p.on('c'), false) << "-c should be false";
	ASSERT_EQ(p.argscount, 0) << "argscount should be 0";
}

TEST(ParserTest, ReverseOpt) {
	char args[] = "TEST.EXE\0-c\0-b\0-a\0\0";
	char * argv[MAX_ARGV];// = {0};
	int const argc = argv_from_args(argv, args);

	lib::argument_parser_t p("abc:");
	p.parse(argc, argv);
#ifdef VERBOSE
	lib::print_argv(argv);
	p.print();

	lib::argument_parser_t::item_t * it = p.item('c');
	lib::log("ITEM 'c' on=%d val=|%s|\n", it->on, it->val);
#endif

	ASSERT_EQ(p.on('a'), true) << "-a should be on";
	ASSERT_EQ(p.on('b'), false) << "-b should be on";
	ASSERT_EQ(p.on('c'), false) << "-c should be false";
	ASSERT_EQ(p.set('c'), true) << "-c should be set";
	ASSERT_EQ(strcmp(p.opt('c'), "-b"), 0) << "-c should have value '-b'";
	ASSERT_EQ(p.argscount, 0) << "argscount should be 0";
}

TEST(ParserTest, ReverseOptAndPos) {
	char args[] = "TEST.EXE\0-c\0-b\0-a\0hiho\0\0";
	char * argv[MAX_ARGV];// = {0};
	int const argc = argv_from_args(argv, args);

	lib::argument_parser_t p("abc:");
	p.parse(argc, argv);
#ifdef VERBOSE
	lib::print_argv(argv);
	p.print();
#endif
	ASSERT_EQ(p.on('a'), true) << "-a should be on";
	ASSERT_EQ(p.on('b'), false) << "-b should be off";
	ASSERT_EQ(p.on('c'), false) << "-c should be off";
	ASSERT_EQ(p.set('c'), true) << "-c should be set";
	ASSERT_EQ(strcmp(p.opt('c'), "-b"), 0) << "-c should have value '-b'";
	ASSERT_EQ(p.argscount, 1) << "argscount should be 0";
}

}
