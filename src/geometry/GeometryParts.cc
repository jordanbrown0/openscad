#include "geometry/Geometry.h"

#include <boost/foreach.hpp>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "geometry/linalg.h"
#include "utils/printutils.h"

#if NEEDSWORK
GeometryParts::GeometryParts(Geometry::Geometries geometries) : children(std::move(geometries))
{
}
#endif

std::unique_ptr<Geometry> GeometryParts::copy() const
{
  return std::make_unique<GeometryParts>(*this);
}

size_t GeometryParts::memsize() const
{
  size_t sum = 0;
  for (const auto& item : this->children) {
    sum += item.second->memsize();
  }
  return sum;
}

BoundingBox GeometryParts::getBoundingBox() const
{
  BoundingBox bbox;
  for (const auto& item : this->children) {
    bbox.extend(item.second->getBoundingBox());
  }
  return bbox;
}

std::string GeometryParts::dump() const
{
  // NEEDSWORK this dump should also show the keys, but I don't
  // know where it's used.
  std::stringstream out;
  for (const auto& item : this->children) {
    out << item.second->dump();
  }
  return out.str();
}

unsigned int GeometryParts::getDimension() const
{
  unsigned int dim = 0;
  for (const auto& item : this->children) {
    if (!dim) dim = item.second->getDimension();
    else if (dim != item.second->getDimension()) {
      // Shouldn't this test be done when the structure is being built?
      LOG(message_group::Warning, "Mixing 2D and 3D objects is not supported.");
      break;
    }
  }
  return dim;
}

bool GeometryParts::isEmpty() const
{
  for (const auto& item : this->children) {
    if (!item.second->isEmpty()) return false;
  }
  return true;
}
