#include "postcard_util.hpp"
#include "hero_sendcard.hpp"
#include "proto_head.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
extern "C"
{
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>
}

std::map<uint32_t, std::vector<CHeroSendCard::conf_t> > CHeroSendCard::m_confs;
std::map<uint32_t, std::vector<std::set<uint32_t> > > CHeroSendCard::m_flags;

bool CHeroSendCard::init()
{
    m_proxy_fd = -1;
    m_switch_fd = -1;

    if (load_xmlconf("../etc/hero.xml", CHeroSendCard::load_cache_xml) == -1)
    {
        ERROR_LOG("load hero.xml failed");
        return false;
    }

    return true;
}

void CHeroSendCard::onProcPkgCli(void* pkg, int pkglen, fdsession_t* fdsess)
{
    if (pkglen != PROTO_HEAD_LEN + 4)
    {
        ERROR_LOG("invalid pkg len pkglen = %u", pkglen);
        return;
    }

    proto_head_t *ph = (proto_head_t*)pkg;
    uint32_t *ip = (uint32_t*)((char*)pkg + PROTO_HEAD_LEN);

    switch (ph->cmd)
    {
    case 0xC080:
        {
            code_t code;
            if (!g_ipDict.find(*ip, code))
            {
                DEBUG_LOG("ipdict can't find ip=%u.%u.%u.%u ip = %u",
                      *(((char*)ip) + 0),
                      *(((char*)ip) + 1),
                      *(((char*)ip) + 2),
                      *(((char*)ip) + 3));
                return;
            }
            DEBUG_LOG("cmd:%u uid:%u ip=%u.%u.%u.%u", ph->cmd, ph->id,
                      *(((char*)ip) + 0),
                      *(((char*)ip) + 1),
                      *(((char*)ip) + 2),
                      *(((char*)ip) + 3));
            
            sendCard(ph->id, code);
            break;
        }
    default:
        ERROR_LOG("invalid cmd:%u", ph->cmd);
        break;
    }

    return;
}

void CHeroSendCard::sendCard(userid_t userid, const code_t &code)
{
    std::map<uint32_t, std::vector<conf_t> >::iterator it;
    it = m_confs.find(code.province_code);
    if (it == m_confs.end())
        return;

    time_t cur = time(0);

    for (uint32_t i = 0; i < (*it).second.size(); ++i)
    {
        if ((*it).second[i].city_code == 0 || (*it).second[i].city_code == code.city_code)
        {
            if (cur < (*it).second[i].begin || cur > (*it).second[i].end)
                continue;
            
            std::map<uint32_t, std::vector<std::set<uint32_t> > >::iterator its;
            its = m_flags.find(code.province_code);
            if (its == m_flags.end())
            {
                ERROR_LOG("flags can't find code.province_code");
                continue; 
            }

            // 已经发过就不发了.
            if ((*its).second[i].find(userid) != (*its).second[i].end())
            {
                continue;
            }

            if (m_proxy_fd == -1)
            {
                DEBUG_LOG("m_proxy_fd == -1 connect_to_service");
                m_proxy_fd = connect_to_service(config_get_strval("service_dbproxy"), 0, 65535, 1);
            }

            proto_head_t *ph = (proto_head_t *)((*it).second[i].buf);
            ph->len = (*it).second[i].proto_len;
            ph->id = userid;
            if (-1 == net_send(m_proxy_fd, (char*)ph, ph->len))
            {
                ERROR_LOG("net_send error uid=%u", ph->id);
            }
            else
            {
                DEBUG_LOG("sendcard uid=%u", ph->id);
                (*its).second[i].insert(userid);
            }
        }
    }
}

void CHeroSendCard::onProcPkgSer(int fd, void* pkg, int pkglen)
{
    if (m_proxy_fd == fd)
    {
        if ((uint32_t)pkglen < PROTO_HEAD_LEN)
        {
            ERROR_LOG("proxy return len error");
            return;
        }

        proto_head_t *ph = (proto_head_t *)(pkg);
        if (ph->ret != 0)
        {
            ERROR_LOG("proxy ret=%u cmd=%u uid=%u", ph->ret, ph->cmd, ph->id);
            return;
        }
    }
    else if (m_switch_fd == fd)
    {
    }
}

void CHeroSendCard::onLinkDown(int fd)
{
    if (m_proxy_fd == fd)
    {
        DEBUG_LOG("proxy link down");
        m_proxy_fd = -1;
    }
    else if (m_switch_fd == fd)
    {
        m_switch_fd = -1;
    }
    else
        return;
}

struct user_mail_send_t
{
    uint32_t flag;
    uint32_t type;
    uint32_t themeid;
    uint32_t senderid;
    char sendernick[16];
    char title[64];
    char message[256];
    char items[64];
    char ids[32];
}__attribute__((packed));

int CHeroSendCard::load_cache_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char begin[64];
            char end[64];
            uint32_t province;
            uint32_t city;

            user_mail_send_t ums;           
            memset(&ums, 0, sizeof(ums));
 
            DECODE_XML_PROP_STR(begin,      cur, "begin");
            DECODE_XML_PROP_STR(end,        cur, "end");
            DECODE_XML_PROP_UINT32(province,cur, "province");
            DECODE_XML_PROP_UINT32(city,    cur, "city");
            DECODE_XML_PROP_UINT32(ums.flag,cur, "flag");
            DECODE_XML_PROP_UINT32(ums.type,cur, "type");
            DECODE_XML_PROP_UINT32(ums.themeid, cur, "themeid");
            DECODE_XML_PROP_UINT32(ums.senderid,cur, "senderid");
            DECODE_XML_PROP_STR(ums.sendernick, cur, "sendernick");
            DECODE_XML_PROP_STR(ums.title, cur, "title");
            DECODE_XML_PROP_STR(ums.message, cur, "message");

            conf_t conf;
            memset(&conf, 0, sizeof(conf));

            if (!stringToTime(begin, conf.begin))
            {
                ERROR_LOG("read begin time failed");
                return -1;
            }
            if (!stringToTime(end, conf.end))
            {
                ERROR_LOG("read end time failed");
                return -1;
            }

            conf.province_code = province;
            conf.city_code = city;

            proto_head_t *ph = (proto_head_t *)(conf.buf);
            ph->seq = 0;
            ph->cmd = 0x09B9;
            ph->ret = 0;
            
            DEBUG_LOG("==========load conf==========");           

            int p = sizeof(proto_head_t);
            DEBUG_LOG("load conf flag = %u", ums.flag);           
            DEBUG_LOG("load conf type = %u", ums.type);           
            DEBUG_LOG("load conf themeid = %u", ums.themeid);
            DEBUG_LOG("load conf senderid = %u", ums.senderid);
            DEBUG_LOG("load conf sendernick = %s", ums.sendernick);
            DEBUG_LOG("load conf title = %s", ums.title);
            DEBUG_LOG("load conf message = %s", ums.message);
          
            memcpy(&conf.buf[p], &ums, sizeof(ums));
            p += sizeof(ums);
            conf.proto_len = p;

            std::map<uint32_t, std::vector<conf_t> >::iterator it;
            it = m_confs.find(conf.province_code);
            if (it == m_confs.end())
            {
                m_confs[conf.province_code] = std::vector<conf_t>();
                m_flags[conf.province_code] = std::vector<std::set<uint32_t> >();
            }
            m_confs[conf.province_code].push_back(conf);
            m_flags[conf.province_code].push_back(std::set<uint32_t>());
        }
        cur = cur->next;
    }

    return 0; 
}

