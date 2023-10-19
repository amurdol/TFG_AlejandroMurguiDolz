/**
  ******************************************************************************
  * @file    PLUTON-Comms.c
  * @brief   This is a code contains all the necessary functions to code and
  * 		 decode LoRa packets of PLUTON-Comms protocol
  * @author  Alejandro Murgui Dolz
  * @version V1.0
  * @date    09-October-2023
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "PLUTON-Comms.h"

/**
 * @brief   Calculate the length of a PCP frame.
 *
 * @details This function calculates the length of a PCP (Protocol Control Protocol) frame
 *          based on the provided callsign and optional data length. PCP frames consist of
 *          a callsign and function ID, which are always present, and optional data, which
 *          may be present depending on the optional data length.
 *
 * @param   callsign    A pointer to the callsign string.
 * @param   optDataLen  The length of optional data (0 if not present).
 *
 * @return  The calculated length of the PCP frame, or an error code if invalid input.
 *          - ERR_CALLSIGN_INVALID: The callsign pointer is NULL.
 */
int16_t PCP_Get_Frame_Length(char* callsign, uint8_t optDataLen) {
	// check callsign
	if(callsign == NULL) {
		return(ERR_CALLSIGN_INVALID);
	}

	// callsign and function ID fields are always present
	int16_t frameLen = strlen(callsign) + 1;

	// optDataLen and optData might be present
	if(optDataLen > 0) {
		frameLen += 1 + optDataLen;
	}

	return(frameLen);
}

/**
 * @brief   Calculate the default length of a PCP frame with no optional data.
 *
 * @details This function calculates the length of a PCP (Protocol Control Protocol) frame
 *          with no optional data based on the provided callsign. The default frame length
 *          is equivalent to the frame length when optional data length is 0.
 *
 * @param   callsign    A pointer to the callsign string.
 *
 * @return  The calculated length of the PCP frame with no optional data, or an error code
 *          if invalid input.
 *          - ERR_CALLSIGN_INVALID: The callsign pointer is NULL.
 */
int16_t PCP_Get_Frame_Length_Default(char* callsign) {
	return PCP_Get_Frame_Length(callsign,0);
}

/**
 * @brief   Calculate the length of optional data within a PCP frame.
 *
 * @details This function calculates the length of optional data within a PCP (Protocol Control Protocol)
 *          frame based on the provided callsign, frame buffer, and frame length.
 *
 * @param   callsign    A pointer to the callsign string.
 * @param   frame       A pointer to the PCP frame buffer.
 * @param   frameLen    The length of the PCP frame.
 *
 * @return  The length of optional data within the PCP frame, or an error code if the input is invalid.
 *          - ERR_CALLSIGN_INVALID: The callsign pointer is NULL.
 *          - ERR_FRAME_INVALID: The frame buffer or its length is invalid.
 *          - ERR_LENGTH_MISMATCH: The length of optional data in the frame doesn't match the expected length.
 */
int16_t PCP_Get_OptData_Length(char* callsign, uint8_t* frame, uint8_t frameLen) {
	// check callsign
	if(callsign == NULL) {
		return(ERR_CALLSIGN_INVALID);
	}

	// check frame buffer
	if(frame == NULL) {
		return(ERR_FRAME_INVALID);
	}

	// check frame length
	if(frameLen < strlen(callsign) + 1) {
		return(ERR_FRAME_INVALID);
	} else if(frameLen == strlen(callsign) + 1) {
		return(0);
	}

	// unencrypted frame
	int16_t optDataLen = frame[strlen(callsign) + 1];

	// check if optDataLen field matches the expected length
	if(optDataLen != (uint8_t)(frameLen - strlen(callsign) - 2)) {
		// length mismatch
		return(ERR_LENGTH_MISMATCH);
	}

	return(optDataLen);
}

/**
 * @brief   Extract the Function ID from a PCP frame.
 *
 * @details This function extracts the Function ID from a PCP (Protocol Control Protocol) frame based on
 *          the provided callsign, frame buffer, and frame length.
 *
 * @param   callsign    A pointer to the callsign string.
 * @param   frame       A pointer to the PCP frame buffer.
 * @param   frameLen    The length of the PCP frame.
 *
 * @return  The extracted Function ID from the PCP frame, or an error code if the input is invalid.
 *          - ERR_CALLSIGN_INVALID: The callsign pointer is NULL.
 *          - ERR_FRAME_INVALID: The frame buffer or its length is invalid.
 */
int16_t PCP_Get_FunctionID(char* callsign, uint8_t* frame, uint8_t frameLen) {
	// check callsign
	if(callsign == NULL) {
		return(ERR_CALLSIGN_INVALID);
	}

	// check frame buffer
	if(frame == NULL) {
		return(ERR_FRAME_INVALID);
	}

	// check frame length
	if(frameLen < strlen(callsign) + 1) {
		return(ERR_FRAME_INVALID);
	}

	return((int16_t)frame[strlen(callsign)]);
}

/**
 * @brief   Extract optional data from a PCP frame.
 *
 * @details This function extracts optional data from a PCP (Protocol Control Protocol) frame based on
 *          the provided callsign, frame buffer, frame length, and an optional data buffer.
 *
 * @param   callsign    A pointer to the callsign string.
 * @param   frame       A pointer to the PCP frame buffer.
 * @param   frameLen    The length of the PCP frame.
 * @param   optData     A pointer to the optional data buffer where extracted data will be stored.
 *
 * @return  An error code indicating the result of the extraction:
 *          - ERR_CALLSIGN_INVALID: The callsign pointer is NULL or doesn't match the frame.
 *          - ERR_FRAME_INVALID: The frame buffer or its length is invalid.
 *          - ERR_NONE: Optional data successfully extracted (or no optional data present).
 */
int16_t PCP_Get_OptData(char* callsign, uint8_t* frame, uint8_t frameLen, uint8_t* optData) {
	// check callsign
	if(callsign == NULL) {
		return(ERR_CALLSIGN_INVALID);
	}

	// check frame
	if(frame == NULL) {
		return(ERR_FRAME_INVALID);
	}

	// get frame pointer
	uint8_t* framePtr = frame;

	// check callsign
	if(memcmp(framePtr, callsign, strlen(callsign)) != 0) {
		// incorrect callsign
		return(ERR_CALLSIGN_INVALID);
	}
	framePtr += strlen(callsign);

	// skip function ID
	framePtr += 1;
	// get optional data (if present)
	if(frameLen > strlen(callsign) + 1) {
		if(optData == NULL) {
			return(ERR_FRAME_INVALID);
		}

		// get option data length
		uint8_t optDataLen = *framePtr;
		framePtr += 1;

		// get optional data
		memcpy(optData, framePtr, optDataLen);
		framePtr += optDataLen;
	}

	return(ERR_NONE);

}

/**
 * @brief   Encode data into a PCP (Protocol Control Protocol) frame.
 *
 * @details This function encodes data into a PCP frame using the provided callsign, function ID, optional data
 *          length, and optional data. The encoded frame is stored in the 'frame' buffer.
 *
 * @param   frame       A pointer to the buffer where the PCP frame will be stored.
 * @param   callsign    A pointer to the callsign string.
 * @param   functionId  The function ID to be encoded into the frame.
 * @param   optDataLen  The length of the optional data (0 if none).
 * @param   optData     A pointer to the optional data to be encoded (ignored if optDataLen is 0).
 *
 * @return  An error code indicating the result of the encoding:
 *          - ERR_NONE: Data successfully encoded into the frame.
 */
int16_t PCP_Encode(uint8_t* frame, char* callsign, uint8_t functionId, uint8_t optDataLen, uint8_t* optData) {
	// get frame pointer
	uint8_t* framePtr = frame;

	// set callsign
	memcpy(framePtr, callsign, strlen(callsign));
	framePtr += strlen(callsign);

	// set function ID
	*framePtr = functionId;
	framePtr += 1;
	if(optDataLen > 0) {
		// set optional data length
		*framePtr = optDataLen;
		framePtr += 1;

		// set optional data
		memcpy(framePtr, optData, optDataLen);
		framePtr += optDataLen;
	}


	return(ERR_NONE);

}

/**
 * @brief   Encode data into a PCP (Protocol Control Protocol) frame with default optional data.
 *
 * @details This function encodes data into a PCP frame using the provided callsign, function ID, and default optional
 *          data (empty optional data). The encoded frame is stored in the 'frame' buffer.
 *
 * @param   frame       A pointer to the buffer where the PCP frame will be stored.
 * @param   callsign    A pointer to the callsign string.
 * @param   functionId  The function ID to be encoded into the frame.
 *
 * @return  An error code indicating the result of the encoding:
 *          - ERR_NONE: Data successfully encoded into the frame.
 */
int16_t PCP_Encode_Default(uint8_t* frame, char* callsign, uint8_t functionId) {
	return PCP_Encode(frame, callsign, functionId, 0, NULL);
}
