#include "core0.h"


#define TIME_POLL_CAN_SEND 1000
#define TIME_POLL_CAN_RX   10


static void IRAM_ATTR can_greta_rx_callback(CAN_FRAME *frame);
static void IRAM_ATTR cast_payload(uint8_t *casted_payload, BytesUnion pl, uint8_t length);


void function_core_0 (void *parameters) {

#if DEBUG_CORES
    Serial.print("Task 0 initialized running on core: ");
    Serial.println(xPortGetCoreID());
#endif

    delay(1000);


    // button_setup are called here s.t. attachToInterrupt is called in core0 s.t.
    //  the management of interrupts and debouncing will be done by core0
    button_setup(&limit_switch_begin_parameters);
    button_setup(&limit_switch_end_parameters);
    button_setup(&shift_up_button_parameters);
    button_setup(&shift_down_button_parameters);
    button_setup(&calibration_button_parameters);


    CAN0.setCANPins(CAN_RX_PIN, CAN_TX_PIN);
    while (!CAN0.begin(500000)) {
        #if DEBUG_CAN
            Serial.println("CAN error");
        #endif
        delay(100);
    }

    CAN0.watchFor(POLICANBENT_GRETA_DATA_FRAME_ID);         // initialize filters for reading: only greta data frame
    CAN0.setCallback(0, can_greta_rx_callback);

    #if DEBUG_CAN
        Serial.println("CAN initialized");
    #endif


    struct policanbent_gb_data_t data_payload;
    struct policanbent_gb_error_t error_payload;
    uint8_t destination_payload[POLICANBENT_GB_DATA_LENGTH];        // same of POLICANBENT_GB_ERROR_LENGTH
    CAN_FRAME can_tx_msg_data;
    int i;

    can_tx_msg_data.rtr = 0;
    can_tx_msg_data.id = POLICANBENT_GB_DATA_FRAME_ID;
    can_tx_msg_data.extended = POLICANBENT_GB_DATA_IS_EXTENDED;
    can_tx_msg_data.length = POLICANBENT_GB_DATA_LENGTH;

    uint64_t time_can_sending = millis();
    uint64_t time_can_receiving = millis();

    while (1) {
        if (millis() - time_can_sending >= TIME_POLL_CAN_SEND) {
            data_payload.gb_gear = policanbent_gb_data_gb_gear_encode(g_current_gear);

            if (policanbent_gb_data_pack(destination_payload, &data_payload, POLICANBENT_GB_DATA_LENGTH) < 0) {
                #if DEBUG_CAN
                    Serial.println("Error in creating CAN data payload");
                #endif
            }
            else {
                for (i=0; i<POLICANBENT_GB_DATA_LENGTH; i++) {
                    can_tx_msg_data.data.uint8[i] = destination_payload[i];
                }

                CAN0.sendFrame(can_tx_msg_data);


                #if DEBUG_CAN >= 2
                    Serial.print("CAN status:");
                    Serial.print("      Current gear: "); Serial.print(g_current_gear);
                    Serial.print("    Transmitted payload: "); Serial.print(data_payload.gb_gear);
                    Serial.print("    Destination payload[0]: "); Serial.println(destination_payload[0]);
                #endif
            }

            if (g_calibration_flag) {
                webserver_calibration();
                g_calibration_flag = 0;
            }

            time_can_sending = millis();
        }

        //Serial.println("core0");
        delay(10);
    }
}


void IRAM_ATTR cast_payload(uint8_t *casted_payload, BytesUnion pl, uint8_t length) {
    for (int i = 0; i < length; i++) {
        casted_payload[i] = pl.byte[i];
        #if DEBUG_CAN >= 2
            Serial.print(casted_payload[i]);
        #endif /* DEBUG_CAN >= 2 */
    }
    #if DEBUG_CAN >= 2
        Serial.println();
    #endif /* DEBUG_CAN >= 2 */
}


void IRAM_ATTR can_greta_rx_callback(CAN_FRAME *frame) {
    uint8_t pl[8];
    cast_payload(pl, frame->data, frame->length);

    switch (frame->id) {
        case POLICANBENT_GRETA_DATA_FRAME_ID: {
            struct policanbent_greta_data_t d;
            int ret = policanbent_greta_data_unpack(&d, pl, frame->length);

            #if DEBUG_CAN
                if (ret) {
                    Serial.println("CAN frame unpacking failed.");
                }
            #endif

            if (d.rx_shifting == 2) {
                shift_up_pressed = 1;
                shift_down_pressed = 0;
            } else if (d.rx_shifting == 1) {
                shift_up_pressed = 0;
                shift_down_pressed = 1;
            } else {
                shift_up_pressed = shift_down_pressed = 0;
            }

            char buf[30];
            sprintf(buf, "%d", d.rx_shifting);

            #if DEBUG_CAN
                Serial.print("Received payload GRETA DATA: ");
                Serial.println(buf);
            #endif
        }
    }

    return;
}


void IRAM_ATTR limit_switch_begin_isr() {
    switch_begin_pressed = button_interrupt_service_routine(&limit_switch_begin_parameters);
}


void IRAM_ATTR limit_switch_end_isr() {
    switch_end_pressed = button_interrupt_service_routine(&limit_switch_end_parameters);
}


void IRAM_ATTR shift_up_button_isr() {
    shift_up_pressed = button_interrupt_service_routine(&shift_up_button_parameters);
}


void IRAM_ATTR shift_down_button_isr() {
    shift_down_pressed = button_interrupt_service_routine(&shift_down_button_parameters);
}


void IRAM_ATTR calibration_button_isr() {
    calibration_button_pressed = button_interrupt_service_routine(&calibration_button_parameters);
}