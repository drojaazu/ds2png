#include "digitizer3.hpp"

using namespace std;

const ds_file get_ds_file(istream *indata) {
  const ds_file_header header = get_header(indata);

  // skip 0 padding, move to image dat
  indata->ignore(224);

  auto images = vector<ds_img>();
  images.reserve(header.img_count);
  for (u16 img_iter = 0; img_iter < header.img_count; img_iter++) {
    images.push_back(get_img(indata));
  }

  const ds_file _out = {.header = header, .images = images};

  return _out;
}

const ds_file_header get_header(istream *indata) {
  u32 ver = 0;
  u16 unk1 = 0;
  u16 img_count = 0;
  u32 data_size = 0;
  u32 unk2 = 0;
  char id[16];

  indata->read((char *)&ver, sizeof(ver));
  ver = htonl(ver);

  indata->read((char *)&unk1, sizeof(unk1));
  unk1 = htons(unk1);

  indata->read((char *)&img_count, sizeof(img_count));
  img_count = htons(img_count);

  indata->read((char *)&data_size, sizeof(data_size));
  data_size = htonl(data_size);

  indata->read((char *)&unk2, sizeof(unk2));
  unk2 = htonl(unk2);

  indata->read(id, sizeof(id));

  string temp(begin(id), end(id));
  if (temp.find("DIGITIZER") == string::npos)
    throw runtime_error(
        "Input does not appear to be a valid Digitizer III file!");

  ds_file_header _out = {.ver = ver,
                         .unk1 = unk1,
                         .img_count = img_count,
                         .data_size = data_size,
                         .unk2 = unk2};

  memcpy(_out.id, id, sizeof(id));

  return _out;
}

const ds_img get_img(istream *indata) {
  // read image header first
  u16 unk1;
  u16 data_offset;
  u16 pos_x;
  u16 pos_y;
  u16 width;
  u16 height;
  u16 frame_time;
  u16 img_index;
  char img_name[16];

  indata->read((char *)&unk1, sizeof(unk1));
  unk1 = htons(unk1);

  indata->read((char *)&data_offset, sizeof(data_offset));
  data_offset = htons(data_offset);

  indata->read((char *)&pos_x, sizeof(pos_x));
  pos_x = htons(pos_x);

  indata->read((char *)&pos_y, sizeof(pos_y));
  pos_y = htons(pos_y);

  indata->read((char *)&width, sizeof(width));
  width = htons(width);

  indata->read((char *)&height, sizeof(height));
  height = htons(height);

  indata->read((char *)&frame_time, sizeof(frame_time));
  frame_time = htons(frame_time);

  indata->read((char *)&img_index, sizeof(img_index));
  img_index = htons(img_index);

  indata->read(img_name, sizeof(img_name));

  ds_img_header header = {.unk1 = unk1,
                          .data_offset = data_offset,
                          .pos_x = pos_x,
                          .pos_y = pos_y,
                          .width = width,
                          .height = height,
                          .frame_time = frame_time,
                          .img_index = img_index};

  memcpy(header.img_name, img_name, sizeof(img_name));

  // data offset is relative to start of img data
  // header size is 0x20
  u16 palsize = (header.data_offset - 0x20) / 4;
  const auto palette = get_palette(indata, palsize);

  const auto bmp_data = new u16[header.width * header.height];

  for (u16 data_iter = 0; data_iter < (header.width * header.height);
       data_iter++) {
    indata->read((char *)&bmp_data[data_iter], 2);
    bmp_data[data_iter] = htons(bmp_data[data_iter]);
  }

  const ds_img _out = {
      .header = header, .palette = palette, .bmp_data = bmp_data};

  return _out;
}

const vector<ds_color> get_palette(istream *indata, u16 count) {
  vector<ds_color> _out;
  _out.reserve(count);
  ds_color this_color;
  for (u16 color_iter = 0; color_iter < count; color_iter++) {
    indata->read((char *)&(this_color.color_id), sizeof(this_color.color_id));
    this_color.color_id = htons(this_color.color_id);
    indata->read((char *)&(this_color.color), sizeof(this_color.color));
    // this_color.color = htons(this_color.color);
    _out.push_back(this_color);
  }
  return _out;
}
