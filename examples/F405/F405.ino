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

#include <stm32dshot.h>
#include <dshot/stm32f4/stm32f405.h>

#include <vector>

static const uint8_t LED_PIN = PB5;

static std::vector<uint8_t> MOTOR_PINS = {PB_0};

static const uint32_t FREQUENCY = 8000;

static Stm32F405Dshot dshot;

// DSHOT timer interrupt
extern "C" void handleDmaIrq(uint8_t id)
{
    dshot.handleDmaIrq(id);
}

static float motorval;

static void reboot(void)
{
    __enable_irq();
    HAL_RCC_DeInit();
    HAL_DeInit();
    SysTick->CTRL = SysTick->LOAD = SysTick->VAL = 0;
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

    const uint32_t p = (*((uint32_t *) 0x1FFF0000));
    __set_MSP( p );

    void (*SysMemBootJump)(void);
    SysMemBootJump = (void (*)(void)) (*((uint32_t *) 0x1FFF0004));
    SysMemBootJump();

    NVIC_SystemReset();
}


void serialEvent(void)
{
    if (Serial.available()) {

        if (Serial.read() == 'R') {
            reboot();
        }

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

static void blinkLed(const uint32_t usec)
{
    static uint32_t prev;

    if (usec-prev > 500000) {
        static bool on;
        on = !on;
        digitalWrite(LED_PIN, on);
        prev = usec;
    }
}

void setup(void)
{
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);

    dshot.begin(MOTOR_PINS);
}

void loop(void)
{
    const auto usec = micros();

    prompt(usec);

    run(usec);

    blinkLed(usec);
}
