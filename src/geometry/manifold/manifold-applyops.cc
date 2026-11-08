// this file is split into many separate cgalutils* files
// in order to workaround gcc 4.9.1 crashing on systems with only 2GB of RAM

#ifdef ENABLE_MANIFOLD

#include <memory>

#include "core/AST.h"
#include "core/enums.h"
#include "core/node.h"
#include "core/progress.h"
#include "geometry/Geometry.h"
#include "geometry/PolySet.h"
#include "geometry/manifold/ManifoldGeometry.h"
#include "geometry/manifold/manifoldutils.h"
#include "utils/printutils.h"

namespace ManifoldUtils {

Location getLocation(const std::shared_ptr<const AbstractNode>& node)
{
  return node && node->modinst ? node->modinst->location() : Location::NONE;
}

/*!
   Applies op to all children and returns the result.
   The child list should be guaranteed to contain non-NULL 3D or empty Geometry objects
 */
std::shared_ptr<GeometryParts> applyOperator3DManifold(const Geometry::Geometries& children,
                                                          OpenSCADOperator op)
{
  if (op == OpenSCADOperator::HULL) {
    std::vector<manifold::vec3> pts;
    for (const auto& item : children) {
      if (!item.second) continue;
      auto& chgeom = item.second;
      if (const auto *mani = dynamic_cast<const ManifoldGeometry *>(chgeom.get())) {
        pts.reserve(pts.size() + mani->numVertices());
        mani->foreachVertexUntilTrue([&](auto& p) {
          pts.push_back(p);
          return false;
        });
      } else if (const auto *ps = dynamic_cast<const PolySet *>(chgeom.get())) {
        pts.reserve(pts.size() + ps->indices.size() * 3);
        for (const auto& p : ps->indices) {
          for (const auto& ind : p) {
            auto& v = ps->vertices[ind];
            pts.push_back({v[0], v[1], v[2]});
          }
        }
      } else {
        // NEEDSWORK GeometryParts:  arguably, hull of a named part should be an error.
        auto chN = createManifoldFromGeometry(chgeom);
        if (chN && !chN->isEmpty()) {
          chN->foreachVertexUntilTrue([&](auto& p) {
            pts.push_back(p);
            return false;
          });
        }
      }
      if (item.first) item.first->progress_report();
    }
    if (pts.empty()) return nullptr;
    auto result = std::make_shared<GeometryParts>();
    result->add(std::nullopt, std::make_shared<ManifoldGeometry>(manifold::Manifold::Hull(pts)));
    return result;
  }

  auto geom = std::make_shared<GeometryParts>();

  bool foundFirst = false;

  for (const auto& item : children) {
    // NEEDSWORK GeometryParts!!!  This is the most key place.
    std::shared_ptr<const Geometry>chN = item.second;

    // Some special cases for empty children.
    if (!chN || chN->isEmpty()) {
      // Intersecting something with nothing results in nothing.
      if (op == OpenSCADOperator::INTERSECTION) {
        geom = nullptr;
        break;
      }
      // Differencing anything from nothing results in nothing.
      if (op == OpenSCADOperator::DIFFERENCE && !foundFirst) {
        geom = nullptr;
        break;
      }
      // And for everything else, we can just ignore empties.
      continue;
    }

    // Initialize geom with first expected geometric object
    if (!foundFirst) {
      if (auto gp = std::dynamic_pointer_cast<const GeometryParts>(chN)) {
        // First child is a GeometryParts; transcribe it.
        // NEEDSWORK:  can I just use a constructor to do this?
        for (auto childItem : gp->getChildren()) {
          std::optional<std::string> key = childItem.first;
          std::shared_ptr<const Geometry> g = childItem.second;
          geom->add(key, g);
        }
      } else {
        // First child is something else; convert it.
        // NEEDSWORK probably we should ensure that the children
        // are always GeometryParts, and then we wouldn't need
        // this.
        auto mg = createManifoldFromGeometry(chN);
        geom->add(std::nullopt, mg);
      }
      foundFirst = true;
      continue;
    }

    // If the child isn't a GeometryParts, convert it into one.
    // NEEDSWORK As above, probably we should ensure that the child will always be a GeometryParts
    // so that we don't need conversion.
    auto gpchild = std::dynamic_pointer_cast<const GeometryParts>(chN);
    if (!gpchild) {
      auto gptemp = std::make_shared<GeometryParts>();
      gptemp->add(std::nullopt, createManifoldFromGeometry(chN));
      gpchild = gptemp;
    }

    //createManifoldFromGeometry(item.second) : nullptr;
    switch (op) {
    case OpenSCADOperator::UNION:
      for (auto part : gpchild->getChildren()) {
        auto partName = part.first;
        auto partGeom = createManifoldFromGeometry(part.second);
        auto existing = createManifoldFromGeometry(geom->get(partName));
        if (existing) {
          geom->replace(partName, *existing + *partGeom);
        } else {
          geom->add(partName, partGeom);
        }
      }
      break;
    case OpenSCADOperator::INTERSECTION:
      // NEEDSWORK I'm not sure what "right" is, but I'm pretty sure this isn't even close to it.
      for (auto part : gpchild->getChildren()) {
        auto partName = part.first;
        auto partGeom = createManifoldFromGeometry(part.second);
        auto existing = createManifoldFromGeometry(geom->get(partName));
        if (existing) {
          geom->replace(partName, *existing * *partGeom);
        } else {
          geom->add(partName, partGeom);
        }
      }
      break;
    case OpenSCADOperator::DIFFERENCE:
      // NEEDSWORK I'm not sure what "right" is, but I'm pretty sure this isn't even close to it.
      for (auto part : gpchild->getChildren()) {
        auto partName = part.first;
        auto partGeom = createManifoldFromGeometry(part.second);
        auto existing = createManifoldFromGeometry(geom->get(partName));
        if (existing) {
          geom->replace(partName, *existing - *partGeom);
        } else {
          geom->add(partName, partGeom);
        }
      }
      break;
    case OpenSCADOperator::MINKOWSKI:
      // NEEDSWORK I'm not sure what "right" is, but I'm pretty sure this isn't even close to it.
      for (auto part : gpchild->getChildren()) {
        auto partName = part.first;
        auto partGeom = createManifoldFromGeometry(part.second);
        auto existing = createManifoldFromGeometry(geom->get(partName));
        if (existing) {
          geom->replace(partName, existing->minkowski(*partGeom));
        } else {
          geom->add(partName, partGeom);
        }
      }
      break;
    default:                             LOG(message_group::Error, "Unsupported CGAL operator: %1$d", static_cast<int>(op));
    }
    if (item.first) item.first->progress_report();
  }
  return geom;
}

};  // namespace ManifoldUtils

#endif  // ENABLE_MANIFOLD
