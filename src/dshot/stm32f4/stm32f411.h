/*
   This file is part of DshotSTM32.

   DshotSTM32 is free software: you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the Free Software
   Foundation, either version 3 of the License, or (at your option) any later
   version.

   DshotSTM32 is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE. See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with
   DshotSTM32. If not, see <https://www.gnu.org/licenses/>.
 */

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
