/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include "usb_headset.h"
#include "volume_ctrl.h"
#include "ring_buf.h"
#include "usb_headset_settings.h"

#include "ssd1306.h"
#include "ssd1306_fonts.h"

#if USB_HID_AUDIO_CTRL_ACTIVE == 1
#include "usb_hid_status.h"
#endif //USB_HID_AUDIO_CTRL_ACTIVE
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s2;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_spi3_rx;

TIM_HandleTypeDef htim3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
usb_headset_settings_t current_settings;

// Buffer for speaker data
ring_buf_t spk_ring_buffer;
uint32_t *spk_ring_buffer_storage;

// Buffer for microphone data
ring_buf_t mic_ring_buffer;
uint32_t *mic_ring_buffer_storage;

// Buffer for speaker data
i2s_32b_audio_sample spk_32b_i2s_buffer[SAMPLE_BUFFER_SIZE];
uint8_t spk_usb_read_buf[SAMPLE_BUFFER_SIZE * 2 * 4 * 2]; // Max size of audio sample is  2 * 4. 2 Channels, 4 byte width sample
uint32_t spk_i2s_buf[SAMPLE_BUFFER_SIZE * 2];

// Buffer for microphone data
uint32_t mic_usb_24b_buffer[SAMPLE_BUFFER_SIZE];
uint16_t mic_usb_16b_buffer[SAMPLE_BUFFER_SIZE];
uint8_t mic_usb_read_buf[SAMPLE_BUFFER_SIZE * 2 * 4 * 2]; // Max size of audio sample is  2 * 4. 2 Channels, 4 byte width sample
uint32_t mic_i2s_buf[SAMPLE_BUFFER_SIZE * 2];

#if CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2
i2s_32b_audio_sample mic_i2s_read_buffer[SAMPLE_BUFFER_SIZE];
#else // CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2
uint32_t mic_i2s_read_buffer[SAMPLE_BUFFER_SIZE];
#endif // CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2

#if USB_HID_AUDIO_CTRL_ACTIVE == 1
usb_hid_status_t hid_status;
void check_buttons(void);
void usb_hid_task(void);
#endif //USB_HID_AUDIO_CTRL_ACTIVE

void led_blinking_task(void);

void refresh_i2s_connections(void);

int32_t usb_to_i2s_32b_sample_convert(int32_t sample, uint32_t volume_db);
int32_t usb_to_i2s_16b_sample_convert(int32_t sample, uint32_t volume_db);

void usb_headset_mute_handler(int8_t bChannelNumber, int8_t mute_in);
void usb_headset_volume_handler(int8_t bChannelNumber, int16_t volume_in);
void usb_headset_current_sample_rate_handler(uint32_t current_sample_rate_in);
void usb_headset_current_resolution_handler(uint8_t itf,
		uint8_t current_resolution_in);
void usb_headset_current_status_set_handler(uint8_t itf,
		uint32_t blink_interval_ms_in);

void usb_headset_tud_audio_rx_done_pre_read_handler(uint8_t rhport,
		uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out,
		uint8_t cur_alt_setting);
void usb_headset_tud_audio_tx_done_pre_load_handler(uint8_t rhport, uint8_t itf,
		uint8_t ep_in, uint8_t cur_alt_setting);
void usb_headset_tud_audio_tx_done_post_load_handler(uint8_t rhport,
		uint16_t n_bytes_copied, uint8_t itf, uint8_t ep_in,
		uint8_t cur_alt_setting);

int spk_machine_i2s_write_stream(uint32_t *buf_in, size_t size);
int mic_machine_i2s_read_stream(uint32_t *buf_in, size_t size);

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s);
//void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

uint32_t feed_spk_dma(uint32_t *dma_buffer_p,
		uint32_t sizeof_half_dma_buffer_in_bytes);

uint32_t empty_mic_dma(uint32_t *dma_buffer_p,
		uint32_t sizeof_half_dma_buffer_in_words);

void status_update_task(void);
void display_ssd1306_info(void);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S2_Init(void);
static void MX_I2S3_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM3_Init();
	MX_I2C1_Init();
	MX_I2S2_Init();
	MX_I2S3_Init();
	MX_USB_OTG_FS_PCD_Init();
	/* USER CODE BEGIN 2 */

	current_settings.spk_sample_rate = I2S_SPK_RATE_DEF;
	current_settings.spk_resolution =
	CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX;
	current_settings.spk_blink_interval_ms = BLINK_NOT_MOUNTED;

	current_settings.mic_resolution =
	CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX;
	current_settings.mic_blink_interval_ms = BLINK_NOT_MOUNTED;
	current_settings.usr_mic_mute = false;

	current_settings.status_updated = false;

	usb_headset_set_mute_set_handler(usb_headset_mute_handler);
	usb_headset_set_volume_set_handler(usb_headset_volume_handler);
	usb_headset_set_current_sample_rate_set_handler(
			usb_headset_current_sample_rate_handler);
	usb_headset_set_current_resolution_set_handler(
			usb_headset_current_resolution_handler);
	usb_headset_set_current_status_set_handler(
			usb_headset_current_status_set_handler);

	usb_headset_set_tud_audio_rx_done_pre_read_set_handler(
			usb_headset_tud_audio_rx_done_pre_read_handler);
	usb_headset_set_tud_audio_tx_done_pre_load_set_handler(
			usb_headset_tud_audio_tx_done_pre_load_handler);
	usb_headset_set_tud_audio_tx_done_post_load_set_handler(
			usb_headset_tud_audio_tx_done_post_load_handler);

	for (int i = 0; i < (CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1); i++) {
		current_settings.spk_volume[i] = DEFAULT_VOLUME;
		current_settings.spk_mute[i] = 0;
		current_settings.spk_volume_db[i] = vol_to_db_convert_enc(
				current_settings.spk_mute[i], current_settings.spk_volume[i]);
	}

	HAL_GPIO_WritePin(LED_MIC_MUTE_GPIO_Port, LED_MIC_MUTE_Pin, GPIO_PIN_SET);

	usb_headset_init();
	refresh_i2s_connections();

	TU_LOG1("Headset running\r\n");

	// Init SSD
	ssd1306_Init();

#if USB_HID_AUDIO_CTRL_ACTIVE == 1
	memset(&hid_status, 0x0, sizeof(hid_status));
	HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
#endif //USB_HID_AUDIO_CTRL_ACTIVE

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		usb_headset_task(); // TinyUSB device task
		led_blinking_task();
		status_update_task();
#if USB_HID_AUDIO_CTRL_ACTIVE == 1
		usb_hid_task();
#endif //USB_HID_AUDIO_CTRL_ACTIVE

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void) {
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	/** Initializes the peripherals clock
	 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 384;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 5;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief I2S2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2S2_Init(void) {

	/* USER CODE BEGIN I2S2_Init 0 */

	/* USER CODE END I2S2_Init 0 */

	/* USER CODE BEGIN I2S2_Init 1 */

	/* USER CODE END I2S2_Init 1 */
	hi2s2.Instance = SPI2;
	hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
	hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
	hi2s2.Init.DataFormat = I2S_DATAFORMAT_32B;
	hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
	hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
	hi2s2.Init.CPOL = I2S_CPOL_LOW;
	hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
	hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	if (HAL_I2S_Init(&hi2s2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2S2_Init 2 */

	/* USER CODE END I2S2_Init 2 */

}

/**
 * @brief I2S3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2S3_Init(void) {

	/* USER CODE BEGIN I2S3_Init 0 */

	/* USER CODE END I2S3_Init 0 */

	/* USER CODE BEGIN I2S3_Init 1 */

	/* USER CODE END I2S3_Init 1 */
	hi2s3.Instance = SPI3;
	hi2s3.Init.Mode = I2S_MODE_MASTER_RX;
	hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
	hi2s3.Init.DataFormat = I2S_DATAFORMAT_32B;
	hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
	hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_48K;
	hi2s3.Init.CPOL = I2S_CPOL_LOW;
	hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
	hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	if (HAL_I2S_Init(&hi2s3) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2S3_Init 2 */

	/* USER CODE END I2S3_Init 2 */

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_Encoder_InitTypeDef sConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 65535;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * @brief USB_OTG_FS Initialization Function
 * @param None
 * @retval None
 */
static void MX_USB_OTG_FS_PCD_Init(void) {

	/* USER CODE BEGIN USB_OTG_FS_Init 0 */

	/* USER CODE END USB_OTG_FS_Init 0 */

	/* USER CODE BEGIN USB_OTG_FS_Init 1 */

	/* USER CODE END USB_OTG_FS_Init 1 */
	hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
	hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
	hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
	hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
	if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USB_OTG_FS_Init 2 */

	/* USER CODE END USB_OTG_FS_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	/* DMA1_Stream4_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(ONBOARD_LED_GPIO_Port, ONBOARD_LED_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED_MIC_MUTE_GPIO_Port, LED_MIC_MUTE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : ONBOARD_LED_Pin */
	GPIO_InitStruct.Pin = ONBOARD_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ONBOARD_LED_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LED_MIC_MUTE_Pin */
	GPIO_InitStruct.Pin = LED_MIC_MUTE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_MIC_MUTE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USR_MIC_MUTE_BTN_Pin */
	GPIO_InitStruct.Pin = USR_MIC_MUTE_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(USR_MIC_MUTE_BTN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USR_SPK_MUTE_BTN_Pin */
	GPIO_InitStruct.Pin = USR_SPK_MUTE_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(USR_SPK_MUTE_BTN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : USR_SCAN_NEXT_BTN_Pin USR_SCAN_PREV_BTN_Pin USR_PLAY_PAUSE_BTN_Pin */
	GPIO_InitStruct.Pin = USR_SCAN_NEXT_BTN_Pin | USR_SCAN_PREV_BTN_Pin
			| USR_PLAY_PAUSE_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void refresh_i2s_connections(void) {
	HAL_I2S_StateTypeDef dma_state;
	// Stop speaker DMA
	dma_state = HAL_I2S_GetState(&hi2s2);
	if (dma_state != HAL_I2S_STATE_READY) {
		HAL_StatusTypeDef stop_status = HAL_I2S_DMAStop(&hi2s2);
		if (stop_status != HAL_OK) {
			Error_Handler();
		}
	}

	// Stop microphone DMA
	dma_state = HAL_I2S_GetState(&hi2s3);
	if (dma_state != HAL_I2S_STATE_READY) {
		HAL_StatusTypeDef stop_status = HAL_I2S_DMAStop(&hi2s3);
		if (stop_status != HAL_OK) {
			Error_Handler();
		}
	}

	if (spk_ring_buffer_storage != NULL) {
		free(spk_ring_buffer_storage);
	}

	current_settings.samples_in_i2s_frame_min =
			(current_settings.spk_sample_rate) / 1000;
	current_settings.samples_in_i2s_frame_max =
			(current_settings.spk_sample_rate + 999) / 1000;

	// Ring buffer contains 2 ms data
	spk_ring_buffer_storage = m_new(uint32_t,
			current_settings.samples_in_i2s_frame_max * 2 * 2);
	ringbuf_init(&spk_ring_buffer, spk_ring_buffer_storage,
			current_settings.samples_in_i2s_frame_max * 2 * 2);

	mic_ring_buffer_storage = m_new(uint32_t,
			current_settings.samples_in_i2s_frame_max * 2 * 2);
	ringbuf_init(&mic_ring_buffer, mic_ring_buffer_storage,
			current_settings.samples_in_i2s_frame_max * 2 * 2);

	// Clear memories
	memset(spk_i2s_buf, 0x0, sizeof(spk_i2s_buf));
	memset(mic_i2s_buf, 0x0, sizeof(mic_i2s_buf));
	memset(mic_usb_24b_buffer, 0x0, sizeof(mic_usb_24b_buffer));
	memset(mic_usb_16b_buffer, 0x0, sizeof(mic_usb_16b_buffer));

	uint16_t num_of_samples_in_buffer =
			current_settings.samples_in_i2s_frame_max * 2;

	// Run transmit DMA
	HAL_StatusTypeDef tx_status = HAL_I2S_Transmit_DMA(&hi2s2, spk_i2s_buf,
			num_of_samples_in_buffer);
	if (tx_status != HAL_OK) {
		Error_Handler();
	}

	HAL_StatusTypeDef rx_status = HAL_I2S_Receive_DMA(&hi2s3, mic_i2s_buf,
			num_of_samples_in_buffer);
	if (rx_status != HAL_OK) {
		Error_Handler();
	}
}

void usb_headset_mute_handler(int8_t bChannelNumber, int8_t mute_in) {
	current_settings.spk_mute[bChannelNumber] = mute_in;
	current_settings.spk_volume_db[bChannelNumber] = vol_to_db_convert_enc(
			current_settings.spk_mute[bChannelNumber],
			current_settings.spk_volume[bChannelNumber]);

	current_settings.spk_volume_mul_db[0] = current_settings.spk_volume_db[0]
			* current_settings.spk_volume_db[1]; // Left ch volume
	current_settings.spk_volume_mul_db[1] = current_settings.spk_volume_db[0]
			* current_settings.spk_volume_db[2]; // Right ch volume

	current_settings.status_updated = true;
}

void usb_headset_volume_handler(int8_t bChannelNumber, int16_t volume_in) {
	current_settings.spk_volume[bChannelNumber] = volume_in;
	current_settings.spk_volume_db[bChannelNumber] = vol_to_db_convert_enc(
			current_settings.spk_mute[bChannelNumber],
			current_settings.spk_volume[bChannelNumber]);

	current_settings.spk_volume_mul_db[0] = current_settings.spk_volume_db[0]
			* current_settings.spk_volume_db[1]; // Left ch volume
	current_settings.spk_volume_mul_db[1] = current_settings.spk_volume_db[0]
			* current_settings.spk_volume_db[2]; // Right ch volume

	current_settings.status_updated = true;
}

void usb_headset_current_sample_rate_handler(uint32_t current_sample_rate_in) {
	current_settings.spk_sample_rate = current_sample_rate_in;
	refresh_i2s_connections();
	current_settings.status_updated = true;
}

void usb_headset_current_resolution_handler(uint8_t itf,
		uint8_t current_resolution_in) {
	if ((itf == 0) || (itf == ITF_NUM_AUDIO_STREAMING_SPK))
		current_settings.spk_resolution = current_resolution_in;
	if ((itf == 0) || (itf == ITF_NUM_AUDIO_STREAMING_MIC))
		current_settings.mic_resolution = current_resolution_in;

	refresh_i2s_connections();
	current_settings.status_updated = true;
}

void usb_headset_current_status_set_handler(uint8_t itf,
		uint32_t blink_interval_ms_in) {
	if ((itf == 0) || (itf == ITF_NUM_AUDIO_STREAMING_SPK))
		current_settings.spk_blink_interval_ms = blink_interval_ms_in;
	if ((itf == 0) || (itf == ITF_NUM_AUDIO_STREAMING_MIC))
		current_settings.mic_blink_interval_ms = blink_interval_ms_in;
	current_settings.status_updated = true;
}

void usb_headset_tud_audio_rx_done_pre_read_handler(uint8_t rhport,
		uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out,
		uint8_t cur_alt_setting) {
	uint32_t volume_db_left = current_settings.spk_volume_mul_db[0];
	uint32_t volume_db_right = current_settings.spk_volume_mul_db[1];

	if (current_settings.spk_blink_interval_ms == BLINK_STREAMING) {
		// Speaker data size received in the last frame
		uint16_t usb_spk_data_size = tud_audio_n_read(func_id, spk_usb_read_buf,
				n_bytes_received);
		//uint16_t usb_spk_data_size = tud_audio_read(spk_usb_read_buf,
		//		n_bytes_received);
		uint16_t usb_sample_count = 0;

		if (current_settings.spk_resolution == 16) {
			int16_t *in = (int16_t*) spk_usb_read_buf;
			usb_sample_count = usb_spk_data_size / 4; // 4 bytes per sample 2b left, 2b right

			//if (usb_sample_count >= current_settings.samples_in_i2s_frame_min) {
			for (int i = 0; i < usb_sample_count; i++) {
				int32_t left = in[i * 2 + 0];
				int32_t right = in[i * 2 + 1];

				left = usb_to_i2s_16b_sample_convert(left, volume_db_left);
				right = usb_to_i2s_16b_sample_convert(right, volume_db_right);

				spk_32b_i2s_buffer[i].left = left;
				spk_32b_i2s_buffer[i].right = right;
			}
			spk_machine_i2s_write_stream(&spk_32b_i2s_buffer[0],
					usb_sample_count * 2);
			//}
		} else if (current_settings.spk_resolution == 24) {
			int32_t *in = (int32_t*) spk_usb_read_buf;
			usb_sample_count = usb_spk_data_size / 8; // 8 bytes per sample 4b left, 4b right

			//if (usb_sample_count >= current_settings.samples_in_i2s_frame_min) {
			for (int i = 0; i < usb_sample_count; i++) {
				int32_t left = in[i * 2 + 0];
				int32_t right = in[i * 2 + 1];

				left = usb_to_i2s_32b_sample_convert(left, volume_db_left);
				right = usb_to_i2s_32b_sample_convert(right, volume_db_right);

				spk_32b_i2s_buffer[i].left = left;
				spk_32b_i2s_buffer[i].right = right;
			}
			spk_machine_i2s_write_stream(&spk_32b_i2s_buffer[0],
					usb_sample_count * 2);
			//}
		}
	}
}

void usb_headset_tud_audio_tx_done_pre_load_handler(uint8_t rhport, uint8_t itf,
		uint8_t ep_in, uint8_t cur_alt_setting) {
	if (current_settings.mic_resolution == 24) {
		uint32_t buffer_size = current_settings.samples_in_i2s_frame_min
				* CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_TX
				* CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX;
		tud_audio_write(&(mic_usb_24b_buffer[0]), buffer_size);
	} else {
		uint32_t buffer_size = current_settings.samples_in_i2s_frame_min
				* CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX
				* CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX;
		tud_audio_write(&(mic_usb_16b_buffer[0]), buffer_size);
	}
}

#if CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2
void usb_headset_tud_audio_tx_done_post_load_handler(uint8_t rhport,
		uint16_t n_bytes_copied, uint8_t itf, uint8_t ep_in,
		uint8_t cur_alt_setting) {

	// Read data from microphone
	uint32_t buffer_size_read = current_settings.samples_in_i2s_frame_min * 2;
	int num_words_read = mic_machine_i2s_read_stream(&mic_i2s_read_buffer[0],
			buffer_size_read);
	int num_of_frames_read = num_words_read / 2;
	if (num_of_frames_read >= current_settings.samples_in_i2s_frame_min) {

		for (uint32_t i = 0; i < num_of_frames_read; i++) {
			if (current_settings.mic_resolution == 24) {
				if (current_settings.usr_mic_mute == false) {
					int32_t left_24b = (int32_t) mic_i2s_read_buffer[i].left
							<< MIC_FORMAT_24B_TO_24B_SHIFT_VAL; // Magic number
					int32_t right_24b =
							(int32_t) mic_i2s_read_buffer[i].right
									<< MIC_FORMAT_24B_TO_24B_SHIFT_VAL; // Magic number

					mic_usb_24b_buffer[i * 2] = left_24b; // TODO: check this value
					mic_usb_24b_buffer[i * 2 + 1] = right_24b; // TODO: check this value
				} else {
					mic_usb_24b_buffer[i * 2] = 0x0;
					mic_usb_24b_buffer[i * 2 + 1] = 0x0;
				}
			} else {
				if (current_settings.usr_mic_mute == false) {
					int32_t left_16b = (int32_t) mic_i2s_read_buffer[i].left
							>> MIC_FORMAT_24B_TO_16B_SHIFT_VAL; // Magic number
					int32_t right_16b =
							(int32_t) mic_i2s_read_buffer[i].right
									>> MIC_FORMAT_24B_TO_16B_SHIFT_VAL; // Magic number

					mic_usb_16b_buffer[i * 2] = left_16b; // TODO: check this value
					mic_usb_16b_buffer[i * 2 + 1] = right_16b; // TODO: check this value
				} else {
					mic_usb_16b_buffer[i * 2] = 0x0; // TODO: check this value
					mic_usb_16b_buffer[i * 2 + 1] = 0x0; // TODO: check this value
				}
			}
		}
	}
}
#else // CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2
void usb_headset_tud_audio_tx_done_post_load_handler(uint8_t rhport,
		uint16_t n_bytes_copied, uint8_t itf, uint8_t ep_in,
		uint8_t cur_alt_setting) {

	// Read data from microphone
	uint32_t buffer_size_read = current_settings.samples_in_i2s_frame_min;
	int num_words_read = mic_machine_i2s_read_stream(&mic_i2s_read_buffer[0],
			buffer_size_read);
	int num_of_frames_read = num_words_read;

	if (num_of_frames_read >= current_settings.samples_in_i2s_frame_min) {
		for (uint32_t i = 0; i < num_of_frames_read; i++) {
			if (current_settings.mic_resolution == 24) {
				if (current_settings.usr_mic_mute == false) {
					int32_t mono_24b = (int32_t) mic_i2s_read_buffer[i]
							<< MIC_FORMAT_24B_TO_24B_SHIFT_VAL; // Magic number

					mic_usb_24b_buffer[i] = mono_24b; // TODO: check this value
				} else {
					mic_usb_24b_buffer[i] = 0x0; // TODO: check this value
				}
			} else {
				if (current_settings.usr_mic_mute == false) {
					int32_t mono_24b = (int32_t) mic_i2s_read_buffer[i]
							>> MIC_FORMAT_24B_TO_16B_SHIFT_VAL; // Magic number

					mic_usb_16b_buffer[i] = mono_24b; // TODO: check this value
				} else {
					mic_usb_16b_buffer[i] = 0x0; // TODO: check this value
				}
			}
		}
	}
}
#endif //#if CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2

int32_t usb_to_i2s_32b_sample_convert(int32_t sample, uint32_t volume_db) {
	int64_t sample_tmp = (int64_t) sample * (int64_t) volume_db;
	sample_tmp = sample_tmp >> 30;
	return (int32_t) sample_tmp;
//	return (int32_t)sample;
}

int32_t usb_to_i2s_16b_sample_convert(int32_t sample, uint32_t volume_db) {
	int64_t sample_tmp = (int64_t) sample * (int64_t) volume_db;
	sample_tmp = sample_tmp >> (30 - 16);
	return (int32_t) sample_tmp;
//	return (int32_t)sample;
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void) {
	static uint32_t start_ms = 0;
	static bool led_state = false;

	uint32_t cur_time_ms = board_millis();

	// Blink every interval ms
	if (cur_time_ms - start_ms < current_settings.spk_blink_interval_ms)
		return;
	start_ms += current_settings.spk_blink_interval_ms;

	board_led_write(led_state);
	led_state = 1 - led_state;
}

int spk_machine_i2s_write_stream(uint32_t *buf_in, size_t size) {
	if (size == 0) {
		return 0;
	}

	// copy audio samples from the app buffer to the ring buffer
	// loop, reading samples until the app buffer is emptied
	// for uasyncio mode, the loop will make an early exit if the ring buffer becomes full

	for(uint32_t a_index = 0; a_index < size; a_index++){
		if(ringbuf_push_half_word_swap(&spk_ring_buffer, buf_in[a_index]) == false){
			return a_index;
		}
	}

	return size;
}

int mic_machine_i2s_read_stream(uint32_t *buf_in, size_t size) {
	if (size == 0) {
		return 0;
	}

	uint32_t available_data_words = ringbuf_available_data(&mic_ring_buffer);

	if (available_data_words >= size) {
		for (uint32_t i = 0; i < size; i++) {
			ringbuf_pop_half_word_swap(&mic_ring_buffer, &buf_in[i]);
		}
		return size;
	} else {
		// underflow.  clear buffer to transmit "silence" on the I2S bus
		memset(buf_in, 0, size * sizeof(uint32_t));
		return size;
	}
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
	feed_spk_dma(&spk_i2s_buf[0], current_settings.samples_in_i2s_frame_max);
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
	feed_spk_dma(&spk_i2s_buf[current_settings.samples_in_i2s_frame_max],
			current_settings.samples_in_i2s_frame_max);
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
	empty_mic_dma(&mic_i2s_buf[0], current_settings.samples_in_i2s_frame_max);
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
	empty_mic_dma(&mic_i2s_buf[current_settings.samples_in_i2s_frame_max],
			current_settings.samples_in_i2s_frame_max);
}

/**
 * @brief  EXTI line detection callbacks.
 * @param  GPIO_Pin Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	// No interrupts are connected...
	// Nothing to do here

//	static uint32_t prev_btb_press__ms = 0;
//
//	uint32_t cur_time_ms = board_millis();
//
//	if (cur_time_ms - prev_btb_press__ms < 50)
//		return;
//
//	prev_btb_press__ms = cur_time_ms;
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s){
	refresh_i2s_connections();
}

uint32_t feed_spk_dma(uint32_t *dma_buffer_p,
		uint32_t sizeof_half_dma_buffer_in_words) {
	// when data exists, copy samples from ring buffer
	uint32_t available_data_words = ringbuf_available_data(&spk_ring_buffer);
	if (available_data_words > sizeof_half_dma_buffer_in_words) {
		available_data_words = sizeof_half_dma_buffer_in_words;
	}

	if (available_data_words >= sizeof_half_dma_buffer_in_words) {
		for (uint32_t i = 0; i < sizeof_half_dma_buffer_in_words; i++) {
			ringbuf_pop(&spk_ring_buffer, &dma_buffer_p[i]);
		}
		return available_data_words;
	} else {
		// underflow.  clear buffer to transmit "silence" on the I2S bus
		memset(dma_buffer_p, 0,
				sizeof_half_dma_buffer_in_words * sizeof(uint32_t));
		return sizeof_half_dma_buffer_in_words;
	}
}

#if CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2
uint32_t empty_mic_dma(uint32_t *dma_buffer_p,
		uint32_t sizeof_half_dma_buffer_in_words) {
	// when space exists, copy samples into ring buffer
	if (ringbuf_available_space(&mic_ring_buffer)
			>= sizeof_half_dma_buffer_in_words) {
		for (uint32_t i = 0; i < sizeof_half_dma_buffer_in_words; i++) {
			ringbuf_push(&mic_ring_buffer, dma_buffer_p[i]);
		}
	}
}
#else // CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2
uint32_t empty_mic_dma(uint32_t *dma_buffer_p,
		uint32_t sizeof_half_dma_buffer_in_words) {
	// when space exists, copy samples into ring buffer
	if (ringbuf_available_space(&mic_ring_buffer)
			>= sizeof_half_dma_buffer_in_words) {
		for (uint32_t i = 0; i < sizeof_half_dma_buffer_in_words; i+=2) {
			ringbuf_push(&mic_ring_buffer, dma_buffer_p[i]);
		}
	}
}
#endif// CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX == 2

//--------------------------------------------------------------------+
// STATUS UPDATE TASK
//--------------------------------------------------------------------+
void status_update_task(void) {
	static uint32_t prev_status_update__ms = 0;

	uint32_t cur_time_ms = board_millis();

	HAL_GPIO_WritePin(LED_MIC_MUTE_GPIO_Port, LED_MIC_MUTE_Pin,
			current_settings.usr_mic_mute ? GPIO_PIN_SET : GPIO_PIN_RESET);

	// Update status once per second
	if (cur_time_ms - prev_status_update__ms < 1000)
		return;

	prev_status_update__ms = cur_time_ms;

	if (current_settings.status_updated == true) {
		current_settings.status_updated = false;
		display_ssd1306_info();
	}
}

void display_ssd1306_info(void) {
	char fmt_tmp_str[20] = "";

	ssd1306_Fill(Black);

	switch (current_settings.spk_blink_interval_ms) {
	case BLINK_NOT_MOUNTED: {
		ssd1306_SetCursor(4, 0);
		ssd1306_WriteString("Spk not mounted", Font_6x8, White);
		break;
	}
	case BLINK_SUSPENDED: {
		ssd1306_SetCursor(4, 0);
		ssd1306_WriteString("Spk suspended", Font_6x8, White);
		break;
	}
	case BLINK_MOUNTED: {
		ssd1306_SetCursor(4, 0);
		ssd1306_WriteString("Spk mounted", Font_6x8, White);
		break;
	}
	case BLINK_STREAMING: {
		char spk_streaming_str[20] = "Spk stream: ";
		memset(fmt_tmp_str, 0x0, sizeof(fmt_tmp_str));

		itoa(current_settings.spk_resolution, fmt_tmp_str, 10);
		strcat(spk_streaming_str, fmt_tmp_str);
		strcat(spk_streaming_str, " bit");

		ssd1306_SetCursor(4, 0);
		ssd1306_WriteString(spk_streaming_str, Font_6x8, White);
		break;
	}
	default: {
		ssd1306_SetCursor(4, 0);
		ssd1306_WriteString("Spk unknown", Font_6x8, White);
		break;
	}
	}

	switch (current_settings.mic_blink_interval_ms) {
	case BLINK_NOT_MOUNTED: {
		ssd1306_SetCursor(4, 8);
		ssd1306_WriteString("Mic not mounted", Font_6x8, White);
		break;
	}
	case BLINK_SUSPENDED: {
		ssd1306_SetCursor(4, 8);
		ssd1306_WriteString("Mic suspended", Font_6x8, White);
		break;
	}
	case BLINK_MOUNTED: {
		ssd1306_SetCursor(4, 8);
		ssd1306_WriteString("Mic mounted", Font_6x8, White);
		break;
	}
	case BLINK_STREAMING: {
		char mic_streaming_str[20] = "Mic stream: ";
		memset(fmt_tmp_str, 0x0, sizeof(fmt_tmp_str));

		itoa(current_settings.mic_resolution, fmt_tmp_str, 10);
		strcat(mic_streaming_str, fmt_tmp_str);
		strcat(mic_streaming_str, " bit");

		ssd1306_SetCursor(4, 8);
		ssd1306_WriteString(mic_streaming_str, Font_6x8, White);
		break;
	}
	default: {
		ssd1306_SetCursor(4, 8);
		ssd1306_WriteString("Mic unknown", Font_6x8, White);
		break;
	}
	}

	{
		char freq_str[20] = "Freq: ";

		memset(fmt_tmp_str, 0x0, sizeof(fmt_tmp_str));

		itoa((current_settings.spk_sample_rate), fmt_tmp_str, 10);
		strcat(freq_str, fmt_tmp_str);
		strcat(freq_str, " Hz");

		ssd1306_SetCursor(4, 16);
		ssd1306_WriteString(freq_str, Font_6x8, White);

	}

	{
		char vol_m_str[20] = "Vol M:";
		char vol_l_str[20] = "Vol L:";
		char vol_r_str[20] = "Vol R:";

		memset(fmt_tmp_str, 0x0, sizeof(fmt_tmp_str));
		itoa((current_settings.spk_volume[0] >> ENC_NUM_OF_FP_BITS),
				fmt_tmp_str, 10);
		strcat(vol_m_str, fmt_tmp_str);

		strcat(vol_m_str, " Mute: ");
		strcat(vol_m_str, (current_settings.spk_mute[0] ? "T" : "F"));

		memset(fmt_tmp_str, 0x0, sizeof(fmt_tmp_str));
		itoa((current_settings.spk_volume[1] >> ENC_NUM_OF_FP_BITS),
				fmt_tmp_str, 10);
		strcat(vol_l_str, fmt_tmp_str);

		strcat(vol_l_str, " Mute: ");
		strcat(vol_l_str, (current_settings.spk_mute[1] ? "T" : "F"));

		memset(fmt_tmp_str, 0x0, sizeof(fmt_tmp_str));
		itoa((current_settings.spk_volume[2] >> ENC_NUM_OF_FP_BITS),
				fmt_tmp_str, 10);
		strcat(vol_r_str, fmt_tmp_str);

		strcat(vol_r_str, " Mute: ");
		strcat(vol_r_str, (current_settings.spk_mute[2] ? "T" : "F"));

		ssd1306_SetCursor(4, 24);
		ssd1306_WriteString(vol_m_str, Font_6x8, White);
		ssd1306_SetCursor(4, 32);
		ssd1306_WriteString(vol_l_str, Font_6x8, White);
		ssd1306_SetCursor(4, 40);
		ssd1306_WriteString(vol_r_str, Font_6x8, White);
	}
	ssd1306_UpdateScreen();
}

#if USB_HID_AUDIO_CTRL_ACTIVE == 1
//-----------------------------------------------------------------
//                 USB HID
//-----------------------------------------------------------------

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
		hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
	// TODO not Implemented
	(void) instance;
	(void) report_id;
	(void) report_type;
	(void) buffer;
	(void) reqlen;

	return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
		hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
	(void) instance;

//  if (report_type == HID_REPORT_TYPE_OUTPUT)
//  {
//    // Set keyboard LED e.g Capslock, Numlock etc...
//    if (report_id == REPORT_ID_KEYBOARD)
//    {
//      // bufsize should be (at least) 1
//      if ( bufsize < 1 ) return;
//
//      uint8_t const kbd_leds = buffer[0];
//
//      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
//      {
//        // Capslock On: disable blink, turn led on
//        blink_interval_ms = 0;
//        board_led_write(true);
//      }else
//      {
//        // Caplocks Off: back to normal blink
//        board_led_write(false);
//        blink_interval_ms = BLINK_MOUNTED;
//      }
//    }
//  }
}

void check_buttons(void) {
	//--------------
	// Checking play/pause, scan prev/next buttons
	//--------------
	GPIO_PinState play_pause_pin_state = HAL_GPIO_ReadPin(
			USR_PLAY_PAUSE_BTN_GPIO_Port, USR_PLAY_PAUSE_BTN_Pin);
	GPIO_PinState scan_prev_pin_state = HAL_GPIO_ReadPin(
			USR_SCAN_PREV_BTN_GPIO_Port, USR_SCAN_PREV_BTN_Pin);
	GPIO_PinState scan_next_pin_state = HAL_GPIO_ReadPin(
			USR_SCAN_NEXT_BTN_GPIO_Port, USR_SCAN_NEXT_BTN_Pin);

	if ((play_pause_pin_state == GPIO_PIN_SET)
			&& (hid_status.btn_play_pause_status == GPIO_PIN_RESET)) {
		hid_status.custom_ctrl_scan_code_updated = true;
		hid_status.cur_custom_ctrl_scan_code =
				MY_TUD_HID_CONSUMER_PLAY_PAUSE_CODE;
	}
	hid_status.btn_play_pause_status = play_pause_pin_state;

	if ((scan_prev_pin_state == GPIO_PIN_SET)
			&& (hid_status.btn_scan_prev_status == GPIO_PIN_RESET)) {
		hid_status.custom_ctrl_scan_code_updated = true;
		hid_status.cur_custom_ctrl_scan_code =
				MY_TUD_HID_CONSUMER_SCAN_PREVIOUS_CODE;
	}
	hid_status.btn_scan_next_status = scan_prev_pin_state;

	if ((scan_next_pin_state == GPIO_PIN_SET)
			&& (hid_status.btn_scan_next_status == GPIO_PIN_RESET)) {
		hid_status.custom_ctrl_scan_code_updated = true;
		hid_status.cur_custom_ctrl_scan_code =
				MY_TUD_HID_CONSUMER_SCAN_NEXT_CODE;
	}
	hid_status.btn_scan_next_status = scan_next_pin_state;
	//--------------

	//--------------
	// Checking MIC mute button
	//--------------
	GPIO_PinState mic_mute_pin_state = HAL_GPIO_ReadPin(
			USR_MIC_MUTE_BTN_GPIO_Port, USR_MIC_MUTE_BTN_Pin);
	if ((mic_mute_pin_state == GPIO_PIN_SET)
			&& (hid_status.btn_mic_mute_status == GPIO_PIN_RESET)) {
		current_settings.usr_mic_mute = !current_settings.usr_mic_mute;
	}
	hid_status.btn_mic_mute_status = mic_mute_pin_state;

	//--------------
	// Checking SPK mute button
	//--------------
	GPIO_PinState spk_mute_pin_state = HAL_GPIO_ReadPin(
			USR_SPK_MUTE_BTN_GPIO_Port, USR_SPK_MUTE_BTN_Pin);
	if ((spk_mute_pin_state == GPIO_PIN_RESET)
			&& (hid_status.btn_spk_mute_status == GPIO_PIN_SET)) {
		hid_status.cur_custom_ctrl_scan_code = MY_TUD_HID_CONSUMER_MUTE_CODE;
		hid_status.custom_ctrl_scan_code_updated = true;
	}
	hid_status.btn_spk_mute_status = spk_mute_pin_state;
}

void usb_hid_task(void) {
	static uint32_t prev_hid_tasc_call__ms = 0;
	static bool prev_media_report_is_not_empty = false;

	uint32_t cur_time_ms = board_millis();

	// Update status once per second
	if (cur_time_ms - prev_hid_tasc_call__ms < 50)
		return;

	//--------------
	// Checking rotary encoder
	//--------------
	int16_t volume_rotary_encoder_cntr = __HAL_TIM_GET_COUNTER(&htim3);
	if (volume_rotary_encoder_cntr
			!= hid_status.volume_rotary_encoder_cntr_prev) {
		hid_status.cur_custom_ctrl_scan_code =
				(volume_rotary_encoder_cntr
						> hid_status.volume_rotary_encoder_cntr_prev) ?
						MY_TUD_HID_CONSUMER_VOLUME_INCREMENT_CODE :
						MY_TUD_HID_CONSUMER_VOLUME_DECREMENT_CODE;

		hid_status.custom_ctrl_scan_code_updated = true;

		hid_status.volume_rotary_encoder_cntr_prev = volume_rotary_encoder_cntr;
	}
	//--------------

	//--------------
	// Checking buttons
	//--------------
	check_buttons();
	//--------------

	// Remote wakeup
	if (tud_suspended() && (hid_status.cur_custom_ctrl_scan_code != 0x0)
			&& (hid_status.custom_ctrl_scan_code_updated == true)) {
		// Wake up host if we are in suspend mode
		// and REMOTE_WAKEUP feature is enabled by host
		tud_remote_wakeup();
	} else {
		if (!tud_hid_ready())
			return;

		if (hid_status.custom_ctrl_scan_code_updated == true) {
			tud_hid_report(REPORT_ID_CONSUMER_CONTROL,
					&(hid_status.cur_custom_ctrl_scan_code),
					sizeof(hid_status.cur_custom_ctrl_scan_code));

			hid_status.cur_custom_ctrl_scan_code = 0x0;
			hid_status.custom_ctrl_scan_code_updated = false;
			prev_media_report_is_not_empty = true;
		} else {
			if (prev_media_report_is_not_empty) {
				uint8_t empty_scan_code = 0x0;
				tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &(empty_scan_code),
						sizeof(empty_scan_code));
				prev_media_report_is_not_empty = false;
			}
		}
	}
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
#endif // USB_HID_AUDIO_CTRL_ACTIVE

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
