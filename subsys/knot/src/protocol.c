/**
 * Put your licence here
 */

#if defined(CONFIG_KNOT_LOG)
#define SYS_LOG_DOMAIN "knot/protocol"
#define KNOT_LOG_ENABLED 1
#endif

#include <zephyr.h>
#include <init.h>
#include <toolchain.h>

#include <knot/core.h>

static int knot_protocol_init(const struct device *unused)
{
	KNOT_INFO("Initializing KNoT protocol");

	return 0;
}

SYS_INIT(knot_protocol_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
