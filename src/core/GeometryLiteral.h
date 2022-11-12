#pragma once

#include "Expression.h"
#include "Value.h"
#include "LocalScope.h"
#include "Context.h"
#include "AST.h"

class GeometryLiteral : public Expression
{
public:
  GeometryLiteral(const Location& loc);
  Value evaluate(const std::shared_ptr<const Context>& context) const override;
  void print(std::ostream& stream, const std::string& indent) const override;
  bool isLiteral() const override;
  LocalScope body;
};

class HybridLiteral : public Expression
{
public:
  HybridLiteral(const Location& loc);
  Value evaluate(const std::shared_ptr<const Context>& context) const override;
  void print(std::ostream& stream, const std::string& indent) const override;
  bool isLiteral() const override;
  LocalScope body;
};
