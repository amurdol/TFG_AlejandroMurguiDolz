/**
  ******************************************************************************
  * @file    Main_App.c
  * @brief   This is a code contains all the necessary functions for the main program
  * @author  Alejandro Murgui Dolz
  * @version V1.0
  * @date    09-October-2023
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Main_App.h"
//--------------LoRa-------------------------
LoRa myLoRa;
//--------------UART-------------------------
#define UART_RX_BUFFER_SIZE 128
char uartRxBuffer[UART_RX_BUFFER_SIZE];
volatile uint8_t uartRxIndex = 0;
// Variable global para almacenar el carácter recibido
char SerialCmd;

// flags
_Bool interruptEnabled = 1;
_Bool transmissionReceived = 0;

volatile uint32_t Time0 = 0;
volatile uint32_t Time1 = 0;
volatile uint32_t Time2 = 0;
volatile uint32_t timeElapsed1 = 0;
volatile uint32_t timeElapsed2 = 0;

/**
 * @brief   Handle external interrupt DIO0.
 *
 * @details Set a flag to attend the received message
 *
 * @param   None
 *
 * @return  None
 */
void onInterrupt(){

	if (!interruptEnabled) {
		return;
	}
	transmissionReceived = 1;
	Time2 = HAL_GetTick();
	timeElapsed2 = Time2 - Time1;
	/*char timeElapsedStr[20];
	snprintf(timeElapsedStr, sizeof(timeElapsedStr), "%lu", timeElapsed1);
	HAL_UART_Transmit(&huart5, (uint8_t*)"Time1: ", strlen("Time1: "), 100);
	HAL_UART_Transmit(&huart5, (uint8_t*)timeElapsedStr, strlen(timeElapsedStr), 100);
	snprintf(timeElapsedStr, sizeof(timeElapsedStr), "%lu", timeElapsed2);
	HAL_UART_Transmit(&huart5, (uint8_t*)" Time2: ", strlen(" Time2: "), 100);
	HAL_UART_Transmit(&huart5, (uint8_t*)timeElapsedStr, strlen(timeElapsedStr), 100);*/

}

/**
 * @brief   Sends a LoRa frame with the specified function ID and optional data.
 *
 * @details This function constructs a LoRa frame with the provided function ID and optional data,
 *          transmits it using LoRa communication, and checks the transmission success.
 *
 * @param   functionId  The function ID to be included in the LoRa frame.
 * @param   optDataLen  The length of the optional data to be included in the frame.
 * @param   optData     A pointer to the optional data to be included in the frame.
 *
 * @return  None
 */
void sendFrame(uint8_t functionId, uint8_t optDataLen, uint8_t* optData) {
	// build frame
	uint8_t len = PCP_Get_Frame_Length(callsign, optDataLen);
	uint8_t* frame = (uint8_t*)malloc(len * sizeof(uint8_t));
	PCP_Encode(frame, callsign, functionId, optDataLen, optData);

	// send data
	uint8_t state = LoRa_transmit(&myLoRa, frame, len, 200);
	free(frame);

	// check transmission success
	if (state == 1) {
		HAL_UART_Transmit(&huart5, (uint8_t*)"sent successfully!\r\n", strlen("sent successfully!\r\n"), 100);
	} else {
		HAL_UART_Transmit(&huart5, (uint8_t*)"failed\r\n", strlen("failed\r\n"), 100);
	}
}

/**
 * @brief   Sends a LoRa frame with a default configuration and the specified function ID.
 *
 * @details This function constructs a LoRa frame with default configuration values and the provided
 *          function ID, transmits it using LoRa communication, and checks the transmission success.
 *
 * @param   functionId  The function ID to be included in the LoRa frame.
 *
 * @return  transmissionSucces 1: success 0: failed
 */

void sendFrame_Default(uint8_t functionId){
	// build frame
	uint8_t len = PCP_Get_Frame_Length_Default(callsign);
	uint8_t* frame = (uint8_t*)malloc(len * sizeof(uint8_t));
	PCP_Encode_Default(frame, callsign, functionId);
	// send data and check transmission success
	if (LoRa_transmit(&myLoRa, frame, len, 100) == 1) {
		HAL_UART_Transmit(&huart5, (uint8_t*)"sent successfully!\r\n", strlen("sent successfully!\r\n"), 100);
	} else {
		HAL_UART_Transmit(&huart5, (uint8_t*)"failed\r\n", strlen("failed\r\n"), 100);
	}
	free(frame);
}

/**
 * @brief   Prints a list of available controls over UART communication.
 *
 * @details This function sends a series of control options and descriptions over UART communication
 *          to provide information about available controls to the user.
 *
 * @param   None
 *
 * @return  None
 */
void printControls(){
	HAL_UART_Transmit(&huart5, (uint8_t*)"------------- Controls -------------\r\n", strlen("------------- Controls -------------\r\n"), 100);
	HAL_UART_Transmit(&huart5, (uint8_t*)"p - send ping frame\r\n", strlen("p - send ping frame\r\n"), 100);
	HAL_UART_Transmit(&huart5, (uint8_t*)"l - request last packet info\r\n", strlen("l - request last packet info\r\n"), 100);
	HAL_UART_Transmit(&huart5, (uint8_t*)"------------------------------------\r\n", strlen("------------------------------------\r\n"), 100);
}

/**
 * @brief   Decodes and processes a received LoRa frame.
 *
 * @details This function receives a LoRa frame, extracts information from it, and processes it.
 *          It prints raw data, function ID, and optional data if available, and performs specific
 *          actions based on the function ID.
 *
 * @param   respFrame   A pointer to the received LoRa frame.
 * @param   respLen     The length of the received LoRa frame.
 *
 * @return  None
 */
void decode(uint8_t* respFrame, uint8_t respLen) {
	// print raw data
	HAL_UART_Transmit(&huart5, (uint8_t*)"Received ", strlen("Received "), 100);
	char respLenStr[4];
	sprintf(respLenStr, "%d", respLen);
	HAL_UART_Transmit(&huart5, (uint8_t*)respLenStr, strlen(respLenStr), 100);
	HAL_UART_Transmit(&huart5, (uint8_t*)" bytes:\r\n", strlen(" bytes:\r\n"), 100);

	// get function ID
	uint8_t functionId = PCP_Get_FunctionID(callsign, respFrame, respLen);


	// check optional data
	uint8_t* respOptData = NULL;
	uint8_t respOptDataLen = 0;

	// public frame
	respOptDataLen = PCP_Get_OptData_Length(callsign, respFrame, respLen);

	HAL_UART_Transmit(&huart5, (uint8_t*)"Optional data (", strlen("Optional data ("), 100);
	char respOptDataLenStr[4];
	sprintf(respOptDataLenStr, "%d", respOptDataLen);
	HAL_UART_Transmit(&huart5, (uint8_t*)respOptDataLenStr, strlen(respOptDataLenStr), 100);
	HAL_UART_Transmit(&huart5, (uint8_t*)" bytes):\r\n", strlen(" bytes):\r\n"), 100);

	if (respOptDataLen > 0) {
		// read optional data
		uint8_t respOptData[respOptDataLen];
		// public frame
		PCP_Get_OptData(callsign, respFrame, respLen, respOptData);
	}

	// process received frame
	switch (functionId) {
	case RESP_PONG:
		HAL_UART_Transmit(&huart5, (uint8_t*)"Pong!\r\n", strlen("Pong!\r\n"), 100);
		break;

	case RESP_PACKET_INFO: {
		HAL_UART_Transmit(&huart5, (uint8_t*)"Packet info:\r\n", strlen("Packet info:\r\n"), 100);

		HAL_UART_Transmit(&huart5, (uint8_t*)"SNR = ", strlen("SNR = "), 100);
		char respOptDataStr[10];
		double calculatedValue = respOptData[0] / 4.0;
		snprintf(respOptDataStr, sizeof(respOptDataStr), "%.2f", calculatedValue);
		HAL_UART_Transmit(&huart5, (uint8_t*)respOptDataStr, strlen(respOptDataStr), 100);
		HAL_UART_Transmit(&huart5, (uint8_t*)" dB\r\n", strlen(" dB\r\n"), 100);

		HAL_UART_Transmit(&huart5, (uint8_t*)"RSSI = ", strlen("RSSI = "), 100);
		calculatedValue = respOptData[1]/ -2.0;
		snprintf(respOptDataStr, sizeof(respOptDataStr), "%.2f", calculatedValue);
		HAL_UART_Transmit(&huart5, (uint8_t*)respOptDataStr, strlen(respOptDataStr), 100);
		HAL_UART_Transmit(&huart5, (uint8_t*)" dBm\r\n", strlen(" dBm\r\n"), 100);

		uint16_t counter = 0;
		HAL_UART_Transmit(&huart5, (uint8_t*)"valid LoRa frames = ", strlen("valid LoRa frames = "), 100);
		memcpy(&counter, respOptData + 2, sizeof(uint16_t));
		char counterStr[10];
		sprintf(counterStr, "%d", counter);
		HAL_UART_Transmit(&huart5, (uint8_t*)counterStr, strlen(counterStr), 100);
		HAL_UART_Transmit(&huart5, (uint8_t*)"\r\n", strlen("\r\n"), 100);

		HAL_UART_Transmit(&huart5, (uint8_t*)"invalid LoRa frames = ", strlen("invalid LoRa frames = "), 100);
		memcpy(&counter, respOptData + 4, sizeof(uint16_t));
		sprintf(counterStr, "%d", counter);
		HAL_UART_Transmit(&huart5, (uint8_t*)counterStr, strlen(counterStr), 100);
		HAL_UART_Transmit(&huart5, (uint8_t*)"\r\n", strlen("\r\n"), 100);

		HAL_UART_Transmit(&huart5, (uint8_t*)"valid FSK frames = ", strlen("valid FSK frames = "), 100);
		memcpy(&counter, respOptData + 6, sizeof(uint16_t));
		sprintf(counterStr, "%d", counter);
		HAL_UART_Transmit(&huart5, (uint8_t*)counterStr, strlen(counterStr), 100);
		HAL_UART_Transmit(&huart5, (uint8_t*)"\r\n", strlen("\r\n"), 100);

		HAL_UART_Transmit(&huart5, (uint8_t*)"invalid FSK frames = ", strlen("invalid FSK frames = "), 100);
		memcpy(&counter, respOptData + 8, sizeof(uint16_t));
		sprintf(counterStr, "%d", counter);
		HAL_UART_Transmit(&huart5, (uint8_t*)counterStr, strlen(counterStr), 100);
		HAL_UART_Transmit(&huart5, (uint8_t*)"\r\n", strlen("\r\n"), 100);
	} break;

	default:
		HAL_UART_Transmit(&huart5, (uint8_t*)"Unknown function ID!\r\n", strlen("Unknown function ID!\r\n"), 100);
		break;
	}

	if (respOptDataLen > 0) {
		free(respOptData);
	}
}

/**
 * @brief   Sends a ping frame over LoRa communication.
 *
 * @details This function sends a ping frame over LoRa communication and notifies the user
 *          of the action by transmitting a message over UART communication.
 *
 * @param   None
 *
 * @return  transmissionSucces 1: success 0: failed
 */
void sendPing() {
	HAL_UART_Transmit(&huart5, (uint8_t*)"Sending ping frame ... ", strlen("Sending ping frame ... "), 100);

	// send the frame
	sendFrame_Default(CMD_PING);
}

/**
 * @brief   Requests information about the last received packet over LoRa communication.
 *
 * @details This function sends a request for information about the last received packet
 *          over LoRa communication and notifies the user of the action by transmitting
 *          a message over UART communication.
 *
 * @param   None
 *
 * @return  transmissionSucces 1: success 0: failed
 */
void requestPacketInfo() {
	HAL_UART_Transmit(&huart5, (uint8_t*)"Requesting last packet info ... ", strlen("Requesting last packet info ... "), 100);

	// send the frame
	sendFrame_Default(CMD_GET_PACKET_INFO);
}

/**
 * @brief   Initializes and configures the LoRa module.
 *
 * @details This function initializes and configures the LoRa module by setting various parameters,
 *          such as frequency, spreading factor, bandwidth, CRC rate, power, etc. It also sets up
 *          the necessary GPIO pins and SPI interface for communication with the LoRa module.
 *
 * @param   None
 *
 * @return  An int16_t status code indicating the initialization status of the LoRa module:
 *          - LORA_OK: Initialization successful.
 *          - LORA_ERROR: Failed to initialize the LoRa module.
 */
uint8_t setLoRa(){
	myLoRa = newLoRa();
	myLoRa.CS_port         = NSS_GPIO_Port;
	myLoRa.CS_pin          = NSS_Pin;
	myLoRa.reset_port      = RST_GPIO_Port;
	myLoRa.reset_pin       = RST_Pin;
	myLoRa.DIO0_port       = DIO0_GPIO_Port;
	myLoRa.DIO0_pin        = DIO0_Pin;
	myLoRa.hSPIx           = &hspi1;

	myLoRa.frequency             = LORA_FREQUENCY;             	// default = 433 MHz
	myLoRa.spredingFactor        = SPREADING_FACTOR;            	// default = SF_7
	myLoRa.bandWidth             = BANDWIDTH;       	// default = BW_125KHz
	myLoRa.crcRate               = CODING_RATE;          	// default = CR_4_5
	myLoRa.power                 = OUTPUT_POWER;      	// default = 20db
	myLoRa.overCurrentProtection = CURRENT_LIMIT;             	// default = 100 mA
	myLoRa.preamble              = LORA_PREAMBLE_LEN;              	// default = 8;

	// set up radio module
	LoRa_reset(&myLoRa);
	int16_t LoRa_stat = LoRa_init(&myLoRa);
	return LoRa_stat;
}

/**
 * @brief   Initializes the LoRa application.
 *
 * @details This function initializes the LoRa application by setting up UART reception
 *          in interrupt mode, initializing the LoRa radio module, and starting to listen
 *          for incoming LoRa packets. It also prints initialization status messages over UART.
 *
 * @param   None
 *
 * @return  None
 */
void LoraApp_init(){
	// Iniciar la recepción UART en modo interrupción
	HAL_UART_Receive_IT(&huart5, (uint8_t *)&uartRxBuffer[uartRxIndex], 1);
	HAL_UART_Transmit(&huart5, (uint8_t*)"PLUTON-UPV Ground Station Demo Code\r\n", strlen("PLUTON-UPV Ground Station Demo Code\r\n"), 100);

	// initialize the radio
	int state = setLoRa();


	if (state == LORA_OK) {
		HAL_UART_Transmit(&huart5, (uint8_t*)"Initialization successful!\r\n", strlen("Initialization successful!\r\n"), 100);
	} else {
		HAL_UART_Transmit(&huart5, (uint8_t*)"Failed to initialize\r\n", strlen("Failed to initialize\r\n"), 100);
		while (1);
	}

	// begin listening for packets
	LoRa_startReceiving(&myLoRa);
	printControls();
}
/**
 * @brief   Process serial commands and manage UART communication.
 *
 * @details This function processes serial commands received over UART and manages UART
 *          communication. It disables UART reception interrupts, processes the received
 *          command (e.g., 'p' for sending a ping frame or 'l' for requesting packet info),
 *          and then re-enables UART reception interrupts to listen for the next command.
 *
 * @param   None
 *
 * @return  None
 */
void LoraApp_loopSerial(){
	// disable reception interrupt
	interruptEnabled = 0;
	// Carácter recibido en la UART5
	SerialCmd = uartRxBuffer[uartRxIndex]; // Almacena el carácter en la variable global
	// process serial command
	switch (SerialCmd) {
	case 'p':
		sendPing();
		break;
	case 'l':
		requestPacketInfo();
		break;
	default:
		HAL_UART_Transmit(&huart5, (uint8_t*)"Unknown command: ", strlen("Unknown command: "), 100);
		HAL_UART_Transmit(&huart5, (uint8_t*)&SerialCmd, sizeof(SerialCmd), 100);
		HAL_UART_Transmit(&huart5, (uint8_t*)"\r\n", strlen("\r\n"), 100);
		break;
	}
	LoRa_startReceiving(&myLoRa);
	Time0 = HAL_GetTick();
	// Incrementar el índice y evitar desbordamiento
	uartRxIndex = (uartRxIndex + 1) % UART_RX_BUFFER_SIZE;

	// Reiniciar la recepción para esperar el próximo carácter
	HAL_UART_Receive_IT(&huart5, (uint8_t *)&uartRxBuffer[uartRxIndex], 1);

	interruptEnabled = 1;

}

/**
 * @brief   Process received LoRa data and decode the received frame.
 *
 * @details This function reads received data from the LoRa module, checks if new data
 *          has been received, and if so, it processes the received frame by decoding it.
 *          It temporarily disables the reception interrupt, decodes the received frame,
 *          and then re-enables the reception interrupt for further data reception.
 *
 * @param   None
 *
 * @return  None
 */
void LoraApp_loopReceive(){
		// check if new data were received
		if (transmissionReceived) {
			// disable reception interrupt
			interruptEnabled = 0;
			transmissionReceived = 0;

			// read received data
			uint8_t* respFrame = (uint8_t*)malloc(20 * sizeof(uint8_t));
			size_t respLen = LoRa_receive(&myLoRa, respFrame, 11);
			Time1 = HAL_GetTick();
			timeElapsed1 = Time1 - Time0;


			// check reception success
			decode(respFrame, respLen);

			/*if (state == ERR_NONE) {
			      decode(respFrame, respLen);

			    } else if (state == ERR_CRC_MISMATCH) {//ERR_CRC_MISMATCH
			      Serial.println(F("Got CRC error!"));
			      Serial.print(F("Received "));
			      Serial.print(respLen);
			      Serial.println(F(" bytes:"));
			      //PRINT_BUFF(respFrame, respLen);

			    } else {
			      Serial.println(F("Reception failed, code "));
			      Serial.println(state);

			    }*/
			free(respFrame);
			// enable reception interrupt
			LoRa_startReceiving(&myLoRa);
			interruptEnabled = 1;
		}


}



