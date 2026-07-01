#pragma once

#include <exception>

class RowNotFoundException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Row not found at given index!";
    }
};

class InvalidQueryException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Invalid query syntax!";
    }
};

class TypeMismatchException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Type mismatch — expected a different data type!";
    }
};