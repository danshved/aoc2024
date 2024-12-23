#ifndef __AOC_GRID_H__
#define __AOC_GRID_H__

#include <iterator>
#include <unordered_map>

#include "collections.h"

// Coordinates on a grid. i grows "down" or "towards the south".
// j grows "right" or "towards the east".
struct Coord {
    int i, j;

    Coord() : i(0), j(0) {}

    Coord(int the_i, int the_j) : i(the_i), j(the_j) {}

    Coord(const std::tuple<int, int>& t)
        : i(std::get<0>(t)), j(std::get<1>(t)) {};

    Coord(const Coord&) = default;
    Coord& operator=(const Coord&) = default;
    Coord(Coord&&) = default;
    Coord& operator=(Coord&&) = default;

    Coord& operator=(const std::tuple<int, int>& t) {
        i = std::get<0>(t);
        j = std::get<1>(t);
        return *this;
    }

    Coord operator+(const Coord& other) const {
        return {i + other.i, j + other.j};
    }

    Coord operator-(const Coord& other) const {
        return {i - other.i, j - other.j};
    }

    Coord operator*(int k) const {
        return {i * k, j * k};
    }

    friend Coord operator*(int k, const Coord& c) {
        return c * k;
    }

    Coord& operator+=(const Coord& other) {
        return *this = *this + other;
    }

    Coord& operator-=(const Coord& other) {
        return *this = *this - other;
    }

    Coord& operator*=(int k) {
        return *this = *this * k;
    }

    bool operator<=>(const Coord&) const = default;

    // Rotates 90 degrees "right", i.e. clockwise.
    Coord RotateRight() const {
        return {j, -i};
    }

    // Rotates 90 degrees "left", i.e. counterclockwise.
    Coord RotateLeft() const {
        return {-j, i};
    }

    // Rotates 180 degees.
    Coord Flip() const {
        return {-i, -j};
    }

    // Manhattan a.k.a. Taxicab metric.
    int Manhattan() const {
        return abs(i) + abs(j);
    }

    // Chessboard a.k.a. Chebyshev metric.
    int Chess() const {
        return std::max(abs(i), abs(j));
    }
};

template <>
struct std::hash<Coord> {
    size_t operator()(const Coord& c) const {
        return SeqHash(c.i, c.j);
    }
};

const Coord kNorth = {-1, 0}, kSouth = {1, 0}, kWest = {0, -1}, kEast = {0, 1};
const Coord kDirs[4] = {kNorth, kWest, kSouth, kEast};
const std::unordered_map<char, Coord> kCharToDir =
    {{'^', kNorth}, {'v', kSouth}, {'<', kWest}, {'>', kEast}};

bool InBounds(const Coord& c, int size_i, int size_j) {
    return c.i >= 0 && c.i < size_i && c.j >= 0 && c.j < size_j;
}

// Helper object that presents the grid as a range:
//
// for (Coord c : Bounds(size_i, size_j)) {...}
class Bounds {
   public:
    class Iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = Coord;

        Iterator() {}

        Iterator(const Coord& cur, int size_j) : cur_(cur), size_j_(size_j) {}

        Coord operator*() const {
            return cur_;
        }

        Iterator& operator++() {
            if (size_j_ > 0) {
                cur_.j++;
                while (cur_.j >= size_j_) {
                    cur_.j -= size_j_;
                    cur_.i++;
                }
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator current = *this;
            ++(*this);
            return current;
        }

        bool operator==(const Iterator& other) const {
            if (size_j_ != other.size_j_) {
                return false;
            }
            if (size_j_ <= 0) {
                return true;
            }
            return cur_ == other.cur_;
        }

       private:
        Coord cur_;
        int size_j_ = 0;
    };

    using iterator = Iterator;

    Bounds(int size_i, int size_j) : size_i_(size_i), size_j_(size_j) {
        assert(size_i >= 0);
        assert(size_j >= 0);
    }

    Iterator begin() const {
        return Iterator(Coord{0, 0}, size_j_);
    }

    Iterator end() const {
        return Iterator(Coord{size_i_, 0}, size_j_);
    }

   private:
    int size_i_;
    int size_j_;
};

// Infinite range going around a given cell in the order of non-decreasing
// Manhattan metric.
class ManhattanSpiral {
   public:
    class Iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = Coord;

        Iterator() {}

        Iterator(Coord start, Coord cur) : start_(start), cur_(cur) {}

        Coord operator*() const {
            return cur_;
        }

        Iterator& operator++() {
            if (cur_.i < start_.i && cur_.j <= start_.j) {
                cur_ += {1, -1};
            } else if (cur_.i >= start_.i && cur_.j < start_.j) {
                cur_ += {1, 1};
            } else if (cur_.i > start_.i && cur_.j >= start_.j) {
                cur_ += {-1, 1};
            } else if (cur_.i <= start_.i && cur_.j > start_.j) {
                cur_ += {-1, -1};
            }

            if (cur_.j == start_.j && cur_.i >= start_.i) {
                cur_ += {1, 0};
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator current = *this;
            ++(*this);
            return current;
        }

        bool operator==(const Iterator&) const = default;

       private:
        Coord start_;
        Coord cur_;
    };
    using iterator = Iterator;

    ManhattanSpiral(const Coord& start) : start_(start) {}

    Iterator begin() const {
        return Iterator(start_, start_);
    }

    std::unreachable_sentinel_t end() const {
        return std::unreachable_sentinel;
    }

   private:
    Coord start_;
};

// Infinite range going around a given cell in the order of non-decreasing
// chessboard metric.
class ChessSpiral {
   public:
    class Iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = Coord;

        Iterator() {}

        Iterator(Coord start, Coord cur) : start_(start), cur_(cur) {}

        Coord operator*() const {
            return cur_;
        }

        Iterator& operator++() {
            Coord delta = cur_ - start_;

            if (InRange(delta.j, delta.i, -delta.j)) {
                cur_ += {1, 0};
            } else if (InRange(-delta.i, delta.j, delta.i)) {
                cur_ += {0, 1};
            } else if (InRange(-delta.j, -delta.i, delta.j)) {
                cur_ += {-1, 0};
            } else if (InRange(delta.i, -delta.j, -delta.i)) {
                cur_ += {0, -1};
            }

            delta = cur_ - start_;
            if (delta.i == delta.j && delta.i >= 0) {
                cur_ += {1, 1};
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator current = *this;
            ++(*this);
            return current;
        }

        bool operator==(const Iterator&) const = default;

       private:
        static bool InRange(int a, int b, int c) {
            return a <= b && b < c;
        }

        Coord start_;
        Coord cur_;
    };
    using iterator = Iterator;

    ChessSpiral(const Coord& start) : start_(start) {}

    Iterator begin() const {
        return Iterator(start_, start_);
    }

    std::unreachable_sentinel_t end() const {
        return std::unreachable_sentinel;
    }

   private:
    Coord start_;
};

// Convenient struct to represent a pair (position, direction).
struct PosDir {
    Coord pos;
    Coord dir;

    PosDir() {};

    PosDir(const Coord& the_pos, const Coord& the_dir)
        : pos(the_pos), dir(the_dir) {}

    bool operator<=>(const PosDir&) const = default;

    PosDir Step() const {
        return {pos + dir, dir};
    }

    PosDir StepBack() const {
        return {pos - dir, dir};
    }

    PosDir RotateRight() const {
        return {pos, dir.RotateRight()};
    }

    PosDir RotateLeft() const {
        return {pos, dir.RotateLeft()};
    }

    PosDir Flip() const {
        return {pos, dir.Flip()};
    }
};

template <>
struct std::hash<PosDir> {
    size_t operator()(const PosDir& s) const {
        return SeqHash(s.pos, s.dir);
    }
};

#endif