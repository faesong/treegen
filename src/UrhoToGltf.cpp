#include "UrhoToGltf.hpp"


#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/IndexBuffer.h>

fx::gltf::Primitive appendPrimitiveToGltfDocument (
        fx::gltf::Document& pDocument,
        Urho3D::VertexBuffer* pVertexBuffer,
        Urho3D::IndexBuffer* pIndexBuffer,
        const Urho3D::BoundingBox& pBounds) {
    const uint32_t vertex_size = pVertexBuffer->GetVertexSize();
    const uint32_t vertex_count = pVertexBuffer->GetVertexCount();
    const uint32_t vertex_data_size = vertex_count * vertex_size;

    const uint32_t index_count = pIndexBuffer->GetIndexCount();
    const uint32_t index_data_size =
        index_count * pIndexBuffer->GetIndexSize();

    fx::gltf::Primitive primitive;

    if (vertex_count == 0) {
        return primitive; // TODO handle properly?
    }

    // TODO3: only 1 buffer supported for now
    if (pDocument.buffers.size() == 0) {
        pDocument.buffers.push_back(fx::gltf::Buffer{});
    }
    auto& buffer0 = pDocument.buffers[0];

    auto occupied_space = static_cast<uint32_t>(buffer0.data.size());
    auto space_left = std::numeric_limits<uint32_t>::max() - occupied_space;
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
        pVertexBuffer->GetShadowData(),
        vertex_data_size);

    fx::gltf::BufferView view0;
    view0.name = "nonameview0";
    view0.buffer = 0;
    view0.byteOffset = buffer0_prev_size;
    view0.byteLength = vertex_data_size;
    view0.byteStride = vertex_size;
    view0.target = fx::gltf::BufferView::TargetType::ArrayBuffer;

    std::memcpy(buffer0.data.data() + buffer0_prev_size + vertex_data_size,
        pIndexBuffer->GetShadowData(),
        index_data_size);

    const uint32_t view0_pos = static_cast<uint32_t>(pDocument.bufferViews.size());
    pDocument.bufferViews.push_back(view0);

    fx::gltf::BufferView view1;
    view1.name = "nonameview1";
    view1.buffer = 0;
    view1.byteOffset = buffer0_prev_size + vertex_data_size;
    view1.byteLength = index_data_size;
    view1.byteStride = 0;
    view1.target = fx::gltf::BufferView::TargetType::ElementArrayBuffer;

    const uint32_t view1_pos = static_cast<uint32_t>(pDocument.bufferViews.size());
    pDocument.bufferViews.push_back(view1);

    auto vert_elements = pVertexBuffer->GetElements();
    for (size_t el_i = 0; el_i < vert_elements.size(); ++el_i) {
        const auto& el = vert_elements[el_i];

        fx::gltf::Accessor accessor;
        accessor.bufferView = static_cast<int32_t>(view0_pos);
        accessor.count = vertex_count;
        accessor.normalized = false;

        const auto& bb = pBounds;

        switch (el.semantic_) {
        case Urho3D::SEM_POSITION:
            accessor.byteOffset = el.offset_;
            accessor.componentType = fx::gltf::Accessor::ComponentType::Float;
            accessor.type = fx::gltf::Accessor::Type::Vec3;
            accessor.name = "accessor_vertex_positions";
            accessor.max = { bb.max_.x_, bb.max_.y_, bb.max_.z_ };
            accessor.min = { bb.min_.x_, bb.min_.y_, bb.min_.z_ };
            primitive.attributes["POSITION"] = static_cast<uint32_t>(pDocument.accessors.size());
            break;
        case Urho3D::SEM_NORMAL:
            accessor.byteOffset = el.offset_;
            accessor.componentType = fx::gltf::Accessor::ComponentType::Float;
            accessor.type = fx::gltf::Accessor::Type::Vec3;
            accessor.name = "accessor_vertex_normals";
            accessor.max = { 1, 1, 1 };
            accessor.min = { -1, -1, -1 };
            primitive.attributes["NORMAL"] = static_cast<uint32_t>(pDocument.accessors.size());
            break;
        case Urho3D::SEM_TEXCOORD:
            accessor.byteOffset = el.offset_;
            accessor.componentType = fx::gltf::Accessor::ComponentType::Float;
            accessor.type = fx::gltf::Accessor::Type::Vec2;
            accessor.name = "accessor_texcoords";
            accessor.max = { 1, 1 };
            accessor.min = { 0, 0 };
            // TODO: more than 1 texcoord
            primitive.attributes["TEXCOORD_0"] = static_cast<uint32_t>(pDocument.accessors.size());
            break;
            //TODO7: other vertex attributes
        case Urho3D::SEM_BINORMAL:
        case Urho3D::SEM_TANGENT:
        case Urho3D::SEM_COLOR:
        case Urho3D::SEM_BLENDWEIGHTS:
        case Urho3D::SEM_BLENDINDICES:
        case Urho3D::SEM_OBJECTINDEX:
        default:
            continue; // important to skip adding invalid accessor
        }
        pDocument.accessors.push_back(accessor);
    }

    fx::gltf::Accessor accessor_indices;
    accessor_indices.bufferView = static_cast<int32_t>(view1_pos);
    accessor_indices.byteOffset = 0;
    accessor_indices.count = index_count;
    accessor_indices.normalized = false;
    accessor_indices.componentType = fx::gltf::Accessor::ComponentType::UnsignedShort;
    accessor_indices.type = fx::gltf::Accessor::Type::Scalar;
    accessor_indices.name = "nonameaccessor_indices_indices";
    accessor_indices.max = { float(vertex_count - 1) };
    accessor_indices.min = { 0 };

    primitive.indices = static_cast<int32_t>(pDocument.accessors.size());
    pDocument.accessors.push_back(accessor_indices);

    return primitive;
}

void appendModelToGltfDocument (fx::gltf::Document& pDocument,
                                Urho3D::SharedPtr<Urho3D::Model>& pModel) {
    fx::gltf::Mesh mesh;
    mesh.name = "meshname";

    auto vertex_buffers = pModel->GetVertexBuffers();
    auto index_buffers = pModel->GetIndexBuffers();

    if (index_buffers.size() != vertex_buffers.size()) {
        throw std::runtime_error("IndexBuffers.size() != VertexBuffers.size()");
    }

    for (decltype(index_buffers.size()) i = 0; i < index_buffers.size(); ++i) {
        mesh.primitives.push_back(
            appendPrimitiveToGltfDocument(pDocument,
                vertex_buffers[i],
                index_buffers[i],
                // TODO proper bounds
                pModel->GetBoundingBox()));
    }

    if (false) {
        // https://discourse.urho3d.io/t/need-a-bit-of-clarification-on-how-models-vertexbuffers-and-geometries-organized/6070/2

        // as I understan, proper Urho3D::Model has all the vertex/index
        // buffers' pointers from each geometry copied to Model itself too.
        // so this whole block of code is disabled.

        // but it is left here just in case... for now, urho exports only data
        // which is doubled in Model itself, so we stay consistant with that
        const auto& geoms = pModel->GetGeometries();

        for (const auto& geoms_ : geoms) {
            for (const auto& geom : geoms_) {
                const auto& g_vbuffs = geom->GetVertexBuffers();
                // TODO taking first lod vertex buffer only?
                mesh.primitives.push_back(
                    appendPrimitiveToGltfDocument(pDocument,
                        g_vbuffs[0],
                        geom->GetIndexBuffer(),
                        pModel->GetBoundingBox()));
            }
        }
    }

    pDocument.meshes.push_back(mesh);

    fx::gltf::Node node;
    node.mesh = static_cast<int32_t>(pDocument.meshes.size()) - 1;
    pDocument.nodes.push_back(node);
}
