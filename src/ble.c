#include "ble.h"

/* Bluetooth (BLE) */
static int incoming_message(uint16_t conn_handle,
                            uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, 
                            void *arg)
{
    int data = *(ctxt->om->om_data);
    if(data == 49)
    {
        ESP_LOGI(“BLE”, “Valid data!”);
    }
    return 0;
}

/* Define the UUIDs */
static const struct ble_gatt_svc_def gat_svcs[] =
{
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID128_DECLARE(0xff, 0xee, 0xdd, 0xcc, 
                                    0xbb, 0xaa, 0x99, 0x88, 
                                    0x77, 0x66, 0x55, 0x44,
                                    0x33, 0x22, 0x11, 0x00),
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID128_DECLARE(0xfb, 0x34, 0x9b, 0x5f, 
                                            0x80, 0x00, 0x00, 0x80, 
                                            0x00, 0x10, 0x00, 0x00, 
                                            0xe1, 0xff, 0x00, 0x00),
                .flags = BLE_GATT_CHR_F_WRITE,
                .access_cb = incoming_message
            },
            {
                0
            }
        }
    },
    {
        0
    }
};

void host_task(void *param){
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/* Settings / callbacks for BLE GAP and advertisement */
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch(event->type)
    {
        case BLE_GAP_EVENT_CONNECT:
            if(event->connect.status != 0)
            {
                ble_app_advertise();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            ble_app_advertise();
            break;
 
        case BLE_GAP_EVENT_ADV_COMPLETE:
            ble_app_advertise();
            break;
 
        case BLE_GAP_EVENT_SUBSCRIBE:
            break;

        default:
            break;
    }
    return 0;
}


void ble_app_advertise(void)
{
    // Set advertising parameters and execute

    //Set up struct for setting advertising fields
    struct ble_hs_adv_fields* fields;
    memset(&fields, 0, sizeof(fields));

    /* Set fields you want */
    // Power levels
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_DISC_LTD;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    // Name
    fields.name = (uint8_t *)ble_svc_gap_device_name();
    fields.name_len = strlen(ble_svc_gap_device_name());
    fields.name_is_complete = 1;

    // finalize setting of fields
    ble_gap_adv_set_fields(&fields);

    // Setup struct for setting advertise parameters
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));

    // connection type
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;

    // discovery mode
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    // start advertising
    ble_gap_adv_start(ble_address_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

void ble_app_on_sync(void){
    ble_hs_id_infer_auto(0, &ble_address_type);
    ble_app_advertise();
}

void init_ble(){
    // Init nimBLE internals
    esp_nimble_hci_and_controller_init();
    nimble_port_init();

    // Set up name
    ble_svc_gap_device_name_set("BOARDLOCK");

    // Init svc
    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_gatts_count_cfg(gat_svcs);
    ble_gatts_add_sbcs(gat_svcs);

    // jump to when ble syncs
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);
}*/