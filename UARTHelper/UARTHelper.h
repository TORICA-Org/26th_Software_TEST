#pragma once

#include "Parameters.h"
#include <TORICA_ICS.h>
#include <TORICA_UART.h>

// `extern`宣言すれば`TransmitUART.h`をインクルードしたファイルで使えるようになる
extern SerialPIO SerialWireless();
extern SerialPIO SerialSD_ICS();
extern SerialPIO SerialCondition();

// 関数のプロトタイプ宣言
void initUART();
void transmitHeader();
void transmitLog(int);
