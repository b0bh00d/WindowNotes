#pragma once

// Inheritable (C++11) class to disable copying and moving^M
class no_copy_or_move
{
protected:
    no_copy_or_move() = default;
    ~no_copy_or_move() = default;

    no_copy_or_move(no_copy_or_move const &) = delete;
    no_copy_or_move(no_copy_or_move &&) = delete;
    void operator=(no_copy_or_move const &x) = delete;
};

