#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "LoRa.h"
#include "PLUTON-Comms.h"
#include <stdlib.h>
#include <stdio.h>


// modem configuration
#define LORA_FREQUENCY        433   // MHz
#define BANDWIDTH             BW_125KHz   // kHz
#define SPREADING_FACTOR      SF_9      // -
#define CODING_RATE           CR_4_5       // 4/5
#define SYNC_WORD             0x12    // used as LoRa "sync word", or twice repeated as FSK sync word (0x1212) LoraWAN 0x34
#define OUTPUT_POWER          POWER_20db      // dBm
#define CURRENT_LIMIT         130     // mA
#define LORA_PREAMBLE_LEN     8       // symbols

// satellite callsign
char callsign[] = "PLUTON-UPV";

/* Private function prototypes -----------------------------------------------*/
void onInterrupt();
void sendFrame(uint8_t functionId, uint8_t optDataLe, uint8_t* optData);
void sendFrame_Default(uint8_t functionId);
void printControls();
void decode(uint8_t* respFrame, uint8_t respLen);
void sendPing();
void requestPacketInfo();
uint8_t setLoRa();
void LoraApp_init();
void LoraApp_loopSerial();
void LoraApp_loopReceive();
