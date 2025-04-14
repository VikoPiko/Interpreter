#include <stdexcept>
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

const float pi = 3.14159265358979323846; // константа за числото Пи
const float e = 2.71828182845904523536;  // константа за числото Е

// базов/абстрактен клас за изрази
class Expression
{
public:
    virtual ~Expression() = default;
    virtual double evaluate() const = 0;
};

// връща стойността на числото
//  например: 5.0, 3.14, -2.0
class Number : public Expression
{
private:
    double value;

public:
    Number(double value) : value(value) {}
    double evaluate() const override { return value; }
};

// базов клас за операции - абстрактен
class Operation : public Expression
{
protected:
    Expression *left;
    Expression *right;

public:
    Operation(Expression *left, Expression *right) : left(left), right(right) {}
    virtual ~Operation()
    {
        delete left;
        delete right;
    }
};

class Addition : public Operation
{
public:
    Addition(Expression *left, Expression *right) : Operation(left, right) {}
    double evaluate() const override { return left->evaluate() + right->evaluate(); }
};

class Subtraction : public Operation
{
public:
    Subtraction(Expression *left, Expression *right) : Operation(left, right) {}
    double evaluate() const override { return left->evaluate() - right->evaluate(); }
};

class Multiplication : public Operation
{
public:
    Multiplication(Expression *left, Expression *right) : Operation(left, right) {}
    double evaluate() const override { return left->evaluate() * right->evaluate(); }
};

class Division : public Operation
{
public:
    Division(Expression *left, Expression *right) : Operation(left, right) {}
    double evaluate() const override
    {
        double rightValue = right->evaluate();
        if (rightValue == 0.0)
        {
            throw std::runtime_error("Cannot divide by zero");
        }
        return left->evaluate() / rightValue;
    }
};

// Base class for Lisp operations
class LispOperation : public Expression
{
protected:
    std::vector<Expression *> operands;

public:
    LispOperation(const std::vector<Expression *> &operands) : operands(operands) {}

    virtual ~LispOperation()
    {
        for (auto operand : operands)
        {
            delete operand;
        }
    }
};

// Addition operation for Lisp
class LispAddition : public LispOperation
{
public:
    LispAddition(const std::vector<Expression *> &operands) : LispOperation(operands) {}

    double evaluate() const override
    {
        double result = 0;
        for (auto operand : operands)
        {
            result += operand->evaluate();
        }
        return result;
    }
};

// Subtraction operation for Lisp
class LispSubtraction : public LispOperation
{
public:
    LispSubtraction(const std::vector<Expression *> &operands) : LispOperation(operands) {}

    double evaluate() const override
    {
        double result = operands[0]->evaluate();
        for (size_t i = 1; i < operands.size(); ++i)
        {
            result -= operands[i]->evaluate();
        }
        return result;
    }
};

// Multiplication operation for Lisp
class LispMultiplication : public LispOperation
{
public:
    LispMultiplication(const std::vector<Expression *> &operands) : LispOperation(operands) {}

    double evaluate() const override
    {
        double result = 1;
        for (auto operand : operands)
        {
            result *= operand->evaluate();
        }
        return result;
    }
};

// Division operation for Lisp
class LispDivision : public LispOperation
{
public:
    LispDivision(const std::vector<Expression *> &operands) : LispOperation(operands) {}

    double evaluate() const override
    {
        double result = operands[0]->evaluate();
        for (size_t i = 1; i < operands.size(); ++i)
        {
            double divisor = operands[i]->evaluate();
            if (divisor == 0)
            {
                throw std::runtime_error("Division by zero");
            }
            result /= divisor;
        }
        return result;
    }
};

#endif