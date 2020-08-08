#ifndef DARKSTARDTSCONVERTER_STRUCTURES_HPP
#define DARKSTARDTSCONVERTER_STRUCTURES_HPP

#include <variant>
#include <array>
#include "endian_arithmetic.hpp"

namespace darkstar::dts
{
  namespace endian = boost::endian;
  using file_tag = std::array<std::byte, 4>;

  template<std::size_t Size>
  constexpr std::array<std::string_view, Size> make_keys(const char *(&&keys)[Size])
  {
    std::array<std::string_view, Size> result;
    for (auto i = 0; i < Size; i++)
    {
      result[i] = keys[i];
    }
    return result;
  }

  constexpr file_tag to_tag(const std::array<std::uint8_t, 4> values)
  {
    file_tag result{};

    for (auto i = 0u; i < values.size(); i++)
    {
      result[i] = std::byte{ values[i] };
    }
    return result;
  }

  constexpr file_tag pers_tag = to_tag({ 'P', 'E', 'R', 'S' });

  using version = endian::little_int32_t;

  struct file_info
  {
    endian::little_int32_t file_length;
    endian::little_int16_t class_name_length;
  };

  struct tag_header
  {
    dts::file_tag tag;
    dts::file_info file_info;
    std::vector<std::byte> class_name;
    dts::version version;
  };

  struct vector3f
  {
    constexpr static auto keys = make_keys({ "x", "y", "z" });
    float x;
    float y;
    float z;
  };

  struct vector3f_pair
  {
    constexpr static auto keys = make_keys({ "min", "max" });
    vector3f min;
    vector3f max;
  };

  static_assert(sizeof(vector3f) == sizeof(std::array<float, 3>));

  struct quaternion4s
  {
    constexpr static auto keys = make_keys({ "x", "y", "z", "w" });
    endian::little_int16_t x;
    endian::little_int16_t y;
    endian::little_int16_t z;
    endian::little_int16_t w;
  };

  struct quaternion4f
  {
    constexpr static auto keys = make_keys({ "x", "y", "z", "w" });
    float x;
    float y;
    float z;
    float w;
  };

  static_assert(sizeof(quaternion4s) == sizeof(std::array<endian::little_int16_t, 4>));

  struct rgb_data
  {
    constexpr static auto keys = make_keys({ "red", "green", "blue", "rgbFlags" });

    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t rgb_flags;
  };

  namespace shape::v2
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numNodes",
        "numSequences",
        "numSubSequences",
        "numKeyFrames",
        "numTransforms",
        "numNames",
        "numObjects",
        "numDetails",
        "numMeshes",
        "numTransitions" });

      endian::little_int32_t num_nodes;
      endian::little_int32_t num_sequences;
      endian::little_int32_t num_sub_sequences;
      endian::little_int32_t num_key_frames;
      endian::little_int32_t num_transforms;
      endian::little_int32_t num_names;
      endian::little_int32_t num_objects;
      endian::little_int32_t num_details;
      endian::little_int32_t num_meshes;
      endian::little_int32_t num_transitions;
    };

    struct keyframe
    {
      constexpr static auto keys = make_keys({ "position", "keyValue" });
      float position;
      endian::little_uint32_t key_value;
    };

    struct sequence
    {
      constexpr static auto keys = make_keys({ "nameIndex",
        "cyclic",
        "duration",
        "priority" });
      endian::little_int32_t name_index;
      endian::little_int32_t cyclic;
      float duration;
      endian::little_int32_t priority;
    };
  }// namespace shape::v2

  namespace shape::v5
  {
    struct footer
    {
      constexpr static auto keys = make_keys({ "numDefaultMaterials" });
      endian::little_int32_t num_default_materials;
    };
  }// namespace shape::v5


  namespace shape::v6
  {
    struct transform
    {
      constexpr static auto keys = make_keys({ "rotation", "translation", "scale" });
      quaternion4f rotation;
      vector3f translation;
      vector3f scale;
    };

    struct transition
    {
      constexpr static auto keys = make_keys({ "startSequence",
        "endSequence",
        "startPosition",
        "endPosition",
        "duration",
        "transform" });
      endian::little_int32_t start_sequence;
      endian::little_int32_t end_sequence;
      float start_position;
      float end_position;
      float duration;
      transform transform;
    };
  }// namespace shape::v6

  namespace shape::v7
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numNodes",
        "numSequences",
        "numSubSequences",
        "numKeyFrames",
        "numTransforms",
        "numNames",
        "numObjects",
        "numDetails",
        "numMeshes",
        "numTransitions",
        "numFrameTriggers" });

      endian::little_int32_t num_nodes;
      endian::little_int32_t num_sequences;
      endian::little_int32_t num_sub_sequences;
      endian::little_int32_t num_key_frames;
      endian::little_int32_t num_transforms;
      endian::little_int32_t num_names;
      endian::little_int32_t num_objects;
      endian::little_int32_t num_details;
      endian::little_int32_t num_meshes;
      endian::little_int32_t num_transitions;
      endian::little_int32_t num_frame_triggers;
    };

    struct data
    {
      constexpr static auto keys = make_keys({ "radius", "centre" });
      float radius;
      vector3f centre;
    };

    static_assert(sizeof(data) == sizeof(std::array<float, 4>));

    struct node
    {
      constexpr static auto keys = make_keys({ "name", "parent", "numSubSequences", "firstSubSequence", "defaultTransform" });
      endian::little_int32_t name;
      endian::little_int32_t parent;
      endian::little_int32_t num_sub_sequences;
      endian::little_int32_t first_sub_sequence;
      endian::little_int32_t default_transform;
    };

    struct sequence
    {
      constexpr static auto keys = make_keys({ "nameIndex",
        "cyclic",
        "duration",
        "priority",
        "firstFrameTrigger",
        "numFrameTriggers",
        "numIflSubSequences",
        "firstIflSubSequence" });
      endian::little_int32_t name_index;
      endian::little_int32_t cyclic;
      float duration;
      endian::little_int32_t priority;
      endian::little_int32_t first_frame_trigger;
      endian::little_int32_t num_frame_triggers;
      endian::little_int32_t num_ifl_sub_sequences;
      endian::little_int32_t first_ifl_sub_sequence;
    };

    struct sub_sequence
    {
      constexpr static auto keys = make_keys({ "sequenceIndex", "numKeyFrames", "firstKeyFrame" });
      endian::little_int32_t sequence_index;
      endian::little_int32_t num_key_frames;
      endian::little_int32_t first_key_frame;
    };


    struct keyframe
    {
      constexpr static auto keys = make_keys({ "position", "keyValue", "matIndex" });
      float position;
      endian::little_uint32_t key_value;
      endian::little_uint32_t mat_index;
    };

    struct transform
    {
      constexpr static auto keys = make_keys({ "rotation", "translation", "scale" });
      quaternion4s rotation;
      vector3f translation;
      vector3f scale;
    };

    static_assert(sizeof(transform) == sizeof(std::array<std::int32_t, 8>));

    using name = std::array<char, 24>;

    struct object
    {
      constexpr static auto keys = make_keys({ "nameIndex", "flags", "meshIndex", "nodeIndex", "depFlags", "dep", "objectOffset", "numSubSequences", "firstSubSequence" });
      endian::little_int16_t name_index;
      endian::little_int16_t flags;
      endian::little_int32_t mesh_index;
      endian::little_int32_t node_index;
      endian::little_int32_t dep_flags;
      std::array<vector3f, 3> dep;
      vector3f object_offset;
      endian::little_int32_t num_sub_sequences;
      endian::little_int32_t first_sub_sequence;
    };

    struct detail
    {
      constexpr static auto keys = make_keys({ "nameIndex", "size" });
      endian::little_int32_t name_index;
      float size;
    };

    struct transition
    {
      constexpr static auto keys = make_keys({ "startSequence",
        "endSequence",
        "startPosition",
        "endPosition",
        "duration",
        "rotation",
        "translation",
        "scale" });
      endian::little_int32_t start_sequence;
      endian::little_int32_t end_sequence;
      float start_position;
      float end_position;
      float duration;
      quaternion4s rotation;
      vector3f translation;
      vector3f scale;
    };

    struct frame_trigger
    {
      constexpr static auto keys = make_keys({ "position",
        "value" });
      float position;
      float value;
    };

    struct footer
    {
      constexpr static auto keys = make_keys({ "numDefaultMaterials",
        "alwaysNode" });
      endian::little_int32_t num_default_materials;
      endian::little_int32_t always_node;
    };

    using has_material_list_flag = endian::little_int32_t;
  }// namespace shape::v7

  namespace shape::v8
  {
    struct data
    {
      constexpr static auto keys = make_keys({ "radius", "centre", "bounds" });
      float radius;
      vector3f centre;
      vector3f_pair bounds;
    };

    static_assert(sizeof(data) == sizeof(std::array<float, 10>));

    struct node
    {
      constexpr static auto keys = make_keys({ "name", "parent", "numSubSequences", "firstSubSequence", "defaultTransform" });
      endian::little_int16_t name;
      endian::little_int16_t parent;
      endian::little_int16_t num_sub_sequences;
      endian::little_int16_t first_sub_sequence;
      endian::little_int16_t default_transform;
    };

    struct sub_sequence
    {
      constexpr static auto keys = make_keys({ "sequenceIndex", "numKeyFrames", "firstKeyFrame" });
      endian::little_int16_t sequence_index;
      endian::little_int16_t num_key_frames;
      endian::little_int16_t first_key_frame;
    };


    struct keyframe
    {
      constexpr static auto keys = make_keys({ "position", "keyValue", "matIndex" });
      float position;
      endian::little_uint16_t key_value;
      endian::little_uint16_t mat_index;
    };

    struct transform
    {
      constexpr static auto keys = make_keys({ "rotation", "translation" });
      quaternion4s rotation;
      vector3f translation;
    };

    using name = std::array<char, 24>;

    struct object
    {
      constexpr static auto keys = make_keys({ "nameIndex", "flags", "meshIndex", "nodeIndex",
                                              "objectOffset", "numSubSequences", "firstSubSequence" });
      endian::little_int16_t name_index;
      endian::little_int16_t flags;
      endian::little_int32_t mesh_index;
      endian::little_int16_t node_index;
      vector3f object_offset;
      endian::little_int16_t num_sub_sequences;
      endian::little_int16_t first_sub_sequence;
    };

    struct transition
    {
      constexpr static auto keys = make_keys({ "startSequence",
        "endSequence",
        "startPosition",
        "endPosition",
        "duration",
        "transformation" });
      endian::little_int32_t start_sequence;
      endian::little_int32_t end_sequence;
      float start_position;
      float end_position;
      float duration;
      transform transformation;
    };

    using has_material_list_flag = endian::little_int32_t;
  }// namespace shape::v8

  namespace mesh::v1
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numVerts",
        "vertsPerFrame",
        "numTextureVerts",
        "numFaces",
        "numFrames",
        "scale",
        "origin",
        "radius" });

      endian::little_int32_t num_verts;
      endian::little_int32_t verts_per_frame;
      endian::little_int32_t num_texture_verts;
      endian::little_int32_t num_faces;
      endian::little_int32_t num_frames;
      vector3f scale;
      vector3f origin;
      float radius;
    };
  }// namespace mesh::v1

  namespace mesh::v2
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numVerts",
        "vertsPerFrame",
        "numTextureVerts",
        "numFaces",
        "numFrames",
        "textureVertsPerFrame",
        "scale",
        "origin",
        "radius" });

      endian::little_int32_t num_verts;
      endian::little_int32_t verts_per_frame;
      endian::little_int32_t num_texture_verts;
      endian::little_int32_t num_faces;
      endian::little_int32_t num_frames;
      endian::little_int32_t texture_verts_per_frame;
      vector3f scale;
      vector3f origin;
      float radius;
    };

    struct vertex
    {
      constexpr static auto keys = make_keys({ "x", "y", "z", "normal" });
      std::uint8_t x;
      std::uint8_t y;
      std::uint8_t z;
      std::uint8_t normal;
    };

    static_assert(sizeof(vertex) == sizeof(std::int32_t));

    struct texture_vertex
    {
      constexpr static auto keys = make_keys({ "x", "y" });
      float x;
      float y;
    };

    struct face
    {
      constexpr static auto keys = make_keys({ "vi1", "ti1", "vi2", "ti2", "vi3", "ti3", "material" });
      endian::little_int32_t vi1;
      endian::little_int32_t ti1;
      endian::little_int32_t vi2;
      endian::little_int32_t ti2;
      endian::little_int32_t vi3;
      endian::little_int32_t ti3;
      endian::little_int32_t material;
    };

    struct frame
    {
      constexpr static auto keys = make_keys({ "firstVert" });
      endian::little_int32_t first_vert;
    };
  }// namespace mesh::v2

  namespace mesh::v3
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numVerts",
        "vertsPerFrame",
        "numTextureVerts",
        "numFaces",
        "numFrames",
        "textureVertsPerFrame",
        "radius" });

      endian::little_int32_t num_verts;
      endian::little_int32_t verts_per_frame;
      endian::little_int32_t num_texture_verts;
      endian::little_int32_t num_faces;
      endian::little_int32_t num_frames;
      endian::little_int32_t texture_verts_per_frame;
      float radius;
    };

    using mesh::v2::vertex;
    using mesh::v2::texture_vertex;
    using mesh::v2::face;

    struct frame
    {
      constexpr static auto keys = make_keys({ "firstVert", "scale", "origin" });
      endian::little_int32_t first_vert;
      vector3f scale;
      vector3f origin;
    };
  }// namespace mesh::v3

  namespace material_list::v2
  {
    struct material
    {
      constexpr static auto keys = make_keys({ "flags", "alpha", "index", "rgbData", "fileName" });

      endian::little_int32_t flags;
      float alpha;
      endian::little_int32_t index;
      rgb_data rgb_data;

      std::array<char, 32> file_name;
    };
  }// namespace material_list::v2

  namespace material_list::v3
  {
    struct header
    {
      constexpr static auto keys = make_keys({ "numDetails",
        "numMaterials" });
      endian::little_int32_t num_details;
      endian::little_int32_t num_materials;
    };

    struct material
    {
      constexpr static auto keys = make_keys({ "flags", "alpha", "index", "rgbData", "fileName", "type", "elasticity", "friction" });

      endian::little_int32_t flags;
      float alpha;
      endian::little_int32_t index;
      rgb_data rgb_data;

      std::array<char, 32> file_name;

      endian::little_int32_t type;
      float elasticity;
      float friction;
    };
  }// namespace material_list::v3

  struct mesh_v1
  {
    constexpr static auto version = 1;
    constexpr static auto keys = make_keys({ "header", "vertices", "textureVertices", "faces", "frames" });

    mesh::v1::header header;
    std::vector<mesh::v3::vertex> vertices;
    std::vector<mesh::v3::texture_vertex> texture_vertices;
    std::vector<mesh::v3::face> faces;
    std::vector<mesh::v2::frame> frames;
  };


  struct mesh_v2
  {
    constexpr static auto version = 2;
    constexpr static auto keys = make_keys({ "header", "vertices", "textureVertices", "faces", "frames" });

    mesh::v2::header header;
    std::vector<mesh::v3::vertex> vertices;
    std::vector<mesh::v3::texture_vertex> texture_vertices;
    std::vector<mesh::v3::face> faces;
    std::vector<mesh::v2::frame> frames;
  };

  struct mesh_v3
  {
    constexpr static auto version = 3;
    constexpr static auto keys = make_keys({ "header", "vertices", "textureVertices", "faces", "frames" });

    mesh::v3::header header;
    std::vector<mesh::v3::vertex> vertices;
    std::vector<mesh::v3::texture_vertex> texture_vertices;
    std::vector<mesh::v3::face> faces;
    std::vector<mesh::v3::frame> frames;
  };

  using mesh_variant = std::variant<mesh_v1, mesh_v2, mesh_v3>;

  struct material_list_v2
  {
    constexpr static auto version = 2;
    constexpr static auto keys = make_keys({ "header", "materials" });

    material_list::v3::header header;
    std::vector<material_list::v2::material> materials;
  };


  struct material_list_v3
  {
    constexpr static auto version = 3;
    constexpr static auto keys = make_keys({ "header", "materials" });

    material_list::v3::header header;
    std::vector<material_list::v3::material> materials;
  };

  using material_list_variant = std::variant<darkstar::dts::material_list_v2, darkstar::dts::material_list_v3>;

  struct shape_v2
  {
    constexpr static auto version = 2;
    constexpr static auto keys = make_keys({ "header",
      "data",
      "nodes",
      "sequences",
      "subSequences",
      "keyframes",
      "transforms",
      "names",
      "objects",
      "details",
      "transitions",
      "meshes",
      "materialList" });

    shape::v2::header header;
    shape::v7::data data;
    std::vector<shape::v7::node> nodes;
    std::vector<shape::v2::sequence> sequences;
    std::vector<shape::v7::sub_sequence> sub_sequences;
    std::vector<shape::v2::keyframe> keyframes;
    std::vector<shape::v6::transform> transforms;
    std::vector<shape::v7::name> names;
    std::vector<shape::v7::object> objects;
    std::vector<shape::v7::detail> details;
    std::vector<shape::v6::transition> transitions;
    std::vector<mesh_variant> meshes;

    material_list_variant material_list;
  };

  struct shape_v3
  {
    constexpr static auto version = 3;
    constexpr static auto keys = make_keys({ "header",
      "data",
      "nodes",
      "sequences",
      "subSequences",
      "keyframes",
      "transforms",
      "names",
      "objects",
      "details",
      "transitions",
      "meshes",
      "materialList" });

    shape::v2::header header;
    shape::v7::data data;
    std::vector<shape::v7::node> nodes;
    std::vector<shape::v2::sequence> sequences;
    std::vector<shape::v7::sub_sequence> sub_sequences;
    std::vector<shape::v7::keyframe> keyframes;
    std::vector<shape::v6::transform> transforms;
    std::vector<shape::v7::name> names;
    std::vector<shape::v7::object> objects;
    std::vector<shape::v7::detail> details;
    std::vector<shape::v6::transition> transitions;
    std::vector<mesh_variant> meshes;

    material_list_variant material_list;
  };

  struct shape_v5
  {
    constexpr static auto version = 5;
    constexpr static auto keys = make_keys({ "header",
      "data",
      "nodes",
      "sequences",
      "subSequences",
      "keyframes",
      "transforms",
      "names",
      "objects",
      "details",
      "transitions",
      "frameTriggers",
      "footer",
      "meshes",
      "materialList" });

    shape::v7::header header;
    shape::v7::data data;
    std::vector<shape::v7::node> nodes;
    std::vector<shape::v7::sequence> sequences;
    std::vector<shape::v7::sub_sequence> sub_sequences;
    std::vector<shape::v7::keyframe> keyframes;
    std::vector<shape::v6::transform> transforms;
    std::vector<shape::v7::name> names;
    std::vector<shape::v7::object> objects;
    std::vector<shape::v7::detail> details;
    std::vector<shape::v6::transition> transitions;
    std::vector<shape::v7::frame_trigger> frame_triggers;
    shape::v5::footer footer;
    std::vector<mesh_variant> meshes;

    material_list_variant material_list;
  };

  struct shape_v6
  {
    constexpr static auto version = 6;
    constexpr static auto keys = make_keys({ "header",
      "data",
      "nodes",
      "sequences",
      "subSequences",
      "keyframes",
      "transforms",
      "names",
      "objects",
      "details",
      "transitions",
      "frameTriggers",
      "footer",
      "meshes",
      "materialList" });

    shape::v7::header header;
    shape::v7::data data;
    std::vector<shape::v7::node> nodes;
    std::vector<shape::v7::sequence> sequences;
    std::vector<shape::v7::sub_sequence> sub_sequences;
    std::vector<shape::v7::keyframe> keyframes;
    std::vector<shape::v6::transform> transforms;
    std::vector<shape::v7::name> names;
    std::vector<shape::v7::object> objects;
    std::vector<shape::v7::detail> details;
    std::vector<shape::v6::transition> transitions;
    std::vector<shape::v7::frame_trigger> frame_triggers;
    shape::v7::footer footer;
    std::vector<mesh_variant> meshes;

    material_list_variant material_list;
  };

  struct shape_v7
  {
    constexpr static auto version = 7;
    constexpr static auto keys = make_keys({ "header",
      "data",
      "nodes",
      "sequences",
      "subSequences",
      "keyframes",
      "transforms",
      "names",
      "objects",
      "details",
      "transitions",
      "frameTriggers",
      "footer",
      "meshes",
      "materialList" });

    shape::v7::header header;
    shape::v7::data data;
    std::vector<shape::v7::node> nodes;
    std::vector<shape::v7::sequence> sequences;
    std::vector<shape::v7::sub_sequence> sub_sequences;
    std::vector<shape::v7::keyframe> keyframes;
    std::vector<shape::v7::transform> transforms;
    std::vector<shape::v7::name> names;
    std::vector<shape::v7::object> objects;
    std::vector<shape::v7::detail> details;
    std::vector<shape::v7::transition> transitions;
    std::vector<shape::v7::frame_trigger> frame_triggers;
    shape::v7::footer footer;
    std::vector<mesh_variant> meshes;

    material_list_variant material_list;
  };

  struct shape_v8
  {
    constexpr static auto version = 8;
    constexpr static auto keys = make_keys({ "header",
      "data",
      "nodes",
      "sequences",
      "subSequences",
      "keyframes",
      "transforms",
      "names",
      "objects",
      "details",
      "transitions",
      "frameTriggers",
      "footer",
      "meshes",
      "materialList" });

    shape::v7::header header;
    shape::v8::data data;
    std::vector<shape::v8::node> nodes;
    std::vector<shape::v7::sequence> sequences;
    std::vector<shape::v8::sub_sequence> sub_sequences;
    std::vector<shape::v8::keyframe> keyframes;
    std::vector<shape::v8::transform> transforms;
    std::vector<shape::v8::name> names;
    std::vector<shape::v8::object> objects;
    std::vector<shape::v7::detail> details;
    std::vector<shape::v8::transition> transitions;
    std::vector<shape::v7::frame_trigger> frame_triggers;
    shape::v7::footer footer;
    std::vector<mesh_variant> meshes;

    material_list_variant material_list;
  };

  using shape_variant = std::variant<shape_v2, shape_v3, shape_v5, shape_v6, shape_v7, shape_v8>;
}// namespace darkstar::dts

#endif//DARKSTARDTSCONVERTER_STRUCTURES_HPP