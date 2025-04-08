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
    for (char c : expression)
    {
        if (isspace(c))
        {
            continue;
        }
        if (isOperator(c) || c == '(' || c == ')')
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

int main()
{
    std::cout << "Pi Value: " << pi << std::endl;
    std::string expression;
    std::cout << "Enter an expression: ";
    std::getline(std::cin, expression);

    std::vector<std::string> tokens = tokenize(expression);
    std::vector<std::string> postfix = infixToPostfix(tokens);
    Expression *exprTree = tree(postfix);

    try
    {
        std::cout << "Result: " << exprTree->evaluate() << std::endl;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    delete exprTree; // Освобождаване на паметта

    return 0;
}