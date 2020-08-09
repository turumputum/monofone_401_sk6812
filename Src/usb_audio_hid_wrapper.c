#include "usb_audio_hid_wrapper.h"
#include "usbd_ctlreq.h"
#include "usbd_audio.h"
#include "usbd_hid.h"

static uint8_t USBD_AUDIOHID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_AUDIOHID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_AUDIOHID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_AUDIOHID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_AUDIOHID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_AUDIOHID_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_AUDIOHID_EP0_TxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_AUDIOHID_SOF(USBD_HandleTypeDef *pdev);
static uint8_t USBD_AUDIOHID_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_AUDIOHID_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t *USBD_AUDIOHID_GetCfgDesc(uint16_t *length);
static uint8_t *USBD_AUDIOHID_GetDeviceQualifierDesc(uint16_t *length);

#define AUDIO_SAMPLE_FREQ(frq)         (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

#define AUDIO_PACKET_SZE(frq)          (uint8_t)(((frq * 2U * 2U)/1000U) & 0xFFU), \
                                       (uint8_t)((((frq * 2U * 2U)/1000U) >> 8) & 0xFFU)

extern USBD_ClassTypeDef USBD_AUDIO;
extern USBD_ClassTypeDef USBD_HID;
void * AudioClass;
void * AudioClassData;
void * AudioUserData;
void * HIDClass;
void * HIDClassData;
void * HIDUserData;

typedef enum {
	Audio,
	HID
} IFace;
void SwitchPdevTo(USBD_HandleTypeDef* pdev, IFace to);
/**
  * @}
  */

/** @defgroup USBD_AUDIOHID_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_AUDIOHID_ClassDriver =
{
  USBD_AUDIOHID_Init,
  USBD_AUDIOHID_DeInit,
  USBD_AUDIOHID_Setup,
  USBD_AUDIOHID_EP0_TxReady,
  USBD_AUDIOHID_EP0_RxReady,
  USBD_AUDIOHID_DataIn,
  USBD_AUDIOHID_DataOut,
  USBD_AUDIOHID_SOF,
  USBD_AUDIOHID_IsoINIncomplete,
  USBD_AUDIOHID_IsoOutIncomplete,
  USBD_AUDIOHID_GetCfgDesc,
  USBD_AUDIOHID_GetCfgDesc,
  USBD_AUDIOHID_GetCfgDesc,
  USBD_AUDIOHID_GetDeviceQualifierDesc,
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
/* USB AUDIOHID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_AUDIOHID_CfgDesc[USB_AUDIO_CONFIG_DESC_SIZ] __ALIGN_END =
{
		/* Configuration 1 */
		0x09,                                 /* bLength */
		USB_DESC_TYPE_CONFIGURATION,          /* bDescriptorType */
		LOBYTE(USB_AUDIO_CONFIG_DESC_SIZ),    /* wTotalLength  109 bytes*/
		HIBYTE(USB_AUDIO_CONFIG_DESC_SIZ),
		0x03,                                 /* bNumInterfaces */
		0x01,                                 /* bConfigurationValue */
		0x00,                                 /* iConfiguration */
		0xC0,                                 /* bmAttributes  BUS Powred*/
		0x32,                                 /* bMaxPower = 100 mA*/
		/* 09 byte*/

		/******** IAD  */
		0x08, /* bLength */
		0x0B, /* bDescriptorType */
		0, /* bFirstInterface */
		0x02, /* bInterfaceCount */
		0x01, /* bFunctionClass */
		0x00, /* bFunctionSubClass */
		0x00, /* bFunctionProtocol */
		0x00, /* iFunction (Index of string descriptor describing this function) */
		/* 08 bytes */

		/* USB Speaker Standard interface descriptor */
		AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
		USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
		0x00,                                 /* bInterfaceNumber */
		0x00,                                 /* bAlternateSetting */
		0x00,                                 /* bNumEndpoints */
		USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
		AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
		AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
		0x00,                                 /* iInterface */
		/* 09 byte*/

		/* USB Speaker Class-specific AC Interface Descriptor */
		AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
		AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
		0x00,          /* 1.00 */             /* bcdADC */
		0x01,
		0x27,                                 /* wTotalLength = 39*/
		0x00,
		0x01,                                 /* bInCollection */
		0x01,                                 /* baInterfaceNr */
		/* 09 byte*/

		/* USB Speaker Input Terminal Descriptor */
		AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
		AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
		0x01,                                 /* bTerminalID */
		0x01,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
		0x01,
		0x00,                                 /* bAssocTerminal */
		0x01,                                 /* bNrChannels */
		0x00,                                 /* wChannelConfig 0x0000  Mono */
		0x00,
		0x00,                                 /* iChannelNames */
		0x00,                                 /* iTerminal */
		/* 12 byte*/

		/* USB Speaker Audio Feature Unit Descriptor */
		0x09,                                 /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
		AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
		AUDIO_OUT_STREAMING_CTRL,             /* bUnitID */
		0x01,                                 /* bSourceID */
		0x01,                                 /* bControlSize */
		AUDIO_CONTROL_MUTE,                   /* bmaControls(0) */
		0,                                    /* bmaControls(1) */
		0x00,                                 /* iTerminal */
		/* 09 byte*/

		/*USB Speaker Output Terminal Descriptor */
		0x09,      /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
		AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
		0x03,                                 /* bTerminalID */
		0x01,                                 /* wTerminalType  0x0301*/
		0x03,
		0x00,                                 /* bAssocTerminal */
		0x02,                                 /* bSourceID */
		0x00,                                 /* iTerminal */
		/* 09 byte*/

		/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
		/* Interface 1, Alternate Setting 0                                             */
		AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
		USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
		0x01,                                 /* bInterfaceNumber */
		0x00,                                 /* bAlternateSetting */
		0x00,                                 /* bNumEndpoints */
		USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
		AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
		AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
		0x00,                                 /* iInterface */
		/* 09 byte*/

		/* USB Speaker Standard AS Interface Descriptor - Audio Streaming Operational */
		/* Interface 1, Alternate Setting 1                                           */
		AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
		USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
		0x01,                                 /* bInterfaceNumber */
		0x01,                                 /* bAlternateSetting */
		0x01,                                 /* bNumEndpoints */
		USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
		AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
		AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
		0x00,                                 /* iInterface */
		/* 09 byte*/

		/* USB Speaker Audio Streaming Interface Descriptor */
		AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
		AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
		0x01,                                 /* bTerminalLink */
		0x01,                                 /* bDelay */
		0x01,                                 /* wFormatTag AUDIO_FORMAT_PCM  0x0001 */
		0x00,
		/* 07 byte*/

		/* USB Speaker Audio Type III Format Interface Descriptor */
		0x0B,                                 /* bLength */
		AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
		AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
		AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
		0x02,                                 /* bNrChannels */
		0x02,                                 /* bSubFrameSize :  2 Bytes per frame (16bits) */
		16,                                   /* bBitResolution (16-bits per sample) */
		0x01,                                 /* bSamFreqType only one frequency supported */
		AUDIO_SAMPLE_FREQ(USBD_AUDIO_FREQ),   /* Audio sampling frequency coded on 3 bytes */
		/* 11 byte*/

		/* Endpoint 1 - Standard Descriptor */
		AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
		USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
		AUDIO_OUT_EP,                         /* bEndpointAddress 1 out endpoint */
		USBD_EP_TYPE_ISOC,                    /* bmAttributes */
		AUDIO_PACKET_SZE(USBD_AUDIO_FREQ),    /* wMaxPacketSize in Bytes (Freq(Samples)*2(Stereo)*2(HalfWord)) */
		AUDIO_FS_BINTERVAL,                   /* bInterval */
		0x00,                                 /* bRefresh */
		0x00,                                 /* bSynchAddress */
		/* 09 byte*/

		/* Endpoint - Audio Streaming Descriptor*/
		AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */
		AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
		AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
		0x00,                                 /* bmAttributes */
		0x00,                                 /* bLockDelayUnits */
		0x00,                                 /* wLockDelay */
		0x00,
		/* 07 byte*/

		/*HID descriptors */
		0x09,                                               /* bLength: Configuration Descriptor size */
		USB_DESC_TYPE_CONFIGURATION,                        /* bDescriptorType: Configuration */
		USB_HID_CONFIG_DESC_SIZ,
		/* wTotalLength: Bytes returned */
		0x00,
		0x01,                                               /* bNumInterfaces: 1 interface */
		0x01,                                               /* bConfigurationValue: Configuration value */
		0x00,                                               /* iConfiguration: Index of string descriptor describing the configuration */
		0xE0,                                               /* bmAttributes: bus powered and Support Remote Wake-up */
		0x32,                                               /* MaxPower 100 mA: this current is used for detecting Vbus */

		/************** Descriptor of Joystick Mouse interface ****************/
		/* 09 */
		0x09,                                               /* bLength: Interface Descriptor size */
		USB_DESC_TYPE_INTERFACE,                            /* bDescriptorType: Interface descriptor type */
		0x02,                                               /* bInterfaceNumber: Number of Interface */
		0x00,                                               /* bAlternateSetting: Alternate setting */
		0x01,                                               /* bNumEndpoints */
		0x03,                                               /* bInterfaceClass: HID */
		0x01,                                               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
		0x02,                                               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
		0,                                                  /* iInterface: Index of string descriptor */
		/******************** Descriptor of Joystick Mouse HID ********************/
		/* 18 */
		0x09,                                               /* bLength: HID Descriptor size */
		HID_DESCRIPTOR_TYPE,                                /* bDescriptorType: HID */
		0x11,                                               /* bcdHID: HID Class Spec release number */
		0x01,
		0x00,                                               /* bCountryCode: Hardware target country */
		0x01,                                               /* bNumDescriptors: Number of HID class descriptors to follow */
		0x22,                                               /* bDescriptorType */
		HID_MOUSE_REPORT_DESC_SIZE,                         /* wItemLength: Total length of Report descriptor */
		0x00,
		/******************** Descriptor of Mouse endpoint ********************/
		/* 27 */
		0x07,                                               /* bLength: Endpoint Descriptor size */
		USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType:*/

		HID_EPIN_ADDR,                                      /* bEndpointAddress: Endpoint Address (IN) */
		0x03,                                               /* bmAttributes: Interrupt endpoint */
		HID_EPIN_SIZE,                                      /* wMaxPacketSize: 4 Byte max */
		0x00,
		HID_FS_BINTERVAL,                                   /* bInterval: Polling Interval */
		/* 34 */
} ;


#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
/* USB Standard Device Descriptor */
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_AUDIOHID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup USBD_AUDIOHID_Private_Functions
  * @{
  */

/**
  * @brief  USBD_AUDIOHID_Init
  *         Initialize the AUDIOHID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_AUDIOHID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	SwitchPdevTo(pdev, Audio);
	uint8_t ret=0;
	ret = USBD_AUDIO.Init(pdev,cfgidx);
	//SwitchPdevTo(pdev, HID);
	//USBD_HID.Init(pdev,cfgidx);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_AUDIOHID_Init
  *         DeInitialize the AUDIOHID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_AUDIOHID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	SwitchPdevTo(pdev, Audio);
	USBD_AUDIO.DeInit(pdev,cfgidx);
	//	SwitchPdevTo(pdev, HID);
//		USBD_HID.DeInit(pdev,cfgidx);
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_AUDIOHID_Setup
  *         Handle the AUDIOHID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_AUDIOHID_Setup(USBD_HandleTypeDef *pdev,
                                   USBD_SetupReqTypedef *req)
{
  USBD_StatusTypeDef ret = USBD_OK;
 if (req->wIndex == 2)
 {
	// SwitchPdevTo(pdev, HID);
	//   	USBD_HID.Setup(pdev,req);
 }
 else
 {
	 SwitchPdevTo(pdev, Audio);
	   	ret = USBD_AUDIO.Setup(pdev,req);

 }
  	return ret;
}


/**
  * @brief  USBD_AUDIOHID_GetCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_AUDIOHID_GetCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_AUDIOHID_CfgDesc);
  return USBD_AUDIOHID_CfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_AUDIOHID_DeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_AUDIOHID_DeviceQualifierDesc);
  return USBD_AUDIOHID_DeviceQualifierDesc;
}


/**
  * @brief  USBD_AUDIOHID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIOHID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_AUDIOHID_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_AUDIOHID_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	SwitchPdevTo(pdev, Audio);
	USBD_AUDIO.EP0_RxReady(pdev);
  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_AUDIOHID_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_AUDIOHID_EP0_TxReady(USBD_HandleTypeDef *pdev)
{
	SwitchPdevTo(pdev, Audio);
	USBD_AUDIO.EP0_TxSent(pdev);
  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_AUDIOHID_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_AUDIOHID_SOF(USBD_HandleTypeDef *pdev)
{
	SwitchPdevTo(pdev, Audio);
		USBD_AUDIO.SOF(pdev);
  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_AUDIOHID_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIOHID_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_AUDIOHID_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIOHID_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_AUDIOHID_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_AUDIOHID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	SwitchPdevTo(pdev, Audio);
	USBD_AUDIO.DataOut(pdev, epnum);
  return (uint8_t)USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_AUDIOHID_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_AUDIOHID_DeviceQualifierDesc);

  return USBD_AUDIOHID_DeviceQualifierDesc;
}

void SwitchPdevTo(USBD_HandleTypeDef* pdev, IFace to)
{
	if (to==Audio)
	{
		//pdev->pClass=AudioClass;
		pdev->pClassData=AudioClassData;
		pdev->pUserData=AudioUserData;
	}
	if (to==HID)
	{
		//pdev->pClass=HIDClass;
		pdev->pClassData=HIDClassData;
		pdev->pUserData=HIDUserData;
	}
}
/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
