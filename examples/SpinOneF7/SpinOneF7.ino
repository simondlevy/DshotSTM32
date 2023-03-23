/*
   Spin a brushless motor using an STM32 F705 MCU.

   NOTE: To get this working on the Adafruit Feather F705, use the
   Generic F705RGTx part number in the Tools menu.

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

#include <dshot_stm32f7.h>

#include <vector>

static std::vector<uint8_t> stream1MotorPins = {PC8};
static std::vector<uint8_t> stream2MotorPins = {PC9};

static const uint8_t PIN = PC9;

static std::vector<uint8_t> pins = {PIN};

static const uint32_t FREQUENCY = 8000;

static Stm32F7Dshot dshot;

extern "C" void DMA2_Stream1_IRQHandler(void) 
{
    dshot.handleDmaIrqStream1();
}

static float motorval = 0.1;

void serialEvent(void)
{
    if (Serial.available()) {
        Serial.read();
        motorval = motorval == 0 ? 0.1 : 0;
    }
}

static void run(const uint32_t usec)
{
    static uint32_t prev;

    if (usec-prev > 1000000/FREQUENCY) {
        prev = usec;
        float motorvals[2] = {motorval, motorval};
        dshot.write(motorvals);
    }
}

static void prompt(const uint32_t usec)
{
    static uint32_t prev;

    if (usec-prev > 1000000) {
        prev = usec;
        Serial.println(
                motorval == 0 ?
                "After removing propellers, hit Enter to start motor" :
                "Hit Enter to stop motor"
                );
    }
}

void setup(void)
{
    Serial.begin(115200);

    dshot.begin(pins);
}

void loop(void)
{
    const auto usec = micros();

    prompt(usec);

    run(usec);
}
