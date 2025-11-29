// By Lucas
#include "variable.h"
#include <parse/parser.h>
#include <cmath>

Variable::Type Variable::getType() const
{
    return type;
}
// template <typename T>
// T Variable::getValue()
// {
//     if (type == REFERENCE)
//     {
//         return std::get<Reference>(value).get()->getValue<T>();
//     }
//     return std::get<T>(value);
// }
Variable::Type Variable::getRefType()
{
    if (type == REFERENCE)
    {
        // return INTEGER;
        return std::get<Reference>(value).get()->getRefType();
    }
    return type;
}

void Variable::setValue(long v)
{
    if (type == REFERENCE)
    {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = INTEGER;
}
void Variable::setValue(double v)
{
    if (type == REFERENCE)
    {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = DECIMAL;
}
void Variable::setValue(std::string v)
{
    if (type == REFERENCE)
    {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = STRING;
}
void Variable::setValue(bool v)
{
    if (type == REFERENCE)
    {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = BOOLEAN;
}
void Variable::setValue(std::shared_ptr<Class> v)
{
    if (type == REFERENCE)
    {
        std::get<Reference>(value).get()->setValue(v);
        return;
    }
    value = v;
    type = CLASS;
}
void Variable::setValue()
{
    if (type == REFERENCE)
    {
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
Variable *Reference::get()
{
    return ref;
}

Variable Variable::makeRef(Variable &v)
{
    Variable var;
    var.value = Reference(&v);
    var.type = REFERENCE;
    return var;
}

Variable Variable::compareEqual(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Numeric <-> Boolean
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double leftVal = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                           : (type == INTEGER ? (double)getValue<long>() : getValue<double>());

        double rightVal = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                             : (bType == INTEGER ? (double)b.getValue<long>() : b.getValue<double>());

        return Variable(leftVal == rightVal);
    }

    if (type == INTEGER && bType == DECIMAL)
        return Variable((double)getValue<long>() == b.getValue<double>());
    if (type == DECIMAL && bType == INTEGER)
        return Variable(getValue<double>() == (double)b.getValue<long>());

    // Strict same-type comparisons
    if (type != bType)
        return Variable(false);

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() == b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() == b.getValue<double>());
    case STRING:
        return Variable(getValue<std::string>() == b.getValue<std::string>());
    case BOOLEAN:
        return Variable(getValue<bool>() == b.getValue<bool>());
    case NULL_TYPE:
        return Variable(true);
    default:
        break;
    }
    throw ParserException(ParserException::UNSUPPORTED_OPERATOR, "'==' cannot be used for " + getName() + " value", curToken.getLine(), curToken.getCol());
}

Variable Variable::compareInequal(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type comparisons
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double left = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                        : (type == INTEGER ? getValue<long>() : getValue<double>());
        double right = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                          : (bType == INTEGER ? b.getValue<long>() : b.getValue<double>());
        return Variable(left != right);
    }

    if (type != bType)
    {
        if (type == INTEGER && bType == DECIMAL)
            return Variable(getValue<long>() != b.getValue<double>());
        if (type == DECIMAL && bType == INTEGER)
            return Variable(getValue<double>() != b.getValue<long>());
        return Variable(false);
    }

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() != b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() != b.getValue<double>());
    case STRING:
        return Variable(getValue<std::string>() != b.getValue<std::string>());
    case BOOLEAN:
        return Variable(getValue<bool>() != b.getValue<bool>());
    case NULL_TYPE:
        return Variable(false);
    default:
        break;
    }

    throw ParserException(ParserException::UNSUPPORTED_OPERATOR, "'!=' cannot be used for " + getName() + " value", curToken.getLine(), curToken.getCol());
}

Variable Variable::compareAnd(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();
    if (type != bType)
    {
        return Variable(false);
    }
    switch (type)
    {
    case BOOLEAN:
        return Variable(getValue<bool>() && b.getValue<bool>());
    default:
        break;
    }
    throw ParserException(ParserException::UNSUPPORTED_OPERATOR, "'or' can only be used for boolean value, but a " + getName() + " value was given instead", curToken.getLine(), curToken.getCol());
}

Variable Variable::compareOr(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();
    if (type != bType)
    {
        return Variable(false);
    }
    switch (type)
    {
    case BOOLEAN:
        return Variable(getValue<bool>() || b.getValue<bool>());
    default:
        break;
    }
    throw ParserException(ParserException::UNSUPPORTED_OPERATOR, "'or' can only be used for boolean value, but a " + getName() + " value was given instead", curToken.getLine(), curToken.getCol());
}
Variable Variable::compareGreater(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type comparisons
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double left = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                        : (type == INTEGER ? getValue<long>() : getValue<double>());
        double right = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                          : (bType == INTEGER ? b.getValue<long>() : b.getValue<double>());
        return Variable(left > right);
    }

    if (type != bType)
    {
        if (type == INTEGER && bType == DECIMAL)
            return Variable(getValue<long>() > b.getValue<double>());
        if (type == DECIMAL && bType == INTEGER)
            return Variable(getValue<double>() > b.getValue<long>());
        throw ParserException(ParserException::UNMATCH_OPERAND, "'>' cannot be used between " + getName() + " and " + b.getName(), curToken.getLine(), curToken.getCol());
    }

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() > b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() > b.getValue<double>());
    case STRING:
        return Variable(getValue<std::string>() > b.getValue<std::string>());
    case BOOLEAN:
        return Variable(getValue<bool>() > b.getValue<bool>());
    case NULL_TYPE:
        return Variable(false);
    default:
        break;
    }
    throw ParserException(ParserException::UNMATCH_OPERAND, "'>' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}

Variable Variable::compareSmaller(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type comparisons
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double left = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                        : (type == INTEGER ? getValue<long>() : getValue<double>());
        double right = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                          : (bType == INTEGER ? b.getValue<long>() : b.getValue<double>());
        return Variable(left < right);
    }

    if (type != bType)
    {
        if (type == INTEGER && bType == DECIMAL)
            return Variable(getValue<long>() < b.getValue<double>());
        if (type == DECIMAL && bType == INTEGER)
            return Variable(getValue<double>() < b.getValue<long>());
        throw ParserException(ParserException::UNMATCH_OPERAND, "'<' cannot be used between " + getName() + " and " + b.getName(), curToken.getLine(), curToken.getCol());
    }

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() < b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() < b.getValue<double>());
    case STRING:
        return Variable(getValue<std::string>() < b.getValue<std::string>());
    case BOOLEAN:
        return Variable(getValue<bool>() < b.getValue<bool>());
    case NULL_TYPE:
        return Variable(false);
    default:
        break;
    }
    throw ParserException(ParserException::UNMATCH_OPERAND, "'<' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}
Variable Variable::compareGreaterEqual(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type comparisons
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double left = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                        : (type == INTEGER ? getValue<long>() : getValue<double>());
        double right = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                          : (bType == INTEGER ? b.getValue<long>() : b.getValue<double>());
        return Variable(left >= right);
    }

    if (type != bType)
    {
        if (type == INTEGER && bType == DECIMAL)
            return Variable(getValue<long>() >= b.getValue<double>());
        if (type == DECIMAL && bType == INTEGER)
            return Variable(getValue<double>() >= b.getValue<long>());
        throw ParserException(ParserException::UNMATCH_OPERAND, "'>=' cannot be used between " + getName() + " and " + b.getName(), curToken.getLine(), curToken.getCol());
    }

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() >= b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() >= b.getValue<double>());
    case STRING:
        return Variable(getValue<std::string>() >= b.getValue<std::string>());
    case BOOLEAN:
        return Variable(getValue<bool>() >= b.getValue<bool>());
    case NULL_TYPE:
        return Variable(false);
    default:
        break;
    }
    throw ParserException(ParserException::UNMATCH_OPERAND, "'>=' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}

Variable Variable::compareSmallerEqual(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type comparisons
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double left = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                        : (type == INTEGER ? getValue<long>() : getValue<double>());
        double right = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                          : (bType == INTEGER ? b.getValue<long>() : b.getValue<double>());
        return Variable(left <= right);
    }

    if (type != bType)
    {
        if (type == INTEGER && bType == DECIMAL)
            return Variable(getValue<long>() <= b.getValue<double>());
        if (type == DECIMAL && bType == INTEGER)
            return Variable(getValue<double>() <= b.getValue<long>());
        throw ParserException(ParserException::UNMATCH_OPERAND, "'<=' cannot be used between " + getName() + " and " + b.getName(), curToken.getLine(), curToken.getCol());
    }

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() <= b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() <= b.getValue<double>());
    case STRING:
        return Variable(getValue<std::string>() <= b.getValue<std::string>());
    case BOOLEAN:
        return Variable(getValue<bool>() <= b.getValue<bool>());
    case NULL_TYPE:
        return Variable(false);
    default:
        break;
    }
    throw ParserException(ParserException::UNMATCH_OPERAND, "'<=' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}
Variable Variable::add(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    if (type == STRING || bType == STRING)
    {
        std::string s1;
        if (type == STRING) s1 = getValue<std::string>();
        else if (type == INTEGER) s1 = std::to_string(getValue<long>());
        else if (type == DECIMAL) {
            s1 = std::to_string(getValue<double>());
            // Remove trailing zeros for cleaner string representation
            s1.erase(s1.find_last_not_of('0') + 1, std::string::npos);
            if (s1.back() == '.') s1.pop_back();
        }
        else if (type == BOOLEAN) s1 = getValue<bool>() ? "true" : "false";

        std::string s2;
        if (bType == STRING) s2 = b.getValue<std::string>();
        else if (bType == INTEGER) s2 = std::to_string(b.getValue<long>());
        else if (bType == DECIMAL) {
            s2 = std::to_string(b.getValue<double>());
            s2.erase(s2.find_last_not_of('0') + 1, std::string::npos);
            if (s2.back() == '.') s2.pop_back();
        }
        else if (bType == BOOLEAN) s2 = b.getValue<bool>() ? "true" : "false";

        return Variable(s1 + s2);
    }
    // Handle numeric/bool cross-type comparisons
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double left = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                        : (type == INTEGER ? getValue<long>() : getValue<double>());
        double right = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                          : (bType == INTEGER ? b.getValue<long>() : b.getValue<double>());
        return Variable(left + right);
    }

    if (type != bType)
    {
        if (type == INTEGER && bType == DECIMAL)
            return Variable(getValue<long>() + b.getValue<double>());
        if (type == DECIMAL && bType == INTEGER)
            return Variable(getValue<double>() + b.getValue<long>());
        throw ParserException(ParserException::UNMATCH_OPERAND, "'+' cannot be used between " + getName() + " and " + b.getName(), curToken.getLine(), curToken.getCol());
    }

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() + b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() + b.getValue<double>());
    case STRING:
        return Variable(getValue<std::string>() + b.getValue<std::string>());
    case BOOLEAN:
        return Variable((getValue<bool>() ? 1 : 0) + (b.getValue<bool>() ? 1 : 0));
    default:
        break;
    }
    throw ParserException(ParserException::UNMATCH_OPERAND, "'+' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}
Variable Variable::subtract(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type comparisons
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double left = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                        : (type == INTEGER ? getValue<long>() : getValue<double>());
        double right = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                          : (bType == INTEGER ? b.getValue<long>() : b.getValue<double>());
        return Variable(left - right);
    }

    if (type != bType)
    {
        if (type == INTEGER && bType == DECIMAL)
            return Variable(getValue<long>() - b.getValue<double>());
        if (type == DECIMAL && bType == INTEGER)
            return Variable(getValue<double>() - b.getValue<long>());
        throw ParserException(ParserException::UNMATCH_OPERAND, "'-' cannot be used between " + getName() + " and " + b.getName(), curToken.getLine(), curToken.getCol());
    }

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() - b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() - b.getValue<double>());
    case BOOLEAN:
        return Variable((getValue<bool>() ? 1 : 0) - (b.getValue<bool>() ? 1 : 0));
    default:
        break;
    }
    throw ParserException(ParserException::UNMATCH_OPERAND, "'-' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}
Variable Variable::mutliply(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type comparisons
    if ((type == BOOLEAN && (bType == INTEGER || bType == DECIMAL)) ||
        (bType == BOOLEAN && (type == INTEGER || type == DECIMAL)))
    {
        double left = (type == BOOLEAN) ? (getValue<bool>() ? 1.0 : 0.0)
                                        : (type == INTEGER ? getValue<long>() : getValue<double>());
        double right = (bType == BOOLEAN) ? (b.getValue<bool>() ? 1.0 : 0.0)
                                          : (bType == INTEGER ? b.getValue<long>() : b.getValue<double>());
        return Variable(left * right);
    }

    if (type != bType)
    {
        if (type == INTEGER && bType == DECIMAL)
            return Variable(getValue<long>() * b.getValue<double>());
        if (type == DECIMAL && bType == INTEGER)
            return Variable(getValue<double>() * b.getValue<long>());
        throw ParserException(ParserException::UNMATCH_OPERAND, "'*' cannot be used between " + getName() + " and " + b.getName(), curToken.getLine(), curToken.getCol());
    }

    switch (type)
    {
    case INTEGER:
        return Variable(getValue<long>() * b.getValue<long>());
    case DECIMAL:
        return Variable(getValue<double>() * b.getValue<double>());
    case BOOLEAN:
        return Variable((getValue<bool>() ? 1 : 0) * (b.getValue<bool>() ? 1 : 0));
    default:
        break;
    }
    throw ParserException(ParserException::UNMATCH_OPERAND, "'*' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}
Variable Variable::divide(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type
    auto toDouble = [](Variable &v, const Token& curToken, std::string name) -> double
    {
        switch (v.getRefType())
        {
        case INTEGER:
            return static_cast<double>(v.getValue<long>());
        case DECIMAL:
            return v.getValue<double>();
        case BOOLEAN:
            return v.getValue<bool>() ? 1.0 : 0.0;
        default:
            throw ParserException(ParserException::UNMATCH_OPERAND, "'/' cannot be used for " + name, curToken.getLine(), curToken.getCol());
        }
    };

    // Convert both sides to double for division calculation
    if ((type == INTEGER || type == DECIMAL || type == BOOLEAN) &&
        (bType == INTEGER || bType == DECIMAL || bType == BOOLEAN))
    {
        double left = toDouble(*this, curToken, getName());
        double right = toDouble(b, curToken, getName());

        if (right == 0.0)
            throw ParserException(ParserException::DIVIDE_BY_ZERO, "division by zero", curToken.getLine(), curToken.getCol());

        double result = left / right;

        // ✅ If both operands are integers and divide evenly → return INTEGER
        if (type == INTEGER && bType == INTEGER)
        {
            long lhs = getValue<long>();
            long rhs = b.getValue<long>();
            if (lhs % rhs == 0)
                return Variable(lhs / rhs);
        }

        // Otherwise → promote to DECIMAL
        return Variable(result);
    }

    throw ParserException(ParserException::UNMATCH_OPERAND, "'/' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}
Variable Variable::modulus(Variable &b, const Token &curToken)
{
    Type bType = b.getRefType();

    // Handle numeric/bool cross-type
    auto toDouble = [](Variable &v, const Token& curToken, std::string name) -> double
    {
        switch (v.getRefType())
        {
        case INTEGER:
            return static_cast<double>(v.getValue<long>());
        case DECIMAL:
            return v.getValue<double>();
        case BOOLEAN:
            return v.getValue<bool>() ? 1.0 : 0.0;
        default:
            throw ParserException(ParserException::UNMATCH_OPERAND, "'/' cannot be used for " + name, curToken.getLine(), curToken.getCol());
        }
    };

    // Convert both sides to double for division calculation
    if ((type == INTEGER || type == DECIMAL || type == BOOLEAN) &&
        (bType == INTEGER || bType == DECIMAL || bType == BOOLEAN))
    {
        double left = toDouble(*this, curToken, getName());
        double right = toDouble(b, curToken, getName());

        if (right == 0.0)
            throw ParserException(ParserException::DIVIDE_BY_ZERO, "division by zero", curToken.getLine(), curToken.getCol());

        double result = std::fmod(left, right);
        if(floor(result)==ceil(result))
            return Variable(static_cast<long>(result));

        // Otherwise → promote to DECIMAL
        return Variable(result);
    }

    throw ParserException(ParserException::UNMATCH_OPERAND, "'/' cannot be used for " + getName(), curToken.getLine(), curToken.getCol());
}
Variable Variable::reverse(const Token &curToken)
{

    // Handle numeric/bool cross-type
    

    // Convert both sides to double for division calculation
    if (type == BOOLEAN)
    {
        // Otherwise → promote to DECIMAL
        return Variable(!getValue<bool>());
    }

    throw ParserException(ParserException::UNMATCH_OPERAND, "'!' can only be used for boolean type", curToken.getLine(), curToken.getCol());
}
std::string Variable::getName()
{
    switch (type)
    {
    case BOOLEAN:
        return "bool";
    case INTEGER:
        return "int";
    case DECIMAL:
        return "decimal";
    case STRING:
        return "string";
    case CLASS:
        return "class";
    case NULL_TYPE:
        return "null";
    case REFERENCE:
        return "reference";
    case FUNCTION:
        return "function";
    default:
        return "unknown";
    }
}
std::string Variable::getName(const Variable& variable)
{
    switch (variable.type)
    {
    case BOOLEAN:
        return "bool";
    case INTEGER:
        return "int";
    case DECIMAL:
        return "decimal";
    case STRING:
        return "string";
    case CLASS:
        return "class";
    case NULL_TYPE:
        return "null";
    case REFERENCE:
        return "reference";
    case FUNCTION:
        return "function";
    default:
        return "unknown";
    }
}

std::string Variable::getName(const Type& type)
{
    switch (type)
    {
    case BOOLEAN:
        return "bool";
    case INTEGER:
        return "int";
    case DECIMAL:
        return "decimal";
    case STRING:
        return "string";
    case CLASS:
        return "class";
    case NULL_TYPE:
        return "null";
    case REFERENCE:
        return "reference";
    case FUNCTION:
        return "function";
    default:
        return "unknown";
    }
}