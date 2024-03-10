#include "BaseItem.hpp"

void BaseItem::visit(Visitor& visitor) {
    visitor.apply(*this);
}