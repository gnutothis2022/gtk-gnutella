/*
 * Copyright (c) 2001-2003, Richard Eckart
 *
 * THIS FILE IS AUTOGENERATED! DO NOT EDIT!
 * This file is generated from gnet_props.ag using autogen.
 * Autogen is available at http://autogen.sourceforge.net/.
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

#ifndef _gnet_property_priv_h_
#define _gnet_property_priv_h_

#include <glib.h>

#include "lib/prop.h"

#ifdef CORE_SOURCES

#define GNET_PROPERTY(name) 	(gnet_property_variable_ ## name)
#define GNET_PROPERTY_PTR(name)	(&gnet_property_variable_ ## name)

/*
 * Includes specified by "uses"-statement in .ag file
 */
#include "core/sockets.h"
#include "core/bsched.h"
#include "lib/misc.h"
#include "if/core/nodes.h"
#include "if/core/sockets.h"
#include "dht/kuid.h"
#include "if/dht/routing.h"
#include "upnp/upnp.h"


extern const gboolean gnet_property_variable_reading_hostfile;
extern const gboolean gnet_property_variable_reading_ultrafile;
extern const gboolean gnet_property_variable_ancient_version;
extern const char   *gnet_property_variable_new_version_str;
extern const guint32  gnet_property_variable_up_connections;
extern const guint32  gnet_property_variable_normal_connections;
extern const guint32  gnet_property_variable_max_connections;
extern const guint32  gnet_property_variable_node_leaf_count;
extern const guint32  gnet_property_variable_node_normal_count;
extern const guint32  gnet_property_variable_node_ultra_count;
extern const guint32  gnet_property_variable_max_downloads;
extern const guint32  gnet_property_variable_max_host_downloads;
extern const guint32  gnet_property_variable_max_uploads;
extern const guint32  gnet_property_variable_max_uploads_ip;
extern const host_addr_t  gnet_property_variable_local_ip;
extern const host_addr_t  gnet_property_variable_local_ip6;
extern const time_t  gnet_property_variable_current_ip_stamp;
extern const time_t  gnet_property_variable_current_ip6_stamp;
extern const guint32  gnet_property_variable_average_ip_uptime;
extern const guint32  gnet_property_variable_average_ip6_uptime;
extern const time_t  gnet_property_variable_start_stamp;
extern const guint32  gnet_property_variable_average_servent_uptime;
extern const guint32  gnet_property_variable_listen_port;
extern const host_addr_t  gnet_property_variable_forced_local_ip;
extern const host_addr_t  gnet_property_variable_forced_local_ip6;
extern const guint32  gnet_property_variable_connection_speed;
extern const gboolean gnet_property_variable_compute_connection_speed;
extern const guint32  gnet_property_variable_search_max_items;
extern const guint32  gnet_property_variable_ul_usage_min_percentage;
extern const guint32  gnet_property_variable_download_connecting_timeout;
extern const guint32  gnet_property_variable_download_push_sent_timeout;
extern const guint32  gnet_property_variable_download_connected_timeout;
extern const guint32  gnet_property_variable_download_retry_timeout_min;
extern const guint32  gnet_property_variable_download_retry_timeout_max;
extern const guint32  gnet_property_variable_download_max_retries;
extern const guint32  gnet_property_variable_download_retry_timeout_delay;
extern const guint32  gnet_property_variable_download_retry_busy_delay;
extern const guint32  gnet_property_variable_download_retry_refused_delay;
extern const guint32  gnet_property_variable_download_retry_stopped_delay;
extern const guint32  gnet_property_variable_download_overlap_range;
extern const guint32  gnet_property_variable_upload_connecting_timeout;
extern const guint32  gnet_property_variable_upload_connected_timeout;
extern const guint32  gnet_property_variable_search_reissue_timeout;
extern const guint32  gnet_property_variable_ban_ratio_fds;
extern const guint32  gnet_property_variable_ban_max_fds;
extern const guint32  gnet_property_variable_banned_count;
extern const guint32  gnet_property_variable_max_banned_fd;
extern const guint32  gnet_property_variable_incoming_connecting_timeout;
extern const guint32  gnet_property_variable_node_connecting_timeout;
extern const guint32  gnet_property_variable_node_connected_timeout;
extern const guint32  gnet_property_variable_node_sendqueue_size;
extern const guint32  gnet_property_variable_node_tx_flowc_timeout;
extern const guint32  gnet_property_variable_node_rx_flowc_ratio;
extern const guint32  gnet_property_variable_max_ttl;
extern const guint32  gnet_property_variable_my_ttl;
extern const guint32  gnet_property_variable_hard_ttl_limit;
extern const guint32  gnet_property_variable_dbg;
extern const guint32  gnet_property_variable_ban_debug;
extern const guint32  gnet_property_variable_cpu_debug;
extern const guint32  gnet_property_variable_dmesh_debug;
extern const guint32  gnet_property_variable_gmsg_debug;
extern const guint32  gnet_property_variable_share_debug;
extern const guint32  gnet_property_variable_node_debug;
extern const guint32  gnet_property_variable_socket_debug;
extern const guint32  gnet_property_variable_bootstrap_debug;
extern const guint32  gnet_property_variable_http_debug;
extern const guint32  gnet_property_variable_download_debug;
extern const guint32  gnet_property_variable_fileinfo_debug;
extern const guint32  gnet_property_variable_upload_debug;
extern const guint32  gnet_property_variable_lib_debug;
extern const guint32  gnet_property_variable_url_debug;
extern const guint32  gnet_property_variable_dh_debug;
extern const guint32  gnet_property_variable_dq_debug;
extern const guint32  gnet_property_variable_vmsg_debug;
extern const guint32  gnet_property_variable_query_debug;
extern const guint32  gnet_property_variable_search_debug;
extern const guint32  gnet_property_variable_udp_debug;
extern const guint32  gnet_property_variable_qrp_debug;
extern const guint32  gnet_property_variable_routing_debug;
extern const guint32  gnet_property_variable_ggep_debug;
extern const guint32  gnet_property_variable_pcache_debug;
extern const guint32  gnet_property_variable_hsep_debug;
extern const guint32  gnet_property_variable_tls_debug;
extern const guint32  gnet_property_variable_parq_debug;
extern const gboolean gnet_property_variable_parq_optimistic;
extern const guint32  gnet_property_variable_parq_size_always_continue;
extern const guint32  gnet_property_variable_parq_time_always_continue;
extern const guint32  gnet_property_variable_parq_ban_bad_maxcountwait;
extern const gboolean gnet_property_variable_net_buffer_shortage;
extern const gboolean gnet_property_variable_stop_host_get;
extern const gboolean gnet_property_variable_bws_in_enabled;
extern const gboolean gnet_property_variable_bws_out_enabled;
extern const gboolean gnet_property_variable_bws_gin_enabled;
extern const gboolean gnet_property_variable_bws_glin_enabled;
extern const gboolean gnet_property_variable_bws_gout_enabled;
extern const gboolean gnet_property_variable_bws_glout_enabled;
extern const gboolean gnet_property_variable_bw_ul_usage_enabled;
extern const gboolean gnet_property_variable_bw_allow_stealing;
extern const gboolean gnet_property_variable_clear_complete_downloads;
extern const gboolean gnet_property_variable_clear_failed_downloads;
extern const gboolean gnet_property_variable_clear_unavailable_downloads;
extern const gboolean gnet_property_variable_clear_finished_downloads;
extern const gboolean gnet_property_variable_search_remove_downloaded;
extern const gboolean gnet_property_variable_force_local_ip;
extern const gboolean gnet_property_variable_force_local_ip6;
extern const gboolean gnet_property_variable_bind_to_forced_local_ip;
extern const gboolean gnet_property_variable_bind_to_forced_local_ip6;
extern const gboolean gnet_property_variable_use_netmasks;
extern const gboolean gnet_property_variable_allow_private_network_connection;
extern const gboolean gnet_property_variable_use_ip_tos;
extern const gboolean gnet_property_variable_download_delete_aborted;
extern const gboolean gnet_property_variable_proxy_auth;
extern const char   *gnet_property_variable_socks_user;
extern const char   *gnet_property_variable_socks_pass;
extern const host_addr_t  gnet_property_variable_proxy_addr;
extern const char   *gnet_property_variable_proxy_hostname;
extern const guint32  gnet_property_variable_proxy_port;
extern const guint32  gnet_property_variable_proxy_protocol;
extern const guint32  gnet_property_variable_network_protocol;
extern const gboolean gnet_property_variable_use_ipv6_trt;
extern const host_addr_t  gnet_property_variable_ipv6_trt_prefix;
extern const guint32  gnet_property_variable_hosts_in_catcher;
extern const guint32  gnet_property_variable_hosts_in_ultra_catcher;
extern const guint32  gnet_property_variable_hosts_in_bad_catcher;
extern const guint32  gnet_property_variable_max_hosts_cached;
extern const guint32  gnet_property_variable_max_ultra_hosts_cached;
extern const guint32  gnet_property_variable_max_bad_hosts_cached;
extern const guint32  gnet_property_variable_max_high_ttl_msg;
extern const guint32  gnet_property_variable_max_high_ttl_radius;
extern const guint64  gnet_property_variable_bw_http_in;
extern const guint64  gnet_property_variable_bw_http_out;
extern const guint64  gnet_property_variable_bw_gnet_in;
extern const guint64  gnet_property_variable_bw_gnet_out;
extern const guint64  gnet_property_variable_bw_gnet_lin;
extern const guint64  gnet_property_variable_bw_gnet_lout;
extern const guint32  gnet_property_variable_search_queries_forward_size;
extern const guint32  gnet_property_variable_search_queries_kick_size;
extern const guint32  gnet_property_variable_search_answers_forward_size;
extern const guint32  gnet_property_variable_search_answers_kick_size;
extern const guint32  gnet_property_variable_search_muid_track_amount;
extern const guint32  gnet_property_variable_other_messages_kick_size;
extern const guint32  gnet_property_variable_hops_random_factor;
extern const gboolean gnet_property_variable_send_pushes;
extern const guint32  gnet_property_variable_min_dup_msg;
extern const guint32  gnet_property_variable_min_dup_ratio;
extern const char   *gnet_property_variable_scan_extensions;
extern const gboolean gnet_property_variable_scan_ignore_symlink_dirs;
extern const gboolean gnet_property_variable_scan_ignore_symlink_regfiles;
extern const char   *gnet_property_variable_save_file_path;
extern const char   *gnet_property_variable_move_file_path;
extern const char   *gnet_property_variable_bad_file_path;
extern const char   *gnet_property_variable_shared_dirs_paths;
extern const char   *gnet_property_variable_local_netmasks_string;
extern const guint32  gnet_property_variable_total_downloads;
extern const guint32  gnet_property_variable_ul_running;
extern const guint32  gnet_property_variable_ul_quick_running;
extern const guint32  gnet_property_variable_ul_registered;
extern const guint32  gnet_property_variable_total_uploads;
extern const char    gnet_property_variable_servent_guid[GUID_RAW_SIZE];
extern const char    gnet_property_variable_kuid[KUID_RAW_SIZE];
extern const gboolean gnet_property_variable_use_swarming;
extern const gboolean gnet_property_variable_use_aggressive_swarming;
extern const guint32  gnet_property_variable_dl_minchunksize;
extern const guint32  gnet_property_variable_dl_maxchunksize;
extern const gboolean gnet_property_variable_auto_download_identical;
extern const gboolean gnet_property_variable_auto_feed_download_mesh;
extern const gboolean gnet_property_variable_strict_sha1_matching;
extern const gboolean gnet_property_variable_is_firewalled;
extern const gboolean gnet_property_variable_is_inet_connected;
extern const gboolean gnet_property_variable_is_udp_firewalled;
extern const gboolean gnet_property_variable_recv_solicited_udp;
extern const gboolean gnet_property_variable_gnet_compact_query;
extern const gboolean gnet_property_variable_download_optimistic_start;
extern const gboolean gnet_property_variable_library_rebuilding;
extern const gboolean gnet_property_variable_sha1_rebuilding;
extern const gboolean gnet_property_variable_sha1_verifying;
extern const gboolean gnet_property_variable_file_moving;
extern const gboolean gnet_property_variable_prefer_compressed_gnet;
extern const gboolean gnet_property_variable_online_mode;
extern const gboolean gnet_property_variable_download_require_urn;
extern const gboolean gnet_property_variable_download_require_server_name;
extern const guint32  gnet_property_variable_max_ultrapeers;
extern const guint32  gnet_property_variable_quick_connect_pool_size;
extern const guint32  gnet_property_variable_max_leaves;
extern const guint32  gnet_property_variable_search_handle_ignored_files;
extern const guint32  gnet_property_variable_configured_peermode;
extern const guint32  gnet_property_variable_current_peermode;
extern const guint32  gnet_property_variable_sys_nofile;
extern const guint64  gnet_property_variable_sys_physmem;
extern const guint32  gnet_property_variable_dl_queue_count;
extern const guint32  gnet_property_variable_dl_running_count;
extern const guint32  gnet_property_variable_dl_active_count;
extern const guint32  gnet_property_variable_dl_aqueued_count;
extern const guint32  gnet_property_variable_dl_pqueued_count;
extern const guint32  gnet_property_variable_fi_all_count;
extern const guint32  gnet_property_variable_fi_with_source_count;
extern const guint32  gnet_property_variable_dl_qalive_count;
extern const guint64  gnet_property_variable_dl_byte_count;
extern const guint64  gnet_property_variable_ul_byte_count;
extern const gboolean gnet_property_variable_pfsp_server;
extern const guint32  gnet_property_variable_pfsp_first_chunk;
extern const guint32  gnet_property_variable_pfsp_minimum_filesize;
extern const guint32  gnet_property_variable_crawler_visit_count;
extern const guint32  gnet_property_variable_udp_crawler_visit_count;
extern const gboolean gnet_property_variable_host_runs_ntp;
extern const gboolean gnet_property_variable_ntp_detected;
extern const guint32  gnet_property_variable_clock_skew;
extern const gboolean gnet_property_variable_node_monitor_unstable_ip;
extern const gboolean gnet_property_variable_node_monitor_unstable_servents;
extern const gboolean gnet_property_variable_dl_remove_file_on_mismatch;
extern const guint32  gnet_property_variable_dl_mismatch_backout;
extern const char   *gnet_property_variable_server_hostname;
extern const gboolean gnet_property_variable_give_server_hostname;
extern const guint32  gnet_property_variable_reserve_gtkg_nodes;
extern const guint32  gnet_property_variable_unique_nodes;
extern const guint32  gnet_property_variable_download_rx_size;
extern const guint32  gnet_property_variable_node_rx_size;
extern const guint32  gnet_property_variable_upload_tx_size;
extern const guint32  gnet_property_variable_dl_http_latency;
extern const time_t  gnet_property_variable_node_last_ultra_check;
extern const time_t  gnet_property_variable_node_last_ultra_leaf_switch;
extern const gboolean gnet_property_variable_up_req_avg_servent_uptime;
extern const gboolean gnet_property_variable_up_req_avg_ip_uptime;
extern const gboolean gnet_property_variable_up_req_node_uptime;
extern const gboolean gnet_property_variable_up_req_not_firewalled;
extern const gboolean gnet_property_variable_up_req_enough_conn;
extern const gboolean gnet_property_variable_up_req_enough_fd;
extern const gboolean gnet_property_variable_up_req_enough_mem;
extern const gboolean gnet_property_variable_up_req_enough_bw;
extern const gboolean gnet_property_variable_up_req_good_udp;
extern const guint32  gnet_property_variable_search_queue_size;
extern const guint32  gnet_property_variable_search_queue_spacing;
extern const gboolean gnet_property_variable_enable_shell;
extern const guint32  gnet_property_variable_remote_shell_timeout;
extern const guint32  gnet_property_variable_entry_removal_timeout;
extern const gboolean gnet_property_variable_node_watch_similar_queries;
extern const guint32  gnet_property_variable_node_queries_half_life;
extern const guint32  gnet_property_variable_node_requery_threshold;
extern const time_t  gnet_property_variable_library_rescan_started;
extern const time_t  gnet_property_variable_library_rescan_finished;
extern const guint32  gnet_property_variable_library_rescan_duration;
extern const time_t  gnet_property_variable_qrp_indexing_started;
extern const guint32  gnet_property_variable_qrp_indexing_duration;
extern const guint32  gnet_property_variable_qrp_memory;
extern const time_t  gnet_property_variable_qrp_timestamp;
extern const guint32  gnet_property_variable_qrp_computation_time;
extern const time_t  gnet_property_variable_qrp_patch_timestamp;
extern const guint32  gnet_property_variable_qrp_patch_computation_time;
extern const guint32  gnet_property_variable_qrp_generation;
extern const guint32  gnet_property_variable_qrp_slots;
extern const guint32  gnet_property_variable_qrp_slots_filled;
extern const guint32  gnet_property_variable_qrp_fill_ratio;
extern const guint32  gnet_property_variable_qrp_conflict_ratio;
extern const guint32  gnet_property_variable_qrp_hashed_keywords;
extern const guint32  gnet_property_variable_qrp_patch_raw_length;
extern const guint32  gnet_property_variable_qrp_patch_length;
extern const guint32  gnet_property_variable_qrp_patch_comp_ratio;
extern const guint32  gnet_property_variable_ancient_version_left_days;
extern const gboolean gnet_property_variable_file_descriptor_shortage;
extern const gboolean gnet_property_variable_file_descriptor_runout;
extern const gboolean gnet_property_variable_convert_spaces;
extern const gboolean gnet_property_variable_convert_evil_chars;
extern const gboolean gnet_property_variable_convert_old_filenames;
extern const gboolean gnet_property_variable_tls_enforce;
extern const gboolean gnet_property_variable_gnet_deflate_enabled;
extern const gboolean gnet_property_variable_enable_udp;
extern const gboolean gnet_property_variable_process_oob_queries;
extern const gboolean gnet_property_variable_send_oob_queries;
extern const gboolean gnet_property_variable_proxy_oob_queries;
extern const gboolean gnet_property_variable_uploads_stalling;
extern const gboolean gnet_property_variable_use_global_hostiles_txt;
extern const gboolean gnet_property_variable_use_so_linger;
extern const guint32  gnet_property_variable_tcp_defer_accept_timeout;
extern const gboolean gnet_property_variable_browse_host_enabled;
extern const guint32  gnet_property_variable_html_browse_count;
extern const guint32  gnet_property_variable_html_browse_served;
extern const guint32  gnet_property_variable_qhits_browse_count;
extern const guint32  gnet_property_variable_qhits_browse_served;
extern const gboolean gnet_property_variable_overloaded_cpu;
extern const guint32  gnet_property_variable_download_buffer_size;
extern const gboolean gnet_property_variable_browse_copied_to_passive;
extern const gboolean gnet_property_variable_display_metric_units;
extern const guint32  gnet_property_variable_rudp_debug;
extern const guint32  gnet_property_variable_dht_debug;
extern const guint32  gnet_property_variable_oob_proxy_debug;
extern const gboolean gnet_property_variable_enable_local_socket;
extern const guint32  gnet_property_variable_max_simultaneous_downloads_per_file;
extern const gboolean gnet_property_variable_dump_received_gnutella_packets;
extern const gboolean gnet_property_variable_search_results_expose_relative_paths;
extern const guint32  gnet_property_variable_shell_debug;
extern const guint32  gnet_property_variable_download_queue_frozen;
extern const guint32  gnet_property_variable_sq_debug;
extern const guint32  gnet_property_variable_push_proxy_debug;
extern const guint32  gnet_property_variable_thex_files_requested;
extern const guint32  gnet_property_variable_thex_files_served;
extern const guint32  gnet_property_variable_tigertree_debug;
extern const gboolean gnet_property_variable_tth_rebuilding;
extern const gboolean gnet_property_variable_tth_verifying;
extern const guint32  gnet_property_variable_search_max_results;
extern const guint32  gnet_property_variable_browse_host_max_results;
extern const time_t  gnet_property_variable_latest_svn_release_date;
extern const guint32  gnet_property_variable_latest_svn_release_revision;
extern const char   *gnet_property_variable_latest_svn_release_signature;
extern const guint32  gnet_property_variable_tls_cache_max_hosts;
extern const guint32  gnet_property_variable_tls_cache_max_time;
extern const gboolean gnet_property_variable_tth_auto_discovery;
extern const gboolean gnet_property_variable_enable_hackarounds;
extern const guint32  gnet_property_variable_g2_cache_max_hosts;
extern const guint32  gnet_property_variable_g2_cache_max_time;
extern const guint32  gnet_property_variable_g2_debug;
extern const gboolean gnet_property_variable_dl_resource_switching;
extern const gboolean gnet_property_variable_parq_enabled;
extern const guint32  gnet_property_variable_pfsp_last_chunk;
extern const gboolean gnet_property_variable_beautify_filenames;
extern const gboolean gnet_property_variable_enable_dht;
extern const guint32  gnet_property_variable_cq_debug;
extern const guint32  gnet_property_variable_dht_lookup_debug;
extern const guint32  gnet_property_variable_dht_storage_debug;
extern const guint32  gnet_property_variable_dht_publish_debug;
extern const guint64  gnet_property_variable_bw_dht_lookup_out;
extern const guint64  gnet_property_variable_bw_dht_lookup_in;
extern const gboolean gnet_property_variable_sticky_guid;
extern const gboolean gnet_property_variable_sticky_kuid;
extern const guint32  gnet_property_variable_dht_ulq_debug;
extern const gboolean gnet_property_variable_dht_storage_in_memory;
extern const guint32  gnet_property_variable_download_trace;
extern const guint32  gnet_property_variable_upload_trace;
extern const guint32  gnet_property_variable_gnet_trace;
extern const guint32  gnet_property_variable_push_proxy_trace;
extern const guint32  gnet_property_variable_http_trace;
extern const guint32  gnet_property_variable_verify_debug;
extern const guint32  gnet_property_variable_local_addr_cache_max_hosts;
extern const guint32  gnet_property_variable_local_addr_cache_max_time;
extern const guint32  gnet_property_variable_local_addr_debug;
extern const gboolean gnet_property_variable_dump_transmitted_gnutella_packets;
extern const guint32  gnet_property_variable_mq_tcp_debug;
extern const guint32  gnet_property_variable_mq_udp_debug;
extern const guint32  gnet_property_variable_node_udp_sendqueue_size;
extern const guint32  gnet_property_variable_clock_debug;
extern const guint32  gnet_property_variable_fw_debug;
extern const guint32  gnet_property_variable_host_debug;
extern const guint32  gnet_property_variable_dht_roots_debug;
extern const guint32  gnet_property_variable_lib_stats;
extern const gboolean gnet_property_variable_spam_lut_in_memory;
extern const guint32  gnet_property_variable_spam_debug;
extern const guint32  gnet_property_variable_lockfile_debug;
extern const guint32  gnet_property_variable_zalloc_debug;
extern const guint32  gnet_property_variable_palloc_debug;
extern const guint32  gnet_property_variable_rxbuf_debug;
extern const gboolean gnet_property_variable_zalloc_always_gc;
extern const guint32  gnet_property_variable_vmm_debug;
extern const guint32  gnet_property_variable_shutdown_debug;
extern const char   *gnet_property_variable_country_limits;
extern const guint32  gnet_property_variable_ctl_debug;
extern const gboolean gnet_property_variable_log_dropped_gnutella;
extern const guint32  gnet_property_variable_whitelist_debug;
extern const guint32  gnet_property_variable_dht_tcache_debug;
extern const guint32  gnet_property_variable_publisher_debug;
extern const guint32  gnet_property_variable_dht_trace;
extern const gboolean gnet_property_variable_bws_dht_out_enabled;
extern const guint64  gnet_property_variable_bw_dht_out;
extern const guint32  gnet_property_variable_node_dht_sendqueue_size;
extern const guint32  gnet_property_variable_bsched_debug;
extern const guint32  gnet_property_variable_dht_stable_debug;
extern const guint32  gnet_property_variable_reload_debug;
extern const guint32  gnet_property_variable_move_debug;
extern const guint32  gnet_property_variable_qhit_debug;
extern const guint32  gnet_property_variable_version_debug;
extern const guint64  gnet_property_variable_cpu_freq_min;
extern const guint64  gnet_property_variable_cpu_freq_max;
extern const guint32  gnet_property_variable_dht_boot_status;
extern const guint32  gnet_property_variable_dht_configured_mode;
extern const guint32  gnet_property_variable_dht_current_mode;
extern const guint32  gnet_property_variable_omalloc_debug;
extern const guint32  gnet_property_variable_hcache_debug;
extern const char    gnet_property_variable_randomness[KUID_RAW_SIZE];
extern const guint32  gnet_property_variable_average_servent_downtime;
extern const time_t  gnet_property_variable_shutdown_time;
extern const guint32  gnet_property_variable_alive_debug;
extern const guint32  gnet_property_variable_vxml_debug;
extern const guint32  gnet_property_variable_upnp_debug;
extern const guint32  gnet_property_variable_soap_debug;
extern const guint32  gnet_property_variable_soap_trace;
extern const gboolean gnet_property_variable_allow_firewalled_ultra;
extern const gboolean gnet_property_variable_enable_upnp;
extern const gboolean gnet_property_variable_upnp_possible;
extern const gboolean gnet_property_variable_port_mapping_required;
extern const gboolean gnet_property_variable_port_mapping_possible;
extern const guint32  gnet_property_variable_natpmp_debug;
extern const gboolean gnet_property_variable_enable_natpmp;
extern const gboolean gnet_property_variable_natpmp_possible;
extern const guint32  gnet_property_variable_tx_debug;
extern const guint32  gnet_property_variable_rx_debug;
extern const guint32  gnet_property_variable_inputevt_debug;
extern const guint32  gnet_property_variable_bg_debug;
extern const gboolean gnet_property_variable_port_mapping_successful;
extern const gboolean gnet_property_variable_uploads_bw_no_stealing;
extern const gboolean gnet_property_variable_uploads_bw_ignore_stolen;
extern const gboolean gnet_property_variable_uploads_bw_uniform;
extern const gboolean gnet_property_variable_enable_http_pipelining;
extern const guint32  gnet_property_variable_dl_pipeline_maxchunksize;
extern const gboolean gnet_property_variable_enable_guess;
extern const guint32  gnet_property_variable_guess_server_debug;
extern const guint32  gnet_property_variable_guess_client_debug;
extern const guint32  gnet_property_variable_max_guess_hosts_cached;
extern const guint32  gnet_property_variable_hosts_in_guess_catcher;
extern const guint32  gnet_property_variable_max_guess_intro_hosts_cached;
extern const guint32  gnet_property_variable_hosts_in_guess_intro_catcher;
extern const guint32  gnet_property_variable_dbstore_debug;
extern const char    gnet_property_variable_session_id[GUID_RAW_SIZE];
extern const gboolean gnet_property_variable_pfsp_rare_server;
extern const gboolean gnet_property_variable_enable_guess_client;
extern const guint64  gnet_property_variable_bw_guess_out;
extern const guint32  gnet_property_variable_matching_debug;
extern const guint32  gnet_property_variable_tsync_debug;
extern const gboolean gnet_property_variable_query_request_partials;
extern const gboolean gnet_property_variable_query_answer_partials;
extern const gboolean gnet_property_variable_query_answer_whats_new;
extern const gboolean gnet_property_variable_search_smart_stop;
extern const guint32  gnet_property_variable_whats_new_search_max_results;
extern const guint32  gnet_property_variable_passive_search_max_results;
extern const gboolean gnet_property_variable_log_dup_gnutella_same_node;
extern const gboolean gnet_property_variable_log_dup_gnutella_higher_ttl;
extern const gboolean gnet_property_variable_log_dup_gnutella_other_node;
extern const gboolean gnet_property_variable_log_new_gnutella;
extern const gboolean gnet_property_variable_log_gnutella_routing;
extern const gboolean gnet_property_variable_log_bad_gnutella;
extern const gboolean gnet_property_variable_log_spam_query_hit;
extern const guint32  gnet_property_variable_max_ultra6_hosts_cached;
extern const guint32  gnet_property_variable_hosts_in_ultra6_catcher;
extern const guint32  gnet_property_variable_hosts_in_guess6_catcher;
extern const guint32  gnet_property_variable_hosts_in_guess6_intro_catcher;
extern const guint32  gnet_property_variable_max_guess6_hosts_cached;
extern const guint32  gnet_property_variable_max_guess6_intro_hosts_cached;
extern const guint32  gnet_property_variable_xmalloc_debug;
extern const guint32  gnet_property_variable_qhit_bad_debug;
extern const guint32  gnet_property_variable_guid_debug;
extern const guint32  gnet_property_variable_tx_deflate_debug;
extern const char   *gnet_property_variable_tx_debug_addrs;
extern const char   *gnet_property_variable_dump_rx_addrs;
extern const char   *gnet_property_variable_dump_tx_from_addrs;
extern const char   *gnet_property_variable_dump_tx_to_addrs;
extern const gboolean gnet_property_variable_guess_maximize_bw;
extern const guint32  gnet_property_variable_udp_sched_debug;
extern const guint32  gnet_property_variable_tx_ut_debug_flags;
extern const char   *gnet_property_variable_rx_debug_addrs;
extern const guint32  gnet_property_variable_rx_ut_debug_flags;
extern const gboolean gnet_property_variable_log_sr_udp_tx;
extern const gboolean gnet_property_variable_log_sr_udp_rx;
extern const guint32  gnet_property_variable_secure_oob_debug;
extern const gboolean gnet_property_variable_log_vmsg_tx;
extern const gboolean gnet_property_variable_log_vmsg_rx;
extern const guint32  gnet_property_variable_dht_tcache_debug_flags;
extern const gboolean gnet_property_variable_log_weird_dht_headers;
extern const guint32  gnet_property_variable_dht_rpc_debug;
extern const gboolean gnet_property_variable_log_uhc_pings_rx;
extern const gboolean gnet_property_variable_log_uhc_pings_tx;
extern const gboolean gnet_property_variable_clean_shutdown;
extern const gboolean gnet_property_variable_clean_restart;
extern const guint32  gnet_property_variable_dht_keys_debug;
extern const guint32  gnet_property_variable_dht_values_debug;
extern const guint32  gnet_property_variable_pid;
extern const guint32  gnet_property_variable_http_range_debug;
extern const guint32  gnet_property_variable_upnp_mapping_lease_time;
extern const gboolean gnet_property_variable_user_auto_restart;
extern const guint32  gnet_property_variable_tm_debug;
extern const guint32  gnet_property_variable_tmalloc_debug;
extern const guint32  gnet_property_variable_evq_debug;
extern const guint32  gnet_property_variable_max_g2hub_hosts_cached;
extern const guint32  gnet_property_variable_hosts_in_g2hub_catcher;
extern const gboolean gnet_property_variable_enable_g2;
extern const guint32  gnet_property_variable_node_g2_count;
extern const guint32  gnet_property_variable_max_g2_hubs;
extern const gboolean gnet_property_variable_log_bad_g2;
extern const gboolean gnet_property_variable_log_dropped_g2;
extern const guint32  gnet_property_variable_g2_rpc_debug;
extern const gboolean gnet_property_variable_log_query_hits;
extern const gboolean gnet_property_variable_log_query_hit_records;
extern const guint32  gnet_property_variable_g2_browse_count;
extern const guint32  gnet_property_variable_g2_browse_served;
extern const gboolean gnet_property_variable_log_sending_g2;
extern const time_t  gnet_property_variable_session_start_stamp;
extern const gboolean gnet_property_variable_tcp_no_listening;
extern const gboolean gnet_property_variable_query_trace;
extern const gboolean gnet_property_variable_inputevt_trace;
extern const gboolean gnet_property_variable_lock_contention_trace;
extern const gboolean gnet_property_variable_lock_sleep_trace;
extern const gboolean gnet_property_variable_running_topless;
extern const gboolean gnet_property_variable_send_oob_ind_reliably;
extern const guint32  gnet_property_variable_adns_debug;


prop_set_t *gnet_prop_init(void);
void gnet_prop_shutdown(void);

#endif /* CORE_SOURCES */

#endif /* _gnet_property_priv_h_ */

/* vi: set ts=4 sw=4 cindent: */
