/*
 * usb_audio_hid_wrapper.h
 *
 *  Created on: 17 мар. 2020 г.
 *      Author: Dmitry
 */

#ifndef USB_AUDIO_HID_WRAPPER_H_
#define USB_AUDIO_HID_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_TEMPLATE
  * @brief This file is the header file for usbd_template_core.c
  * @{
  */


/** @defgroup USBD_TEMPLATE_Exported_Defines
  * @{
  */
//#define TEMPLATE_EPIN_ADDR                 0x81U
//#define TEMPLATE_EPIN_SIZE                 0x10U

//#define USB_TEMPLATE_CONFIG_DESC_SIZ       64U

/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef USBD_AUDIOHID_ClassDriver;
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif




#endif /* USB_AUDIO_HID_WRAPPER_H_ */
