#pragma once

#include  <fx/gltf.h>

void appendModelToGltfDocument (fx::gltf::Document &pDocument,
                                Urho3D::SharedPtr<Urho3D::Model> &pModel) {
    auto vertex_buffers = pModel->GetVertexBuffers();
    auto descr = vertex_buffers[0]->GetElements();
    for (auto el : descr) {
        URHO3D_LOGINFO(std::to_string(int(el.index_)).c_str());
    }
    auto index_buffers = pModel->GetIndexBuffers();

    const uint32_t vertex_size = vertex_buffers[0]->GetVertexSize();
    const uint32_t vertex_count = vertex_buffers[0]->GetVertexCount();
    const uint32_t vertex_data_size = vertex_count * vertex_size;

    const uint32_t index_count = index_buffers[0]->GetIndexCount();
    const uint32_t index_data_size =
        index_count * index_buffers[0]->GetIndexSize();

    // TODO3: only 1 buffer supported for now
    if (pDocument.buffers.size() == 0) {
        pDocument.buffers.push_back(fx::gltf::Buffer{});
    }
    auto& buffer0 = pDocument.buffers[0];

    const uint32_t space_left =
        std::numeric_limits<uint32_t>::max() - buffer0.data.size();
    if ((vertex_data_size + index_data_size) > space_left) {
        throw std::runtime_error(
            "fx::gltf::Document::buffer[0] got too big, sorry");
    }



    const uint32_t buffer0_prev_size = pDocument.buffers[0].byteLength;
    const uint32_t buffer0_new_size =
        buffer0_prev_size + vertex_data_size + index_data_size;
    assert(buffer0.byteLength == buffer0.data.size());

    buffer0.data.resize(buffer0_new_size);
    buffer0.byteLength = buffer0_new_size;

    std::memcpy(buffer0.data.data() + buffer0_prev_size,
                vertex_buffers[0]->GetShadowData(),
                vertex_data_size);

    fx::gltf::BufferView view0;
    view0.name = "nonameview0";
    view0.buffer = 0;
    view0.byteOffset = buffer0_prev_size;
    view0.byteLength = vertex_data_size;
    view0.byteStride = vertex_size;
    view0.target = fx::gltf::BufferView::TargetType::ArrayBuffer;

    std::memcpy(buffer0.data.data() + buffer0_prev_size + vertex_data_size,
                index_buffers[0]->GetShadowData(),
                index_data_size);

    const uint32_t view0_pos = pDocument.bufferViews.size();
    pDocument.bufferViews.push_back(view0);

    fx::gltf::BufferView view1;
    view1.name = "nonameview1";
    view1.buffer = 0;
    view1.byteOffset = buffer0_prev_size + vertex_data_size;
    view1.byteLength = index_data_size;
    view1.byteStride = 0;
    view1.target = fx::gltf::BufferView::TargetType::ElementArrayBuffer;

    const uint32_t view1_pos = pDocument.bufferViews.size();
    pDocument.bufferViews.push_back(view1);

    auto bb = pModel->GetBoundingBox();

    fx::gltf::Accessor accessor_positions;
    accessor_positions.bufferView = view0_pos;
    accessor_positions.byteOffset = 0;
    accessor_positions.count = vertex_count;
    accessor_positions.normalized = false;
    accessor_positions.componentType = fx::gltf::Accessor::ComponentType::Float;
    accessor_positions.type = fx::gltf::Accessor::Type::Vec3;
    accessor_positions.name = "accessor_vertex_positions";
    accessor_positions.max = { bb.max_.x_, bb.max_.y_, bb.max_.z_ };
    accessor_positions.min = { bb.min_.x_, bb.min_.y_, bb.min_.z_ };

    const uint32_t accessor_positions_pos = pDocument.accessors.size();
    pDocument.accessors.push_back(accessor_positions);

    fx::gltf::Accessor accessor_normals;
    accessor_normals.bufferView = view0_pos;
    accessor_normals.byteOffset = 3*4;
    accessor_normals.count = vertex_count;
    accessor_normals.normalized = false;
    accessor_normals.componentType = fx::gltf::Accessor::ComponentType::Float;
    accessor_normals.type = fx::gltf::Accessor::Type::Vec3;
    accessor_normals.name = "nonameaccessor_vertex_normals";
    accessor_normals.max = { 1, 1, 1 };
    accessor_normals.min = { -1, -1, -1 };

    const uint32_t accessor_normals_pos = pDocument.accessors.size();
    pDocument.accessors.push_back(accessor_normals);

    fx::gltf::Accessor accessor_texcoords;
    accessor_texcoords.bufferView = view0_pos;
    accessor_texcoords.byteOffset = 24;
    accessor_texcoords.count = vertex_count;
    accessor_texcoords.normalized = false;
    accessor_texcoords.componentType = fx::gltf::Accessor::ComponentType::Float;
    accessor_texcoords.type = fx::gltf::Accessor::Type::Vec2;
    accessor_texcoords.name = "nonameaccessor_texcoords";
    accessor_texcoords.max = { 1, 1 };
    accessor_texcoords.min = { 0, 0 };

    const uint32_t accessor_texcoords_pos = pDocument.accessors.size();
    pDocument.accessors.push_back(accessor_texcoords);


    fx::gltf::Accessor accessor_indices;
    accessor_indices.bufferView = view1_pos;
    accessor_indices.byteOffset = 0;
    accessor_indices.count = index_count;
    accessor_indices.normalized = false;
    accessor_indices.componentType = fx::gltf::Accessor::ComponentType::UnsignedShort;
    accessor_indices.type = fx::gltf::Accessor::Type::Scalar;
    accessor_indices.name = "nonameaccessor_indices_indices";
    accessor_indices.max = { bb.max_.x_, bb.max_.y_, bb.max_.z_ };
    accessor_indices.min = { bb.min_.x_, bb.min_.y_, bb.min_.z_ };

    const uint32_t accessor_indices_pos = pDocument.accessors.size();
    pDocument.accessors.push_back(accessor_indices);

    // TODO fixed vertex attributes

    fx::gltf::Primitive primitive;
    primitive.indices = accessor_indices_pos;
    primitive.attributes = {
                            { "POSITION", accessor_positions_pos }, // # of Accessor
                            { "NORMAL", accessor_normals_pos },
                            { "TEXCOORD_0", accessor_texcoords_pos },
    };

    fx::gltf::Mesh mesh;
    mesh.name = "meshname";
    mesh.primitives = { primitive };
    pDocument.meshes.push_back(mesh);

    fx::gltf::Node node;
    node.mesh = pDocument.meshes.size() - 1;
    pDocument.nodes.push_back(node);
}
