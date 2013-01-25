/**
 *============================================================
 *  @file      switch.cpp
 *  @brief
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}

#include "dispatcher.h"
#include "proto.hpp"
#include "timer_func.h"
#include "global.h"

uint32_t max_msg_count = 0;

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  init_service(int isparent)
{
	if (!isparent) {
		setup_timer();
		init_all_timer_type();
		init_funcs();
        const char *udp_ip = get_server_ip();
        uint16_t udp_port = get_server_port();
        bind_udp_socket(udp_ip, udp_port);
		g_dirty_word = new Cdirty_word();
		if (!g_dirty_word->is_inited)
			return -1;
		regist_timers();
        BOOT_LOG(0,"online udp listen:%s:%d %u",udp_ip, udp_port, max_msg_count);
	}

	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  fini_service(int isparent)
{
	if (!isparent) {
		destroy_timer();
	}
	return 0;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_events()
{
	handle_timer();
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{	
	if (avail_len < 4) {
		return 0;
	}

	const svr_proto_t* pkg = reinterpret_cast<const svr_proto_t*>(avail_data);
	if ((pkg->len > pkg_size) || (pkg->len < sizeof(svr_proto_t))) {
		ERROR_LOG("invalid len=%u from fd=%d", pkg->len, fd);
		return -1;
	}

	return pkg->len;
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	//DEBUG_LOG("CLIENT\t[%u]", fdsess->fd);
	return dispatch(data, fdsess);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
	//DEBUG_LOG("CLIENT CLOSE\t[%u]", fd);
}

/**
  * @brief AsyncServer框架要求实现的接口之一。 
  *
  */
extern "C" void on_fd_closed(int fd)
{
}

extern "C" void proc_udp_pkg(int fd,void* data, int len)
{
	//DEBUG_LOG("GET UDP PACKET");
	if (len < (int)sizeof(svr_proto_t))
		return;

	/*  
	svr_proto_t* pkg = (svr_proto_t*)data;
	if (len != pkg->len)
		return;
	*/
	dispatch(data, NULL);
}
