#pragma once

#include "dshot/stm32f4.h"

class Stm32F411Dshot : public Stm32F4Dshot {

    protected:

       virtual void initPortsAndMotors(const std::vector<uint8_t> & motorPins)
        {
            initStream1(0);

            initMotor(motorPins, 0, 0); 
            initMotor(motorPins, 1, 0);
            initMotor(motorPins, 2, 0);
            initMotor(motorPins, 3, 0);
        }        

    public:

        Stm32F411Dshot(protocol_t protocol=DSHOT600)
            : Stm32F4Dshot(1, protocol)
        {
        }
};
