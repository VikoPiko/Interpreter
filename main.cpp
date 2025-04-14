#include <iostream>
#include <string>
#include <vector>

#include "expressions.h"

bool isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int Priority(char op)
{
    switch (op)
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    default:
        return 0;
    }
}

// Функция за проверка на валидността на израза (разделя на токени)
std::vector<std::string> tokenize(const std::string &expression)
{
    std::vector<std::string> tokens;
    std::string token;
    bool insideString = false;

    for (char c : expression)
    {
        if (isspace(c) && !insideString)
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        // Detect parentheses as separate tokens
        if (c == '(' || c == ')')
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(std::string(1, c));
        }
        else
        {
            token += c;
        }
    }

    if (!token.empty())
    {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> infixToPostfix(const std::vector<std::string> &tokens)
{
    std::vector<std::string> output;
    std::vector<char> stack;

    for (const std::string &token : tokens)
    {
        if (isdigit(token[0]))
        {
            output.push_back(token);
        }
        else if (isOperator(token[0]))
        {
            while (!stack.empty() && Priority(stack.back()) >= Priority(token[0]))
            {
                output.push_back(std::string(1, stack.back()));
                stack.pop_back();
            }
            stack.push_back(token[0]);
        }
        else if (token == "(")
        {
            stack.push_back('(');
        }
        else if (token == ")")
        {
            while (!stack.empty() && stack.back() != '(')
            {
                output.push_back(std::string(1, stack.back()));
                stack.pop_back();
            }
            stack.pop_back();
        }
    }

    while (!stack.empty())
    {
        output.push_back(std::string(1, stack.back()));
        stack.pop_back();
    }

    return output;
}

Expression *tree(const std::vector<std::string> &postfix)
{
    std::vector<Expression *> stack;

    for (const std::string &token : postfix)
    {
        if (isdigit(token[0]))
        {
            stack.push_back(new Number(std::stod(token)));
        }
        else if (isOperator(token[0]))
        {
            Expression *right = stack.back();
            stack.pop_back();
            Expression *left = stack.back();
            stack.pop_back();

            switch (token[0])
            {
            case '+':
                stack.push_back(new Addition(left, right));
                break;
            case '-':
                stack.push_back(new Subtraction(left, right));
                break;
            case '*':
                stack.push_back(new Multiplication(left, right));
                break;
            case '/':
                stack.push_back(new Division(left, right));
                break;
            }
        }
    }

    return stack.back();
}

Expression *parseLispExpression(const std::vector<std::string> &tokens, size_t &index)
{
    if (index >= tokens.size())
    {
        throw std::runtime_error("Unexpected end of tokens in Lisp expression");
    }

    std::string token = tokens[index];

    // Skip opening parentheses '('
    if (token == "(")
    {
        index++; // Move to the next token

        if (index >= tokens.size())
        {
            throw std::runtime_error("Unexpected end of expression after '('");
        }

        std::string operatorSymbol = tokens[index];
        index++; // Move to the next token

        std::vector<Expression *> operands;

        // Parse operands until we encounter a closing parenthesis ')'
        while (index < tokens.size() && tokens[index] != ")")
        {
            if (tokens[index] == "(")
            {
                operands.push_back(parseLispExpression(tokens, index)); // Recursively handle nested expressions
            }
            else if (isdigit(tokens[index][0]) || (tokens[index][0] == '-' && tokens[index].size() > 1 && isdigit(tokens[index][1]))) // handle negative numbers
            {
                operands.push_back(new Number(std::stod(tokens[index])));
                index++; // Move to the next token
            }
            else
            {
                throw std::runtime_error("Unsupported operand in Lisp expression: " + tokens[index]);
            }
        }

        if (index >= tokens.size() || tokens[index] != ")")
        {
            throw std::runtime_error("Expected closing ')' in Lisp expression");
        }
        index++; // Skip closing parenthesis ')'

        // Based on the operator, create the corresponding operation object
        if (operatorSymbol == "+")
        {
            return new LispAddition(operands);
        }
        else if (operatorSymbol == "-")
        {
            return new LispSubtraction(operands);
        }
        else if (operatorSymbol == "*")
        {
            return new LispMultiplication(operands);
        }
        else if (operatorSymbol == "/")
        {
            return new LispDivision(operands);
        }
        else
        {
            throw std::runtime_error("Unsupported operator in Lisp expression: " + operatorSymbol);
        }
    }
    else
    {
        throw std::runtime_error("Unexpected token: " + token);
    }
}

int main()
{
    std::string expression;
    std::cout << "Enter an expression (infix or Lisp): ";
    std::getline(std::cin, expression);

    std::vector<std::string> tokens = tokenize(expression);

    // Check if the expression looks like Lisp (based on the first token being a parenthesis)
    if (tokens[0] == "(")
    {
        size_t index = 0;
        try
        {
            // Parse the Lisp expression
            Expression *exprTree = parseLispExpression(tokens, index);
            std::cout << "Result: " << exprTree->evaluate() << std::endl;
            delete exprTree;
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    else
    {
        // Infix expression, convert to postfix and evaluate
        std::vector<std::string> postfix = infixToPostfix(tokens);
        Expression *exprTree = tree(postfix);

        try
        {
            std::cout << "Result: " << exprTree->evaluate() << std::endl;
            delete exprTree;
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}