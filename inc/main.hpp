#include <getopt.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <sstream>

#include "digitizer3.hpp"
#include "types.hpp"

#include <png++/png.hpp>

void process_args(int argc, char** argv);
void print_help();
std::map<u16, u8> make_pal_entry_map(std::vector<ds_color> ds_palette);
std::vector<png::color> make_png_pal(std::vector<ds_color> ds_colors);
void gen_file_info(ds_file* workfile, std::ofstream* info_file);
void gen_img_info(ds_img* img, std::ofstream* info_file);
