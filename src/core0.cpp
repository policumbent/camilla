#include "core0.h"


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

    // CAN0.watchFor();         // initialize filters for reading (for now disabled, since reading disabled)

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

    while (1) {

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
    
        delay(1000);
    }
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