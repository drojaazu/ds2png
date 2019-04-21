#ifndef DIGITIZER3_H
#define DIGITIZER3_H
#include <arpa/inet.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>
#include "types.hpp"

/*
        Sega Digitizer System III data
        Based on work done by Rich Whitehouse
        http://richwhitehouse.com/pubdata/dig3_spec.txt
*/

// file header
struct ds_file_header {
  u32 ver;        // just a guess
  u16 unk1;       // unknown; always 0 in the files we have available
                  // (or next value is 32 bit)
  u16 img_count;  // number of images within the file
  u32 data_size;
  u32 unk2;     // unknown; always 0 in the files we have available
  char id[16];  // identifier string; "DIGITIZER 3 Ver2"
};

// 0x0 padding for 224 bytes aftrer this point
// image data begins at 0x100

// for each image
struct ds_img_header {
  u16 unk1;  // unknown; always 0 in the files we have available
             // (or next value is 32 bit)
  u16 data_offset;
  u16 pos_x;
  u16 pos_y;
  u16 width;
  u16 height;
  u16 frame_time;  // this is a guess, did not see sufficient evidence in
                   // the data set to confirm it.
  u16 img_index;
  char img_name[16];  // not terminated, trailed by spaces
};

struct ds_color {
  u16 color_id;
  u16 color;  // GGGGRRRRxxxxBBBB (Game Gear format)
};

struct ds_img {
  ds_img_header header;
  std::vector<ds_color> palette;
  u16 *bmp_data;
};

struct ds_file {
  ds_file_header header;
  std::vector<ds_img> images;
};

const ds_file get_ds_file(std::istream *indata);
const ds_file_header get_header(std::istream *indata);
const ds_img get_img(std::istream *indata);
const std::vector<ds_color> get_palette(std::istream *indata, u16 count);

void rev16(char *in);
void rev32(char *in);

#endif