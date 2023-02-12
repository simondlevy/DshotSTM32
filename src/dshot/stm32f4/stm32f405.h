#pragma once

#include "dshot/stm32f4.h"

class Stm32F405Dshot : public Stm32F4Dshot {

    protected:

        virtual void initPortsAndMotors(const std::vector<uint8_t> & motorPins) override
        {
            initStream1(1);
            initStream2(0);

            initMotor(motorPins, 0, 0); 
            initMotor(motorPins, 1, 0);
            initMotor(motorPins, 2, 1);
            initMotor(motorPins, 3, 1);
        }        

    public:

        Stm32F405Dshot(protocol_t protocol=DSHOT600)
            : Stm32F4Dshot(2, protocol)
        {
        }
};
