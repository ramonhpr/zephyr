#ifndef __KNOT_CORE_H
#define __KNOT_CORE_H
/* KNoT subsystem logging helpers */

#if defined(KNOT_LOG_ENABLED)
#if !defined(SYS_LOG_DOMAIN)
#define SYS_LOG_DOMAIN "knot"
#endif /* !SYS_LOG_DOMAIN */

#undef SYS_LOG_LEVEL
#ifndef KNOT_SYS_LOG_LEVEL
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_KNOT_LEVEL
#else
#define SYS_LOG_LEVEL KNOT_SYS_LOG_LEVEL
#endif /* !KNOT_SYS_LOG_LEVEL */
#define KNOT_DBG(fmt, ...) SYS_LOG_DBG("(%p): " fmt, k_current_get(), \
				      ##__VA_ARGS__)
#define KNOT_ERR(fmt, ...) SYS_LOG_ERR(fmt, ##__VA_ARGS__)
#define KNOT_WARN(fmt, ...) SYS_LOG_WRN(fmt, ##__VA_ARGS__)
#define KNOT_INFO(fmt, ...) SYS_LOG_INF(fmt,  ##__VA_ARGS__)
#define KNOT_ASSERT(cond) do {				     \
		if (!(cond)) {					     \
			KNOT_ERR("{assert: '" #cond "' failed}");     \
		} } while (0)
#define KNOT_ASSERT_INFO(cond, fmt, ...) do {			     \
		if (!(cond)) {					     \
			KNOT_ERR("{assert: '" #cond "' failed} " fmt, \
				##__VA_ARGS__);			     \
		} } while (0)
#else /* KNOT_LOG_ENABLED */
#define KNOT_DBG(...)
#define KNOT_ERR(...)
#define KNOT_INFO(...)
#define KNOT_WARN(...)
#define KNOT_ASSERT(...)
#define KNOT_ASSERT_INFO(...)
#endif /* KNOT_LOG_ENABLED */

#include <kernel.h>

#include <logging/sys_log.h>
#include <string.h>

#endif // __KNOT_CORE_H
