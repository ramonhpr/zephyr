/**
 * Put your licence here
 */

#ifndef __KNOT_STATE_H
#define __KNOT_STATE_H
/* KNoT protocol client states */
enum knot_state {
	STATE_DISCONNECTED=0,
	STATE_ACCEPTING,
	STATE_CONNECTED,
	STATE_AUTHENTICATING,
	STATE_REGISTERING,
	STATE_SCHEMA,
	STATE_SCHEMA_RESP,
	STATE_ONLINE,
	STATE_RUNNING,
	STATE_ERROR
};

u8_t current_state;

u8_t knot_get_state();
char *knot_state2str(u8_t state);
#endif // __KNOT_STATE_H
