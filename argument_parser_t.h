#pragma once
#include <getopt.h>
#include <stdlib.h>

#include "log.h"

struct argument_parser_t
{
	char const * const USAGE = "Usage: %s [-v] [-s SQUARE-SIZE] OUT-PATH FEN-TEXT FONT-PATH\n";
	int const POS_ARGS_COUNT = 3;
	int const DEFAULT_SQUARE_SIZE = 64;

	bool arg_help = false;
	bool arg_verbose = false;
	int arg_square_size = DEFAULT_SQUARE_SIZE;
	char const * arg_out_path = "out.ppm";
	char const * arg_fen_text = nullptr;
	char const * arg_font_path = nullptr;

	int error = 0;

	int parse(int argc, char ** argv)
	{
		error = 0;
		int opt = 0;

		while ((opt = getopt(argc, argv, "hvs:")) != -1)
		{
			switch (opt)
			{
				case 'h': arg_help = true; break;
				case 'v': arg_verbose = true; break;
				case 's': arg_square_size = atoi(optarg); break;
				default:
					log(USAGE, argv[0]);
					error = -1;
					return false;
			}
		}

		if (arg_verbose)
		{
			log("OPT help: %s\n", arg_help ? "yes" : "no");
			log("OPT verbose: %s\n", arg_verbose ? "yes" : "no");
			log("OPT square size: %d\n", arg_square_size);
		}

		if (optind + POS_ARGS_COUNT > argc)
		{
			log("%s: expected %d more argument after options\n", argv[0], POS_ARGS_COUNT + (optind - argc));
			log(USAGE, argv[0]);
			error = optind - argc;
			return false;
		}

		// for (int i=optind; i<argc; ++i)
			// printf("name argument %d: |%s|\n", i-optind+1, argv[i]);

		arg_out_path = argv[optind+0];
		arg_fen_text = argv[optind+1];
		arg_font_path = argv[optind+2];

		if (arg_verbose)
		{
			log("ARG outpath: |%s|\n", arg_out_path);
			log("ARG fentext: |%s|\n", arg_fen_text);
			log("ARG fontpath: |%s|\n", arg_font_path);
		}

		if (arg_square_size <= 0) arg_square_size = DEFAULT_SQUARE_SIZE;

		return true;
	}

};
