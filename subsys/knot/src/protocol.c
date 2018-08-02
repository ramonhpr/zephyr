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
#include <board.h>
#include <gpio.h>

#include <knot/core.h>

#include "protocol.h"
#include "states.h"

#define KNOT_STACK_SIZE 1024
#define FSM_PRIORITY 7

static struct k_thread knot_thread;
K_THREAD_STACK_DEFINE(thread_stack, KNOT_STACK_SIZE);

static u8_t current_state = STATE_DISCONNECTED;
struct device *gpio0;

static void fsm_run(void)
{
	int aux = 0;
	KNOT_INFO("Starting KNoT finite state machine");
	while(1) {
		KNOT_DBG("Thing is on state %s", knot_state2str(current_state));
		switch(current_state) {
		case STATE_DISCONNECTED:
//			gpio_pin_write(gpio0, LED0_GPIO_PIN, aux%2);
			aux++;
			break;
		case STATE_ACCEPTING:
			break;
		case STATE_CONNECTED:
			break;
		case STATE_AUTHENTICATING:
			break;
		case STATE_REGISTERING:
			break;
		case STATE_SCHEMA:
			break;
		case STATE_SCHEMA_RESP:
			break;
		case STATE_ONLINE:
			break;
		case STATE_RUNNING:
			break;
		case STATE_ERROR:
			break;
		}
		k_sleep(K_SECONDS(2));
	}
}

static void fsm_init(void)
{
	k_thread_create(&knot_thread, thread_stack,
	K_THREAD_STACK_SIZEOF(thread_stack), (k_thread_entry_t)fsm_run, NULL,
	NULL, NULL, K_PRIO_COOP(FSM_PRIORITY), 0, 0);
}

static void gpio_init(void)
{
#if defined(CONFIG_SOC_SERIES_NRF52X)
	gpio0 = device_get_binding(LED0_GPIO_PORT);

	gpio_pin_configure(gpio0, LED0_GPIO_PIN, GPIO_DIR_OUT);
#endif
}

static int knot_protocol_init(const struct device *unused)
{
	KNOT_INFO("Initializing KNoT protocol");

	fsm_init();

	gpio_init();

	return 0;
}

SYS_INIT(knot_protocol_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
