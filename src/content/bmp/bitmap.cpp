#include "bitmap.hpp"
#include <set>
#include <map>
#include <iostream>
#include <cmath>


namespace studio::content::bmp
{
  using file_tag = std::array<std::byte, 4>;

  constexpr file_tag bmp_alt1_tag = shared::to_tag<4>({ 'B', 'M', 'P', ' ' });

  constexpr file_tag bmp_alt2_tag = shared::to_tag<4>({ 'B', 'M', 'P', ':' });

  constexpr file_tag pbmp_tag = shared::to_tag<4>({ 'P', 'B', 'M', 'P' });

  constexpr file_tag pba_tag = shared::to_tag<4>({ 'P', 'B', 'M', 'A' });

  constexpr file_tag header_tag = shared::to_tag<4>({ 'h', 'e', 'a', 'd' });

  constexpr file_tag data_tag = shared::to_tag<4>({ 'd', 'a', 't', 'a' });

  constexpr file_tag detail_tag = shared::to_tag<4>({ 'D', 'E', 'T', 'L' });

  constexpr file_tag palette_tag = shared::to_tag<4>({ 'P', 'i', 'D', 'X' });

  constexpr std::array<std::byte, 2> windows_bmp_tag = { std::byte{ 66 }, std::byte{ 77 } };// BM

  constexpr std::array<std::byte, 2> special_reserved_tag = { std::byte{ 0xF7 }, std::byte{ 0xF5 } };// for palettes

  bool is_microsoft_bmp(std::basic_istream<std::byte>& raw_data)
  {
    file_tag header{};
    raw_data.read(reinterpret_cast<std::byte*>(&header), sizeof(header));

    raw_data.seekg(-int(sizeof(header)), std::ios::cur);

    return header != bmp_alt1_tag && header != bmp_alt2_tag &&
           header[0] == windows_bmp_tag[0] && header[1] == windows_bmp_tag[1];
  }

  template<typename AlignmentType, typename PixelType>
  [[maybe_unused]] std::size_t read_pixel_data(std::basic_istream<std::byte>& raw_data, std::vector<PixelType>& raw_pixels, std::int32_t width, std::int32_t height, std::int32_t bit_depth)
  {
    const auto x_stride = width * bit_depth / 8;
    const auto padding = shared::get_padding_size(x_stride, sizeof(AlignmentType));

    if (padding == 0)
    {
      raw_data.read(reinterpret_cast<std::byte*>(raw_pixels.data()), raw_pixels.size() * sizeof(PixelType));
      return raw_pixels.size();
    }
    else
    {
      std::vector<std::byte> padding_bytes(padding);

      auto pos = 0u;

      auto start = raw_data.tellg();
      for (auto i = 0u; i < height; ++i)
      {
        if (pos > raw_pixels.size())
        {
          break;
        }

        raw_data.read(reinterpret_cast<std::byte*>(raw_pixels.data()) + pos, x_stride);
        raw_data.read(padding_bytes.data(), padding_bytes.size());
        pos += x_stride;
      }

      return raw_data.tellg() - start;
    }
  }

  windows_bmp_data get_bmp_data(std::basic_istream<std::byte>& raw_data)
  {
    windows_bmp_header header{};
    raw_data.read(reinterpret_cast<std::byte*>(&header), sizeof(header));

    if (header.tag != windows_bmp_tag)
    {
      throw std::invalid_argument("File data is not BMP based.");
    }

    windows_bmp_info info{};

    raw_data.read(reinterpret_cast<std::byte*>(&info), sizeof(info));


    std::vector<pal::colour> colours;

    const auto num_pixels = info.width * info.height;
    std::vector<std::int32_t> indexes;
    if (info.bit_depth <= 8)
    {
      indexes.reserve(num_pixels);
      int num_colours = static_cast<int>(std::pow(float(2), info.bit_depth));
      colours.reserve(num_colours);

      for (auto i = 0; i < num_colours; ++i)
      {
        std::array<std::byte, 4> quad{};
        raw_data.read(quad.data(), sizeof(quad));
        colours.emplace_back(pal::colour{ quad[2], quad[1], quad[0], std::byte{ 255 } });
      }

      std::vector<std::byte> raw_pixels(num_pixels, std::byte{});

      read_pixel_data<std::int32_t>(raw_data, raw_pixels, info.width, info.height, info.bit_depth);

      std::transform(raw_pixels.begin(), raw_pixels.end(), std::back_inserter(indexes), [](auto value) {
        return static_cast<std::int32_t>(value);
      });
    }
    else if (info.bit_depth == 24)
    {
      colours.reserve(num_pixels);
      std::vector<std::array<std::byte, 3>> image_colours(num_pixels);
      read_pixel_data<std::int32_t>(raw_data, image_colours, info.width, info.height, info.bit_depth);

      std::transform(image_colours.begin(), image_colours.end(), std::back_inserter(colours), [](auto& colour) {
        return pal::colour{ colour[2], colour[1], colour[0], std::byte(0xFF) };
      });
    }

    return {
      header,
      info,
      colours,
      indexes
    };
  }

  void write_bmp_data(std::basic_ostream<std::byte>& raw_data, const std::vector<pal::colour>& colours, const std::vector<std::byte>& pixels, std::int32_t width, std::int32_t height, std::int32_t bit_depth)
  {
    windows_bmp_header header{};
    header.tag = windows_bmp_tag;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.offset = sizeof(header) + sizeof(windows_bmp_info) + int(colours.size()) * sizeof(pal::colour);

    windows_bmp_info info{ 0 };
    info.info_size = sizeof(info);
    info.width = width;
    info.height = height;
    info.planes = 1;
    info.bit_depth = bit_depth;
    info.compression = 0;
    info.image_size = width * height;

    const auto num_pixels = info.width * info.height * (info.bit_depth / 8);

    if (pixels.size() != num_pixels)
    {
      throw std::invalid_argument("The pixels vector does not have the correct number of pixels.");
    }

    header.file_size = header.offset + num_pixels;

    raw_data.write(reinterpret_cast<std::byte*>(&header), sizeof(header));
    raw_data.write(reinterpret_cast<std::byte*>(&info), sizeof(info));

    for (auto& colour : colours)
    {
      std::array<std::byte, 4> quad{ colour.blue, colour.green, colour.red, std::byte{ 0 } };
      raw_data.write(quad.data(), sizeof(quad));
    }

    raw_data.write(pixels.data(), pixels.size());
  }

  bool is_phoenix_bmp(std::basic_istream<std::byte>& raw_data)
  {
    std::array<std::byte, 4> header{};
    raw_data.read(header.data(), sizeof(header));

    raw_data.seekg(-int(sizeof(header)), std::ios::cur);

    return header == pbmp_tag;
  }

  pbmp_data get_pbmp_data(std::basic_istream<std::byte>& raw_data)
  {
    const auto start = std::size_t(raw_data.tellg());
    std::array<std::byte, 4> header{};
    endian::little_uint32_t file_size{};

    raw_data.read(header.data(), sizeof(header));
    raw_data.read(reinterpret_cast<std::byte*>(&file_size), sizeof(file_size));

    if (header != pbmp_tag)
    {
      throw std::invalid_argument("File data is not PBMP based. Offset is " + std::to_string(raw_data.tellg()));
    }

    pbmp_header bmp_header{};
    endian::little_uint32_t detail_levels{};
    endian::little_uint32_t palette_index{};
    std::vector<std::byte> pixels;

    auto end = start + file_size + sizeof(header) + sizeof(file_size) + sizeof(std::int32_t) + sizeof(std::array<std::int32_t, 6>);

    while (std::size_t(raw_data.tellg()) < end)
    {
      std::array<std::byte, 4> chunk_header{};
      endian::little_uint32_t chunk_size{};

      raw_data.read(chunk_header.data(), chunk_header.size());
      raw_data.read(reinterpret_cast<std::byte*>(&chunk_size), sizeof(chunk_size));

      if (chunk_header == header_tag)
      {
        raw_data.read(reinterpret_cast<std::byte*>(&bmp_header), sizeof(bmp_header));

        const auto num_pixels = bmp_header.width * bmp_header.height * (bmp_header.bit_depth / 8);
        pixels = std::vector<std::byte>(num_pixels, std::byte{});
      }
      else if (chunk_header == data_tag)
      {
        auto bytes_read = read_pixel_data<std::int32_t>(raw_data, pixels, bmp_header.width, bmp_header.height, bmp_header.bit_depth);
        // PBMP files contain mip maps of the main image.
        // For now, we won't worry about them.
        raw_data.seekg(chunk_size - bytes_read, std::ios::cur);
      }
      else if (chunk_header == detail_tag)
      {
        raw_data.read(reinterpret_cast<std::byte*>(&detail_levels), sizeof(detail_levels));
      }
      else if (chunk_header == palette_tag)
      {
        raw_data.read(reinterpret_cast<std::byte*>(&palette_index), sizeof(palette_index));
      }
      else if (chunk_header == pbmp_tag)
      {
        raw_data.seekg(-int(sizeof(file_size)), std::ios::cur);
        raw_data.seekg(-int(sizeof(pbmp_tag)), std::ios::cur);
        break;
      }
      else
      {
        if (chunk_size == 0)
        {
          break;
        }
        raw_data.seekg(chunk_size, std::ios::cur);
      }
    }

    return {
      bmp_header,
      detail_levels,
      palette_index,
      pixels
    };
  }

  void write_pbmp_data(std::basic_ofstream<std::byte>& raw_data, std::int32_t width, std::int32_t height, const std::vector<pal::colour>& colours, const std::vector<std::byte>& pixels)
  {
    raw_data.write(pbmp_tag.data(), sizeof(pbmp_tag));

    auto file_size_pos = raw_data.tellp();

    endian::little_int32_t file_size = 0;
    raw_data.write(reinterpret_cast<std::byte*>(&file_size), sizeof(file_size));

    raw_data.write(header_tag.data(), sizeof(header_tag));
    endian::little_int32_t header_size = sizeof(pbmp_header);
    raw_data.write(reinterpret_cast<std::byte*>(&header_size), sizeof(header_size));
    pbmp_header header{};
    header.version = 3;
    header.width = width;
    header.height = height;
    header.bit_depth = 8;
    header.flags = 8;

    raw_data.write(reinterpret_cast<std::byte*>(&header), sizeof(header));

    auto pal_bytes = pal::write_pal_data(raw_data, colours);

    raw_data.write(data_tag.data(), sizeof(data_tag));
    header_size = std::int32_t(pixels.size());
    raw_data.write(reinterpret_cast<std::byte*>(&header_size), sizeof(header_size));
    raw_data.write(pixels.data(), pixels.size());

    raw_data.write(detail_tag.data(), sizeof(detail_tag));
    header_size = sizeof(std::int32_t);
    raw_data.write(reinterpret_cast<std::byte*>(&header_size), sizeof(header_size));
    endian::little_int32_t num_details = 1;
    raw_data.write(reinterpret_cast<std::byte*>(&num_details), sizeof(num_details));

    raw_data.write(palette_tag.data(), sizeof(palette_tag));
    header_size = sizeof(std::int32_t);
    raw_data.write(reinterpret_cast<std::byte*>(&header_size), sizeof(header_size));
    endian::little_int32_t palette_index = 1;
    raw_data.write(reinterpret_cast<std::byte*>(&palette_index), sizeof(palette_index));

    raw_data.seekp(file_size_pos, std::ios::beg);

    file_size = std::int32_t(sizeof(std::int32_t) + sizeof(pbmp_header) + pal_bytes + sizeof(std::array<std::int32_t, 2>) + pixels.size());
    raw_data.write(reinterpret_cast<std::byte*>(&file_size), sizeof(file_size));
  }

  bool is_phoenix_bmp_array(std::basic_istream<std::byte>& raw_data)
  {
    std::array<std::byte, 4> header{};
    raw_data.read(header.data(), sizeof(header));

    raw_data.seekg(-int(sizeof(header)), std::ios::cur);

    return header == pba_tag;
  }

  std::vector<pbmp_data> get_pba_data(std::basic_istream<std::byte>& raw_data)
  {
    std::vector<pbmp_data> results;

    std::array<std::byte, 4> header{};
    endian::little_uint32_t count{};

    raw_data.read(header.data(), sizeof(header));
    raw_data.read(reinterpret_cast<std::byte*>(&count), sizeof(count));

    if (header != pba_tag)
    {
      throw std::invalid_argument("File data is not PBA based.");
    }

    raw_data.read(header.data(), sizeof(header));

    raw_data.read(reinterpret_cast<std::byte*>(&count), sizeof(count));

    // Count appears twice in the files. Or it means something else. Haven't seen a file where it is something else.
    raw_data.read(reinterpret_cast<std::byte*>(&count), sizeof(count));
    raw_data.read(reinterpret_cast<std::byte*>(&count), sizeof(count));

    results.reserve(count);

    for (auto i = 0u; i < count; ++i)
    {
      results.emplace_back(get_pbmp_data(raw_data));
    }

    return results;
  }

  template<typename IndexType>
  std::vector<IndexType> remap_bitmap(const std::vector<IndexType>& pixels,
                                         const std::vector<pal::colour>& original_colours,
                                         const std::vector<pal::colour>& other_colours,
                                         bool only_unique)
  {
    if (pixels.empty())
    {
      return pixels;
    }

    if (original_colours == other_colours)
    {
      return pixels;
    }

    std::vector<IndexType> results;
    results.reserve(pixels.size());
    std::map<std::size_t, std::size_t> palette_map;

    for (auto x = 0u; x < original_colours.size(); x++)
    {
      auto& colour = original_colours[x];
      auto result = std::find_if(other_colours.begin(), other_colours.end(), [&](auto& other) {
             return other.red == colour.red && other.green == colour.green && other.blue == colour.blue;
      });

      if (result != other_colours.end())
      {
        auto other_index = std::distance(other_colours.begin(), result);
        palette_map[x] = other_index;
      }
      else
      {
        std::map<double, std::size_t> distances;
        for (auto y = 0u; y < other_colours.size(); y++)
        {
          auto& other = other_colours[y];
          auto distance = pal::colour_distance(colour, other);
          distances[distance] = y;
        }

        if (only_unique)
        {
          for (auto distance : distances)
          {
            auto already_exists = std::find_if(palette_map.begin(), palette_map.end(), [&](auto& elem) {
                   return elem.second == distance.second;
            });

            if (already_exists == palette_map.end())
            {
              palette_map[x] = distance.second;
              break;
            }
          }

          if (auto inserted = palette_map.find(x); inserted == palette_map.end())
          {
            palette_map[x] = distances.begin()->second;
          }
        }
        else
        {
          palette_map[x] = distances.begin()->second;
        }
      }
    }

    for (auto pixel : pixels)
    {
      auto new_index = palette_map[std::size_t(pixel)];
      results.emplace_back(IndexType(new_index));
    }

    return results;
  }

  std::vector<std::byte> remap_bitmap(const std::vector<std::byte>& pixels,
                                         const std::vector<pal::colour>& original_colours,
                                         const std::vector<pal::colour>& other_colours,
                                         bool only_unique)
  {
    return remap_bitmap<std::byte>(pixels, original_colours, other_colours, only_unique);
  }


  std::vector<std::int32_t> remap_bitmap(const std::vector<std::int32_t>& pixels,
    const std::vector<pal::colour>& original_colours,
    const std::vector<pal::colour>& other_colours,
    bool only_unique)
  {
    return remap_bitmap<std::int32_t>(pixels, original_colours, other_colours, only_unique);
  }
}// namespace studio::content::bmp
