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

	if (fno.fsize < 1) {
		printf("file not found %s\n", fno.fname);
		HAL_Delay(1000);
	} else {
		printf("found %s\n", fno.fname);

		printf("Start loop\n");

		while (1) {

			playWavFile(fno.fname);

		}
	}

}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (end_of_file_reached) {
		return;
	}

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

	//-------------------------Read RIFF Header-------------------------
	unsigned int bytesRead;
	uint8_t riff_header[12];
	res = f_read(&file, riff_header, sizeof(riff_header), &bytesRead);
	if (res != FR_OK) {
		printf("f_read() failed, res = %d\r\n", res);
		//f_close(&file);
		//return -2;
	}

	if (memcmp((const char*) riff_header, "RIFF", 4) != 0) {
		printf("Wrong WAV signature at offset 0: "
				"0x%02X 0x%02X 0x%02X 0x%02X\r\n", riff_header[0],
				riff_header[1], riff_header[2], riff_header[3]);
		//f_close(&file);
		//return -3;
	}
	uint32_t fileSize = 8
			+ (riff_header[4] | (riff_header[5] << 8) | (riff_header[6] << 16)
					| (riff_header[7] << 24));

	//-------------------------Read fmt Header-------------------------
	uint8_t fmt_header[24];
	res = f_read(&file, fmt_header, sizeof(fmt_header), &bytesRead);
	if (res != FR_OK) {
		printf("f_read() failed, res = %d\r\n", res);
		//f_close(&file);
		//return -2;
	}

	if (memcmp((const char*) fmt_header, "fmt ", 4) != 0) {
		printf("Wrong WAV signature at offset 8!\r\n");
		//f_close(&file);
		//return -4;
	}

	uint32_t headerSizeLeft = fmt_header[4] | (fmt_header[5] << 8)
			| (fmt_header[6] << 16) | (fmt_header[7] << 24);
	uint16_t compression = fmt_header[8] | (fmt_header[9] << 8);
	uint16_t channelsNum = fmt_header[10] | (fmt_header[11] << 8);
	uint32_t sampleRate = fmt_header[12] | (fmt_header[13] << 8)
			| (fmt_header[14] << 16) | (fmt_header[15] << 24);
	uint32_t bytesPerSecond = fmt_header[16] | (fmt_header[17] << 8)
			| (fmt_header[18] << 16) | (fmt_header[19] << 24);
	uint16_t bytesPerSample = fmt_header[22] | (fmt_header[23] << 8);

	//-------------------------finde and Read data Header-------------------------
	uint8_t data_header[8];
	uint32_t dataSize;

	for (int i = 1; i < 6; i++) {
		res = f_read(&file, data_header, sizeof(data_header), &bytesRead);
		if (res != FR_OK) {
			printf("f_read() failed, res = %d\r\n", res);
			//f_close(&file);
			//return -2;
		}

		dataSize = data_header[4] | (data_header[5] << 8)
				| (data_header[6] << 16) | (data_header[7] << 24);

		if (memcmp((const char*) data_header, "data", 4) != 0) {
			f_read(&file, data_header, dataSize, &bytesRead);
			//dataSize = fileSize - 8 - 24 - (8 * i);
		} else {
			break;

		}
	}

	printf("--- WAV header ---\r\n"
			"File size: %lu\r\n"
			"Header size left: %lu\r\n"
			"Compression (1 = no compression): %d\r\n"
			"Channels num: %d\r\n"
			"Sample rate: %ld\r\n"
			"Bytes per second: %ld\r\n"
			"Bytes per sample: %d\r\n"
			"Data size: %ld\r\n"
			"------------------\r\n", fileSize, headerSizeLeft, compression,
			channelsNum, sampleRate, bytesPerSecond, bytesPerSample, dataSize);

	if (headerSizeLeft != 16) {
		printf("Wrong `headerSizeLeft` value, 16 expected\r\n");
		//f_close(&file);
		//return -6;
	}

	if (compression != 1) {
		printf("Wrong `compression` value, 1 expected\r\n");
		//f_close(&file);
		//return -7;
	}

	if (channelsNum != 2) {
		printf("Wrong `channelsNum` value, 2 expected\r\n");
		//f_close(&file);
		//return -8;
	}

	if ((sampleRate != 48000) || (bytesPerSample != 16)
			|| (bytesPerSecond != 48000 * 2 * 2)
			|| (dataSize < sizeof(signal_buff1) + sizeof(signal_buff2))) {
		char tmp[] =
				"Wrong file format! 16 bytesPerSample, stereo, with sample rate 48000Hz expected\r\n";

		printf(tmp);
		writeErrorTxt(tmp, strlen(tmp));
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

void writeErrorTxt(char *buff, int len) {

	FRESULT fr;
	FIL errFile;
	fr = f_open(&errFile, "error.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if (fr != FR_OK) {
		printf("f_open() failed, res = %d\r\n", fr);
		return;
	}

	char writeBuff[len];
	snprintf(writeBuff, sizeof(writeBuff), buff);
	unsigned int bytesWritten;
	fr = f_write(&errFile, writeBuff, strlen(writeBuff), &bytesWritten);
	if (fr != FR_OK) {
		printf("f_write() failed, res = %d\r\n", fr);
		return;
	}

	fr = f_close(&errFile);
	if (fr != FR_OK) {
		printf("f_close() failed, res = %d\r\n", fr);
		return;
	}

}
