// 取得函式庫
#include <Wire.h>  //  for DS1307
#include <Time.h>
#include <DS1307RTC.h>
#include "Wire.h"  //for LCM
#include "OLedI2C_Wire.h"
OLedI2C LCD( 0x3c, 16, 2 );
#include <NewTone.h> // new Tone library
tmElements_t tm; //tm使用DS1307程式碼

// 全域變數
bool oldKey[4] = {0, 0, 0, 0}; // 判斷按鈕是否壓住
const byte key[4] = { 9, 8, 7, 6 }; // 對應接腳
bool blueToothAck = 0; // 開啟藍牙
bool heartAck = 0; //  開啟心跳檢測
unsigned int heartData = 0; //  儲存心跳
#define bluetoothVccPin 5  //  藍牙電源腳
#define motorPin 2  //  馬達腳
#define speakPin 3  //  蜂鳴器腳
#define heartVccPin 4  //  心跳電源腳
bool phoneComing = 0; // 來電旗標
String getData;  // 取得藍芽訊息用
bool heartWaitTime = 0; // 延遲傳送心跳
bool Die=0; // 當沒檢測到心跳時 變為1
unsigned long heartHigh=0; // 儲存心跳高峰時間
#define heartHighTime 1000 // 心臟高峰達該值 代表沒檢測到心跳
const bool patchKey=10;

// 心跳變數
#define pulsePin 0                 // Pulse Sensor purple wire connected to analog pin 0
#define blinkPin 13                // pin to blink led at each beat
#define fadePin 5                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// 定義
#define dotFlashSpeed 500
#define flashSpeed  200
#define KeyUpSpeed  100
#define WaitKeyUpSpeed  500
#define maxYears 2100
#define minYears 1995
#define maxMonths 12
#define minMonths 1
#define maxDays 31
#define minDays 1
#define maxHours 23
#define minHours 0
#define maxMins 59
#define minMins 0
#define maxSecs 59
#define minSecs 0

const byte wordData[3][8] = { 0b00000100, 0b00010110, 0b00001101, 0b00000110, //bluetooth
                              0b00001101, 0b00010110, 0b00000100, 0b00000000,
                              0b00000000, 0b00001110, 0b00011011, 0b00010001, //alarm
                              0b00010001, 0b00011111, 0b00000100, 0b00000000,
                              0b00010101, 0b00010101, 0b00010101, 0b00010101, //point
                              0b00010101, 0b00010101, 0b00010101, 0b00010101,
                              /*0b00000010, 0b00000100, 0b00001000, 0b00010000,  //potter point
                              0b00011111, 0b00000001, 0b00000010, 0b00000100*/
                            };

void setup()
{
  Serial.begin(9600);
  // 設定輸出入
  for ( byte i = 0; i <= 3; i++ )
  {
    pinMode( key[i], INPUT );// 輸出模式
    digitalWrite( key[i], HIGH );// 提升電阻
  }
  
  // LCM init
  Wire.begin();
  LCD.init();

  // wrCGRAM
  //setCGRAMCode(uint8_t, uint8_t, uint8_t);  wordData[3][8]
  for ( byte i = 1; i <= 3; i++ )
  {
    for ( byte x = 0; x < 8; x++ )
    {
      LCD.setCGRAMCode( wordData[i - 1][x], i, x );
    }
  }
  LCD.wrCGRAM();

  pinMode(blinkPin, OUTPUT);        // pin that will blink to your heartbeat!
  pinMode(fadePin, OUTPUT);         // pin that will fade to your heartbeat!
  //Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
  // UN-COMMENT THE NEXT LINE IF YOU ARE POWERING The Pulse Sensor AT LOW VOLTAGE,
  // AND APPLY THAT VOLTAGE TO THE A-REF PIN
  //analogReference(EXTERNAL);

  //pinMode( bluetoothVccPin, OUTPUT );
  pinMode( bluetoothVccPin, INPUT );
  pinMode( motorPin, OUTPUT );
  pinMode( heartVccPin, OUTPUT );
  //digitalWrite( bluetoothVccPin, LOW );
  digitalWrite( heartVccPin, LOW );
  
  pinMode( patchKey,INPUT);
  digitalWrite( patchKey, HIGH );// 提升電阻
}

void judgeACK()
{
  alarmClockFunction( 0 );
  countDownFunction( 0 );
}

void loop()
{
  tm.Year = CalendarYrToTm(2014);
  tm.Month = 12;
  tm.Day = 31;
  tm.Hour = 0;
  tm.Minute = 0;
  tm.Second = 0;
  RTC.write(tm);

  /*
  getTimeFunction();
  //countDownFunction( 1 );
  //alarmClockFunction( 1 );
  for(;;)
  {
    //heartFunction();
    //showTimeFunction();
    static bool x=0;
    if(x==0)
    countDownFunction( 1 );
    else
    countDownFunction( 0 );
    if(keyInputFunction(1)==1)
    {
      x=1;
    }
    //stopWatchFunction();
    //alarmClockFunction( 1 );
  }
  error:
  for(;;);*/
  unsigned long oldTime;
  bool cleared = 0; // 顯示功能之畫面清除
  bool cleared1 = 0; // 顯示副程式之前之畫面清除
  getTimeFunction();
  for (;;)
  {
    int i;
    for ( i = 0; i <= 3; i++ )
    {
      oldTime = millis();
      cleared = 0;
      cleared1 = 0;
      switch ( i )
      {
        case 0:
          for (;;)
          {
            if ( getMillisDifference( oldTime ) >= 500 )
            {
              // show program
              if ( cleared1 == 0 )
              {
                LCD.clear();
                cleared1 = 1;
              }
              showTimeFunction();
            } else
            {
              // show name
              if ( cleared == 0 )
              {
                LCD.clear();
                LCD.sendString( "Time", 0, 0 );
                LCD.clear();
                cleared = 1;
              }
              LCD.sendString( "Time", 0, 0 );
            }

            if ( keyInputFunction( 1 ) == 1 )
            {
              break;
            }

            judgeACK();
            heartFunction();
            phoneComingFunction();
            bluetoothFunction();
          }
          cleared1 = 0;
          break;
        case 1:
          for (;;)
          {
            if ( getMillisDifference( oldTime ) >= 500 )
            {
              // show program
              if ( cleared1 == 0 )
              {
                LCD.clear();
                cleared1 = 1;
              }

              if ( alarmClockFunction( 1 ) == 1 )
              {
                i = 4;
              }
            } else
            {
              // show name
              if ( cleared == 0 )
              {
                LCD.clear();
                LCD.sendString( "AlarmClock", 0, 0 );
                LCD.clear();
                cleared = 1;
              }
              LCD.sendString( "AlarmClock", 0, 0 );
            }

            if ( keyInputFunction( 1 ) == 1 )
            {
              break;
            }

            judgeACK();
            heartFunction();
            phoneComingFunction();
            bluetoothFunction();
          }
          cleared1 = 0;
          break;
        case 2:
          for (;;)
          {
            if ( getMillisDifference( oldTime ) >= 500 )
            {
              // show program
              if ( cleared1 == 0 )
              {
                LCD.clear();
                cleared1 = 1;
              }

              if ( countDownFunction( 1 ) == 1 )
              {
                i = 4;
              }
            } else
            {
              // show name
              if ( cleared == 0 )
              {
                LCD.clear();
                LCD.sendString( "CountDown", 0, 0 );
                LCD.clear();
                cleared = 1;
              }
              LCD.sendString( "CountDown", 0, 0 );
            }

            if ( keyInputFunction( 1 ) == 1 )
            {
              LCD.noDoubleHeightFont();
              break;
            }

            judgeACK();
            heartFunction();
            phoneComingFunction();
            bluetoothFunction();
          }
          cleared1 = 0;
          break;
        case 3:
          for (;;)
          {
            if ( getMillisDifference( oldTime ) >= 500 )
            {
              // show program
              if ( cleared1 == 0 )
              {
                LCD.clear();
                cleared1 = 1;
              }

              if ( stopWatchFunction() == 1 )
              {
                i = 4;
              }
            } else
            {
              // show name
              if ( cleared == 0 )
              {
                LCD.clear();
                LCD.sendString( "StopWatch", 0, 0 );
                LCD.clear();
                cleared = 1;
              }
              LCD.sendString( "StopWatch", 0, 0 );
            }

            if ( keyInputFunction( 1 ) == 1 )
            {
              break;
            }

            judgeACK();
            heartFunction();
            phoneComingFunction();
            bluetoothFunction();
          }
      }
    }
  }
}

byte keyInputFunction( byte inputKey )// 按鍵輸入 包含彈跳按鈕 0無按下 1按下 2按住
{
  if ( digitalRead( key[inputKey] ) == 0 ) // 壓下時
  {
    delay(50);
    if ( digitalRead( key[inputKey] ) == 0 )
    {
      if ( oldKey[inputKey] == 1 )
      {
        return 2;
      }
      oldKey[inputKey] = 1;
      return 1;
    }
  }
  oldKey[inputKey] = 0;
  return 0;
}

void LCMClear( byte blocks, byte a, byte b )// 清除畫面的隔數blocks,行a,列b
{
  for ( byte i = 0; i < blocks; i++ )
  {
    LCD.sendString( " ", a + i, b );
  }
}

unsigned long getMillisDifference( unsigned long oldtime )// 輸入過去的時間 回傳時間差
{
  unsigned long localtime = millis();
  if ( localtime < oldtime )
  {
    unsigned long num = 4294967295 - oldtime; // 補丁，修正溢位
    num++;
    num += localtime;
    return num;
  }
  return localtime - oldtime;
}

void sendStringFixZero( int num, byte sizes, byte x, byte y, bool showZero )
{
  char output[sizes + 1];
  itoa( num, output, 10 );
  if ( num < 10 )
  {
    if ( showZero == 1 )
    {
      LCD.sendString( "0", x, y );
    }
    LCD.sendString( output, x + 1, y );
  } else
  {
    LCD.sendString( output, x, y );
  }
}

int getTimeFunctionInside( int num, byte x, byte y, byte sizes, int minNum, int maxNum, bool showZero ) //  取得時間模組化，if 設定完成，回傳值+10000
{
  unsigned long oldTime = millis();
  unsigned long oldTime1 = 0;
  bool setTimeComplete = 0;
  bool flashNum = 0;
  do
  {
    if ( getMillisDifference( oldTime ) >= flashSpeed ) // 取得時間差並比較，影響閃爍速度
    {
      flashNum = !flashNum;
      oldTime = millis();
    }

    // 閃爍處理
    if ( flashNum == 0 )
    {
      sendStringFixZero( num, sizes, x, y, showZero );
    } else
    {
      LCMClear( sizes, x, y );
    }

    // 處理輸入
    if ( keyInputFunction( 0 ) == 1 ) // key0
    {
      setTimeComplete = 1;
      num += 10000;
    }
    if ( keyInputFunction( 1 ) == 1 ) // key1
    {
      setTimeComplete = 1;
    }
    if ( num < 10000 )
    {
      switch ( keyInputFunction( 2 ) ) // key2
      {
        case 1:
          oldTime1 = millis();
          if ( num < maxNum )
          {
            num++;
          }
          break;
        case 2:
          if ( getMillisDifference( oldTime1 ) >= WaitKeyUpSpeed )
          {
            if ( num < maxNum )
            {
              num++;
            }
            oldTime1 = millis();
            while ( keyInputFunction( 2 ) == 2 )
            {
              if ( getMillisDifference( oldTime1 ) >= KeyUpSpeed )
              {
                sendStringFixZero( num, sizes, x, y, showZero );
                if ( num < maxNum )
                {
                  num++;
                  oldTime1 = millis();
                }
              }
            }
          }
          break;
      }
      switch ( keyInputFunction( 3 ) ) // key3
      {
        case 1:
          oldTime1 = millis();
          if ( num > minNum )
          {
            num--;
          }
          break;
        case 2:
          if ( getMillisDifference( oldTime1 ) >= WaitKeyUpSpeed )
          {
            if ( num > minNum )
            {
              num--;
            }
            oldTime1 = millis();
            while ( keyInputFunction( 3 ) == 2 )
            {
              if ( getMillisDifference( oldTime1 ) >= KeyUpSpeed  )
              {
                sendStringFixZero( num, sizes, x, y, showZero );
                if ( num > minNum )
                {
                  num--;
                  oldTime1 = millis();
                }
              }
            }
          }
          break;
      }
    }
  } while ( setTimeComplete == 0 );
  char output[sizes + 1]; // 重新顯示，避免閃爍熄滅BUG
  int num1;
  if ( num >= 10000 )
  {
    num1 = num - 10000;
  } else
  {
    num1 = num;
  }
  sendStringFixZero( num1, sizes, x, y, showZero );
  return num;
}

bool judgeLeapYear( int years, byte months, byte days )// 輸入年月日，輸出1代表閏年輸入錯誤、小月卻輸入31日
{
  bool judge = 0; /* 判斷重複 */
  bool specialYear = 0;
  if (  ( years % 4 == 0 ) && ( years % 100 != 0 ) ) //  判斷閏年
  {
    specialYear = 1;
  }
  if ( years % 400 == 0 )
  {
    specialYear |= 1;
  }
  if ( specialYear == 0 )
  {
    // 平年
    if ( months == 2 )
    {
      if ( days >= 29 )
      {
        judge = 1;
      }
    }
  } else
  {
    // 是閏年 //
    if ( months == 2 )
    {
      if ( days > 29 )
      {
        judge = 1;
      }
    }
  }

  if ( months < 8 ) //判斷小月輸入31日
  {
    if ( months % 2 == 0 )
    {
      if ( days == 31 )
      {
        judge |= 1;
      }
    }
  } else
  {
    if ( months % 2 )
    {
      if ( days == 31 )
      {
        judge |= 1;
      }
    }
  }
  return judge;
}

void getTimeFunction()
{
  LCD.clear();
  //2014/12/31 00 00 00
  RTC.read(tm);
  int years = tmYearToCalendar(tm.Year);
  byte months = tm.Month;
  byte days = tm.Day;
  byte hours = tm.Hour;
  byte mins = tm.Minute;
  byte secs = tm.Second;
  //  年座標 2,0 ~ 5,0
  //  / 6,0
  //  月座標 8,0 ~ 9,0
  //  / 10,0
  //  日座標 12,0 ~ 13,0
  //  ( 13,0   //不用顯示
  //  日期 14,0
  //  ) 15,0
  //  小時座標 3,1 ~ 4.1
  //  : 6.1
  //  分鐘座標 8.1 ~ 9.1
  //  : 11.1
  //  秒座標 13.1 ~ 14.1

  //  顯示範例時間
  sendStringFixZero( years, 4, 2, 0, 0 );
  sendStringFixZero( months, 2, 8, 0, 1 );
  sendStringFixZero( days, 2, 12, 0, 1 );
  sendStringFixZero( hours, 2, 3, 1, 0 );
  sendStringFixZero( mins, 2, 8, 1, 1 );
  sendStringFixZero( secs, 2, 13, 1, 1 );
  LCD.sendString( "/", 6, 0 );
  LCD.sendString( "/", 10, 0 );
  LCD.sendString( ":", 6, 1 );
  LCD.sendString( ":", 11, 1 );


  for (;;)
  {
    for (;;)
    {
      int temp;
      temp = getTimeFunctionInside( years, 2, 0, 4, minYears, maxYears, 0 );
      if ( temp >= 10000 )
      {
        years = temp - 10000;
        break;
      }
      years = temp;
      temp = getTimeFunctionInside( months, 8, 0, 2, minMonths, maxMonths, 1 );
      if ( temp >= 10000 )
      {
        months = temp - 10000;
        break;
      }
      months = temp;
      temp = getTimeFunctionInside( days, 12, 0, 2, minDays, maxDays, 1 );
      if ( temp >= 10000 )
      {
        days = temp - 10000;
        break;
      }
      days = temp;
      temp = getTimeFunctionInside( hours, 3, 1, 2, minHours, maxHours, 0 );
      if ( temp >= 10000 )
      {
        hours = temp - 10000;
        break;
      }
      hours = temp;
      temp = getTimeFunctionInside( mins, 8, 1, 2, minMins, maxMins, 1 );
      if ( temp >= 10000 )
      {
        mins = temp - 10000;
        break;
      }
      mins = temp;
      temp = getTimeFunctionInside( secs, 13, 1, 2, minSecs, maxSecs, 1 );
      if ( temp >= 10000 )
      {
        secs = temp - 10000;
        break;
      }
      secs = temp;
    }
    if ( judgeLeapYear( years, months, days ) == 0 )
    {
      break;
    } else
    {
      LCD.sendString("Error Time ! !", 1, 0 ); /* 顯示輸入錯誤時間之訊息 */
      LCD.sendString("Please re-enter ! !", 2, 1 );
      delay(5000);
      LCD.clear();
      sendStringFixZero( years, 4, 2, 0, 0 );
      sendStringFixZero( months, 2, 8, 0, 1 );
      sendStringFixZero( days, 2, 12, 0, 1 );
      sendStringFixZero( hours, 2, 3, 1, 0 );
      sendStringFixZero( mins, 2, 8, 1, 1 );
      sendStringFixZero( secs, 2, 13, 1, 1 );
      LCD.sendString( "/", 6, 0 );
      LCD.sendString( "/", 10, 0 );
      LCD.sendString( ":", 6, 1 );
      LCD.sendString( ":", 11, 1 );
    }
  }

  /*寫入至DS01307*/
  tm.Year = CalendarYrToTm(years);
  tm.Month = months;
  tm.Day = days;
  tm.Hour = hours;
  tm.Minute = mins;
  tm.Second = secs;
  RTC.write(tm);

  LCD.clear();
  LCD.sendString("Set Complete !", 1, 0);
  delay(1000);
  LCD.clear();
}

//  輸入時間完畢

void sendTorF( bool input, byte x, byte y )
{
  if ( input == 0 )
  {
    LCD.sendString( "OFF", x, y );
  } else
  {
    LCD.sendString( " ", x + 2, y );
    LCD.sendString( "ON", x, y );
  }
}

void showHeartData()
{
  heartPatch();
  char output[4];
  itoa( heartData, output, 10 );
  byte i = 0;
  if ( heartData < 10 )
  {
    i = 2;
    LCD.sendString( "  ", 1, 1 );
  } else
  {
    if ( heartData < 100 )
    {
      i = 1;
      LCD.sendString( " ", 1, 1 );
    }
  }
  LCD.sendString( output, 1 + i, 1 );
}

void showTimeFunction()
{
  RTC.read(tm);
  char temp[5];
  int years = tmYearToCalendar(tm.Year);
  byte months = tm.Month;
  byte days = tm.Day;
  byte hours = tm.Hour;
  byte mins = tm.Minute;
  byte secs = tm.Second;
  sendStringFixZero( years, 4, 5, 0, 0 );
  sendStringFixZero( months, 2, 10, 0, 1 );
  sendStringFixZero( days, 2, 13, 0, 1 );
  sendStringFixZero( hours, 2, 6, 1, 0 );
  sendStringFixZero( mins, 2, 9, 1, 1 );
  sendStringFixZero( secs, 2, 12, 1, 1 );
  LCD.sendString( "/", 9, 0 );
  LCD.sendString( "/", 12, 0 );
  LCD.sendSpecialStringCode( 0x00, 0, 0 ); // bluetooth
  LCD.sendSpecialStringCode( 0x93, 0, 1 ); // 愛心符號
  sendTorF( blueToothAck, 1, 0 );

  if ( blueToothAck == 0 )
  {
    //digitalWrite( bluetoothVccPin, LOW );
  } else
  {
    //digitalWrite( bluetoothVccPin, HIGH );
  }

  if ( heartAck == 0 )
  {
    sendTorF( heartAck, 1, 1 );
    digitalWrite( heartVccPin, LOW );
  } else
  {
    digitalWrite( heartVccPin, HIGH );
    //sendTorF( heartAck, 1, 1 );
    showHeartData();
  }

  static unsigned long oldTime = millis();
  static bool flashDot = 0;
  if ( getMillisDifference( oldTime ) >= dotFlashSpeed  )
  {
    flashDot = !flashDot;
    if ( flashDot == 0 )
    {
      LCD.sendString( ":", 8, 1 );
      LCD.sendString( ":", 11, 1 );
    } else
    {
      LCD.sendString( " ", 8, 1 );
      LCD.sendString( " ", 11, 1 );
    }
    oldTime = millis();
  }

  if (keyInputFunction( 0 ) == 1 )
  {
    getTimeFunction();
  }

  if (keyInputFunction( 2 ) == 1 )
  {
    blueToothAck = !blueToothAck;
  }

  if (keyInputFunction( 3 ) == 1 )
  {
    heartAck = !heartAck;
  }
}

//  顯示時間完畢

unsigned long getMillisDifferencePatch( unsigned long wantTimes )// 輸入"未來"的時間 回傳時間差
{
  unsigned long localtime = millis();
  if ( localtime > wantTimes )
  {
    unsigned long num = 4294967295 - localtime; // 補丁，修正溢位
    num++;
    num += wantTimes;
    return num;
  }
  return wantTimes - localtime;
}

bool waitAllKeyDown()   // 0 => no key down 1 => key down
{
  bool keyState = 0;
  if (keyInputFunction( 0 ) == 1 )
    keyState |= 1;
  if (keyInputFunction( 1 ) == 1 )
    keyState |= 1;
  if (keyInputFunction( 2 ) == 1 )
    keyState |= 1;
  if (keyInputFunction( 3 ) == 1 )
    keyState |= 1;
  return keyState;
}

void countDownIsOut()
{
  LCD.noDoubleHeightFont();
  LCD.clear();
  LCD.sendString( "Countdown", 4 , 0 );
  LCD.sendString( "Time's UP!", 4 , 1 );
  byte num = 0;
  unsigned long oldTime1 = millis();
  unsigned long oldTime2;
  for (;;)
  {
    if ( waitAllKeyDown() == 1 )
    {
      noNewTone( speakPin );
      digitalWrite( motorPin, LOW );
      break;
    }

    if ( getMillisDifference( oldTime1 ) >= 250  )
    {
      oldTime1 = millis();

      if ( num != 6 )
      {
        num++;
        if ( num % 2 == 1 )
        {
          NewTone( speakPin, 988 );
          digitalWrite( motorPin, HIGH );
        } else
        {
          noNewTone( speakPin );
          digitalWrite( motorPin, LOW );
        }
        oldTime2 = millis();
      } else
      {
        if ( getMillisDifference( oldTime2 ) >= 2500  )
        {
          num = 0;
        }
      }
    }
  }
  LCD.clear();
}

bool countDownFunction( bool showProgram )
{
  bool returnACK = 0;
  static unsigned long lastSetTime = 0;
  static unsigned long countDownTime = 0;
  static bool start = 0;
  static bool seted = 0;
  static unsigned long oldTime1 = 1;
  if ( showProgram == 1 )
  {
    oldTime1 = millis();
    LCD.doubleHeightFont();
    unsigned long showTimes = countDownTime;
    unsigned long showHours;
    unsigned long showMins;
    unsigned long showSecs;
    unsigned long showMsecs;
    showHours = ((showTimes / 1000) / 60);
    showHours /= 60;
    showMins = ((showTimes / 1000) - (showHours * 60 * 60)) / 60;
    showSecs = (showTimes / 1000) - (showHours * 60 * 60) - (showMins * 60);
    sendStringFixZero( showHours, 2, 4, 0, 1 );
    sendStringFixZero( showMins, 2, 7, 0, 1 );
    sendStringFixZero( showSecs, 2, 10, 0, 1 );

    static unsigned long oldTime = 0;
    if ( getMillisDifference( oldTime ) >= dotFlashSpeed  )
    {
      static bool flashDot = 0;
      flashDot = !flashDot;
      if ( flashDot == 0 )
      {
        LCD.sendString( ":", 6, 0 );
        LCD.sendString( ":", 9, 0 );
      } else
      {
        LCD.sendString( " ", 6, 0 );
        LCD.sendString( " ", 9, 0 );
      }
      oldTime = millis();
    }

    if ( keyInputFunction( 0 ) == 1 )
    {
      returnACK |= 1;
      start = 0;
      showTimes = lastSetTime;
      LCD.noDoubleHeightFont();
      // -countDown set--
      LCD.sendString( "countDown set", 1, 0 );
      LCD.sendString( ":", 6, 1 );
      LCD.sendString( ":", 9, 1 );
      showHours = ((showTimes / 1000) / 60);
      showHours /= 60;
      showMins = ((showTimes / 1000) - (showHours * 60 * 60)) / 60;
      showSecs = (showTimes / 1000) - (showHours * 60 * 60) - (showMins * 60);
      sendStringFixZero( showHours, 2, 4, 1, 1 );
      sendStringFixZero( showMins, 2, 7, 1, 1 );
      sendStringFixZero( showSecs, 2, 10, 1, 1 );
      byte hours = showHours;
      byte mins = showMins;
      byte secs = showSecs;
      for (;;)
      {
        int temp;
        temp = getTimeFunctionInside( hours, 4, 1, 2, minHours, maxHours, 0 );
        if ( temp >= 10000 )
        {
          hours = temp - 10000;
          break;
        }
        hours = temp;
        temp = getTimeFunctionInside( mins, 7, 1, 2, minMins, maxMins, 1 );
        if ( temp >= 10000 )
        {
          mins = temp - 10000;
          break;
        }
        mins = temp;
        temp = getTimeFunctionInside( secs, 10, 1, 2, minMins, maxMins, 1 );
        if ( temp >= 10000 )
        {
          secs = temp - 10000;
          break;
        }
        secs = temp;
      }
      // --Countdown-----
      // ---set complete-
      LCD.clear();
      LCD.sendString( "Countdown " , 2 , 0 );
      LCD.sendString( "set complete" , 3 , 1 );
      delay(1000);
      LCD.clear();
      seted = 1;
      lastSetTime = (hours * 60) + mins;
      lastSetTime *= 60;
      lastSetTime += secs;
      lastSetTime *= 1000;
      countDownTime = lastSetTime;
      LCD.noDoubleHeightFont();
      LCD.clear();
      start = 1;
      oldTime1 = millis();
    }

    if ( keyInputFunction( 3 ) == 1 )
    {
      returnACK |= 1;
      start = !start;
    }

    if ( start == 0 )
    {
      oldTime1 = millis();
    }
    if ( (start == 1) && (seted == 1) )
    {
      unsigned long temp = getMillisDifference( oldTime1 );
      oldTime1 = millis();
      if ( countDownTime > temp )
      {
        countDownTime -= temp;
      } else
      {
        //time's up
        countDownTime = 0;
        countDownIsOut();
        start = 0;
        seted = 0;
      }
    }
  } else
  {
    if ( start == 0 )
    {
      oldTime1 = millis();
    }
    if ( (start == 1) && (seted == 1) )
    {
      unsigned long temp = getMillisDifference( oldTime1 );
      oldTime1 = millis();
      if ( countDownTime > temp )
      {
        countDownTime -= temp;
      } else
      {
        //time's up
        countDownTime = 0;
        countDownIsOut();
        start = 0;
        seted = 0;
      }
    }
  }
  return returnACK;
}
/*
void countDownFunction( bool showProgram, bool wantset )
{
  byte hours=0;
  byte mins=0;
  byte secs=0;

  static unsigned long countDownTimes;
  static bool seted=0;
  if( showProgram==1 )
  {
    if( (seted==0)||wantset )
    {
      setagain:
      LCD.sendString( "Countdown set", 1 ,0 );
      LCD.sendString( "00",3,1 );
      LCD.sendString( ":",6,1 );
      LCD.sendString( "00",8,1 );
      LCD.sendString( ":",11,1 );
      LCD.sendString( "00",13,1 );
      for(;;)
      {
        int temp;
        temp=getTimeFunctionInside( hours, 3, 1, 2, minHours, maxHours, 0 );
        if ( temp>=10000 )
        {
          hours=temp-10000;
          break;
        }
        hours=temp;
        temp=getTimeFunctionInside( mins, 8, 1, 2, minMins, maxMins, 1 );
        if ( temp>=10000 )
        {
          mins=temp-10000;
          break;
        }
        mins=temp;
        temp=getTimeFunctionInside( secs, 13, 1, 2, minSecs, maxSecs, 1 );
        if ( temp>=10000 )
        {
          secs=temp-10000;
          break;
        }
        secs=temp;
      }
      if( hours==0 )
      {
        if( mins==0 )
        {
          if( secs==0 )
          {
            LCD.clear();
            LCD.sendString( "Please enter", 1, 0 );
            LCD.sendString( "the time!", 6, 1 );
            delay(1000);
            LCD.clear();
            goto setagain;
          }
        }
      }

      unsigned long wantTimes=(hours*60)+mins;  // 目標時間取得並轉毫秒
           wantTimes*=60;
           wantTimes+=secs;

      countDownTimes=(wantTimes*1000)+millis();  //  終結時間點
      seted=1;

      // --Countdown-----
      // ---set complete-
      LCD.sendString( "Countdown " ,2 ,0 );
      LCD.sendString( "set complete" ,3 ,1 );
      delay(1000);
      LCD.clear();
    }else
    {
      LCD.doubleHeightFont();
      unsigned long showCountDownTimesLeft=getMillisDifferencePatch( countDownTimes );
      showCountDownTimesLeft/=1000;
      hours=(showCountDownTimesLeft/60)/60;
      mins=(showCountDownTimesLeft-(hours*60*60))/60;
      secs=showCountDownTimesLeft-(hours*60*60)-(mins*60);
      sendStringFixZero( hours, 2, 4, 0, 0 );
      sendStringFixZero( mins, 2, 7, 0, 1 );
      sendStringFixZero( secs, 2, 10, 0, 1 );

      static unsigned long oldTime=millis();
      static bool flashDot=0;
      if( getMillisDifference( oldTime )>=dotFlashSpeed  )
      {
        flashDot=!flashDot;
        if( flashDot==0 )
        {
          LCD.sendString( ":",6,0 );
          LCD.sendString( ":",9,0 );
        }else
        {
          LCD.sendString( " ",6,0 );
          LCD.sendString( " ",9,0 );
        }
        oldTime=millis();
      }
      if( countDownTimes/1000==millis()/1000 )
      {
        countDownIsOut();
        seted=0;
      }
    }
  }else
  {
    if( countDownTimes/1000==millis()/1000 )
    {
      countDownIsOut();
      seted=0;
    }
  }
}*/

// 倒數計時結束

void clearData( unsigned long *data )
{
  for (byte i = 0; i < 20; i++)
  {
    data[i] = 0;
  }
}

bool stopWatchFunction()
{
  bool returnACK = 0;
  static bool start = 0;
  static unsigned long stopWatchStopTime = 1; // 停止時的時間  已處理距離完成   1才會工作正常 0每次會被寫入 因該是bug
  static unsigned long stopWatchStartTime = 0; // 碼表開始時的時間  沒處理
  static unsigned long data[9] = { 0 };
  static byte index = 0;   // 儲存指標
  static byte indexTemp = 0; // 展示指標

  if ( start == 0 )
  {
    //  ---00:00:00.00
    unsigned long hours;
    unsigned long mins;
    unsigned long secs;
    unsigned long msecs;
    unsigned long showTime = stopWatchStopTime;
    hours = ((showTime / 1000) / 60);
    hours /= 60;
    mins = ((showTime / 1000) - (hours * 60 * 60)) / 60;
    secs = (showTime / 1000) - (hours * 60 * 60) - (mins * 60);
    msecs = (showTime - (hours * 60 * 60 * 1000) - (mins * 60 * 1000) - (secs * 1000));
    msecs /= 10;
    sendStringFixZero( hours, 2, 3, 1, 1 );
    sendStringFixZero( mins, 2, 6, 1, 1 );
    sendStringFixZero( secs, 2, 9, 1, 1 );
    sendStringFixZero( msecs, 2, 12, 1, 1 );
    LCD.sendString( ":", 5, 1 );
    LCD.sendString( ":", 8, 1 );
    LCD.sendString( ".", 11, 1 );

    if ( keyInputFunction( 2 ) == 1 ) // 清除畫面
    {
      returnACK |= 1;
      LCD.sendString( "              ", 1, 0 );
      stopWatchStopTime = 0;
      clearData( data );
      index = 0;
    }

    if ( keyInputFunction( 3 ) == 1 ) // 開始計數碼表
    {
      returnACK |= 1;
      start = 1;
      stopWatchStartTime = millis();
    }

    if ( keyInputFunction( 0 ) == 1 )
    {
      returnACK |= 1;
      if (index == 0)
      {
        // 假如沒存資料 nothing to do.
      } else
      {
        //  -1. 00:00:00.00-
        unsigned long showDataTimes = data[indexTemp];
        unsigned long dataHours = (showDataTimes / 60) / 60;
        unsigned long dataMins = (showDataTimes - (hours * 60 * 60)) / 60;
        unsigned long dataSecs = showDataTimes - (hours * 60 * 60) - (mins * 60);
        unsigned long dataMsecs = (showDataTimes - (hours * 60 * 60) - (mins * 60) - secs) / 10;
        dataHours = ((showDataTimes / 1000) / 60);
        dataHours /= 60;
        dataMins = ((showDataTimes / 1000) - (dataHours * 60 * 60)) / 60;
        dataSecs = (showDataTimes / 1000) - (dataHours * 60 * 60) - (dataMins * 60);
        dataMsecs = (showDataTimes - (dataHours * 60 * 60 * 1000) - (dataMins * 60 * 1000) - (dataSecs * 1000));
        dataMsecs /= 10;
        sendStringFixZero( dataHours, 2, 4, 0, 1 );
        sendStringFixZero( dataMins, 2, 7, 0, 1 );
        sendStringFixZero( dataSecs, 2, 10, 0, 1 );
        sendStringFixZero( dataMsecs, 2, 13, 0, 1 );
        char output[2];
        itoa( indexTemp + 1, output, 10 );
        LCD.sendString( output, 1, 0 );
        LCD.sendString( ".", 2, 0 );
        LCD.sendString( ":", 6, 0 );
        LCD.sendString( ":", 9, 0 );
        LCD.sendString( ".", 12, 0 );
        if ( indexTemp < index - 1 )
        {
          indexTemp++;
        } else
        {
          indexTemp = 0;
        }
        //}
      }
    }

  } else
  {
    unsigned long showTime = getMillisDifference( stopWatchStartTime );
    unsigned long hours;
    unsigned long mins;
    unsigned long secs;
    unsigned long msecs;
    showTime += stopWatchStopTime;
    hours = ((showTime / 1000) / 60);
    hours /= 60;
    mins = ((showTime / 1000) - (hours * 60 * 60)) / 60;
    secs = (showTime / 1000) - (hours * 60 * 60) - (mins * 60);
    msecs = (showTime - (hours * 60 * 60 * 1000) - (mins * 60 * 1000) - (secs * 1000));
    msecs /= 10;
    sendStringFixZero( hours, 2, 3, 1, 1 );
    sendStringFixZero( mins, 2, 6, 1, 1 );
    sendStringFixZero( secs, 2, 9, 1, 1 );
    sendStringFixZero( msecs, 2, 12, 1, 1 );
    LCD.sendString( ":", 5, 1 );
    LCD.sendString( ":", 8, 1 );
    LCD.sendString( ".", 11, 1 );

    if ( keyInputFunction( 3 ) == 1 )
    {
      returnACK |= 1;
      start = 0;
      stopWatchStopTime = showTime;
    }

    if ( keyInputFunction( 2 ) == 1 )
    {
      returnACK |= 1;
      if ( index <= 8 )
      {
        data[index] = showTime;

        unsigned long showDataTimes = showTime;
        unsigned long dataHours;
        unsigned long dataMins;
        unsigned long dataSecs;
        unsigned long dataMsecs;
        dataHours = ((showDataTimes / 1000) / 60);
        dataHours /= 60;
        dataMins = ((showDataTimes / 1000) - (dataHours * 60 * 60)) / 60;
        dataSecs = (showDataTimes / 1000) - (dataHours * 60 * 60) - (dataMins * 60);
        dataMsecs = (showDataTimes - (dataHours * 60 * 60 * 1000) - (dataMins * 60 * 1000) - (dataSecs * 1000));
        dataMsecs /= 10;
        sendStringFixZero( dataHours, 2, 4, 0, 1 );
        sendStringFixZero(  dataMins, 2, 7, 0, 1 );
        sendStringFixZero( dataSecs, 2, 10, 0, 1 );
        sendStringFixZero( dataMsecs, 2, 13, 0, 1 );
        char output[2];
        itoa( index + 1, output, 10 );
        LCD.sendString( output, 1, 0 );

        LCD.sendString( ".", 2, 0 );
        LCD.sendString( ":", 6, 0 );
        LCD.sendString( ":", 9, 0 );
        LCD.sendString( ".", 12, 0 );

        index++;
      }
    }
  }
  return returnACK;
}

// 碼表結束

void alarmClockIsOut()
{
  LCD.clear();
  LCD.sendString( "alarmClock", 4 , 0 );
  LCD.sendString( "Time's UP!", 4 , 1 );
  byte num = 0;
  unsigned long oldTime1 = millis();
  unsigned long oldTime2;
  for (;;)
  {
    if ( waitAllKeyDown() == 1 )
    {
      noNewTone( speakPin );
      digitalWrite( motorPin, LOW );
      break;
    }

    if ( getMillisDifference( oldTime1 ) >= 250  )
    {
      oldTime1 = millis();

      if ( num != 6 )
      {
        num++;
        if ( num % 2 == 1 )
        {
          NewTone( speakPin, 988 );
          digitalWrite( motorPin, HIGH );
        } else
        {
          noNewTone( speakPin );
          digitalWrite( motorPin, LOW );
        }
        oldTime2 = millis();
      } else
      {
        if ( getMillisDifference( oldTime2 ) >= 2500  )
        {
          num = 0;
        }
      }
    }
  }
  LCD.clear();
}

bool alarmClockFunction( bool showProgram )
{
  bool returnACK = 0;
  // ---Alarm Clock---  LCD.sendString( "Alarm Clock", 3, 0 );
  // Alarm Clock set-   LCD.sendString( "Alarm Clock set", 0, 0 );
  // --Alarm Clock      LCD.sendString( "Alarm Clock", 2, 0 );
  // ---set complete-   LCD.sendString( "set complete", 3, 1 );
  static byte hours = 0;
  static byte mins = 0;
  static bool pointACK = 0;
  static bool alarmClockACK = 0;
  static bool patch = 0; //  避免重複響
  static bool patch1ForPoint = 0;
  static byte num = 0;

  if ( showProgram == 1 )
  {
    if ( keyInputFunction( 0 ) == 1 )
    {
      returnACK |= 1;
      LCD.clear();
      LCD.sendString( "Alarm Clock set", 0, 0 );
      sendStringFixZero( hours, 2, 5, 1, 0 );
      sendStringFixZero( mins, 2, 10, 1, 1 );
      LCD.sendString( ":", 8, 1 );

      for (;;)
      {
        int temp;
        temp = getTimeFunctionInside( hours, 5, 1, 2, minHours, maxHours, 0 );
        if ( temp >= 10000 )
        {
          hours = temp - 10000;
          break;
        }
        hours = temp;
        temp = getTimeFunctionInside( mins, 10, 1, 2, minMins, maxMins, 1 );
        if ( temp >= 10000 )
        {
          mins = temp - 10000;
          break;
        }
        mins = temp;
      }
      LCD.clear();
      LCD.sendString( "Alarm Clock", 2, 0 );
      LCD.sendString( "set complete", 3, 1 );
      delay(1000);
      LCD.clear();
    }
    LCD.sendSpecialStringCode( 0x02, 2, 0 );  //  顯示整點符號
    LCD.sendSpecialStringCode( 0x01, 8, 0 );  //  顯示鬧鐘符號
    LCD.sendString( ":", 3, 0 );
    LCD.sendString( ":", 9, 0 );

    sendTorF( pointACK, 4, 0 );    //  顯示ON /  OFF
    sendTorF( alarmClockACK, 10, 0 );

    if ( keyInputFunction( 2 ) == 1 ) //
    {
      returnACK |= 1;
      pointACK = !pointACK;
    }
    if ( keyInputFunction( 3 ) == 1 ) //
    {
      returnACK |= 1;
      alarmClockACK = !alarmClockACK;
    }

    // -----00 : 00----
    sendStringFixZero( hours, 2, 6, 1, 0 );
    sendStringFixZero( mins, 2, 9, 1, 1 );
    static unsigned long oldTime = millis();
    static bool flashDot = 0;
    if ( getMillisDifference( oldTime ) >= dotFlashSpeed  )
    {
      flashDot = !flashDot;
      if ( flashDot == 0 )
      {
        LCD.sendString( ":", 8, 1 );
      } else
      {
        LCD.sendString( " ", 8, 1 );
      }
      oldTime = millis();
    }

    if ( pointACK || alarmClockACK )
    {
      RTC.read(tm);

      if ( pointACK == 1 )
      {
        if ( tm.Minute == 0 )
        {
          if ( tm.Second == 0 )
          {
            // 逼逼
            static unsigned long oldTime1;
            if ( patch1ForPoint == 0 )
            {
              oldTime1 = millis();
              patch1ForPoint = 1;
            }
            if ( getMillisDifference( oldTime1 ) >= 250  )
            {
              oldTime1 = millis();

              if ( num < 4 )
              {
                num++;
                if ( num % 2 == 1 )
                {
                  NewTone( speakPin, 988 );
                  digitalWrite( motorPin, HIGH );
                } else
                {
                  noNewTone( speakPin );
                  digitalWrite( motorPin, LOW );
                }
              }
            }
          } else
          {
            noNewTone( speakPin );
            digitalWrite( motorPin, LOW );
          }
        } else
        {
          patch1ForPoint = 0;
          num = 0;
        }
      }

      if ( alarmClockACK == 1 )
      {
        if ( tm.Hour == hours )
        {
          if ( tm.Minute == mins )
          {
            if ( patch == 0 )
            {
              alarmClockIsOut();
              patch = 1;
            }
          } else
          {
            patch = 0;
          }
        } else
        {
          patch = 0;
        }
      }
    }
  } else
  {
    if ( pointACK || alarmClockACK )
    {
      RTC.read(tm);

      if ( pointACK == 1 )
      {
        if ( tm.Minute == 0 )
        {
          if ( tm.Second == 0 )
          {
            // 逼逼
            static unsigned long oldTime1;
            if ( patch1ForPoint == 0 )
            {
              oldTime1 = millis();
              patch1ForPoint = 1;
            }
            if ( getMillisDifference( oldTime1 ) >= 250  )
            {
              oldTime1 = millis();

              if ( num < 4 )
              {
                num++;
                if ( num % 2 == 1 )
                {
                  NewTone( speakPin, 988 );
                  digitalWrite( motorPin, HIGH );
                } else
                {
                  noNewTone( speakPin );
                  digitalWrite( motorPin, LOW );
                }
              }
            }
          } else
          {
            noNewTone( speakPin );
            digitalWrite( motorPin, LOW );
          }
        } else
        {
          patch1ForPoint = 0;
          num = 0;
        }
      }

      if ( alarmClockACK == 1 )
      {
        if ( tm.Hour == hours )
        {
          if ( tm.Minute == mins )
          {
            if ( patch == 0 )
            {
              alarmClockIsOut();
              patch = 1;
            }
          } else
          {
            patch = 0;
          }
        } else
        {
          patch = 0;
        }
      }
    }
  }
  return returnACK;
}

//  鬧鐘結束


//  心跳


volatile int rate[10];                    // used to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find the inter beat interval
volatile int P = 512;                     // used to find peak in pulse wave
volatile int T = 512;                     // used to find trough in pulse wave
volatile int thresh = 512;                // used to find instant moment of heart beat
volatile int amp = 100;                   // used to hold amplitude of pulse waveform
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = true;       // used to seed rate array so we startup with reasonable BPM


void interruptSetup()
{
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
}


// THIS IS THE TIMER 2 INTERRUPT SERVICE ROUTINE.
// Timer 2 makes sure that we take a reading every 2 miliseconds
ISR(TIMER2_COMPA_vect)
{ // triggered when Timer2 counts to 124
  cli();                                      // disable interrupts while we do this
  Signal = analogRead(pulsePin);              // read the Pulse Sensor
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

  //  find the peak and trough of the pulse wave
  if (Signal < thresh && N > (IBI / 5) * 3)
  { // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T)
    { // T is the trough
      T = Signal;                         // keep track of lowest point in pulse wave
    }
  }

  if (Signal > thresh && Signal > P)
  { // thresh condition helps avoid noise
    P = Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250)
  { // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3) )
    {
      Pulse = true;                               // set the Pulse flag when we think there is a pulse
      digitalWrite(blinkPin, HIGH);               // turn on pin 13 LED
      heartHigh=millis();
      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if (firstBeat)
      { // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                 // clear firstBeat flag
        return;                            // IBI value is unreliable so discard it
      }
      if (secondBeat)
      { // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                 // clear secondBeat flag
        for (int i = 0; i <= 9; i++)
        { // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;
        }
      }

      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                   // clear the runningTotal variable

      for (int i = 0; i <= 8; i++)
      { // shift data in the rate array
        rate[i] = rate[i + 1];            // and drop the oldest IBI value
        runningTotal += rate[i];          // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values
      BPM = 60000 / runningTotal;             // how many beats can fit into a minute? that's BPM!
      QS = true;                              // set Quantified Self flag
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }
  }

  if (Signal < thresh && Pulse == true)
  { // when the values are going down, the beat is over
    digitalWrite(blinkPin, LOW);           // turn off pin 13 LED
        
    if( getMillisDifference( heartHigh )>= heartHighTime )
    {
      Die=1;
    }
    
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp / 2 + T;                  // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 2500)
  { // if 2.5 seconds go by without a beat
    thresh = 512;                          // set thresh default
    P = 512;                               // set P default
    T = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    firstBeat = true;                      // set these to avoid noise
    secondBeat = true;                     // when we get the heartbeat back
  }

  sei();                                     // enable interrupts when youre done!
}// end isr


void ledFadeToBeat()
{
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate, 0, 255); //  keep LED fade value from going into negative numbers!
  analogWrite(fadePin, fadeRate);         //  fade LED
}


void sendDataToProcessing(char symbol, int data )
{
  //Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
  //Serial.println(data);                // the data to send culminating in a carriage return
}

void heartFunction()
{
  static unsigned long oldTime = millis();
  if ( getMillisDifference( oldTime ) >= 20  )
  {
    //sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data
    if (QS == true)
    { // Quantified Self flag is true when arduino finds a heartbeat
      fadeRate = 255;                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
      sendDataToProcessing('B', BPM);  // send heart rate with a 'B' prefix
      heartData = BPM;
      //sendDataToProcessing('Q',IBI);   // send time between beats with a 'Q' prefix
      QS = false;                      // reset the Quantified Self flag for next time
    }

    ledFadeToBeat();
    oldTime = millis();
  }
}

unsigned int heartPatch()
{
  if( heartData > 210 )
  {
    heartData/=3;
  }else
  {
    if( heartData>140 )
    {
      heartData/=2;
    }
  }
}

void bluetoothFunction()
{
  static bool start = 0;
  static bool startReceive = 0;
  if ( blueToothAck == 1 )
  {
    if ( start == 0 )
    {
      //  先清快取再讀資料
      for (;;)
      {
        if ( Serial.available() > 0 )
        {
          Serial.read();
        } else
        {
          start = 1;
          break;
        }
      }
    }

    if ( Serial.available() > 0 )
    {
      if ( startReceive == 0 )
      {
        char val;
        val = Serial.read();
        if ( val == 'P' )
        {
          startReceive = 1;
        }
      } else
      {
        char val;
        val = Serial.read();
        if ( val != 'F')
        {
          getData += val;
        } else
        {
          char  Cmd = getData[0];
          if ( Cmd == '1' )
          {
            phoneComing = 1;
          } else
          {
            phoneComing = 0;
          }
          getData = "";
          startReceive = 0;
        }
      }
    }

    if ( heartAck == 1 )
    {
      static unsigned long oldTime1 = millis();
      static bool Re_oldTime1 = 0;
      if ( heartWaitTime == 0 )
      {
        if ( Re_oldTime1 == 0 )
        {
          oldTime1 = millis();
          Re_oldTime1 = 1;
        }
        if ( getMillisDifference( oldTime1 ) >= 3000 )
        {
          heartWaitTime = 1;
          Re_oldTime1 = 0;
        }
      } else
      {
        static unsigned long oldTime = millis();
        if ( getMillisDifference( oldTime ) >= 500  )
        {
          Serial.print( "H" );
          
          unsigned int tempSWAP=heartData;
          
          if( Die!=1 )
          {
            heartPatch();
          }else
          {
            Die=0;
            heartData=999;
          }
          

          if ( heartData < 10 )
          {
            Serial.print( "00" );
          } else if ( heartData < 100 )
          {
            Serial.print( "0" );
          }

          char output[4];
          itoa( heartData, output, 10 );
          Serial.print( output );
          Serial.print( "F" );
          heartData=tempSWAP;
          oldTime = millis();
        }
      }
    } else
    {
      heartWaitTime = 0;
    }

  } else
  {
    start = 0;
  }
}

void phoneComingFunction()
{
  bool startReceive = 0;
  if ( phoneComing == 1 )
  {
    LCD.clear();
    byte num = 0;
    unsigned long oldTime1 = millis();
    unsigned long oldTime2;
    for (;;)
    {
      if( keyInputFunction(3)==1)
      {
        break;
      }
      
      LCD.sendString( "Phone is Coming!", 0, 0 );

      if ( getMillisDifference( oldTime1 ) >= 250  )
      {
        oldTime1 = millis();

        if ( num != 6 )
        {
          num++;
          if ( num % 2 == 1 )
          {
            NewTone( speakPin, 988 );
            digitalWrite( motorPin, HIGH );
          } else
          {
            noNewTone( speakPin );
            digitalWrite( motorPin, LOW );
          }
          oldTime2 = millis();
        } else
        {
          if ( getMillisDifference( oldTime2 ) >= 2500  )
          {
            num = 0;
          }
        }
      }
      /*
      static unsigned long oldTime=millis();
      static bool startRing=0;
      static bool mode=0;
      static byte waitClock=0;
      if( getMillisDifference( oldTime )>=250  )
      {
        if( waitClock!=3 )
        {
          if( mode==0 )
          {
            digitalWrite( motorPin, HIGH );  //  馬達開啟
            tone( speakPin, 988 );  //  speaker開啟
          }else
          {
            digitalWrite( motorPin, LOW );
            noTone( speakPin );
            waitClock++;
          }
        }else
        {

        }
        mode!=mode;
        oldTime=millis();
      }
      */
      if ( Serial.available() > 0 )
      {
        if ( startReceive == 0 )
        {
          char val;
          val = Serial.read();
          if ( val == 'P' )
          {
            startReceive = 1;
          }
        } else
        {
          char val;
          val = Serial.read();
          if ( val != 'F' )
          {
            getData += val;
          } else
          {
            char  Cmd = getData[0];
            if ( Cmd == '1' )
            {
              phoneComing = 1;
            } else
            {
              phoneComing = 0;
            }
            getData = "";
            startReceive = 0;
          }
        }
      }

      if ( phoneComing == 0 )
      {
        noNewTone( speakPin );
        digitalWrite( motorPin, LOW );
        LCD.clear();
        break;
      }
      delay(200);
    }
  }
}

