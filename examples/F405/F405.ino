#include <stm32dshot.h>
#include <dshot/stm32f4/stm32f405.h>

#include <vector>

static const uint8_t LED_PIN = PB5;

static const uint32_t FREQUENCY = 8000;

static const uint32_t PERIOD_US = 1000000 / FREQUENCY;

static std::vector<uint8_t> MOTOR_PINS = {PB_0};

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

    if (usec-prev > PERIOD_US) {
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
    // XXX for some reason we can't start motors if we remove this!
    pinMode(LED_PIN, OUTPUT);

    Serial.begin(115200);

    dshot.begin(MOTOR_PINS);
}

void loop(void)
{
    const auto usec = micros();

    prompt(usec);

    run(usec);
}
