#include "player.h"

//extern I2S_HandleTypeDef hi2s2;

typedef int bool;

FATFS fs;
int playWavFile(const char *fname);

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
	printf("CardDetect : %d!\r\n", HAL_GPIO_ReadPin(CardDet_GPIO_Port, CardDet_Pin) );


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




