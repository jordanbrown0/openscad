#pragma once

#include "Expression.h"
#include "Value.h"
#include "LocalScope.h"
#include "Context.h"
#include "AST.h"
#include "ModuleInstantiation.h"

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

extern shared_ptr<AbstractNode> spliceGeometry(Arguments& args);

class GeometryInstantiation : public ModuleInstantiation
{
public:
  GeometryInstantiation(shared_ptr<class Expression> expr, const Location& loc);
  std::shared_ptr<AbstractNode> evaluate(const std::shared_ptr<const Context> context) const final;
  ~GeometryInstantiation();
  void print(std::ostream& stream, const std::string& indent, const bool inlined) const final;

private:
  std::shared_ptr<Expression> expr;
};
