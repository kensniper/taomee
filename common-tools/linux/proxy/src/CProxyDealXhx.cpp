#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee/project/utilities.h>

#include "CProxyDealXhx.hpp"
#include "CProxyClientManage.hpp"
#include "CProxyRoute.hpp"
#include "utils.hpp"

using namespace taomee;

int
CProxyDealXhx::customer_service_fixed_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(xhx_db_proto_t);
	pack(pkgbuf, body, bodylen, idx);

	init_db_proto_head(pkgbuf, info, 0, info->waitcmd, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealXhx::get_friends_list_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(xhx_db_proto_t);

	init_db_proto_head(pkgbuf, info, 0, xhx_dbproto_get_friends_list, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealXhx::get_user_nick_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(xhx_db_proto_t);

	init_db_proto_head(pkgbuf, info, 0, xhx_dbproto_get_user_nick, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealXhx::get_user_info_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(xhx_db_proto_t);

	init_db_proto_head(pkgbuf, info, 0, xhx_dbproto_get_user_info, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealXhx::get_user_cloth_info_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	int idx = sizeof(xhx_db_proto_t);

	init_db_proto_head(pkgbuf, info, 0, xhx_dbproto_get_user_cloth_info, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealXhx::send_mail_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	send_mail_t* p_mail_info = reinterpret_cast<send_mail_t*>(body);

	if (p_mail_info->title_len > sizeof(p_mail_info->title) || p_mail_info->msg_len > sizeof(p_mail_info->msg)) {
		ERROR_LOG("gf email msg len error[%u %u %u]", info->userid, p_mail_info->title_len, p_mail_info->msg_len);
		return send_header_to_client(info, info->waitcmd, cli_err_email_len_err);
	}

	uint32_t font_color = 0;
	int idx = sizeof(xhx_db_proto_t);
	pack_h(pkgbuf, p_mail_info->send_id, idx);
	pack(pkgbuf, p_mail_info->nick, NICK_LEN, idx);
	pack_h(pkgbuf, p_mail_info->mail_type, idx);
	pack_h(pkgbuf, font_color, idx);
	pack_h(pkgbuf, p_mail_info->title_len, idx);
	pack(pkgbuf, p_mail_info->title, p_mail_info->title_len, idx);
	pack_h(pkgbuf, p_mail_info->msg_len, idx);
	pack(pkgbuf, p_mail_info->msg, p_mail_info->msg_len, idx);

	DEBUG_LOG("XHX:SEND EMAIL\t[uid=%u mail=%u]", info->userid, p_mail_info->mail_type);
	init_db_proto_head(pkgbuf, info, 0, xhx_dbproto_send_mail, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealXhx::add_items_cmd(request_info_t* info, uint8_t* body, int bodylen)
{
	add_items_t* p_item = reinterpret_cast<add_items_t*>(body);
	CHECK_VAL_EQ(bodylen, static_cast<int>(sizeof(add_items_t) + p_item->item_cnt * sizeof(add_item_info_t)));

	int idx = sizeof(xhx_db_proto_t);
	DEBUG_LOG("XHX:ADD ITEMS\t[uid=%u item_cnt=%u]",info->userid, p_item->item_cnt);
	for (uint32_t i = 0; i < p_item->item_cnt; i++) {
		add_item_info_t* p_item_info = &(p_item->items[i]);
		if (p_item_info->item_type == t_attribute) {
			return add_money(info, p_item_info->item_count);
		}
		//pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
		pack_h(pkgbuf, p_item_info->item_id, idx);
		pack_h(pkgbuf, p_item_info->item_count, idx);
		pack_h(pkgbuf, p_item_info->max_count, idx);
		DEBUG_LOG("XHX:ITEMS\t[id=%u cnt=%u max=%u]", p_item_info->item_id, p_item_info->item_count, p_item_info->max_count);
		break;
	}
	
	init_db_proto_head(pkgbuf, info, 0, xhx_dbproto_add_items, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealXhx::add_money(request_info_t* info, uint32_t add_value)
{
	int idx = sizeof(xhx_db_proto_t);
	pack_h(pkgbuf, add_value, idx);

	DEBUG_LOG("XHX:MONEY\t[count=%u]", add_value);
	init_db_proto_head(pkgbuf, info, 0, xhx_dbproto_add_money, idx);
	return send_request_to_svr(info, pkgbuf, idx);
}

int
CProxyDealXhx::handle_db_return(void* data, int len)
{
	xhx_db_proto_t* pkg = reinterpret_cast<xhx_db_proto_t*>(data);
	//int connfd = pkg->seq >> 16;
	//int idx = pkg->seq & 0xFFFF;

	request_info_t* info = g_clients.get_request_info(pkg->seq);
	if (!info) {
		//ERROR_LOG("fd error[fd=%d]", connfd);
		ERROR_LOG("fd error[fd=%d]", pkg->seq);
		return -1;			    
	}
	/*if (info->id != idx) {
		ERROR_LOG("idx error[fd=%d idx=[%d %d] uid=%u]", connfd, info->id, idx, info->userid);
		return -1;			    
	}*/
	DEBUG_LOG("DB R\t[pkglen=%u idx=%u uid=%u cmd=%d ret=%d]", pkg->len, pkg->seq, pkg->uid, pkg->cmd, pkg->ret);

	int err = -1;
	uint32_t body_len = pkg->len - sizeof(xhx_db_proto_t);

	switch (pkg->cmd) {
		case xhx_dbproto_get_friends_list:
			err = dbproto_get_friends_list_callback(info, pkg->body, body_len, pkg->ret);
			break;
		case xhx_dbproto_get_user_nick:
			err = dbproto_get_user_nick_callback(info, pkg->body, body_len, pkg->ret);
			break;
		case xhx_dbproto_get_user_info:
			err = dbproto_get_user_info_callback(info, pkg->body, body_len, pkg->ret);
			break;
		case xhx_dbproto_send_mail:
			err = dbproto_send_mail_callback(info, pkg->body, body_len, pkg->ret);
			break;
		case xhx_dbproto_add_items:
			err = dbproto_add_items_callback(info, pkg->body, body_len, pkg->ret);
			break;
		case xhx_dbproto_add_money:
			err = dbproto_add_money_callback(info, pkg->body, body_len, pkg->ret);
			break;
		default:
			ERROR_LOG("cmd error[cmd=%d]", pkg->cmd);
	}

	if (err != 0) {
		//close_client_conn(connfd);
		close_client_conn(info->fd);
	}

	return 0;
}

int 
CProxyDealXhx::init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len)
{
	xhx_db_proto_t* proto = reinterpret_cast<xhx_db_proto_t*>(header);

	proto->len = len;
	//proto->seq = (info->fd << 16) | (info->id) ;
	proto->seq = info->id;
	proto->cmd = cmd;
	proto->ret = 0;
	proto->uid = info->userid;

	return 0;
}

int
CProxyDealXhx::dbproto_get_friends_list_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = 0;
	int db_idx = 0;
	uint32_t friends_cnt = 0;
	unpack_h(body, friends_cnt, db_idx);

	CHECK_VAL_EQ((uint32_t)body_len, 4 * friends_cnt + 4);

	DEBUG_LOG("get friends cnt[%u %u]", info->userid, friends_cnt);
	
	idx = sizeof(cli_proto_t);
	pack_h(cli_pkgbuf, friends_cnt, idx);
	pack(cli_pkgbuf, body + db_idx, 4 * friends_cnt, idx);
	init_cli_header(cli_pkgbuf, idx, info, cli_get_friends_list, 0);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealXhx::dbproto_get_user_info_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	pack(cli_pkgbuf, body, body_len, idx);
	init_cli_header(cli_pkgbuf, idx, info, cli_get_user_info, 0);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealXhx::dbproto_get_user_nick_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	pack(cli_pkgbuf, body, NICK_LEN, idx);
	init_cli_header(cli_pkgbuf, idx, info, cli_get_user_nick, ret);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealXhx::dbproto_send_mail_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_send_mail, ret);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealXhx::dbproto_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_add_items, ret);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}

int
CProxyDealXhx::dbproto_add_money_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret)
{
	CHECK_DBERR(info, ret);

	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cli_add_items, ret);
	//g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	g_clients.send_to_client(info->id, cli_pkgbuf, idx);
	
	return 0;
}


