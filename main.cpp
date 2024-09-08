#include "NTControl.h"
#include <random>

#define CHANNEL_ID 3 - 1

int main() {
  // 初始化
  NT_INDEX ntHandle;
  NT_STATUS result;
  const char* systemLocator = "usb:id:2250716012";
  const char* options = "sync";

  result = NT_OpenSystem(&ntHandle, systemLocator, options);

  // 生成随机控制变量
  const signed int StepsMin = -30000;
  const signed int StepsMax = 30000;
  const unsigned int AmplitudeMin = 100;
  const unsigned int AmplitudeMax = 4095;
  const unsigned int FrequencyMin = 1;
  const unsigned int FrequencyMax = 18500;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> StepsDis(StepsMin, StepsMax);
  std::uniform_int_distribution<> AmplitudeDis(AmplitudeMin, AmplitudeMax);
  std::uniform_int_distribution<> FrequencyDis(FrequencyMin, FrequencyMax);

  signed int steps = StepsDis(gen);
  unsigned int amplitude = AmplitudeDis(gen);
  unsigned int frequency = FrequencyDis(gen);

  // 运行电机
  NT_StepMove_S(ntHandle, CHANNEL_ID, steps, amplitude, frequency);

  // 获取陀螺仪数据

  // 写入 csv 文件

  NT_CloseSystem(ntHandle);
  return 0;
}