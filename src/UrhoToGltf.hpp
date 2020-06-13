#pragma once

#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Geometry.h>

#include  <fx/gltf.h>

fx::gltf::Primitive appendPrimitiveToGltfDocument (
    fx::gltf::Document &pDocument,
    Urho3D::VertexBuffer *pVertexBuffer,
    Urho3D::IndexBuffer *pIndexBuffer,
    const Urho3D::BoundingBox& pBounds);

void appendModelToGltfDocument (fx::gltf::Document &pDocument,
                                Urho3D::SharedPtr<Urho3D::Model>& pModel);
