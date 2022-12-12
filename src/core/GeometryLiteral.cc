#include "GeometryLiteral.h"
#include "AST.h"
#include "Expression.h"
#include "Context.h"
#include "ScopeContext.h"
#include "Parameters.h"
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

std::shared_ptr<AbstractNode> Value::GeometryType::getNode() const {
  return node;
}

std::ostream& operator<<(std::ostream& stream, const GeometryType& g)
{
  g.print(stream);
  return stream;
}

HybridLiteral::HybridLiteral(const Location& loc) : Expression(loc)
{
}

bool HybridLiteral::isLiteral() const {
  return false;
}

Value HybridLiteral::evaluate(const std::shared_ptr<const Context>& defining_context) const
{
#if 0
  ContextHandle<ScopeContext> context{Context::create<ScopeContext>(defining_context, &body)};
  std::shared_ptr<AbstractNode> n =
    this->body.instantiateModules(*context, std::make_shared<LiteralNode>());
  return GeometryType(n);
#elseif 0
  ObjectType obj(defining_context->session());
  for (const auto& e : this->children) {
    obj.set(e.first, e.second->evaluate(context));
  }
  return std::move(obj);
#else
  ContextHandle<ScopeContext> context{Context::create<ScopeContext>(defining_context, &body)};
  std::shared_ptr<AbstractNode> n =
    this->body.instantiateModules(*context, std::make_shared<LiteralNode>());
  ObjectType obj(defining_context->session(), n);
  const ValueMap& vm = context->get_lexical_variables();
  for (const auto& e : vm) {
    obj.set(e.first, e.second.clone());
  }
  return std::move(obj);
#endif
}

void HybridLiteral::print(std::ostream& stream, const std::string&) const
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

GeometryInstantiation::GeometryInstantiation(shared_ptr<class Expression> expr, const Location& loc) : ModuleInstantiation(loc) {
  this->expr = expr;
}

GeometryInstantiation::~GeometryInstantiation()
{
}

std::shared_ptr<AbstractNode>
GeometryInstantiation::evaluate(
  const std::shared_ptr<const Context> context) const
{
  Value v = expr->evaluate(context);
  switch (v.type()) {
  case Value::Type::GEOMETRY:
    return v.toGeometry().getNode()->clone();
  case Value::Type::OBJECT:
    {
      shared_ptr<AbstractNode> n = v.toObject().ptr->node;
      if (!n) {
        return std::make_shared<GroupNode>(this);
      }
      return n->clone();
    }
  default:
    print_argConvert_warning("geometry", "value", v, {Value::Type::GEOMETRY}, loc, "???");
    return nullptr;
  }
}

void
GeometryInstantiation::print(
  std::ostream& stream,
  const std::string& indent,
  const bool inlined) const
{
  stream << "(";
  expr->print(stream, indent);
  stream << ");\n";
}
