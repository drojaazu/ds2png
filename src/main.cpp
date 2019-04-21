/*
      Sega Digitizer System data to PNG converter
      Based on work done by Rich Whitehouse

      Author: Damian Rogers
      https://github.com/RyogaMasaki/ds2png
*/

#include "main.hpp"

using namespace png;
using namespace std;

const string version = string("1.0");

istream *infile = nullptr;

bool trns = false;

int main(int argc, char **argv) {
  process_args(argc, argv);

  auto infofile = new ofstream("ds3_info.txt");

  try {
    if (infile == nullptr) {
      infile = &cin;
    } else {
      if (!infile->good()) {
        cerr << "Input file could not be opened" << endl;
        return 1;
      }
    }

    auto workfile = get_ds_file(infile);

    *infofile << showbase << internal << setfill('0');

    gen_file_info(&workfile, infofile);

    for (u16 dsimg_iter = 0; dsimg_iter < workfile.header.img_count;
         dsimg_iter++) {
      ds_img this_dsimg = workfile.images.at(dsimg_iter);

      *infofile << "Frame #" << (dsimg_iter + 1) << endl;
      *infofile << "---------" << endl;
      gen_img_info(&this_dsimg, infofile);

      auto outimg = new image<index_pixel>(this_dsimg.header.width,
                                           this_dsimg.header.height);

      auto outpal = make_png_pal(this_dsimg.palette);
      outimg->set_palette(outpal);

      tRNS trans;
      if (trns) {
        // set first palette entry to transparent
        trans.push_back(0);
        outimg->set_tRNS(trans);
      }

      auto pixbuf = outimg->get_pixbuf();

      auto pal_map = make_pal_entry_map(this_dsimg.palette);
      u32 pxl_count = 0;
      for (u16 line_iter = 0; line_iter < this_dsimg.header.height;
           line_iter++) {
        vector<index_pixel> this_line;
        for (u16 col_iter = 0; col_iter < this_dsimg.header.width; col_iter++) {
          u8 this_pxl = pal_map.at(this_dsimg.bmp_data[pxl_count++]);
          this_line.push_back(this_pxl);
        }
        pixbuf.put_row(line_iter, this_line);
      }
      outimg->set_pixbuf(pixbuf);

      ostringstream outfilename;
      string img_name(begin(this_dsimg.header.img_name),
                      end(this_dsimg.header.img_name));

      img_name.erase(img_name.find_last_not_of(" ") + 1);
      outfilename << internal << setfill('0') << img_name << "_" << setw(2)
                  << dsimg_iter << ".png";

      outimg->write(outfilename.str());
      delete outimg;
    }

    infofile->close();
  } catch (const exception &e) {
    cerr << "Fatal error: " << e.what() << endl;
    if (infile != &cin) delete infile;
    delete infofile;
    return -1;
  }

  if (infile != &cin) delete infile;
  delete infofile;
  return 0;
}

void gen_file_info(ds_file *workfile, ofstream *info_file) {
  *info_file << "Sega Digitizer System 3 file" << endl;
  *info_file << "----------------------------" << endl;
  // id is not \0 terminated, so we'll make it a string
  string id_string(begin(workfile->header.id), end(workfile->header.id));
  *info_file << "ID string: " << id_string << endl;
  *info_file << "Unknown1: " << hex << setw(4) << workfile->header.unk1 << dec
             << endl;
  *info_file << "Number of frames: " << workfile->header.img_count << endl;
  *info_file << "Data size: " << hex << setw(8) << workfile->header.data_size
             << dec << endl;
  *info_file << "Unknown2: " << hex << setw(8) << workfile->header.unk2 << dec
             << endl;
  *info_file << endl;
}

void gen_img_info(ds_img *img, ofstream *info_file) {
  *info_file << "Unknown1: " << hex << setw(4) << img->header.unk1 << dec
             << endl;
  *info_file << "Data offset: " << hex << setw(8) << img->header.data_offset
             << dec << endl;
  *info_file << "X Offset: " << img->header.pos_x << endl;
  *info_file << "Y Offset: " << img->header.pos_y << endl;
  *info_file << "Width: " << img->header.width << endl;
  *info_file << "Height: " << img->header.height << endl;
  *info_file << "Frame time: " << img->header.frame_time << endl;
  *info_file << "Frame index: " << img->header.img_index << endl;
  string img_name(begin(img->header.img_name), end(img->header.img_name));
  *info_file << "Frame name: " << img_name << endl;
  *info_file << endl;
}

map<u16, u8> make_pal_entry_map(vector<ds_color> ds_palette) {
  map<u16, u8> _out;
  for (u16 pal_iter = 0; pal_iter < ds_palette.size(); pal_iter++) {
    ds_color this_color = ds_palette.at(pal_iter);
    _out.insert(pair<u16, u8>(this_color.color_id, pal_iter));
  }

  return _out;
}

vector<color> make_png_pal(vector<ds_color> ds_colors) {
  vector<color> _out;

  for (u16 color_iter = 0; color_iter < ds_colors.size(); color_iter++) {
    u16 this_color = ds_colors.at(color_iter).color;

    u8 red = (this_color & 0x0F00) >> 8;
    // duplicate the data to 'stretch' the color to 8 bits
    red = (red << 4) | red;

    u8 green = (this_color & 0xF000) >> 8;
    green = (green >> 4) | green;

    u8 blue = this_color & 0x000F;
    blue = (blue << 4) | blue;

    _out.push_back(color(red, green, blue));
  }

  return _out;
}

void process_args(int argc, char **argv) {
  const char *const shortOpts = ":i:t";
  const option longOpts[] = {{"input", required_argument, nullptr, 'i'},
                             {nullptr, 0, nullptr, 0}};

  int this_opt = 0;

  while (true) {
    this_opt = getopt_long(argc, argv, shortOpts, longOpts, nullptr);

    if (this_opt == -1) break;

    switch (this_opt) {
      // input
      case 'i':
        infile = new ifstream(optarg);
        break;

      case 't':
        trns = true;
        break;

      case ':':
        cerr << "Missing arg for option: " << (char)optopt << endl;
        exit(1);
        break;
      case '?':
        cerr << "Unknown option: " << (char)optopt << endl;

      default:
        print_help();
        exit(1);
        break;
    }
  }
}

void print_help() {
  cerr << "ds2png version " << version << endl << endl;
  cerr << "Valid options:" << endl;
  cerr << "  --input, -i   Specify input Digitizer file" << endl;
  cerr << "                Otherwise, data will be read from stdin" << endl;
  cerr << "  --trns, -t    Use palette entry 0 as transparency" << endl;
}