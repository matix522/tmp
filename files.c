#define PATH_SIZE 128
#define FILES 32
char fileNames[FILES][PATH_SIZE];
int listedFiles = 0;
const char* currentDir = "0:/";
int isAudioFile(char* file) {
	if (!file) return 0;
	int i = 0;

	while (file[i++])
		;
	int size = i - 2;
	i = 0;
	while (file[i]) {
		if (file[i] == '.') {
			if (i + 3 <= size) {
				return (file[i + 1] == 'w' && file[i + 2] == 'a' && file[i + 3] == 'v');
			}
		}
		i++;
	}
	return 0;
}

void getFiles() {
	DIR dir;
	FRESULT r = f_opendir(&dir, currentDir);
	if (r != FR_OK) {
		return;
	}
	FILINFO fno;
	int i = 0;
	while (f_readdir(&dir, &fno) == FR_OK) {
		if (fno->fname[0] == '\0') break;
		if (i >= FILES) break;
		if (isAudioFile(fno->fname)) {
			strcpy(fileNames[i], currentDir);
			strcat(fileNames[i++], fno.fname);
		}
	}
	listedFiles = i;
	f_closedir(&dir);
}

void openWaveFile(unsigned index, FIL* file) {
	if (index >= listedFiles) {
		file = NULL;
		return;
	}
	if (f_open(file, fileNames[index], FA_READ) == FR_OK) {
		xprintf("wave file %s opened\n", fileNames[index]);
	} else {
		xprintf("wave file %s ERROR\n", fileNames[index]);
	}
}

void showFiles(int startFrom) {
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(0x40FF00FF);

	for (int i = 0; i < 10; i++) {
		if (i + startFrom < listedFiles) {
			BSP_LCD_DisplayStringAt(0, i * 20 + 20, (unsigned char*)fileNames[startFrom + i], LEFT_MODE);
		}
	}
}
int currentVolume = 100;
int deltaVol = 1;

void playstate(int state) {
	if (state)
		BSP_AUDIO_OUT_Resume();
	else
		BSP_AUDIO_OUT_Pause();
}
void changeVolume(int delta) {
	currentVolume -= delta;
	if (currentVolume < 0) currentVolume = 0;
	if (currentVolume > 255) currentVolume = 255;
	BSP_AUDIO_OUT_SetVolume(currentVolume);
}

void play(unsigned index) {
	if (player_state) {
		BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
		f_close(&file);
	}
	openWaveFile(index, &file);

	player_state = 1;
	fpos = 0;
	buf_offs = BUFFER_OFFSET_NONE;
	BSP_AUDIO_OUT_Play((uint16_t*)&buff[0], AUDIO_OUT_BUFFER_SIZE);
}

void selectAction(unsigned x, unsigned y) {
	static unsigned fileOffset;
	if (x < 99999) { // TODO CALCULATE
		int r = (y / 20) - 1;
		if (r == -1) {
			showFiles(++fileOffset);
		} else if (r > 9 && fileOffset > 0) {
			showFiles(--fileOffset);
		} else {
			play(fileOffset + r);
		}
	}
	int pause = 0;		// TODO CALCUALTE
	int resume = 0;		// TODO CALCULATE
	int volumeUP = 0;   // TODO CALCULATE
	int vloumeDOWN = 0; // TODO CALCULATE
	if (pause) {
		playState(0);
	}
	if (resume) {
		playState(1);
	}
	if (pause) {
		changeVolume(deltaVol);
	}
	if (pause) {
		changeVolume(-deltaVol);
	}
}

// uint8_t BSP_AUDIO_OUT_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
// uint8_t BSP_AUDIO_OUT_Play(uint16_t* pBuffer, uint32_t Size);
// void    BSP_AUDIO_OUT_ChangeBuffer(uint16_t *pData, uint16_t Size);
// uint8_t BSP_AUDIO_OUT_Pause(void);
// uint8_t BSP_AUDIO_OUT_Resume(void);
// uint8_t BSP_AUDIO_OUT_Stop(uint32_t Option);
// uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume);
// void    BSP_AUDIO_OUT_SetFrequency(uint32_t AudioFreq);
// void    BSP_AUDIO_OUT_SetAudioFrameSlot(uint32_t AudioFrameSlot);
// uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd);
// uint8_t BSP_AUDIO_OUT_SetOutputMode(uint8_t Output);
// void    BSP_AUDIO_OUT_DeInit(void);