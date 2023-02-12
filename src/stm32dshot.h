#pragma once

#include <vector>

class Stm32Dshot {

    public:

        typedef enum {
            DSHOT150,
            DSHOT300,
            DSHOT600,
        } protocol_t;

    private:

        static const uint16_t MIN_VALUE = 48;
        static const uint16_t MAX_VALUE = 2047;

        static const uint8_t MAX_MOTORS   = 10;
        static const uint8_t MAX_COMMANDS = 3;

        typedef enum {
            COMMAND_STATE_IDLEWAIT,   // waiting for motors to go idle
            COMMAND_STATE_STARTDELAY, // initial delay before a sequence of cmds
            COMMAND_STATE_ACTIVE,     // actively sending command
            COMMAND_STATE_POSTDELAY   // delay period after the cmd has been sent
        } commandState_e;

        typedef struct {
            commandState_e state;
            uint32_t nextCommandCycleDelay;
            uint32_t delayAfterCommandUs;
            uint8_t repeats;
            uint8_t command[MAX_MOTORS];
        } commandControl_t;

        commandControl_t m_commandQueue[MAX_COMMANDS + 1];
        uint8_t          m_commandQueueHead;
        uint32_t         m_outputFreq;
        uint8_t          m_commandQueueTail;
        uint8_t          m_motorCount;

        bool isLastCommand(void)
        {
            return ((m_commandQueueTail + 1) % (MAX_COMMANDS + 1) == m_commandQueueHead);
        }

        bool commandQueueIsEmpty(void)
        {
            return m_commandQueueHead == m_commandQueueTail;
        }

        uint16_t commandGetCurrent(const uint8_t index)
        {
            return m_commandQueue[m_commandQueueTail].command[index];
        }

        bool commandIsProcessing(void)
        {
            if (commandQueueIsEmpty()) {
                return false;
            }

            commandControl_t* command = &m_commandQueue[m_commandQueueTail];

            return
                command->state == COMMAND_STATE_STARTDELAY ||
                command->state == COMMAND_STATE_ACTIVE ||
                (command->state == COMMAND_STATE_POSTDELAY && !isLastCommand()); 
        }

        uint16_t prepareDshotPacket(
                const uint8_t i, const float motorValue)
        {
            uint16_t value = (uint16_t)motorValue;

            if (commandIsProcessing()) {
                value = commandGetCurrent(i);
            }

            uint16_t packet = value << 1;

            // compute checksum
            unsigned csum = 0;
            unsigned csum_data = packet;
            for (auto i=0; i<3; i++) {
                csum ^=  csum_data;   // xor data by nibbles
                csum_data >>= 4;
            }

            // append checksum
            csum &= 0xf;
            packet = (packet << 4) | csum;

            return packet;
        }

     protected:

        Stm32Dshot(const protocol_t protocol)
        {
            m_outputFreq =
                protocol == DSHOT150 ?
                150 : protocol == DSHOT300 ?
                300 : 600;
        }

        virtual void dmaInit(
                const std::vector<uint8_t> & motorPins,
                const uint32_t dshotOutputFreq) = 0;

        virtual void dmaUpdateComplete(void) = 0;

        virtual void dmaUpdateStart(void) = 0;

        virtual void dmaWriteMotor(uint8_t index, uint16_t packet) = 0;

   public:

        void begin(const std::vector<uint8_t> & motorPins)
        {
            dmaInit(motorPins, 1000 * m_outputFreq);

            m_motorCount = motorPins.size();
        }

        // interval [0,1]
        void write(const float motorValues[])
        {
            dmaUpdateStart();

            for (auto k=0; k<m_motorCount; k++) {

                const auto value = motorValues[k];

                auto packet = prepareDshotPacket(k, 
                        value == 0 ?  0 : MIN_VALUE + value * (MAX_VALUE - MIN_VALUE));

                dmaWriteMotor(k, packet);
            }

            dmaUpdateComplete();
        }
};
