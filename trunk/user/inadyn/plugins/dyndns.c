/* Plugin for DynDNS2 API compatible services
 *
 * Supported DDNS providers are:
 *   - DynDNS,
 *   - DNS-O-Matic,
 *   - DynSIP,
 *   - no-ip,
 *   - 3322,
 *   - Hurricane-Electric (HE)
 *   - Loopia, and
 *   - nsupdate.info
 *   - nic.ru (RU-CENTER)
 *
 * Copyright (C) 2003-2004  Narcis Ilisei <inarcis2002@hotpop.com>
 * Copyright (C) 2006       Steve Horbachuk
 * Copyright (C) 2010-2014  Joachim Nilsson <troglobit@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit the Free Software Foundation
 * website at http://www.gnu.org/licenses/gpl-2.0.html or write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "plugin.h"

static int request  (ddns_t       *ctx,   ddns_info_t *info, ddns_alias_t *alias);
static int response (http_trans_t *trans, ddns_info_t *info, ddns_alias_t *alias);

static ddns_system_t dyndns = {
	.name         = "default@dyndns.org",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = DYNDNS_MY_IP_SERVER,
	.checkip_url  = DYNDNS_MY_CHECKIP_URL,

	.server_name  = "members.dyndns.org",
	.server_url   = "/nic/update"
};

static ddns_system_t dnsomatic = {
	.name         = "default@dnsomatic.com",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = "myip.dnsomatic.com",
	.checkip_url  = "/",

	.server_name  = "updates.dnsomatic.com",
	.server_url   = "/nic/update"
};

/* Support for dynsip.org by milkfish, from DD-WRT */
static ddns_system_t dynsip = {
	.name         = "default@dynsip.org",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = DYNDNS_MY_IP_SERVER,
	.checkip_url  = DYNDNS_MY_CHECKIP_URL,

	.server_name  = "dynsip.org",
	.server_url   = "/nic/update"
};

static ddns_system_t noip = {
	.name         = "default@no-ip.com",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = "ip1.dynupdate.no-ip.com",
	.checkip_url  = "/",

	.server_name  = "dynupdate.no-ip.com",
	.server_url   = "/nic/update"
};

static ddns_system_t _3322 = {
	.name         = "dyndns@3322.org",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = "bliao.com",
	.checkip_url  = "/ip.phtml",

	.server_name  = "members.3322.org",
	.server_url   = "/dyndns/update"
};

/* See also tunnelbroker.c for Hurricate Electric's IPv6 service */
static ddns_system_t henet = {
	.name         = "dyndns@he.net",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = "checkip.dns.he.net",
	.checkip_url  = "/",

	.server_name  = "dyn.dns.he.net",
	.server_url   = "/nic/update"
};

/* Note: below is IPv4 only. ipv6.nsupdate.info would work IPv6 only. */
static ddns_system_t nsupdate_info_ipv4 = {
	.name         = "ipv4@nsupdate.info",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = "ipv4.nsupdate.info",
	.checkip_url  = "/myip",

	.server_name  = "ipv4.nsupdate.info",
	.server_url   = "/nic/update"
};

/*
 * Loopia supports HTTPS, for details on supported variables, see
 * https://support.loopia.com/wiki/About_the_DynDNS_support
 */
static ddns_system_t loopia = {
	.name         = "default@loopia.com",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = "dns.loopia.se",
	.checkip_url  = "/checkip/checkip.php",

	.server_name  = "dns.loopia.se",
	.server_url   = "/XDynDNSServer/XDynDNS.php"
};

/*
 * nic.ru (RU-CENTER)
 * https://www.nic.ru/dns/service/dns_hosting/dns_master/dynamic_dns_for_developers.html
 */
static ddns_system_t nic_ru = {
	.name         = "default@nic.ru",

	.request      = (req_fn_t)request,
	.response     = (rsp_fn_t)response,

	.checkip_name = "api.nic.ru:443",
	.checkip_url  = "/dyndns/checkip",

	.server_name  = "api.nic.ru:443",
	.server_url   = "/dyndns/update"
};

static int request(ddns_t *ctx, ddns_info_t *info, ddns_alias_t *alias)
{
	return common_request(ctx, info, alias);
}

static int response(http_trans_t *trans, ddns_info_t *info, ddns_alias_t *alias)
{
	return common_response(trans, info, alias);
}

PLUGIN_INIT(plugin_init)
{
	plugin_register(&dyndns);
	plugin_register(&dnsomatic);
	plugin_register(&dynsip);
	plugin_register(&noip);
	plugin_register(&_3322);
	plugin_register(&henet);
	plugin_register(&nsupdate_info_ipv4);
	plugin_register(&loopia);
	plugin_register(&nic_ru);
}

PLUGIN_EXIT(plugin_exit)
{
	plugin_unregister(&dyndns);
	plugin_unregister(&dnsomatic);
	plugin_unregister(&dynsip);
	plugin_unregister(&noip);
	plugin_unregister(&_3322);
	plugin_unregister(&henet);
	plugin_unregister(&nsupdate_info_ipv4);
	plugin_unregister(&loopia);
	plugin_unregister(&nic_ru);
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
