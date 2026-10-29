#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "core/node.h"
#include "core/AST.h"
#include "core/BaseVisitable.h"
#include "core/ModuleInstantiation.h"

/*!
   Logically groups objects together. Used as a way of passing
   objects around without having to perform unions on them.
 */
class PartNode : public GroupNode
{
public:
  VISITABLE();
  PartNode(const ModuleInstantiation *mi, std::string partName)
    : GroupNode(mi)
  {
printf("PartNode()\n"); fflush(stdout);
    this->partName = partName;
  }

  std::string name() const override;
  std::string toString() const override;

private:
  std::string partName;
};
