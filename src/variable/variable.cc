// By Lucas
#include "variable.h"

Variable::Type Variable::getType() const
{
    return type;
}
template <typename T>
T Variable::getValue()
{
    if (type == REFERENCE) {
        return std::get<Reference>(value).get()->getValue<T>();
    }
    return std::get<T>(value);
}

void Variable::setValue(long v)
{
    if (type == REFERENCE) {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = INTEGER;
}
void Variable::setValue(double v)
{
    if (type == REFERENCE) {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = DECIMAL;
}
void Variable::setValue(std::string v)
{
    if (type == REFERENCE) {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = STRING;
}
void Variable::setValue(bool v)
{
    if (type == REFERENCE) {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = BOOLEAN;
}
void Variable::setValue(std::shared_ptr<Class> v)
{
    if (type == REFERENCE) {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = CLASS;
}
void Variable::setValue()
{
    if (type == REFERENCE) {
        std::get<Reference>(value).get()->setValue();
        return;
    }
    value = Null{};
    type = NULL_TYPE;
}
bool Variable::isNull()
{
    return type == NULL_TYPE;
}
Variable* Reference::get() {
    return ref;
}

Variable Variable::makeRef(Variable& v) {
    Variable var;
    var.value = Reference(&v);
    var.type = REFERENCE;
    return var;
}