#pragma once

#include "Visitor.hpp"

class BaseItem {

    private:

    public:
        BaseItem() {}
        virtual void visit(Visitor& visitor);
};