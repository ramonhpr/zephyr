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
#include <net/net_app.h>

#include "protocol.h"
#include "states.h"

#define KNOT_STACK_SIZE 1024
#define FSM_PRIORITY 7

#define PEER_PORT 9996
#define INIT_TIMEOUT K_SECONDS(10)
#define CONNECTION_TIMEOUT K_SECONDS(10)

static struct k_thread knot_thread;
K_THREAD_STACK_DEFINE(thread_stack, KNOT_STACK_SIZE);

//struct k_delayed_work recv;
//static struct k_sem quit_lock;

struct net_app_ctx net_ctx;
/*struct device *gpio0;

static void gpio_init(void)
{
#if defined(CONFIG_SOC_SERIES_NRF52X)
	gpio0 = device_get_binding(LED0_GPIO_PORT);

	gpio_pin_configure(gpio0, LED0_GPIO_PIN, GPIO_DIR_OUT);
#endif
}
*/

static void on_connected(struct net_app_ctx *ctx,
				     int status,
				     void *user_data)
{
	KNOT_INFO("Connected with status %d", status);
	current_state = STATE_CONNECTED;
}

static void on_recv(struct net_app_ctx *ctx,
				  struct net_pkt *pkt,
				  int status,
				  void *user_data)
{
//	u8_t *ptr = net_pkt_appdata(pkt);
	KNOT_INFO("Received data");
}

static void on_sent(struct net_app_ctx *ctx,
				  int status,
				  void *user_data_send,
				  void *user_data)
{
	KNOT_INFO("Data was sent successfully");
}

static void on_closed(struct net_app_ctx *ctx,
				   int status,
				   void *user_data)
{
	KNOT_INFO("Socket closed");
	current_state = STATE_DISCONNECTED;
}

static int comm_init(void) {
	int ret;

	if (IS_ENABLED(CONFIG_NET_TCP) && !IS_ENABLED(CONFIG_NET_UDP)) {
		KNOT_INFO("Start context with TCP protocol (%s)", CONFIG_NET_APP_PEER_IPV6_ADDR);
		ret = net_app_init_tcp_client(&net_ctx, NULL, NULL,
					CONFIG_NET_APP_PEER_IPV6_ADDR,
					PEER_PORT,
					INIT_TIMEOUT,
					NULL);
		if (ret < 0) {
			KNOT_ERR("Cannot init IPv6 TCP client (%d)", ret);
			return -1;
		}

		ret = net_app_set_cb(&net_ctx, on_connected, on_recv, on_sent, on_closed);
		if (ret < 0) {
			KNOT_ERR("Cannot set callbacks (%d)", ret);
			return -1;
		}
	}

	if (!IS_ENABLED(CONFIG_NET_TCP) && IS_ENABLED(CONFIG_NET_UDP)) {
		KNOT_INFO("Start context with UDP protocol");
		ret = net_app_init_udp_client(&net_ctx, NULL, NULL,
					CONFIG_NET_APP_PEER_IPV6_ADDR,
					PEER_PORT,
					INIT_TIMEOUT,
					NULL);
		if (ret < 0) {
			KNOT_ERR("Cannot init IPv6 UDP client (%d)", ret);
			return -1;
		}
	}
	ret = net_app_set_cb(&net_ctx, on_connected, on_recv, on_sent, on_closed);
	if (ret < 0) {
		KNOT_ERR("Cannot set callbacks (%d)", ret);
		return -1;
	}

	return 0;
}
/*
static void wait_reply(struct k_work *work)
{
	// struct data *data = CONTAINER_OF(work, struct data, recv);

	KNOT_ERR("Data packet not received");

	// send_udp_data(data->udp, data);
}
*/

static void send_udp_data(struct net_app_ctx *ctx, char *data)
{
	struct net_pkt *pkt;
	size_t len;
	int ret;
	int expecting_len;

	pkt = net_app_get_net_pkt(ctx, AF_UNSPEC, K_FOREVER);
	expecting_len = net_pkt_append(pkt, sizeof(data), data,
					K_FOREVER);
	if (!pkt) {
		return;
	}

	len = net_pkt_get_len(pkt);

	KNOT_INFO("Data to send %zu", len);

	ret = net_app_send_pkt(ctx, pkt, NULL, 0, K_FOREVER,
			       UINT_TO_POINTER(len));
	if (ret < 0) {
		KNOT_ERR("Cannot send UDP data to peer (%d)", ret);

		net_pkt_unref(pkt);
	}

}


static void fsm_run(void)
{
	int aux;
	int ret;
	KNOT_INFO("Start KNoT finite state machine");
	while(1) {
		KNOT_DBG("Thing is on state %s", knot_state2str(current_state));
		switch(current_state) {
		case STATE_DISCONNECTED:
//			gpio_pin_write(gpio0, LED0_GPIO_PIN, aux%2);
//			aux++;
	//		k_delayed_work_init(&recv, wait_reply);
			ret = net_app_connect(&net_ctx, CONNECTION_TIMEOUT);
			if (ret < 0) {
				KNOT_ERR("Cannot connect (%d)", ret);
			} 
			break;
		case STATE_ACCEPTING:
			break;
		case STATE_CONNECTED:
			// net_app_close(&net_ctx);
			send_udp_data(&net_ctx, "o");
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

		// k_sem_take(&quit_lock, K_FOREVER);
		k_sleep(K_SECONDS(2));
	}
}

static void fsm_init(void)
{
	k_thread_create(&knot_thread, thread_stack,
	K_THREAD_STACK_SIZEOF(thread_stack), (k_thread_entry_t)fsm_run, NULL,
	NULL, NULL, K_PRIO_COOP(FSM_PRIORITY), 0, 0);
}


static int knot_protocol_init(const struct device *unused)
{
	KNOT_INFO("Initializing KNoT protocol");

	if (comm_init() < 0) {
		return -1;
	}

	fsm_init();

	return 0;
}

SYS_INIT(knot_protocol_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
