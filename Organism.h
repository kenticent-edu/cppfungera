#ifndef CPPFUNGERA_ORGANISM_H
#define CPPFUNGERA_ORGANISM_H

#include "Memory.h"
#include <cstddef>
#include <unordered_map>
#include <stack>

class Organism {
public:
    Organism(std::array<std::size_t, 2> size,
             std::array<std::size_t, 2> begin);

    void exec()
    {
        (this->*map1_.at(f(0)))();
        ip_ = get_ip2(1);
    }

private:
    /* Get the location of the instruction pointer in direction vector
     * multiplied by scalar i */
    std::array<std::size_t, 2> get_ip2(std::size_t i) {
        return {ip_[0]+i*v_[1], ip_[1]+i*v_[0]};
    }

    char f(std::size_t i) {
        auto ip2 = get_ip2(i);
        return (*Memory::get_instance())(ip2[0], ip2[1]);
    }

    void nop() {}

    void up() { v_ = {0, -1}; }

    void down() { v_ = {0, 1}; }

    void left() { v_ = {-1, 0}; }

    void right() { v_ = {1, 0}; }

    void find_pattern();

    void if_zero();

    void zero() { regs_.at(f(1)) = {0, 0}; }

    void one() { regs_.at(f(1)) = {1, 1}; }

    void inc()
    {
        char c;
        if ((c = f(1)) == 'x' || c == 'y')
            ++regs_.at(f(2))[(c == 'x') ? 0 : 1];
        else {
            ++regs_.at(c)[0];
            ++regs_.at(c)[1];
        }
    }

    void dec()
    {
        char c;
        if ((c = f(1)) == 'x' || c == 'y')
            --regs_.at(f(2))[(c == 'x') ? 0 : 1];
        else {
            --regs_.at(c)[0];
            --regs_.at(c)[1];
        }
    }

    void sub()
    {
        auto r1 = regs_.at(f(1)), r2 = regs_.at(f(2));
        regs_.at(f(3)) = {r1[0]-r2[0], r1[1]-r2[1]};
    }

    void read()
    {
        auto r1 = regs_.at(f(1));
        auto opcode = map2_.at((*Memory::get_instance())(r1[0], r1[1]));
        regs_.at(f(2)) = {opcode[0], opcode[1]};
    }

    void write()
    {
        // We don't consider writing with zero child size an error
        if (child_size_[0] != 0 && child_size_[1] != 0) {
            auto r1 = regs_.at(f(1)), r2 = regs_.at(f(2));
            for (const auto &pair : map2_)
                if (r2[0] == pair.second[0] && r2[1] == pair.second[1]) {
                    (*Memory::get_instance())(r1[0], r1[1]) = pair.first;
                    break;
                }
        }
    }

    void alloc_child();

    void split_child();

    // TODO: Add configurable stack size
    void push() { stack_.push(regs_.at(f(1))); }

    void pop()
    {
        regs_.at(f(1)) = stack_.top();
        stack_.pop();
    }

    using operation = void (Organism::*)();
    static const std::unordered_map<char, operation> map1_;
    static const std::unordered_map<char, std::array<unsigned char, 2>> map2_;
    const std::array<std::size_t, 2> size_;
    std::array<std::size_t, 2> ip_;
    std::array<signed char, 2> v_{1, 0};
    std::unordered_map<char, std::array<std::size_t, 2>> regs_{
            {'a', {0, 0}},
            {'b', {0, 0}},
            {'c', {0, 0}},
            {'d', {0, 0}}
    };
    std::array<std::size_t, 2> child_size_{0};
    std::array<std::size_t, 2> child_begin_{0};
    std::stack<std::array<std::size_t, 2>> stack_;
};

#endif //CPPFUNGERA_ORGANISM_H
