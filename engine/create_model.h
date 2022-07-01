// HARFANG(R) Copyright (C) 2021 Emmanuel Julien, NWNC HARFANG. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/render_pipeline.h"

namespace hg {

/// Create a cube render model.
/// @see CreateCapsuleModel, CreateConeModel, CreateCylinderModel, CreatePlaneModel, CreateSphereModel and DrawModel.
Model CreateCubeModel(const VertexLayout &layout, float x, float y, float z);
/// Create a sphere render model.
/// @see CreateCubeModel, CreateConeModel, CreateCylinderModel, CreatePlaneModel, CreateCapsuleModel and DrawModel.
Model CreateSphereModel(const VertexLayout &layout, float radius, int subdiv_x, int subdiv_y);
/// Create a plane render model.
/// @see CreateCubeModel, CreateConeModel, CreateCylinderModel, CreatePlaneModel, CreateCapsuleModel and DrawModel.
Model CreatePlaneModel(const VertexLayout &layout, float width, float length, int subdiv_x, int subdiv_z);
/// Create a cylinder render model.
/// @see CreateCubeModel, CreateConeModel, CreateCylinderModel, CreatePlaneModel, CreateCapsuleModel and DrawModel.
Model CreateCylinderModel(const VertexLayout &layout, float radius, float height, int subdiv_x);
/// Create a cone render model.
/// @see CreateCubeModel, CreateConeModel, CreateCylinderModel, CreatePlaneModel, CreateCapsuleModel and DrawModel.
Model CreateConeModel(const VertexLayout &layout, float radius, float height, int subdiv_x);
/// Create a capsule render model.
/// @see CreateCubeModel, CreateConeModel, CreateCylinderModel, CreatePlaneModel, CreateCapsuleModel and DrawModel.
Model CreateCapsuleModel(const VertexLayout &layout, float radius, float height, int subdiv_x, int subdiv_y);

} // namespace hg
