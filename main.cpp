#include "NTControl.h"
#include <iostream>
#include <random>

#include "Com.h"
#include <math.h>
#include <string.h>
#include <windows.h>

#include "wit_c_sdk.h"
#include <conio.h>
#include <stdio.h>

#include <ctime>
#include <fstream>  // 用于写入文件
#include <string>

#include <chrono>
#include <vector>

#define CHANNEL_ID 3 - 1

const signed int StepsMin = 0;  // 电机控制参数的最大和最小值
const signed int StepsMax = 10000;
const unsigned int AmplitudeMin = 1000;
const unsigned int AmplitudeMax = 4095;
const unsigned int FrequencyMin = 3000;
const unsigned int FrequencyMax = 8000;
const unsigned int RunCount = 20;  // 总运行次数
const unsigned int DelayTime =
    4000;  // 电机运行后延时的时间 ms, 正常运行范围 3 - 5s
unsigned int CurrentRunCount = 0;

static char s_cDataUpdate = 0;  // 通讯配置
int iComPort = 11;
int iBaud = 9600;
int iAddress = 0x50;
static float lastAngle = 0;
void ComRxCallBack(char* p_data, UINT32 uiSize) {
  for (UINT32 i = 0; i < uiSize; i++) {
    WitSerialDataIn(p_data[i]);
  }
}
static void AutoScanSensor(void);
static void SensorUartSend(uint8_t* p_data, uint32_t uiSize);
static void CopeSensorData(uint32_t uiReg, uint32_t uiRegNum);
static void DelayMs(uint16_t ms);

int main() {
  // 初始化
  NT_INDEX ntHandle;
  NT_STATUS result;
  const char* systemLocator = "usb:id:2250716012";
  const char* options = "sync";

  /// 启动粘滑驱动系统
  result = NT_OpenSystem(&ntHandle, systemLocator, options);

  /// 陀螺仪配置
  // float a[3], w[3], Angle[3], h[3];
  // printf("Please enter the gyros serial number:");
  // scanf_s("%d", &iComPort);

  OpenCOMDevice(iComPort, iBaud);
  WitInit(WIT_PROTOCOL_NORMAL, 0x50);
  WitSerialWriteRegister(SensorUartSend);
  WitRegisterCallBack(CopeSensorData);
  AutoScanSensor();

  /// 打开 csv 文件
  std::time_t now = std::time(nullptr);
  std::tm* localTime = std::localtime(&now);
  char timeStamp[12];
  std::strftime(timeStamp, sizeof(timeStamp), "%y%m%d%H%M", localTime);
  std::string fileName = "data_" + std::string(timeStamp) + ".csv";
  std::ofstream outputCsv;
  outputCsv.open(fileName);

  if (!outputCsv.is_open()) {
    std::cerr << "Error: Could not open file " << fileName << std::endl;
    return 1;
  }

  outputCsv << "Steps,Amplitude,Frequency,Angle,DeltaAngle\n";  // 写入首行

  // 配置随机数生成器

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> StepsDis(StepsMin, StepsMax);
  std::uniform_int_distribution<> AmplitudeDis(AmplitudeMin, AmplitudeMax);
  std::uniform_int_distribution<> FrequencyDis(FrequencyMin, FrequencyMax);

  // 读取初始角度
  Sleep(1000);
  float lastAngle = (float)sReg[Roll] / 32768.0f * 180.0f;

  // 主循环
  for (int i = 0; i < RunCount; i++) {
    // 生成随机控制变量
    // signed int steps = StepsDis(gen);
    // unsigned int amplitude = AmplitudeDis(gen);
    signed int steps = 3000;
    unsigned int amplitude = 4000;
    unsigned int frequency = FrequencyDis(gen);

    int dir = 1;
    for (int j = 0; j < 2; j++) {
      if (j == 0) {
        dir = 1;
      } else {
        dir = -1;
      }

      NT_StepMove_S(ntHandle, CHANNEL_ID, steps * dir, amplitude, frequency);
      DWORD runTime = (double)(1000 * steps) / (double)frequency;  // ms
      Sleep(runTime + DelayTime);

      // 获取陀螺仪数据
      float angle = -(float)sReg[Roll] / 32768.0f * 180.0f;
      float deltaAngle = angle - lastAngle;
      lastAngle = angle;
      // Sleep(1000);

      // 写入 csv 文件
      outputCsv << steps * dir << "," << amplitude << "," << frequency << ","
                << angle << "," << deltaAngle << std::endl;

      // 打印当前信息
      std::cout << "Current run count: " << i + 1 << " / " << RunCount
                << std::endl;
      std::cout << steps * dir << "," << amplitude << "," << frequency << ","
                << angle << "," << deltaAngle << std::endl;
    }
  }

  // 逆初始化
  WitDeInit();
  CloseCOMDevice();
  outputCsv.close();
  NT_CloseSystem(ntHandle);
  return 0;
}

static void DelayMs(uint16_t ms) {
  Sleep(ms);
}

static void SensorUartSend(uint8_t* p_data, uint32_t uiSize) {
  SendUARTMessageLength((const char*)p_data, uiSize);
}
static void CopeSensorData(uint32_t uiReg, uint32_t uiRegNum) {
  s_cDataUpdate = 1;
}

static void AutoScanSensor(void) {
  const uint32_t c_uiBaud[7] = {4800,  9600,   19200, 38400,
                                57600, 115200, 230400};
  int i, iRetry;

  for (i = 0; i < 7; i++) {
    CloseCOMDevice();
    OpenCOMDevice(iComPort, c_uiBaud[i]);
    // SetBaundrate(c_uiBaud[i]);
    iRetry = 2;
    do {
      s_cDataUpdate = 0;
      WitReadReg(AX, 3);
      Sleep(100);
      if (s_cDataUpdate != 0) {
        printf("%d baud find sensor\r\n\r\n", c_uiBaud[i]);
        return;
      }
      iRetry--;
    } while (iRetry);
  }
  printf("can not find sensor\r\n");
  printf("please check your connection\r\n");
}