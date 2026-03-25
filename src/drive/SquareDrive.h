#pragma once
#include <algorithm>
#include <utility>

#include "motor/Motor.h"

class SquareDrive {
private:
    Motor *front, *left, *back, *right;

public:
    SquareDrive(Motor* front, Motor* left, Motor* back, Motor* right)
        : front(front), left(left), back(back), right(right) {}

    /**
     *
     * @param x int in the range [-500, 500]
     * @param y int in the range [-500, 500]
     * @param rotation int in the range [-500, 500]
     */
    void setSpeed(const int16_t x, const int16_t y, const int16_t rotation) const {
        left->setSpeed(y + rotation);
        right->setSpeed(y - rotation);
        front->setSpeed(x - rotation);
        back->setSpeed(x + rotation);
        // back and front seem to be flipped when rotating...
    }

    void brake() const {
        front->brake();
        left->brake();
        back->brake();
        right->brake();
    }
};
