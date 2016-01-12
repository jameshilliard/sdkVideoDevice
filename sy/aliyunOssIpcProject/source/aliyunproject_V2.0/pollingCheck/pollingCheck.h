#ifndef POLLINGCHECK_H
#define POLLINGCHECK_H

#define IGNORE 0
#define DOWNLOAD_AUDIOFILE 1
#define START_EMERGENCYCONDITION 2
#define END_EMERGENCYCONDITION 3


typedef struct checkServerInfo
{
	int cmdType;
	int timeCycle;
	int motionTotal;
	int motionDiscTotal;
	int volumeAverage;
	int maxVideoRecordTime;
	int voiceFilePlayDelay;
	int voiceFileSize;
	char voiceFilePath[1024];
}CHECKSERVERINFO;

void *PollingcheckThread(void *param);


#endif

