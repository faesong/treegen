#pragma once

//TODO:
#include  <fx/gltf.h>

// TODO3: too much ident
//void addVertexBuffer (fx::gltf::Document& pDocument,
//                      Urho3D::VertexBuffer& pVertexBuffer) {
//}

void appendModelToGltfDocument (fx::gltf::Document &pDocument,
                                Urho3D::SharedPtr<Urho3D::Model> &pModel) {
        auto vertex_buffers = pModel->GetVertexBuffers();
        auto index_buffers = pModel->GetIndexBuffers();

        const uint32_t vertex_size = vertex_buffers[0]->GetVertexSize();
        const uint32_t vertex_count = vertex_buffers[0]->GetVertexCount();
        const uint32_t vertex_data_size = vertex_count * vertex_size;

        const uint32_t index_count = index_buffers[0]->GetIndexCount();
        const uint32_t index_data_size =
            index_count * index_buffers[0]->GetIndexSize();

        pDocument.buffers.push_back(fx::gltf::Buffer{});
        // TODO: only 1 buffer
        pDocument.buffers[0].data.resize(vertex_data_size + index_data_size); 
        pDocument.buffers[0].byteLength = pDocument.buffers[0].data.size();
        std::memcpy(pDocument.buffers[0].data.data(),
                    vertex_buffers[0]->GetShadowData(),
                    vertex_data_size);

        fx::gltf::BufferView view0;
        view0.name = "nonameview0";
        view0.buffer = 0;
        view0.byteOffset = 0;
        view0.byteLength = vertex_data_size;
        view0.byteStride = vertex_size;
        view0.target = fx::gltf::BufferView::TargetType::ArrayBuffer;

        std::memcpy(pDocument.buffers[0].data.data() + vertex_data_size,
                    index_buffers[0]->GetShadowData(),
                    index_data_size);

        pDocument.bufferViews.push_back(view0);

        fx::gltf::BufferView view1;
        view1.name = "nonameview1";
        view1.buffer = 0;
        view1.byteOffset = vertex_data_size;
        view1.byteLength = index_data_size;
        view1.byteStride = 0;
        view1.target = fx::gltf::BufferView::TargetType::ElementArrayBuffer;

        pDocument.bufferViews.push_back(view1);

        auto bb = pModel->GetBoundingBox();

        fx::gltf::Accessor accessor0;
        accessor0.bufferView = 0;
        accessor0.byteOffset = 0;
        accessor0.count = vertex_count;
        accessor0.normalized = false;
        accessor0.componentType = fx::gltf::Accessor::ComponentType::Float;
        accessor0.type = fx::gltf::Accessor::Type::Vec3;
        accessor0.name = "nonameaccessor0_vertex_positions";
        accessor0.max = { bb.max_.x_, bb.max_.y_, bb.max_.z_ };
        accessor0.min = { bb.min_.x_, bb.min_.y_, bb.min_.z_ };

        pDocument.accessors.push_back(accessor0);

        fx::gltf::Accessor accessor1;
        accessor1.bufferView = 1;
        accessor1.byteOffset = 0;
        accessor1.count = index_count;
        accessor1.normalized = false;
        accessor1.componentType = fx::gltf::Accessor::ComponentType::UnsignedShort;
        accessor1.type = fx::gltf::Accessor::Type::Scalar;
        accessor1.name = "nonameaccessor1_indices";
        accessor1.max = { bb.max_.x_, bb.max_.y_, bb.max_.z_ };
        accessor1.min = { bb.min_.x_, bb.min_.y_, bb.min_.z_ };

        pDocument.accessors.push_back(accessor1);

        // TODO other vertex attributes

        fx::gltf::Primitive primitive;
        primitive.indices = 1;
        primitive.attributes = {
            { "POSITION", 0 }, // # of Accessor
        };

        fx::gltf::Mesh mesh;
        mesh.name = "meshname";
        mesh.primitives = { primitive };
        pDocument.meshes.push_back(mesh);

        fx::gltf::Node node;
        node.mesh = 0;
        pDocument.nodes.push_back(node);
}
