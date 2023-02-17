/*
   Spin a brushless motor using an STM32 F405 MCU.

   NOTE: To get this working on the Adafruit Feather F405, use the
   Generic F405RGTx part number in the Tools menu.

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

#include <dshot.h>
#include <stm32/stm32f4.h>

#include <vector>


// static const uint8_t PIN = PB7;    // F411
static const uint8_t PIN = PC7;  // F405 

static const uint32_t FREQUENCY = 8000;

static Stm32F4Dshot dshot;

// DSHOT timer interrupt
extern "C" void handleDmaIrq(uint8_t id)
{
    dshot.handleDmaIrq(id);
}

static float motorval;

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
        dshot.write(&motorval);
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

    dshot.begin();

    dshot.addMotor(PIN, 0);
}

void loop(void)
{
    const auto usec = micros();

    prompt(usec);

    run(usec);
}
