#ifndef _ORDINAL_HPP_
#define _ORDINAL_HPP_

#include <string>
#include "Exceptions.hpp"

class Ordinal {
private:
    int omegaPart;
    int finitePart;

public:
    Ordinal() : omegaPart(0), finitePart(0) {}

    explicit Ordinal(int finite)
        : omegaPart(0), finitePart(finite) {
        if (finite < 0) {
            throw InvalidArgumentException("Ordinal finite part cannot be negative");
        }
    }

    Ordinal(int omegaCoeff, int finite)
        : omegaPart(omegaCoeff), finitePart(finite) {
        if (omegaCoeff < 0 || finite < 0) {
            throw InvalidArgumentException("Ordinal parts cannot be negative");
        }
    }

    static Ordinal Omega() {
        return Ordinal(1, 0);
    }

    bool IsFinite() const {
        return omegaPart == 0;
    }

    bool IsInfinite() const {
        return omegaPart > 0;
    }

    int GetOmegaPart() const {
        return omegaPart;
    }

    int GetFinitePart() const {
        return finitePart;
    }

    std::string ToString() const {
        if (omegaPart == 0) {
            return std::to_string(finitePart);
        }
        if (finitePart == 0) {
            if (omegaPart == 1) {
                return "omega";
            }
            return std::to_string(omegaPart) + "*omega";
        }
        if (omegaPart == 1) {
            return "omega+" + std::to_string(finitePart);
        }
        return std::to_string(omegaPart) + "*omega+" + std::to_string(finitePart);
    }

    bool operator==(const Ordinal& other) const {
        return omegaPart == other.omegaPart && finitePart == other.finitePart;
    }

    bool operator!=(const Ordinal& other) const {
        return !(*this == other);
    }

    bool operator<(const Ordinal& other) const {
        if (omegaPart != other.omegaPart) {
            return omegaPart < other.omegaPart;
        }
        return finitePart < other.finitePart;
    }

    bool operator>(const Ordinal& other) const {
        return other < *this;
    }

    bool operator<=(const Ordinal& other) const {
        return !(*this > other);
    }

    bool operator>=(const Ordinal& other) const {
        return !(*this < other);
    }

    Ordinal operator+(const Ordinal& other) const {
        if (other.omegaPart > 0) {
            return Ordinal(omegaPart + other.omegaPart, other.finitePart);
        }
        return Ordinal(omegaPart, finitePart + other.finitePart);
    }

    static Ordinal SubtractLeft(const Ordinal& left, const Ordinal& right) {
        if (right < left) {
            throw InvalidArgumentException("Left subtraction is undefined for right < left");
        }

        if (left.omegaPart == right.omegaPart) {
            return Ordinal(right.finitePart - left.finitePart);
        }

        return Ordinal(right.omegaPart - left.omegaPart, right.finitePart);
    }
};

#endif // _ORDINAL_HPP_