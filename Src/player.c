#include "player.h"

extern I2S_HandleTypeDef hi2s2;

typedef int bool;

FATFS fs;

volatile bool end_of_file_reached = false;
volatile bool read_next_chunk = false;
volatile uint16_t *signal_play_buff = NULL;
volatile uint16_t *signal_read_buff = NULL;
volatile uint16_t signal_buff1[4096];
volatile uint16_t signal_buff2[4096];
volatile int adccnt = 0;

void init() {
	FRESULT res;
	printf("hello\n");
	printf("Ready!\r\n");
	printf("CardDetect : %d!\r\n",
			HAL_GPIO_ReadPin(CardDet_GPIO_Port, CardDet_Pin));

	// mount the default drive
	res = f_mount(&fs, "", 1);
	if (res != FR_OK) {
		printf("f_mount() failed, res = %d\r\n", res);
		return;
	}

	printf("f_mount() done!\r\n");

	uint32_t freeClust;
	FATFS *fs_ptr = &fs;
	// Warning! This fills fs.n_fatent and fs.csize!
	res = f_getfree("", &freeClust, &fs_ptr);
	if (res != FR_OK) {
		printf("f_getfree() failed, res = %d\r\n", res);
		return;
	}

	printf("f_getfree() done!\r\n");

	uint32_t totalBlocks = (fs.n_fatent - 2) * fs.csize;
	uint32_t freeBlocks = freeClust * fs.csize;

	printf("Total blocks: %lu (%lu Mb)\r\n", totalBlocks, totalBlocks / 2000);
	printf("Free blocks: %lu (%lu Mb)\r\n", freeBlocks, freeBlocks / 2000);

	DIR dir;
	res = f_opendir(&dir, "/");
	if (res != FR_OK) {
		printf("f_opendir() failed, res = %d\r\n", res);
		return;
	}

	FILINFO fileInfo;
	uint32_t totalFiles = 0;
	uint32_t totalDirs = 0;
	printf("--------\r\nRoot directory:\r\n");
	for (;;) {
		res = f_readdir(&dir, &fileInfo);
		if ((res != FR_OK) || (fileInfo.fname[0] == '\0')) {
			break;
		}

		if (fileInfo.fattrib & AM_DIR) {
			printf("  DIR  %s\r\n", fileInfo.fname);
			totalDirs++;
		} else {
			printf("  FILE %s\r\n", fileInfo.fname);
			totalFiles++;
		}
	}

	printf("(total: %lu dirs, %lu files)\r\n--------\r\n", totalDirs,
			totalFiles);

	res = f_closedir(&dir);
	if (res != FR_OK) {
		printf("f_closedir() failed, res = %d\r\n", res);
		return;
	}
	/*
	 printf("Writing to log.txt...\r\n");

	 char writeBuff[128];
	 snprintf(writeBuff, sizeof(writeBuff),
	 "Total blocks: %lu (%lu Mb); Free blocks: %lu (%lu Mb)\r\n",
	 totalBlocks, totalBlocks / 2000,
	 freeBlocks, freeBlocks / 2000);

	 FIL logFile;
	 res = f_open(&logFile, "log.txt", FA_OPEN_APPEND | FA_WRITE);
	 if(res != FR_OK) {
	 printf("f_open() failed, res = %d\r\n", res);
	 return;
	 }

	 unsigned int bytesToWrite = strlen(writeBuff);
	 unsigned int bytesWritten;
	 res = f_write(&logFile, writeBuff, bytesToWrite, &bytesWritten);
	 if(res != FR_OK) {
	 printf("f_write() failed, res = %d\r\n", res);
	 return;
	 }

	 if(bytesWritten < bytesToWrite) {
	 printf("WARNING! Disk is full.\r\n");
	 }

	 res = f_close(&logFile);
	 if(res != FR_OK) {
	 printf("f_close() failed, res = %d\r\n", res);
	 return;
	 }

	 printf("Reading file...\r\n");
	 FIL msgFile;
	 res = f_open(&msgFile, "log.txt", FA_READ);
	 if(res != FR_OK) {
	 printf("f_open() failed, res = %d\r\n", res);
	 return;
	 }

	 char readBuff[128];
	 unsigned int bytesRead;
	 res = f_read(&msgFile, readBuff, sizeof(readBuff)-1, &bytesRead);
	 if(res != FR_OK) {
	 printf("f_read() failed, res = %d\r\n", res);
	 return;
	 }

	 readBuff[bytesRead] = '\0';
	 printf("```\r\n%s\r\n```\r\n", readBuff);

	 res = f_close(&msgFile);
	 if(res != FR_OK) {
	 printf("f_close() failed, res = %d\r\n", res);
	 return;
	 }
	 */
	// Unmount
	/*    res = f_mount(NULL, "", 0);
	 if(res != FR_OK) {
	 printf("Unmount failed, res = %d\r\n", res);
	 return;
	 }
	 */
	printf("Done!\r\n");
}

void player() {

//	HAL_ADCEx_InjectedStart(&hadc1);
//	HAL_ADCEx_InjectedStart_IT(&hadc1);

//	printf("done\r");
	FIL wavfile;

	///
	FRESULT fr; /* Return value */
	DIR dj; /* Directory object */
	FILINFO fno; /* File information */

	fr = f_findfirst(&dj, &fno, "", "*.wav"); /* Start to search for photo files */
	printf("found %s\n", fno.fname);

	printf("Start loop\n");

	while (1) {

		playWavFile(fno.fname);

	}
	/*	while(1);
	 res = f_open(&wavfile, "kickstarter.wav", FA_READ);
	 if(res != FR_OK) {
	 printf("f_open() failed, res = %d\r\n", res);
	 return;
	 }
	 char readBuff[256];
	 unsigned int bytesRead;
	 do
	 {
	 res = f_read(&wavfile, readBuff, sizeof(readBuff), &bytesRead);
	 if(res != FR_OK) {
	 printf("f_read() failed, res = %d\r\n", res);
	 return;
	 }
	 HAL_I2S_Transmit(&hi2s2,(uint16_t*)readBuff,128,1000);
	 //HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t*)readBuff,2048);
	 } while(bytesRead>0);


	 while(1);
	 */
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (end_of_file_reached)
		return;

	volatile uint16_t *temp = signal_play_buff;
	signal_play_buff = signal_read_buff;
	signal_read_buff = temp;

	int nsamples = sizeof(signal_buff1) / sizeof(signal_buff1[0]);
	HAL_I2S_Transmit_IT(&hi2s2, (uint16_t*) signal_play_buff, nsamples);
	read_next_chunk = true;
}

int playWavFile(const char *fname) {
	printf("Openning %s...\r\n", fname);
	FIL file;
	FRESULT res = f_open(&file, fname, FA_READ);
	if (res != FR_OK) {
		printf("f_open() failed, res = %d\r\n", res);
		return -1;
	}

	printf("File opened, reading...\r\n");

	unsigned int bytesRead;
	uint8_t header[44];
	res = f_read(&file, header, sizeof(header), &bytesRead);
	if (res != FR_OK) {
		printf("f_read() failed, res = %d\r\n", res);
		f_close(&file);
		return -2;
	}

	if (memcmp((const char*) header, "RIFF", 4) != 0) {
		printf("Wrong WAV signature at offset 0: "
				"0x%02X 0x%02X 0x%02X 0x%02X\r\n", header[0], header[1],
				header[2], header[3]);
		f_close(&file);
		return -3;
	}

	if (memcmp((const char*) header + 8, "WAVEfmt ", 8) != 0) {
		printf("Wrong WAV signature at offset 8!\r\n");
		f_close(&file);
		return -4;
	}
	if (memcmp((const char*) header + 36, "data", 4) != 0) {
		printf("Wrong WAV signature at offset 36!\r\n");
		f_close(&file);
		return -5;
	}

	uint32_t fileSize = 8
			+ (header[4] | (header[5] << 8) | (header[6] << 16)
					| (header[7] << 24));
	uint32_t headerSizeLeft = header[16] | (header[17] << 8)
			| (header[18] << 16) | (header[19] << 24);
	uint16_t compression = header[20] | (header[21] << 8);
	uint16_t channelsNum = header[22] | (header[23] << 8);
	uint32_t sampleRate = header[24] | (header[25] << 8) | (header[26] << 16)
			| (header[27] << 24);
	uint32_t bytesPerSecond = header[28] | (header[29] << 8)
			| (header[30] << 16) | (header[31] << 24);
	uint16_t bytesPerSample = header[32] | (header[33] << 8);
	uint16_t bitsPerSamplePerChannel = header[34] | (header[35] << 8);
	uint32_t dataSize = header[40] | (header[41] << 8) | (header[42] << 16)
			| (header[43] << 24);

	printf("--- WAV header ---\r\n"
			"File size: %lu\r\n"
			"Header size left: %lu\r\n"
			"Compression (1 = no compression): %d\r\n"
			"Channels num: %d\r\n"
			"Sample rate: %ld\r\n"
			"Bytes per second: %ld\r\n"
			"Bytes per sample: %d\r\n"
			"Bits per sample per channel: %d\r\n"
			"Data size: %ld\r\n"
			"------------------\r\n", fileSize, headerSizeLeft, compression,
			channelsNum, sampleRate, bytesPerSecond, bytesPerSample,
			bitsPerSamplePerChannel, dataSize);

	if (headerSizeLeft != 16) {
		printf("Wrong `headerSizeLeft` value, 16 expected\r\n");
		f_close(&file);
		return -6;
	}

	if (compression != 1) {
		printf("Wrong `compression` value, 1 expected\r\n");
		f_close(&file);
		return -7;
	}

	if (channelsNum != 2) {
		printf("Wrong `channelsNum` value, 2 expected\r\n");
		f_close(&file);
		return -8;
	}

	if ((sampleRate != 48000) || (bytesPerSample != 4)
			|| (bitsPerSamplePerChannel != 16)
			|| (bytesPerSecond != 48000 * 2 * 2)
			|| (dataSize < sizeof(signal_buff1) + sizeof(signal_buff2))) {
		printf("Wrong file format, 16 bit file with sample "
				"rate 48000 expected\r\n");
		//   f_close(&file);
		//   return -9;
	}

	res = f_read(&file, (uint8_t*) signal_buff1, sizeof(signal_buff1),
			&bytesRead);
	if (res != FR_OK) {
		printf("f_read() failed, res = %d\r\n", res);
		f_close(&file);
		return -10;
	}

	res = f_read(&file, (uint8_t*) signal_buff2, sizeof(signal_buff2),
			&bytesRead);
	if (res != FR_OK) {
		printf("f_read() failed, res = %d\r\n", res);
		f_close(&file);
		return -11;
	}

	read_next_chunk = false;
	end_of_file_reached = false;
	signal_play_buff = signal_buff1;
	signal_read_buff = signal_buff2;

	HAL_StatusTypeDef hal_res;
	int nsamples = sizeof(signal_buff1) / sizeof(signal_buff1[0]);
	hal_res = HAL_I2S_Transmit_IT(&hi2s2, (uint16_t*) signal_buff1, nsamples);
	if (hal_res != HAL_OK) {
		printf("I2S - HAL_I2S_Transmit failed, "
				"hal_res = %d!\r\n", hal_res);
		f_close(&file);
		return -12;
	}



	while (dataSize >= sizeof(signal_buff1)) {

		if (!read_next_chunk) {
			continue;
		}

		read_next_chunk = false;

		res = f_read(&file, (uint8_t*) signal_read_buff, sizeof(signal_buff1),
				&bytesRead);

		if (res != FR_OK) {
			printf("f_read() failed, res = %d\r\n", res);
			f_close(&file);
			return -13;
		}

		dataSize -= sizeof(signal_buff1);
	}

	end_of_file_reached = true;

	res = f_close(&file);
	if (res != FR_OK) {
		printf("f_close() failed, res = %d\r\n", res);
		return -14;
	}

	return 0;
}
