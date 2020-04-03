#pragma once

//TODO:
#include  <fx/gltf.h>

// TODO3: too much ident
//void addVertexBuffer (fx::gltf::Document& pDocument,
//                      Urho3D::VertexBuffer& pVertexBuffer) {
//}

fx::gltf::Document fromUrho (Urho3D::SharedPtr<Urho3D::Model> &pModel) {
        fx::gltf::Document exp;
        auto vertex_buffers = pModel->GetVertexBuffers();
        auto index_buffers = pModel->GetIndexBuffers();

        const uint32_t vertex_size = vertex_buffers[0]->GetVertexSize();
        const uint32_t vertex_count = vertex_buffers[0]->GetVertexCount();
        const uint32_t vertex_data_size = vertex_count * vertex_size;

        const uint32_t index_count = index_buffers[0]->GetIndexCount();
        const uint32_t index_data_size =
            index_count * index_buffers[0]->GetIndexSize();

        exp.buffers.push_back(fx::gltf::Buffer{});
        // TODO: only 1 buffer
        exp.buffers[0].data.resize(vertex_data_size + index_data_size); 
        exp.buffers[0].byteLength = exp.buffers[0].data.size();
        std::memcpy(exp.buffers[0].data.data(),
                    vertex_buffers[0]->GetShadowData(),
                    vertex_data_size);

        exp.bufferViews.push_back(
            // name, buffer, offset, length, stride, target-type
            fx::gltf::BufferView{ "noname",
                                  0,
                                  0,
                                  vertex_data_size,
                                  vertex_size,
                                  fx::gltf::BufferView::TargetType::ArrayBuffer,
                                  {} });

        std::memcpy(exp.buffers[0].data.data() + vertex_data_size,
                    index_buffers[0]->GetShadowData(),
                    index_data_size);

        exp.bufferViews.push_back(
            // name, buffer, offset, length, stride, target-type
            fx::gltf::BufferView{ "noname",
                                   0,
                                   vertex_data_size,
                                   index_data_size,
                                   0,
                                   fx::gltf::BufferView::TargetType::ElementArrayBuffer,
                                   {} });
        auto bb = pModel->GetBoundingBox();
        exp.accessors.push_back(fx::gltf::Accessor{
                0, // bufferView
                    0, // byteOffset
                    vertex_count, // count
                    false, // normalized
                    fx::gltf::Accessor::ComponentType::Float,
                    fx::gltf::Accessor::Type::Vec3,
                    {}, // sparse
                    "noname",
                    {
                        bb.max_.x_, bb.max_.y_, bb.max_.z_
                    }, // max  TODO hardcode
                    {
                        bb.min_.x_, bb.min_.y_, bb.min_.z_
                    }, // min TODO hardcode
                    {} //extra json stuff
            });

        // TODO other vertex attributes

        exp.accessors.push_back(fx::gltf::Accessor{
            1, // bufferView
            0, // byteOffset
            index_count, // count
            false, // normalized
            fx::gltf::Accessor::ComponentType::UnsignedShort,
            fx::gltf::Accessor::Type::Scalar,
            {}, // sparse
            "indices0", // TODO hardcode name
            {float(vertex_count - 1)}, // max
            {0.f}, // min
            {} //extra json stuff
        });

        fx::gltf::Primitive primitive;
        primitive.indices = 1;
        primitive.attributes = {
            { "POSITION", 0 }, // # of Accessor
        };

        fx::gltf::Mesh mesh;
        mesh.name = "meshname";
        mesh.primitives = { primitive };
        exp.meshes.push_back(mesh);

        fx::gltf::Node node;
        node.mesh = 0;
        exp.nodes.push_back(node);
        return exp;
}
