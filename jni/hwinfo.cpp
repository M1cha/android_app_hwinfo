#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "devinfo.h"

#define INVALID_PTN -1
#define PARTITION_ABOOT "/dev/block/platform/msm_sdcc.1/by-name/aboot"

int fsize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET);
    return sz;
}

unsigned char info_buf[4096];
void write_device_info_mmc(device_info *dev)
{
	struct device_info *info = (device_info*) info_buf;
	unsigned long long ptn = 0;
	unsigned long long size;
	int index = INVALID_PTN;

	// open aboot-partition
	FILE* aboot = fopen(PARTITION_ABOOT, "wb");
	if (aboot == NULL) {
		printf("Error opening aboot partition.\n");
		return;
	}

	size = fsize(aboot);

	memcpy(info, dev, sizeof(device_info));

	fseek(aboot, size - 512, SEEK_SET);
	if(fwrite((void *)info_buf, 1, 512, aboot)!=512) {
		printf("ERROR: Cannot write device info\n");
		return;
	}

	fclose(aboot);
}

void read_device_info_mmc(device_info *dev)
{
	struct device_info *info = (device_info*) info_buf;
	unsigned long long ptn = 0;
	unsigned long long size;
	int index = INVALID_PTN;

	// open aboot-partition
	FILE* aboot = fopen(PARTITION_ABOOT, "rb");
	if (aboot == NULL) {
		printf("Error opening aboot partition.\n");
		return;
	}

	size = fsize(aboot);

	fseek(aboot, size - 512, SEEK_SET);
	if(fgets((char *)info_buf, 512, aboot)==NULL) {
		printf("ERROR: Cannot read device info\n");
		return;
	}

	memcpy(dev, info, sizeof(device_info));

	fclose(aboot);
}

extern "C" void Java_com_xiaomi_hwinfo_MainActivity_getABootInfo(
		JNIEnv * env, jobject thiz, jobject jabootinfo) {

	// get object fields
	jclass clazzABootInfo = env->GetObjectClass(jabootinfo);
	jfieldID fieldID_magic = env->GetFieldID(clazzABootInfo, "magic", "Ljava/lang/String;");
	jfieldID fieldID_is_unlocked = env->GetFieldID(clazzABootInfo, "is_unlocked", "Z");
	jfieldID fieldID_is_tampered = env->GetFieldID(clazzABootInfo, "is_tampered", "Z");

	// read aboot data
	device_info device = {DEVICE_MAGIC, 0, 0};
	read_device_info_mmc(&device);

	// copy data to java object
	if(fieldID_magic!=NULL) env->SetObjectField(jabootinfo, fieldID_magic, env->NewStringUTF((char*)device.magic));
	if(fieldID_is_unlocked!=NULL) env->SetBooleanField(jabootinfo, fieldID_is_unlocked, device.is_unlocked);
	if(fieldID_is_tampered!=NULL) env->SetBooleanField(jabootinfo, fieldID_is_tampered, device.is_tampered);
}

typedef struct {
	unsigned int touch_info:4;
	unsigned int soc_info:4;
	unsigned int ddr_info:2;
	unsigned int emmc_info:2;
	unsigned int cpu_info:2;
	unsigned int pmic_info:2;
	unsigned int panel_info:2;
	unsigned int reserved_bit:14;
}HW_INFO;

extern "C" void Java_com_xiaomi_hwinfo_MainActivity_getHwInfo(
		JNIEnv * env, jobject thiz, jobject jhwinfo) {

	char buf[sizeof(HW_INFO)];
	HW_INFO hw_info;

	// get object fields
	jclass clazzHWInfo = env->GetObjectClass(jhwinfo);
	jfieldID fieldID_touch_info = env->GetFieldID(clazzHWInfo, "touch_info", "I");
	jfieldID fieldID_soc_info = env->GetFieldID(clazzHWInfo, "soc_info", "I");
	jfieldID fieldID_ddr_info = env->GetFieldID(clazzHWInfo, "ddr_info", "I");
	jfieldID fieldID_emmc_info = env->GetFieldID(clazzHWInfo, "emmc_info", "I");
	jfieldID fieldID_cpu_info = env->GetFieldID(clazzHWInfo, "cpu_info", "I");
	jfieldID fieldID_pmic_info = env->GetFieldID(clazzHWInfo, "pmic_info", "I");
	jfieldID fieldID_panel_info = env->GetFieldID(clazzHWInfo, "panel_info", "I");
	jfieldID fieldID_reserved_bit = env->GetFieldID(clazzHWInfo, "reserved_bit", "I");

	// open hwinfo node
	FILE* fp = fopen("/proc/hwinfo", "rb");
	if (fp == NULL) {
		printf("Error opening hwinfo node.\n");
		return;
	}

	// read data
	if (fgets(buf, sizeof(HW_INFO), fp) == NULL) {
		printf("Error reading data from hwinfo partition.\n");
		return;
	}

	// close
	fclose(fp);

	// copy data to struct
	*(unsigned int*)&hw_info = (unsigned int)*buf;

	// copy data to java object
	if(fieldID_touch_info!=NULL) env->SetIntField(jhwinfo, fieldID_touch_info, hw_info.touch_info);
	if(fieldID_soc_info!=NULL) env->SetIntField(jhwinfo, fieldID_soc_info, hw_info.soc_info);
	if(fieldID_ddr_info!=NULL) env->SetIntField(jhwinfo, fieldID_ddr_info, hw_info.ddr_info);
	if(fieldID_emmc_info!=NULL) env->SetIntField(jhwinfo, fieldID_emmc_info, hw_info.emmc_info);
	if(fieldID_cpu_info!=NULL) env->SetIntField(jhwinfo, fieldID_cpu_info, hw_info.cpu_info);
	if(fieldID_pmic_info!=NULL) env->SetIntField(jhwinfo, fieldID_pmic_info, hw_info.pmic_info);
	if(fieldID_panel_info!=NULL) env->SetIntField(jhwinfo, fieldID_panel_info, hw_info.panel_info);
	if(fieldID_reserved_bit!=NULL) env->SetIntField(jhwinfo, fieldID_reserved_bit, hw_info.reserved_bit);
}
