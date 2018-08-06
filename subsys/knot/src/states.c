#include <knot/core.h>

#include "states.h"

char *knot_state2str(u8_t state)
{
	switch(state) {
	case STATE_DISCONNECTED:
		return "Disconnected";
	case STATE_ACCEPTING:
		return "Accepting";
	case STATE_CONNECTED:
		return "Connected";
	case STATE_AUTHENTICATING:
		return "Authenticating";
	case STATE_REGISTERING:
		return "Registering";
	case STATE_SCHEMA:
		return "Sending schema";
	case STATE_SCHEMA_RESP:
		return "Waiting schema response";
	case STATE_ONLINE:
		return "Online";
	case STATE_RUNNING:
		return "Running";
	case STATE_ERROR:
		return "Error";
	default:
		return "State Invalid";
	}

}
