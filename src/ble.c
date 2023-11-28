// reference https://github.com/Ineltek-IT/nimble-multiple-services/blob/main/main/gatt_svr.c
#include "ble.h"
#include "fsm.h"

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "host/ble_uuid.h"
#include "../../apps/bleprph/src/bleprph.h"
#include "esp_nimble_hci.h" 
#include "host/util/util.h"//
#include "esp_err.h"
#include "gps.h"

#define TAG "BT" // descriptive tag for debugging

#define DEVICE_NAME "BoardLock BLE" // name of the bluetooth device

/*************************
Functions declared ahead of time (comments later)
**************************/
void ble_store_config_init(void);
static int bleprph_gap_event(struct ble_gap_event *event, void*arg);
static int gatt_svr_chr_write(struct os_mbuf* om, uint16_t min_len, uint16_t max_len, void* dst, uint16_t* len);
static void on_reset(int reason);
void bleprph_host_task(void * param);
static void on_sync(void);
static int gatt_svr_value_access (uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void *arg);
// Index for gatt table
enum {
  IDX_SERVICE,
  IDX_LOCK_STATUS,
  IDX_MEAS_LAT,
  IDX_MEAS_LONG,
  NUM_TABLE_ELEM
};


/*************************
 Variables
**************************/
static uint8_t own_addr_type;
uint16_t conn_handle;
uint16_t chr1_handle, chr2_handle, chr3_handle, chr4_handle, chr5_handle, chr6_handle, chr7_handle, chr8_handle;
const char locked [] = "L"; // defined lock and unlock hexadecimal values
const char unlocked [] = "U";
/*Message Exchange Variables*/
char lock[2] = "W"; // client can write to this value that mcu can read
char lat[13] = "-000.0000000";
char lon[13] = "-000.0000000";
char battery[10] = "0.5%";
char err_mess[40] = "";
char req_coord[2] = "N"; // client can write to this value that mcu can read
char threat[2] = "S";
char update[2] = "N";
uint16_t min_length = 1;  // min input length
uint16_t max_length = 50; // max input length

/*************************
 Unique Identifiers
**************************/
// service uiud: 087a37b6-061f-48fc-bc40-a76c455348be
static const ble_uuid128_t svc_uuid =
    BLE_UUID128_INIT(0xbe, 0x48, 0x53, 0x45, 0x6c, 0xa7, 0x40, 0xbc, 0xfc, 0x48, 0x1f, 0x06, 0xb6, 0x37, 0x7a, 0x08);
//characteristic uuid: e6406127-229e-4acb-a6e3-a40962dd4935 -> lock
static const ble_uuid128_t chr1_uuid = 
    BLE_UUID128_INIT(0x35, 0x49, 0xdd, 0x62, 0x09, 0xa4, 0xe3, 0xa6, 0xcb, 0x4a, 0x9e, 0x22, 0x27, 0x061, 0x40, 0xe6);
//characterstic uuid: ba582c34-f9a0-458d-9ad4-7e694d543e00 -> lat
static const ble_uuid128_t chr2_uuid = 
    BLE_UUID128_INIT(0x00, 0x3e, 0x54, 0x4d, 0x69, 0x7e, 0xd4, 0x9a, 0x8d, 0x45, 0xa0, 0xf9, 0x34, 0x2c, 0x58, 0xba);
//characteristic uuid: affafe18-63f3-40d6-9131-2e1dde1fa594 -> long
static const ble_uuid128_t chr3_uuid = 
    BLE_UUID128_INIT(0x94, 0xa5, 0x1f, 0xde, 0x1d, 0x2e, 0x31, 0x91, 0xd6, 0x40, 0xf3, 0x63, 0x18, 0xfe, 0xfa, 0xaf);
//characteristic uuid: 67725a37-1fb1-4234-85ef-9c8d62588885 -> battery
static const ble_uuid128_t chr4_uuid = 
    BLE_UUID128_INIT(0x85, 0x88, 0x58, 0x62, 0x8d, 0x9c, 0xef, 0x85, 0x34,  0x42, 0xb1, 0x1f, 0x37, 0x5a, 0x72, 0x67);
//characterstic uuid: 449e0a68-18cb-423f-8645-f17d56aa2516 -> messages
static const ble_uuid128_t chr5_uuid = 
    BLE_UUID128_INIT(0x16, 0x25, 0xaa, 0x56, 0x7d, 0xf1, 0x45, 0x86, 0x3f, 0x42, 0xcb, 0x18, 0x68, 0x0a,  0x9e, 0x44);
//characterstic uuid: 459e0a68-18cb-423f-8645-f17d56aa2516 -> req_coord
static const ble_uuid128_t chr6_uuid = 
    BLE_UUID128_INIT(0x16, 0x25, 0xaa, 0x56, 0x7d, 0xf1, 0x45, 0x86, 0x3f, 0x42, 0xcb, 0x18, 0x68, 0x0a,  0x9e, 0x45);
//characterstic uuid: 469e0a68-18cb-423f-8645-f17d56aa2516 -> req_coord
static const ble_uuid128_t chr7_uuid = 
    BLE_UUID128_INIT(0x16, 0x25, 0xaa, 0x56, 0x7d, 0xf1, 0x45, 0x86, 0x3f, 0x42, 0xcb, 0x18, 0x68, 0x0a,  0x9e, 0x46);
//characterstic uuid: 479e0a68-18cb-423f-8645-f17d56aa2516 -> req_coord
static const ble_uuid128_t chr8_uuid = 
    BLE_UUID128_INIT(0x16, 0x25, 0xaa, 0x56, 0x7d, 0xf1, 0x45, 0x86, 0x3f, 0x42, 0xcb, 0x18, 0x68, 0x0a,  0x9e, 0x47);

/*************************
 GATT server configurations
**************************/
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){{
            .uuid = &chr1_uuid.u,     //UUID as given above
            .access_cb = gatt_svr_value_access, //Callback function. When ever 'value' will be accessed by user, this function will execute
            .val_handle = &chr1_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE, //user can read this 'value', can write to it. 
          }, {
            .uuid = &chr2_uuid.u, 
            .access_cb = gatt_svr_value_access,
            .val_handle = &chr2_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
          }, {
            .uuid = &chr3_uuid.u,
            .access_cb = gatt_svr_value_access,
            .val_handle = &chr3_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
          }, {
            .uuid = &chr4_uuid.u,
            .access_cb = gatt_svr_value_access,
            .val_handle = &chr4_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
          }, {
            .uuid = &chr5_uuid.u,
            .access_cb = gatt_svr_value_access,
            .val_handle = &chr5_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
          }, {
            .uuid = &chr6_uuid.u,
            .access_cb = gatt_svr_value_access,
            .val_handle = &chr6_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
          }, {
            .uuid = &chr7_uuid.u,
            .access_cb = gatt_svr_value_access,
            .val_handle = &chr7_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
          }, {
            .uuid = &chr8_uuid.u,
            .access_cb = gatt_svr_value_access,
            .val_handle = &chr8_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
          },
          {
            0, /* No more characteristics in this service.  */
        }},
    },
    {
        0, /* No more services */
    },
};


static int gatt_svr_value_access (uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void *arg) {
/*************************
 description: callback function to access the value within 'value' variable, do not change the arguments since the structure is needed for it to be a callback
 arguments:
  - conn_handle: used by the mcu to define which peripheral accessed it (always 0x0 if only 1 peripheral connected)
  - attr_handle: attribute handle used by mcu to differentiate between different services
  - ctxt: GATT operation being performed
  - arg: not needed in our simple ble case but necessary if more ble functions (ex: notifications)
 return: either 0 or BLE_ATT_ERR_UNLIKELY (0x0E) or some other error code
**************************/
    int val = 0; // placeholder
    //int status; 
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR: 
            ESP_LOGI(TAG, "Reading value stored in MCU\n");
            val = gatt_svr_chr_write(ctxt->om, min_length, max_length, &lock, NULL);
            if (ble_uuid_cmp(ctxt->chr->uuid, &chr1_uuid.u) == 0) {
              ESP_LOGI(TAG, "reading %s", lock);
              val = os_mbuf_append(ctxt->om, &lock, sizeof(lock));
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr2_uuid.u) == 0) {
              ESP_LOGI(TAG, "reading %s", lat);
              val = os_mbuf_append(ctxt->om, &lat, sizeof(lat));
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr3_uuid.u) == 0) {
              ESP_LOGI(TAG, "reading %s", lon);
              val = os_mbuf_append(ctxt->om, &lon, sizeof(lon));
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr4_uuid.u) == 0) {
              ESP_LOGI(TAG, "reading %s", battery);
              val = os_mbuf_append(ctxt->om, &battery, sizeof(battery));
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr5_uuid.u) == 0) {
              ESP_LOGI(TAG, "reading %s", err_mess);
              val = os_mbuf_append(ctxt->om, &err_mess, sizeof(err_mess));
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr6_uuid.u) == 0) {
              ESP_LOGI(TAG, "reading %s", req_coord);
              val = os_mbuf_append(ctxt->om, &req_coord, sizeof(req_coord));
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr7_uuid.u) == 0) {
              ESP_LOGI(TAG, "reading %s", threat);
              val = os_mbuf_append(ctxt->om, &threat, sizeof(threat));
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr8_uuid.u) == 0) {
              ESP_LOGI(TAG, "reading %s", update);
              val = os_mbuf_append(ctxt->om, &update, sizeof(update));
            }

            return val;
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            ESP_LOGI(TAG, "Writing to value stored in MCU\n");
            if (ble_uuid_cmp(ctxt->chr->uuid, &chr1_uuid.u) == 0) {
              val = gatt_svr_chr_write(ctxt->om, min_length, max_length, &lock, NULL);
              ESP_LOGI(TAG, "Wrote %s", lock);
              if (strcmp(lock, locked) == 0) {
                  // TODO: set flags
                  ESP_LOGI(TAG, "Received lock signal");
                  // Set the flags
                  fsm_set_flag(FLAG_IND_LOCKED, 1);
                  fsm();
              }  
              else if (strcmp(lock, unlocked) == 0) {
                  ESP_LOGI(TAG, "Received unlock signal");
                  // Clear the flags
                  fsm_set_flag(FLAG_IND_LOCKED, 0);
                  fsm();
              }
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr5_uuid.u) == 0){
              val = gatt_svr_chr_write(ctxt->om, min_length, max_length, &err_mess, NULL);
              ESP_LOGI(TAG, "Wrote %s", err_mess);
              // TODO: handle error
            }
            else if (ble_uuid_cmp(ctxt->chr->uuid, &chr6_uuid.u) == 0) {
              val = gatt_svr_chr_write(ctxt->om, min_length, max_length, &req_coord, NULL);
              ESP_LOGI(TAG, "Wrote %s", req_coord);
              if (req_coord[0] == 'Y') {
                  ESP_LOGI(TAG, "Requested coordinates");
                  gps_start_polling();
                  req_coord[0] = 'N';
              }  
            }
          return val;
        default:
          return BLE_ATT_ERR_UNLIKELY;
    }
}

/*************************
 Init & DeInit Functions
**************************/
void init_ble() {
/*************************
 description: function to initialize the NimBLE
 arguments: N/A
 return: none
**************************/
    // initialize the ble
    ESP_LOGI(TAG, "Initializing Bluetooth...");

    // programming sequence from here: https://docs.espressif.com/projects/esp-idf/en/v5.1.1/esp32/api-reference/bluetooth/nimble/index.html

    // init host stack and port
    nimble_port_init();

    // init host config params and callbacks (cb)
    ble_hs_cfg.reset_cb = on_reset;
    ble_hs_cfg.sync_cb = on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr; 
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_KEYBOARD_DISP; // bluetooth io

    // init gatt server
    if (gatt_svr_init() != 0) {
        ESP_LOGE(TAG, "ERROR initializing gatt server.");
    }

    // Set device name
    if (ble_svc_gap_device_name_set(DEVICE_NAME) != 0) {
        ESP_LOGE(TAG, "ERROR setting device name.");
    }

    ESP_LOGI(TAG, "Finish setting up BLE. Now running thread...");
    
    // run thread for host stack
    nimble_port_freertos_init(bleprph_host_task);
}

void deinit_ble() {
/*************************
 description: function to deinitialize the NimBLE
 arguments: N/A
 return: none
**************************/
    ESP_LOGI(TAG, "Stopping BLE");

    if (!nimble_port_stop()){ // if stop port successful
        nimble_port_deinit();
        //ESP_ERROR_CHECK(esp_nimble_hci_deinit()); no longer supported
        //ESP_ERROR_CHECK(esp_nimble_hci_and_controller_deinit());
    }
    
    ESP_LOGI(TAG, "Sucessfully stopped BLE.");
}


/*************************
 The following code is taken from the ESP32 example library
 https://github.com/espressif/esp-nimble
**************************/
static int gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len, void *dst, uint16_t *len)
{
  uint16_t om_len;
  int rc;

  om_len = OS_MBUF_PKTLEN(om);
  if (om_len < min_len || om_len > max_len)
  {
    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
  }

  rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
  if (rc != 0)
  {
    return BLE_ATT_ERR_UNLIKELY;
  }

  return 0;
}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
  char buf[BLE_UUID_STR_LEN];

  switch (ctxt->op)
  {
  case BLE_GATT_REGISTER_OP_SVC:
    MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                ctxt->svc.handle);
    break;

  case BLE_GATT_REGISTER_OP_CHR:
    MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                       "def_handle=%d val_handle=%d\n",
                ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                ctxt->chr.def_handle,
                ctxt->chr.val_handle);
    break;

  case BLE_GATT_REGISTER_OP_DSC:
    MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                ctxt->dsc.handle);
    break;

  default:
    assert(0);
    break;
  }
}

int gatt_svr_init(void)
{
  int rc;

  ble_svc_gap_init();
  ble_svc_gatt_init();

  rc = ble_gatts_count_cfg(gatt_svr_svcs);
  if (rc != 0)
  {
    return rc;
  }

  rc = ble_gatts_add_svcs(gatt_svr_svcs);
  if (rc != 0)
  {
    return rc;
  }

  return 0;
}

void
print_addr(const void *addr)
{
    const uint8_t *u8p;

    u8p = addr;
    MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x",
                u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}

static void
bleprph_print_conn_desc(struct ble_gap_conn_desc *desc)
{
  MODLOG_DFLT(INFO, "handle=%d our_ota_addr_type=%d our_ota_addr=",
              desc->conn_handle, desc->our_ota_addr.type);
  print_addr(desc->our_ota_addr.val);
  MODLOG_DFLT(INFO, " our_id_addr_type=%d our_id_addr=",
              desc->our_id_addr.type);
  print_addr(desc->our_id_addr.val);
  MODLOG_DFLT(INFO, " peer_ota_addr_type=%d peer_ota_addr=",
              desc->peer_ota_addr.type);
  print_addr(desc->peer_ota_addr.val);
  MODLOG_DFLT(INFO, " peer_id_addr_type=%d peer_id_addr=",
              desc->peer_id_addr.type);
  print_addr(desc->peer_id_addr.val);
  MODLOG_DFLT(INFO, " conn_itvl=%d conn_latency=%d supervision_timeout=%d "
                    "encrypted=%d authenticated=%d bonded=%d\n",
              desc->conn_itvl, desc->conn_latency,
              desc->supervision_timeout,
              desc->sec_state.encrypted,
              desc->sec_state.authenticated,
              desc->sec_state.bonded);
}

static void
bleprph_advertise(void)
{
  struct ble_gap_adv_params adv_params;
  struct ble_hs_adv_fields fields;
  const char *name;
  int rc;

  /**
   *  Set the advertisement data included in our advertisements:
   *     o Flags (indicates advertisement type and other general info).
   *     o Advertising tx power.
   *     o Device name.
   *     o 16-bit service UUIDs (alert notifications).
   */

  memset(&fields, 0, sizeof fields);

  /* Advertise two flags:
   *     o Discoverability in forthcoming advertisement (general)
   *     o BLE-only (BR/EDR unsupported).
   */
  fields.flags = BLE_HS_ADV_F_DISC_GEN |
                 BLE_HS_ADV_F_BREDR_UNSUP;

  /* Indicate that the TX power level field should be included; have the
   * stack fill this value automatically.  This is done by assigning the
   * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
   */
  fields.tx_pwr_lvl_is_present = 1;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

  name = ble_svc_gap_device_name();
  fields.name = (uint8_t *)name;
  fields.name_len = strlen(name);
  fields.name_is_complete = 1;

  fields.uuids16 = (ble_uuid16_t[]){
      BLE_UUID16_INIT(GATT_SVR_SVC_ALERT_UUID)};
  fields.num_uuids16 = 1;
  fields.uuids16_is_complete = 1;

  rc = ble_gap_adv_set_fields(&fields);
  if (rc != 0)
  {
    MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
    return;
  }

  /* Begin advertising. */
  memset(&adv_params, 0, sizeof adv_params);
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
  rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                         &adv_params, bleprph_gap_event, NULL);
  if (rc != 0)
  {
    MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
    return;
  }
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that forms.
 * bleprph uses the same callback for all connections.
 *
 * @param event                 The type of event being signalled.
 * @param ctxt                  Various information pertaining to the event.
 * @param arg                   Application-specified argument; unused by
 *                                  bleprph.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int
bleprph_gap_event(struct ble_gap_event *event, void *arg)
{
  struct ble_gap_conn_desc desc;
  int rc;

  switch (event->type)
  {
  case BLE_GAP_EVENT_CONNECT:
    /* A new connection was established or a connection attempt failed. */
    MODLOG_DFLT(INFO, "connection %s; status=%d ",
                event->connect.status == 0 ? "established" : "failed",
                event->connect.status);
    if (event->connect.status == 0)
    {
      rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
      assert(rc == 0);
      bleprph_print_conn_desc(&desc);
      fsm_set_flag(FLAG_IND_BLE_PAIRED, 1);
      fsm();
    }
    MODLOG_DFLT(INFO, "\n");

    if (event->connect.status != 0)
    {
      /* Connection failed; resume advertising. */
      bleprph_advertise();
    }
    conn_handle = event->connect.conn_handle;
    return 0;

  case BLE_GAP_EVENT_DISCONNECT:
    MODLOG_DFLT(INFO, "disconnect; reason=%d ", event->disconnect.reason);
    bleprph_print_conn_desc(&event->disconnect.conn);
    MODLOG_DFLT(INFO, "\n");

    /* Connection terminated; resume advertising. */
    ESP_LOGI(TAG, "HERE");
    fsm_set_flag(FLAG_IND_BLE_PAIRED, 0);
    fsm();
    bleprph_advertise();
    return 0;

  case BLE_GAP_EVENT_CONN_UPDATE:
    /* The central has updated the connection parameters. */
    MODLOG_DFLT(INFO, "connection updated; status=%d ",
                event->conn_update.status);
    rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
    assert(rc == 0);
    bleprph_print_conn_desc(&desc);
    MODLOG_DFLT(INFO, "\n");
    rc = ble_gatts_notify(conn_handle, chr1_handle);
    // assert(rc==0);
    rc = ble_gatts_indicate(conn_handle, chr1_handle);
    // assert(rc == 0);
    return 0;

  case BLE_GAP_EVENT_ADV_COMPLETE:
    MODLOG_DFLT(INFO, "advertise complete; reason=%d",
                event->adv_complete.reason);
    bleprph_advertise();
    return 0;
  case BLE_GAP_EVENT_MTU:
    MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                event->mtu.conn_handle,
                event->mtu.channel_id,
                event->mtu.value);
    return 0;
  }

  return 0;
}

static void on_reset(int reason)
{
  MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void on_sync(void)
{
  int rc;

  rc = ble_hs_util_ensure_addr(0);
  assert(rc == 0);

  /* Figure out address to use while advertising (no privacy for now) */
  rc = ble_hs_id_infer_auto(0, &own_addr_type);
  if (rc != 0)
  {
    MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
    return;
  }

  /* Printing ADDR */
  uint8_t addr_val[6] = {0};
  rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);

  MODLOG_DFLT(INFO, "Device Address: ");
  print_addr(addr_val);
  MODLOG_DFLT(INFO, "\n");
  /* Begin advertising. */
  bleprph_advertise();
}

void bleprph_host_task(void *param)
{
  ESP_LOGI(TAG, "BLE Host Task Started");
  /* This function will return only when nimble_port_stop() is executed */
  nimble_port_run();

  nimble_port_freertos_deinit();
}


/*************************
 Message Exchange Variable Functions
**************************/

char* ble_get_lock() {
/*************************
 description: returns lock
 arguments: N/A
 return: none
**************************/
  return lock;
}

char* ble_get_lat() {
/*************************
 description: returns lat
 arguments: N/A
 return: none
**************************/
  return lat;
}

char* ble_get_lon() {
/*************************
 description: returns lon
 arguments: N/A
 return: none
**************************/
  return lon;
}

char* ble_get_err_mess() {
/*************************
 description: returns err_mess
 arguments: N/A
 return: none
**************************/
  return err_mess;
}

void ble_set_err_mess(char str []) {
/*************************
 description: returns err_mess
 arguments: N/A
 return: none
**************************/
  int i = 0;
  while(str[i] != '\0') {
    err_mess[i] = str[i];
    i++;
  }
  err_mess[i] = str[i];

  if (fsm_get_flag(FLAG_IND_BLE_PAIRED)) {
    int rc = ble_gatts_notify(conn_handle, chr5_handle);
    // assert(rc==0);
    rc = ble_gatts_indicate(conn_handle, chr5_handle);
    // assert(rc == 0);
  }
}

char* ble_get_threat() {
/*************************
 description: returns threat
 arguments: N/A
 return: none
**************************/
  return threat;
}

char* ble_get_update() {
/*************************
 description: returns threat
 arguments: N/A
 return: none
**************************/
  return update;
}

char* ble_get_battery() {
/*************************
 description: returns threat
 arguments: N/A
 return: none
**************************/
  return battery;
}

void disconnect_ble() {
    ble_gap_terminate(conn_handle, BLE_HCI_OCF_DISCONNECT_CMD);
    fsm_set_flag(FLAG_IND_BLE_PAIRED, 0);
    fsm();
}