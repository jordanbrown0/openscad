#include "core/NodeVisitor.h"

#include "core/BaseVisitable.h"
#include "core/State.h"
#include "core/node.h"

State NodeVisitor::nullstate(nullptr);

Response NodeVisitor::traverse(const AbstractNode& node, const State& state)
{
printf("NodeVisitor::traverse(AbstractNode)\n"); fflush(stdout);
  State newstate = state;
  newstate.setNumChildren(node.getChildren().size());

  Response response = Response::ContinueTraversal;
  newstate.setPrefix(true);
  newstate.setParent(state.parent());
  response = node.accept(newstate, *this);
if (response == Response::AbortTraversal) {
printf("%s %d %s abort\n", __FILE__, __LINE__, __func__); fflush(stdout);
} else {
printf("%s %d %s not-abort\n", __FILE__, __LINE__, __func__); fflush(stdout);
}

  // Pruned traversals mean don't traverse children
  if (response == Response::ContinueTraversal) {
    newstate.setParent(node.shared_from_this());
    for (const auto& chnode : node.getChildren()) {
      response = this->traverse(*chnode, newstate);
      if (response == Response::AbortTraversal) {
        printf("Abort\n"); fflush(stdout);
        return response;  // Abort immediately
      }
    }
  }

  // Postfix is executed for all non-aborted traversals
  if (response != Response::AbortTraversal) {
    newstate.setParent(state.parent());
    newstate.setPrefix(false);
    newstate.setPostfix(true);
    response = node.accept(newstate, *this);
if (response == Response::AbortTraversal) {
printf("%s %d %s abort\n", __FILE__, __LINE__, __func__); fflush(stdout);
} else {
printf("%s %d %s not-abort\n", __FILE__, __LINE__, __func__); fflush(stdout);
}
  }
printf("%s %d %s\n", __FILE__, __LINE__, __func__); fflush(stdout);
  if (response != Response::AbortTraversal) response = Response::ContinueTraversal;
  if (response == Response::AbortTraversal) {
        printf("Abort2\n"); fflush(stdout);
  } else if (response == Response::ContinueTraversal) {
        printf("Continue\n"); fflush(stdout);
  } else {
        printf("SomethingElse\n"); fflush(stdout);
  }
  return response;
}
