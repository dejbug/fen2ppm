#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <assert.h>
#include <gtest/gtest.h>

// #include <lib/out.h> // log
#include <lib/lib.h> // parse
#include <lib/fen.h>

int main(int, char **)
{
#if 0
	// char const * def_fen = "  3rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w  KQkq - 0 1";
	// char const * def_fen = "  /pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w  KQkq - 0 1";
	// char const * def_fen = "  pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w  KQkq - 0 1";
	// std::string s;
	// int const result = fen_unpack(s, def_fen);
	// if (result)
		// lib::err("! Errors (%d) while unpacking FEN.\n", result);
	// lib::log("FEN |%s|\n", s.data());

	fen2_t fen2;
	fen2.parse(DEFAULT_FEN);
	lib::log("FEN |%s|\n", fen2.raw.data());

	lib::log("COL |");
	for (size_t i=0; i<fen2.raw.size(); ++i)
		lib::log("%d", fen2.side(i));
	lib::log("|\n");

	lib::log("MAP |");
	for (size_t i=0; i<fen2.raw.size(); ++i)
		lib::log("%c", fen2.get(i, "ABCDEFGHIJKL"));
	lib::log("|\n");
#endif
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}

namespace {

TEST(FenTest, fen2_t_1) {
	char const * const text = "  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w  KQkq - 0 1";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, true);
	ASSERT_EQ(fen.ranks_same_size, true);
}

TEST(FenTest, fen2_t_2) {
	char const * const text = "  rnbqkbnr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR w  KQkq - 0 1";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, false);
	ASSERT_EQ(fen.ranks_same_size, false);
}

TEST(FenTest, fen2_t_3) {
	char const * const text = "  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/8  KQkq - 0 1";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, false);
	ASSERT_EQ(fen.ranks_same_size, true);
}

TEST(FenTest, fen2_t_4) {
	char const * const text = "  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/ KQkq - 0 1";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, false);
	ASSERT_EQ(fen.ranks_same_size, true);
}

TEST(FenTest, fen2_t_5) {
	char const * const text = "  2nbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w  KQkq - 0 1";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, false);
	ASSERT_EQ(fen.ranks_same_size, false);
}

TEST(FenTest, fen2_t_6) {
	char const * const text = "abc/3/def";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, true);
	ASSERT_EQ(fen.ranks_same_size, true);
}

TEST(FenTest, fen2_t_7) {
	char const * const text = "abc";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, false);
	ASSERT_EQ(fen.ranks_same_size, true);
}

TEST(FenTest, fen2_t_8) {
	char const * const text = "/abc/";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, false);
	ASSERT_EQ(fen.ranks_same_size, true);
}

TEST(FenTest, fen2_t_9) {
	char const * const text = "//";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, true);
	ASSERT_EQ(fen.ranks_same_size, true);
}

TEST(FenTest, fen2_t_10) {
	char const * const text = "";
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, true);
	ASSERT_EQ(fen.ranks_same_size, true);
}

TEST(FenTest, fen2_t_11) {
	char const * const text = nullptr;
	fen2_t fen;
	fen.parse(text);
	lib::log("FEN |%s|\n", fen.raw.data());
	ASSERT_EQ(fen.square_board, true);
	ASSERT_EQ(fen.ranks_same_size, true);
}

}
