/**
 * Put your licence here
 */

#ifndef __KNOT_PROTOCOL_H
#define __KNOT_PROTOCOL_H
#include <knot_protocol.h>
#include <knot_types.h>

// TODO: Comment correctly for doxygen
typedef int (*data_function)(u8_t sensor_id, knot_msg_data *data);
typedef int (*schema_function)(u8_t sensor_id, knot_msg_schema *schema);
typedef int (*config_function)(u8_t sensor_id, u8_t event_flags,
				u16_t time_sec,
				knot_value_type *lower_limit,
				knot_value_type *upper_limit);

typedef int (*events_function)(knot_msg_data *data);

void knot_protocol_exit(void);

#endif // __KNOT_PROTOCOL
