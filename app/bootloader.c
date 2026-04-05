#include "bl_usart.h"
#include "stdbool.h"
#define PACKET_SIZE_MAX 4096

typedef enum
{
    PACKET_STATE_HEADER,
    PACKET_STATE_OPCODE,
    PACKET_STATE_LENGTH,
    PACKET_STATE_PAYLOAD,
}packet_state_machine_t;
typedef enum
{
    PACKET_OPCODE_ERASE = 0x01,
    PACKET_OPCODE_PROGRAM = 0x02,
    PACKET_OPCODE_VERIFY = 0x03,
    PACKET_OPCODE_BOOT = 0x04,
}packet_opcode_t;
typedef enum
{
    PACKET_ERRCODE_OK = 0,
    PACKET_ERRCODE_OPCODE,
    PACKET_ERRCODE_OVERFLOW,
    PACKET_ERRCODE_TIMEOUT,
    PACKET_ERRCODE_FORMAT,
    PACKET_ERRCODE_VERIFY,
    PACKET_ERRCODE_PARAM,
    PACKET_ERRCODE_UNKNOWN = 0xff,
}packet_errcode_t;

static uint8_t packet_buffer[PACKET_SIZE_MAX];
static uint32_t packet_index;
static packet_state_machine_t packet_state;
static packet_opcode_t packet_opcode;
static uint16_t packet_payload_length;
static bool packet_received;

static void bl_rx_handler(const uint8_t *data, uint32_t size)
{
    while(size --)
    {
        packet_buffer[packet_index++] = *data++;
        switch(packet_state)
        {
            case PACKET_STATE_HEADER:
            {
                if(packet_buffer[0] == 0xAA)
                {
                    packet_state = PACKET_STATE_OPCODE;
                }
                else
                {
                    packet_index = 0;
                    packet_state = PACKET_STATE_HEADER;
                }
                break;
            }
            case PACKET_STATE_OPCODE:
            {
                if(packet_buffer[1] == PACKET_OPCODE_ERASE ||
                    packet_buffer[1] == PACKET_OPCODE_PROGRAM ||
                    packet_buffer[1] == PACKET_OPCODE_VERIFY ||
                    packet_buffer[1] == PACKET_OPCODE_BOOT)
                {
                    packet_opcode = (packet_opcode_t)packet_buffer[1];
                    packet_state = PACKET_STATE_LENGTH;
                }
                else
                {
                    packet_index = 0;
                    packet_state = PACKET_STATE_HEADER;
                }
                break;
            }
            case PACKET_STATE_LENGTH:
            {
                if(packet_index == 4)
                {
                    uint16_t length = packet_buffer[2] | (packet_buffer[3] << 8);
                    if(length <= PACKET_SIZE_MAX - 4)
                    {
                        packet_payload_length = length;
                        packet_state = PACKET_STATE_PAYLOAD;
                    }
                    else
                    {
                        packet_index = 0;
                        packet_state = PACKET_STATE_HEADER;
                    }
                }
                break;

            }
            case PACKET_STATE_PAYLOAD:
            {
                if(packet_index == packet_payload_length + 4)
                {
                    packet_index = 0;
                    packet_state = PACKET_STATE_HEADER;
                    packet_received = true;
                }
                break;
            }
            default:
                break;
        }
    }

}
void bootloader_main(void)
{
    bl_usart_init();
    bl_usart_register_rx_callback(bl_rx_handler);
    while(1)
    {
        if(packet_received)
        {
            bl_usart_write(&packet_opcode, 1); //echo opcode
            bl_usart_write(packet_buffer, packet_payload_length + 4);
            packet_received = false;
        }
    }
}
