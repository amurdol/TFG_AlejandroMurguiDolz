
#include <string.h>
#include <stdint.h>

// status codes
#define ERR_NONE                                      0
#define ERR_CALLSIGN_INVALID                          -1
#define ERR_FRAME_INVALID                             -2
#define ERR_INCORRECT_PASSWORD                        -3
#define ERR_LENGTH_MISMATCH                           -4
#define ERR_CRC_MISMATCH                              -7

// communication protocol definitions
#define RESPONSE_OFFSET                                 0x20
#define PRIVATE_OFFSET                                  0x40

// public commands (unencrypted uplink messages)
#define CMD_PING                                        (0x00)
#define CMD_RETRANSMIT                                  (0x01)
#define CMD_RETRANSMIT_CUSTOM                           (0x02)
#define CMD_TRANSMIT_SYSTEM_INFO                        (0x03)
#define CMD_GET_PACKET_INFO                             (0x04)
#define CMD_GET_STATISTICS                              (0x05)
#define CMD_GET_FULL_SYSTEM_INFO                        (0x06)
#define CMD_STORE_AND_FORWARD_ADD                       (0x07)
#define CMD_STORE_AND_FORWARD_REQUEST                   (0x08)
#define CMD_REQUEST_PUBLIC_PICTURE                      (0x09)

#define NUM_PUBLIC_COMMANDS                             (0x0A)

// public responses (unencrypted downlink messages)
#define RESP_PONG                                       (CMD_PING + RESPONSE_OFFSET)
#define RESP_REPEATED_MESSAGE                           (CMD_RETRANSMIT + RESPONSE_OFFSET)
#define RESP_REPEATED_MESSAGE_CUSTOM                    (CMD_RETRANSMIT_CUSTOM + RESPONSE_OFFSET)
#define RESP_SYSTEM_INFO                                (CMD_TRANSMIT_SYSTEM_INFO + RESPONSE_OFFSET)
#define RESP_PACKET_INFO                                (CMD_GET_PACKET_INFO + RESPONSE_OFFSET)
#define RESP_STATISTICS                                 (CMD_GET_STATISTICS + RESPONSE_OFFSET)
#define RESP_FULL_SYSTEM_INFO                           (CMD_GET_FULL_SYSTEM_INFO + RESPONSE_OFFSET)
#define RESP_STORE_AND_FORWARD_ASSIGNED_SLOT            (CMD_STORE_AND_FORWARD_ADD + RESPONSE_OFFSET)
#define RESP_FORWARDED_MESSAGE                          (CMD_STORE_AND_FORWARD_REQUEST + RESPONSE_OFFSET)
#define RESP_PUBLIC_PICTURE                             (CMD_REQUEST_PUBLIC_PICTURE + RESPONSE_OFFSET)
#define RESP_DEPLOYMENT_STATE                           (NUM_PUBLIC_COMMANDS + RESPONSE_OFFSET)
#define RESP_RECORDED_SOLAR_CELLS                       (NUM_PUBLIC_COMMANDS + 1 + RESPONSE_OFFSET)
#define RESP_CAMERA_STATE                               (NUM_PUBLIC_COMMANDS + 2 + RESPONSE_OFFSET)
#define RESP_RECORDED_IMU                               (NUM_PUBLIC_COMMANDS + 3 + RESPONSE_OFFSET)
#define RESP_MANUAL_ACS_RESULT                          (NUM_PUBLIC_COMMANDS + 4 + RESPONSE_OFFSET)
#define RESP_GPS_LOG                                    (NUM_PUBLIC_COMMANDS + 5 + RESPONSE_OFFSET)
#define RESP_GPS_LOG_STATE                              (NUM_PUBLIC_COMMANDS + 6 + RESPONSE_OFFSET)
#define RESP_FLASH_CONTENTS                             (NUM_PUBLIC_COMMANDS + 7 + RESPONSE_OFFSET)
#define RESP_CAMERA_PICTURE                             (NUM_PUBLIC_COMMANDS + 8 + RESPONSE_OFFSET)
#define RESP_CAMERA_PICTURE_LENGTH                      (NUM_PUBLIC_COMMANDS + 9 + RESPONSE_OFFSET)
#define RESP_GPS_COMMAND_RESPONSE                       (NUM_PUBLIC_COMMANDS + 10 + RESPONSE_OFFSET)

// ACK is the last public function ID
#define RESP_ACKNOWLEDGE                                (PRIVATE_OFFSET - 1)

// private commands (encrypted uplink messages)
#define CMD_DEPLOY                                      (0x00 + PRIVATE_OFFSET)
#define CMD_RESTART                                     (0x01 + PRIVATE_OFFSET)
#define CMD_WIPE_EEPROM                                 (0x02 + PRIVATE_OFFSET)
#define CMD_SET_TRANSMIT_ENABLE                         (0x03 + PRIVATE_OFFSET)
#define CMD_SET_CALLSIGN                                (0x04 + PRIVATE_OFFSET)
#define CMD_SET_SF_MODE                                 (0x05 + PRIVATE_OFFSET)
#define CMD_SET_MPPT_MODE                               (0x06 + PRIVATE_OFFSET)
#define CMD_SET_LOW_POWER_ENABLE                        (0x07 + PRIVATE_OFFSET)
#define CMD_SET_RECEIVE_WINDOWS                         (0x08 + PRIVATE_OFFSET)
#define CMD_RECORD_SOLAR_CELLS                          (0x09 + PRIVATE_OFFSET)
#define CMD_CAMERA_CAPTURE                              (0x0A + PRIVATE_OFFSET)
#define CMD_SET_POWER_LIMITS                            (0x0B + PRIVATE_OFFSET)
#define CMD_SET_RTC                                     (0x0C + PRIVATE_OFFSET)
#define CMD_RECORD_IMU                                  (0x0D + PRIVATE_OFFSET)
#define CMD_RUN_MANUAL_ACS                              (0x0E + PRIVATE_OFFSET)
#define CMD_LOG_GPS                                     (0x0F + PRIVATE_OFFSET)
#define CMD_GET_GPS_LOG                                 (0x10 + PRIVATE_OFFSET)
#define CMD_GET_FLASH_CONTENTS                          (0x11 + PRIVATE_OFFSET)
#define CMD_GET_PICTURE_LENGTH                          (0x12 + PRIVATE_OFFSET)
#define CMD_GET_PICTURE_BURST                           (0x13 + PRIVATE_OFFSET)
#define CMD_ROUTE                                       (0x14 + PRIVATE_OFFSET)
#define CMD_SET_FLASH_CONTENTS                          (0x15 + PRIVATE_OFFSET)
#define CMD_SET_TLE                                     (0x16 + PRIVATE_OFFSET)
#define CMD_GET_GPS_LOG_STATE                           (0x17 + PRIVATE_OFFSET)
#define CMD_RUN_GPS_COMMAND                             (0x18 + PRIVATE_OFFSET)
#define CMD_SET_SLEEP_INTERVALS                         (0x19 + PRIVATE_OFFSET)
#define CMD_ABORT                                       (0x1A + PRIVATE_OFFSET)
#define CMD_MANEUVER                                    (0x1B + PRIVATE_OFFSET)
#define CMD_SET_ADCS_PARAMETERS                         (0x1C + PRIVATE_OFFSET)
#define CMD_ERASE_FLASH                                 (0x1D + PRIVATE_OFFSET)
#define CMD_SET_ADCS_CONTROLLER                         (0x1E + PRIVATE_OFFSET)
#define CMD_SET_ADCS_EPHEMERIDES                        (0x1F + PRIVATE_OFFSET)
#define CMD_DETUMBLE                                    (0x20 + PRIVATE_OFFSET)
#define CMD_SET_IMU_OFFSET                              (0x21 + PRIVATE_OFFSET)
#define CMD_SET_IMU_CALIBRATION                         (0x22 + PRIVATE_OFFSET)

#define NUM_PRIVATE_COMMANDS                            (0x23)


int16_t PCP_Get_Frame_Length(char* callsign, uint8_t optDataLen);
int16_t PCP_Get_Frame_Length_Default(char* callsign);
int16_t PCP_Get_FunctionID(char* callsign, uint8_t* frame, uint8_t frameLen);
int16_t PCP_Get_OptData_Length(char* callsign, uint8_t* frame, uint8_t frameLen);
int16_t PCP_Get_OptData(char* callsign, uint8_t* frame, uint8_t frameLen, uint8_t* optData);
int16_t PCP_Encode(uint8_t* frame, char* callsign, uint8_t functionId, uint8_t optDataLen, uint8_t* optData);
int16_t PCP_Encode_Default(uint8_t* frame, char* callsign, uint8_t functionId);
