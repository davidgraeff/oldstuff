#pragma once

    // Force user to set the button into start position
//     while (!bit_is_set(CURTAIN_BUTTON_READPORT,CURTAIN_BUTTON_FORCEAUTO_PIN) ||
//             !bit_is_set(CURTAIN_BUTTON_READPORT,CURTAIN_BUTTON_DIRECTION_PIN)
//           ) {
//         if (++slowdown_counter == 0) {
//             stella_setValue(STELLA_SET_IMMEDIATELY, 0, ++slowdown_counter2);
//         }
//         stella_process();
//     }


            // Force position if button is set
//             button_state = (CURTAIN_BUTTON_READPORT & _BV(CURTAIN_BUTTON_DIRECTION_PIN)) |
//                            (CURTAIN_BUTTON_READPORT & _BV(CURTAIN_BUTTON_FORCEAUTO_PIN));
//             if (button_state != button_laststate) {
//                 button_laststate = button_state;
//                 if (button_state & _BV(CURTAIN_BUTTON_FORCEAUTO_PIN))
//                 {
//                     curtain_stop();
//                 } else {
//                     if (button_state & _BV(CURTAIN_BUTTON_DIRECTION_PIN)) {
//                         curtain_setPosition(0);
//                     } else {
//                         curtain_setPosition(255);
//                     }
//                 }
//             }
