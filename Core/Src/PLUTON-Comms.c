#include "PLUTON-Comms.h"

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

int16_t PCP_Get_Frame_Length_Default(char* callsign) {
	return PCP_Get_Frame_Length(callsign,0);
}

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

int16_t PCP_Encode_Default(uint8_t* frame, char* callsign, uint8_t functionId) {
	return PCP_Encode(frame, callsign, functionId, 0, NULL);
}
