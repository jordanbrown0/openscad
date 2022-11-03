#include "GeometryLiteral.h"
#include "AST.h"
#include "Expression.h"
#include "Context.h"
#include "ScopeContext.h"
#include "Value.h"
#include "node.h"
#include "Tree.h"

GeometryLiteral::GeometryLiteral(const Location& loc) : Expression(loc)
{
}

bool GeometryLiteral::isLiteral() const {
  return false;
}

Value GeometryLiteral::evaluate(const std::shared_ptr<const Context>& defining_context) const
{
  ContextHandle<ScopeContext> context{Context::create<ScopeContext>(defining_context, &body)};
  std::shared_ptr<AbstractNode> n =
    this->body.instantiateModules(*context, std::make_shared<LiteralNode>());
  return GeometryType(n);
}

void GeometryLiteral::print(std::ostream& stream, const std::string&) const
{
  stream << "{{...";
#if 0
  bool first = true;
  for (const auto& e : this->children) {
    if (first) first = false;
    else stream << ", ";
    // NEEDSWORK does not handle special characters in the key
    stream << Value(e.first) << ":" << *e.second;
  }
#endif
  stream << "}}";
}

Value::GeometryType::GeometryType(std::shared_ptr<AbstractNode> node)
  : node(node) {
}

// Copy explicitly only when necessary
GeometryType GeometryType::clone() const
{
  return GeometryType(this->node);
}


Value Value::GeometryType::operator==(const GeometryType& other) const {
  return this == &other;
}
Value Value::GeometryType::operator!=(const GeometryType& other) const {
  return this != &other;
}
Value Value::GeometryType::operator<(const GeometryType& other) const {
  return Value::undef("operation undefined (geometry < geometry)");
}
Value Value::GeometryType::operator>(const GeometryType& other) const {
  return Value::undef("operation undefined (geometry > geometry)");
}
Value Value::GeometryType::operator<=(const GeometryType& other) const {
  return Value::undef("operation undefined (geometry <= geometry)");
}
Value Value::GeometryType::operator>=(const GeometryType& other) const {
  return Value::undef("operation undefined (geometry >= geometry)");
}

void Value::GeometryType::print(std::ostream& stream) const {
  Tree t(node);
  stream << t.getString(*node, "");
}

std::shared_ptr<AbstractNode> Value::GeometryType::getNodeClone() const {
  return node->cloner();
}

std::ostream& operator<<(std::ostream& stream, const GeometryType& g)
{
  g.print(stream);
  return stream;
}
