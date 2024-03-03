#pragma once

#include<memory>

template<class Subclass, class ParentClass>
class Inherit : public ParentClass {

    public: 
        using Pointer = std::shared_ptr<Subclass>;

        template<typename... Args>
        Inherit(Args... args) : ParentClass(args...) {}

        template<typename... Args>
        static std::shared_ptr<Subclass> create(Args... args) {
            return std::make_shared<Subclass>(args...);
        }
};