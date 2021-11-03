#pragma once
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

// #define VERBOSE

#ifdef NDEBUG
#ifdef VERBOSE
#undef VERBOSE
#endif
#endif

#include "out.h" // log

namespace lib {

struct argument_parser_t
{
	struct item_t {
		bool on = false;
		// std::string val;
		char const * val = nullptr;
	};

	char const * const sig = nullptr;
	size_t siglen = 0;
	size_t argsbegin = 0;
	size_t argscount = 0;
	item_t * items = nullptr;
	char ** argv = nullptr;
	int unknown = 0;
	int missing = 0;

	argument_parser_t(char const * signature) : sig(signature)
	{
		siglen = strlen(sig);
		items = new item_t[siglen];
	}

	~argument_parser_t()
	{
		if (items) { delete[] items; items = nullptr; }
	}

	int parse(int argc, char ** argv)
	{
		optind = 1;
		opterr = 0;
		this->argv = argv; // Required for arg() and print()

		int opt = 0;

		while ((opt = getopt(argc, argv, sig)) != -1)
		{
#ifdef VERBOSE
			char const * const ptr = strchr(sig, opt);
			if (!ptr) {
#else
			item_t * it = item(opt);
			if (opt == '?') {
#endif
#ifdef VERBOSE
				lib::log("* unknown opt '%c' (\\x%x)\n", optopt, optopt);
#endif
				unknown = optopt;
				return false;
			}
			else if (opt == ':') {
#ifdef VERBOSE
				lib::log("* missing arg for opt '%c' (\\x%x)\n", optopt, optopts);
#endif
				missing = optopt;
				return false;
			}
			else {
#ifdef VERBOSE
				size_t const i = ptr - sig;
				lib::log("* known opt %c at %d in '%s' (%s)\n", opt, i, sig, optarg);
				if (optarg) items[i].val = optarg;
				else items[i].on = true;
#else
				if (optarg) it->val = optarg;
				else it->on = true;
#endif
			}
		}

		argsbegin = optind;
		argscount = argc - optind;

#ifdef VERBOSE
		print();
#endif
		return true;
	}

	item_t * item(char opt)
	{
		char const * const ptr = strchr(sig, opt);
		if (!ptr) return nullptr;
		size_t const i = ptr - sig;
		return &items[i];
	}

	bool on(char opt)
	{
		item_t * it = item(opt);
		if (!it) return false;
		return it->on;
	}

	bool set(char opt)
	{
		item_t * it = item(opt);
		if (!it) return false;
		// return it->on || !it->val.empty();
		return it->on || it->val;
	}

	char const * opt(char opt)
	{
		item_t * it = item(opt);
		if (!it) return nullptr;
		// return it->val.c_str();
		return it->val;
	}

	char const * arg(size_t i)
	{
		if (i < argscount)
			return argv[argsbegin+i];
		return nullptr;
	}

	void print() const
	{
#ifndef NDEBUG
		for (size_t i=0; i<siglen; ++i)
		{
			if (sig[i] == ':') continue;
			// else if (!items[i].val.empty())
			else if (items[i].val)
				// lib::log("%d : OPT %c = |%s|\n", i, sig[i], items[i].val.c_str());
				lib::log("%2d : OPT %c = |%s|\n", i, sig[i], items[i].val);
			else
				lib::log("%2d : OPT %c is %s\n", i, sig[i], items[i].on ? "ON" : "OFF");
		}
		for (size_t i=0; i<argscount; ++i)
			lib::log("%2d : ARG |%s|\n", i, argv[i+argsbegin]);
#endif
	}
};

} // lib
