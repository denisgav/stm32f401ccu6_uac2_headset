#ifndef __USB_HID_STATUS__H
#define __USB_HID_STATUS__H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct _usb_hid_status_t {
	uint8_t cur_custom_ctrl_scan_code;
	int16_t volume_rotary_encoder_cntr_prev;

	GPIO_PinState btn_scan_prev_status;
	GPIO_PinState btn_scan_next_status;

	bool custom_ctrl_scan_code_updated;
} usb_hid_status_t;

#ifdef __cplusplus
}
#endif

#endif //__USB_HID_STATUS__H
