/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jerzy Kasenbreg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_

// #include "tusb.h"

#if USB_HID_AUDIO_CTRL_ACTIVE == 1
enum {
	REPORT_ID_CONSUMER_CONTROL = 1,
	REPORT_ID_COUNT
};

#define MY_TUD_HID_CONSUMER_SCAN_NEXT_CODE            0x1
#define MY_TUD_HID_CONSUMER_SCAN_PREVIOUS_CODE        0x2
#define MY_TUD_HID_CONSUMER_STOP_CODE                 0x4
#define MY_TUD_HID_CONSUMER_PLAY_PAUSE_CODE           0x8
#define MY_TUD_HID_CONSUMER_VOLUME_CODE               0x10
#define MY_TUD_HID_CONSUMER_MUTE_CODE                 0x20
#define MY_TUD_HID_CONSUMER_VOLUME_INCREMENT_CODE     0x40
#define MY_TUD_HID_CONSUMER_VOLUME_DECREMENT_CODE     0x80

// Consumer Control Report Descriptor Template
#define MY_TUD_HID_REPORT_DESC_CONSUMER(...) \
		 HID_USAGE_PAGE ( HID_USAGE_PAGE_CONSUMER ),        /* Usage Page (Consumer)                                                          */\
		 HID_USAGE      ( HID_USAGE_CONSUMER_CONTROL ),     /* Usage (Consumer Control)                                                       */\
		 HID_COLLECTION ( HID_COLLECTION_APPLICATION ),     /* Collection (Application)                                                       */\
         /* Report ID if any */                                                                                \
         __VA_ARGS__                                                                                           \
		 HID_USAGE_PAGE ( HID_USAGE_PAGE_CONSUMER ),        /*   Usage Page (Consumer)                                                        */\
		 HID_LOGICAL_MIN(0x00),                             /*   Logical Minimum (0)                                                          */\
		 HID_LOGICAL_MAX(0x01),                             /*   Logical Maximum (1)                                                          */\
		 HID_REPORT_SIZE(0x01),                             /*   Report Size (1)                                                              */\
		 HID_REPORT_COUNT(0x08),                            /*   Report Count (8)                                                             */\
		 HID_USAGE(HID_USAGE_CONSUMER_SCAN_NEXT            ),/*   Usage (Scan Next Track)                                                     */\
		 HID_USAGE(HID_USAGE_CONSUMER_SCAN_PREVIOUS        ),/*   Usage (Scan Previous Track)                                                 */\
		 HID_USAGE(HID_USAGE_CONSUMER_STOP                 ),/*   Usage (Stop)                                                                */\
		 HID_USAGE(HID_USAGE_CONSUMER_PLAY_PAUSE           ),/*   Usage (Play/Pause)                                                          */\
		 HID_USAGE(HID_USAGE_CONSUMER_VOLUME               ),/*   Usage (Eject)                                                               */\
		 HID_USAGE(HID_USAGE_CONSUMER_MUTE                 ),/*   Usage (Mute)                                                                */\
		 HID_USAGE(HID_USAGE_CONSUMER_VOLUME_INCREMENT     ),/*   Usage (Volume Increment)                                                    */\
		 HID_USAGE(HID_USAGE_CONSUMER_VOLUME_DECREMENT     ),/*   Usage (Volume Decrement)                                                    */\
         /*   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)        */                                                \
		 HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),                  \
		 HID_COLLECTION_END,              /* End Collection                                                                                   */\


#endif //USB_HID_AUDIO_CTRL_ACTIVE

// Unit numbers are arbitrary selected
#define UAC2_ENTITY_CLOCK               0x04
// Speaker path
#define UAC2_ENTITY_SPK_INPUT_TERMINAL  0x01
#define UAC2_ENTITY_SPK_FEATURE_UNIT    0x02
#define UAC2_ENTITY_SPK_OUTPUT_TERMINAL 0x03
// Microphone path
#define UAC2_ENTITY_MIC_INPUT_TERMINAL  0x11
#define UAC2_ENTITY_MIC_OUTPUT_TERMINAL 0x13

enum {
	ITF_NUM_AUDIO_CONTROL = 0,
	ITF_NUM_AUDIO_STREAMING_SPK,
	ITF_NUM_AUDIO_STREAMING_MIC,
#if USB_HID_AUDIO_CTRL_ACTIVE == 1
	ITF_NUM_HID,
#endif //USB_HID_AUDIO_CTRL_ACTIVE
	ITF_NUM_TOTAL
};

#define TUD_AUDIO_HEADSET_STEREO_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN\
    + TUD_AUDIO_DESC_STD_AC_LEN\
    + TUD_AUDIO_DESC_CS_AC_LEN\
    + TUD_AUDIO_DESC_CLK_SRC_LEN\
    + TUD_AUDIO_DESC_INPUT_TERM_LEN\
    + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN\
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN\
    + TUD_AUDIO_DESC_INPUT_TERM_LEN\
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN\
    + TUD_AUDIO_DESC_STD_AC_INT_EP_LEN\
    /* Interface 1, Alternate 0 */\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    /* Interface 1, Alternate 1 */\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    + TUD_AUDIO_DESC_CS_AS_INT_LEN\
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN\
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN\
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN\
    /* Interface 1, Alternate 2 */\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    + TUD_AUDIO_DESC_CS_AS_INT_LEN\
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN\
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN\
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN\
    /* Interface 2, Alternate 0 */\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    /* Interface 2, Alternate 1 */\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    + TUD_AUDIO_DESC_CS_AS_INT_LEN\
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN\
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN\
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN\
    /* Interface 2, Alternate 2 */\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    + TUD_AUDIO_DESC_CS_AS_INT_LEN\
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN\
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN\
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN)

#define TUD_AUDIO_HEADSET_STEREO_DESCRIPTOR(_stridx, _epout, _epin, _epint) \
    /* Standard Interface Association Descriptor (IAD) */\
    TUD_AUDIO_DESC_IAD(/*_firstitf*/ ITF_NUM_AUDIO_CONTROL, /*_nitfs*/ 3/*ITF_NUM_TOTAL*/, /*_stridx*/ 0x00),\
    /* Standard AC Interface Descriptor(4.7.1) */\
    TUD_AUDIO_DESC_STD_AC(/*_itfnum*/ ITF_NUM_AUDIO_CONTROL, /*_nEPs*/ 0x01, /*_stridx*/ _stridx),\
    /* Class-Specific AC Interface Header Descriptor(4.7.2) */\
    TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200, /*_category*/ AUDIO_FUNC_HEADSET, /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN+TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN+TUD_AUDIO_DESC_INPUT_TERM_LEN+TUD_AUDIO_DESC_OUTPUT_TERM_LEN+TUD_AUDIO_DESC_INPUT_TERM_LEN+TUD_AUDIO_DESC_OUTPUT_TERM_LEN, /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS),\
    /* Clock Source Descriptor(4.7.2.1) */\
    TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ UAC2_ENTITY_CLOCK, /*_attr*/ 3, /*_ctrl*/ 7, /*_assocTerm*/ 0x00,  /*_stridx*/ 0x00),    \
    /* Input Terminal Descriptor(4.7.2.4) */\
    TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ UAC2_ENTITY_SPK_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x00, /*_clkid*/ UAC2_ENTITY_CLOCK, /*_nchannelslogical*/ 0x02, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ 0 * (AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS), /*_stridx*/ 0x00),\
    /* Feature Unit Descriptor(4.7.2.8) */\
    TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL(/*_unitid*/ UAC2_ENTITY_SPK_FEATURE_UNIT, /*_srcid*/ UAC2_ENTITY_SPK_INPUT_TERMINAL, /*_ctrlch0master*/ (AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS), /*_ctrlch1*/ (AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS), /*_ctrlch2*/ (AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS), /*_stridx*/ 0x00),\
    /* Output Terminal Descriptor(4.7.2.5) */\
    TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ UAC2_ENTITY_SPK_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_OUT_HEADPHONES, /*_assocTerm*/ 0x00, /*_srcid*/ UAC2_ENTITY_SPK_FEATURE_UNIT, /*_clkid*/ UAC2_ENTITY_CLOCK, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00),\
    /* Input Terminal Descriptor(4.7.2.4) */\
    TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ UAC2_ENTITY_MIC_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_IN_GENERIC_MIC, /*_assocTerm*/ 0x00, /*_clkid*/ UAC2_ENTITY_CLOCK, /*_nchannelslogical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ 0 * (AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS), /*_stridx*/ 0x00),\
    /* Output Terminal Descriptor(4.7.2.5) */\
    TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ UAC2_ENTITY_MIC_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x00, /*_srcid*/ UAC2_ENTITY_MIC_INPUT_TERMINAL, /*_clkid*/ UAC2_ENTITY_CLOCK, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00),\
    /* Standard AC Interrupt Endpoint Descriptor(4.8.2.1) */\
    TUD_AUDIO_DESC_STD_AC_INT_EP(/*_ep*/ _epint, /*_interval*/ 0x01), \
    /* Standard AS Interface Descriptor(4.9.1) */\
    /* Interface 1, Alternate 0 - default alternate setting with 0 bandwidth */\
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)(ITF_NUM_AUDIO_STREAMING_SPK), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x05),\
    /* Standard AS Interface Descriptor(4.9.1) */\
    /* Interface 1, Alternate 1 - alternate interface for data streaming */\
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)(ITF_NUM_AUDIO_STREAMING_SPK), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x05),\
    /* Class-Specific AS Interface Descriptor(4.9.2) */\
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ UAC2_ENTITY_SPK_INPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00),\
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */\
    TUD_AUDIO_DESC_TYPE_I_FORMAT(CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX),\
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */\
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epout, /*_attr*/ (uint8_t) ((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ADAPTIVE | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX), /*_interval*/ 0x01),\
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */\
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_MILLISEC, /*_lockdelay*/ 0x0001),\
    /* Interface 1, Alternate 2 - alternate interface for data streaming */\
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)(ITF_NUM_AUDIO_STREAMING_SPK), /*_altset*/ 0x02, /*_nEPs*/ 0x01, /*_stridx*/ 0x05),\
    /* Class-Specific AS Interface Descriptor(4.9.2) */\
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ UAC2_ENTITY_SPK_INPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00),\
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */\
    TUD_AUDIO_DESC_TYPE_I_FORMAT(CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX),\
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */\
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epout, /*_attr*/ (uint8_t) ((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ADAPTIVE | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX), /*_interval*/ 0x01),\
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */\
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_MILLISEC, /*_lockdelay*/ 0x0001),\
    /* Standard AS Interface Descriptor(4.9.1) */\
    /* Interface 2, Alternate 0 - default alternate setting with 0 bandwidth */\
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)(ITF_NUM_AUDIO_STREAMING_MIC), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x04),\
    /* Standard AS Interface Descriptor(4.9.1) */\
    /* Interface 2, Alternate 1 - alternate interface for data streaming */\
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)(ITF_NUM_AUDIO_STREAMING_MIC), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x04),\
    /* Class-Specific AS Interface Descriptor(4.9.2) */\
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ UAC2_ENTITY_MIC_OUTPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00),\
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */\
    TUD_AUDIO_DESC_TYPE_I_FORMAT(CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_TX),\
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */\
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epin, /*_attr*/ (uint8_t) ((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX), /*_interval*/ 0x01),\
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */\
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000),\
    /* Interface 2, Alternate 2 - alternate interface for data streaming */\
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)(ITF_NUM_AUDIO_STREAMING_MIC), /*_altset*/ 0x02, /*_nEPs*/ 0x01, /*_stridx*/ 0x04),\
    /* Class-Specific AS Interface Descriptor(4.9.2) */\
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ UAC2_ENTITY_MIC_OUTPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00),\
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */\
    TUD_AUDIO_DESC_TYPE_I_FORMAT(CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_TX),\
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */\
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epin, /*_attr*/ (uint8_t) ((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX), /*_interval*/ 0x01),\
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */\
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000)

#endif
