#include <Wire.h>
#include <HUSKYLENS.h>
#include <PRIZM.h> // 테트릭스 프리즘 보드 초기화
#define FR A2 // 정면부 라인트레이서 오른쪽
#define FL A3 // 정면부 라인트레이서 왼쪽
#define BR 3 // 측면부 라인트레이서 왼쪽
// #define BL 2 // 측면부 라인트레이서 오른쪽
#define ARRAY_SIZE 3
#define HUSKYLENS_ADDRESS 0x32

enum Color { RED, GREEN, BLUE, YELLOW, NONE };

Color objectArray[ARRAY_SIZE];
PRIZM prizm; // 메인 프리즘 보드
EXPANSION exc; // 익스펜션 컨트롤러 
HUSKYLENS huskylens;

int firstMoving = 0;
int objectColorCount = 0;
int robot_x = 0;
int robot_y = 0;
int middleValue = 512;
int yFlag = 0;

struct Pillar {
    Color color;
    int pillar_x;
    int pillar_y;
};

Pillar pillars[4] = {
    {RED, 0, 0},
    {GREEN, 0, 0},
    {BLUE, 0, 0},
    {YELLOW, 0, 0}
};

Color checkColor(int id) {
    switch (id) {
        case 1: return RED;
        case 2: return GREEN;
        case 3: return BLUE;
        case 4: return YELLOW;
        default: return NONE;        
    }
}

bool isColorInArray(Color color) {
    for (int i = 0; i < objectColorCount; i++) {
        if (objectArray[i] == color) {
            return true;
        }
    }
    return false;
}

void forward() {
    exc.setMotorSpeeds(1, -100, 100);
    exc.setMotorSpeeds(2, 100, -100);
}

void backward() {
    exc.setMotorSpeeds(1, 100, -100);
    exc.setMotorSpeeds(2, -100, 100);
}

void turnRight() {
    exc.setMotorSpeeds(1, -100, 100);
    exc.setMotorSpeeds(2, -100, 100);
}

void turnLeft() {
    exc.setMotorSpeeds(1, 100, -100);
    exc.setMotorSpeeds(2, 100, -100);
}

void goingRight() {
    exc.setMotorSpeeds(1, -100, -100);
    exc.setMotorSpeeds(2, -100, -100);
}

void goingLeft() {
    exc.setMotorSpeeds(1, 90, 90);
    exc.setMotorSpeeds(2, 90, 90);
}
void goingBack() {
    exc.setMotorSpeeds(1, 65, -65);
    exc.setMotorSpeeds(2, -65, 65);
}
void goingFront() {
    exc.setMotorSpeeds(1, -65, 65);
    exc.setMotorSpeeds(2, 65, -65);
}
void motorStop() {
    exc.setMotorSpeeds(1, 0, 0);
    exc.setMotorSpeeds(2, 0, 0);  
}

bool isArrayEmpty() {
    return objectColorCount == 0;
}

bool isArrayFull() {
    return objectColorCount == ARRAY_SIZE;
}
void checkHuskyLens(){
       if (huskylens.request()) {     
                Serial.println("HuskyLens request successful.");
                delay(2000);
                if (huskylens.count() > 1) {    
                    Serial.println("Objects detected.");
                    // 오브젝트 인식
                    HUSKYLENSResult result1 = huskylens.read();
                    HUSKYLENSResult result2 = huskylens.read();

                    // 기둥 및 오브젝트 구분
                    HUSKYLENSResult pillarObject, Object;
                    if (result1.yCenter > result2.yCenter) {
                        pillarObject = result1;
                        Object = result2;
                    } else {
                        pillarObject = result2;
                        Object = result1;
                    }

                    Color pillarColor = checkColor(pillarObject.ID); 
                    Color ObjectColor = checkColor(Object.ID);

                    int nowX = robot_x;
                    int nowY = robot_y;

                    Serial.print("Pillar color: ");
                    Serial.println(pillarColor);
                    Serial.print("Object color: ");
                    Serial.println(ObjectColor);

                    switch (pillarColor) {
                        case RED:
                            pillars[0].pillar_x = nowX;
                            pillars[0].pillar_y = nowY;
                            break;
                        case GREEN:
                            pillars[1].pillar_x = nowX;
                            pillars[1].pillar_y = nowY;
                            break;
                        case BLUE:
                            pillars[2].pillar_x = nowX;
                            pillars[2].pillar_y = nowY;
                            break;
                        default:
                            break;
                    }

                   if (!isColorInArray(ObjectColor)) {
                      if (objectColorCount < ARRAY_SIZE) {
                          objectArray[objectColorCount++] = ObjectColor;
                      } else {
                          Serial.println("Color array limit exceeded.");
                          Serial.println("컬러 배열 한도초과");
                      }
                  } else {
                      Serial.println("ObjectColor already in array.");
                      Serial.println("ObjectColor가 배열에 이미 존재합니다.");
                  }
               }
            }
            
            if(robot_x == 1 && robot_y == 5){ 
              yFlag = 1;                             
              delay(1000);
              goingLeft();
              delay(1000);
            } else if(robot_y != 5) { 
              delay(1000);
              goingLeft(); 
              }
            delay(1000);
}

void setup() {
    Serial.begin(9600);
    Wire.begin();

    while (!huskylens.begin(Wire)) {
        Serial.println("HuskyLens 초기화 실패. 연결을 확인하세요.");
        Serial.println("1. HuskyLens의 'Protocol Type' 설정이 I2C인지 확인하세요.");
        Serial.println("2. HuskyLens와 Tetrix Prizm 보드 간의 연결을 확인하세요.");
        delay(1000); // 1초 대기
    }
    Serial.println("HuskyLens 초기화 성공!");

    // 기본 보드 및 보조 컨트롤러 초기화
    prizm.PrizmBegin();
    prizm.setMotorInvert(2, 1);
    exc.setMotorInvert(1, 1, 1);
    prizm.setServoSpeed(3, 50);
    prizm.setServoPosition(3, 60);
    exc.setMotorInvert(2, 1, 1);

    // 정면부 라인트레이서 초기화
    pinMode(FR, INPUT);
    pinMode(FL, INPUT);
    pinMode(BR, INPUT);

}


void loop() {
      // if(firstMoving == 0){
      //   backward();
      //   delay(4500);
      //   goingLeft();
      //   delay(4500);
      //   firstMoving += 1;
      //   }

        int F_R = analogRead(FR);
        int F_L = analogRead(FL);
        int B_R = !digitalRead(BR);
        // int B_L = !digitalRead(BL);
     
        Serial.print("직진 오른쪽 센서 : ");
        Serial.println(F_R);
        Serial.print("직진 왼쪽 센서 : ");
        Serial.println(F_L);
        Serial.print("옆길 오른쪽 센서 : ");
        Serial.println(B_R);
        // Serial.print("옆길 왼쪽 센서 : ");
        // Serial.println(B_L);
        // Serial.println("------------------");
        delay(100);
        
        if(F_R >= 300 && F_L >= 300){
          goingLeft();
        }else if(F_R <= 300 && F_L >= 600){     
          Serial.println("헐 이거 작동함");
            exc.setMotorSpeeds(1, 20, -20);
            exc.setMotorSpeeds(2, -20, 20);
          }else if(F_R >= 600 && F_L <= 300){
            Serial.println("헐 이게 왜 작동함??");
            exc.setMotorSpeeds(1, -20, 20);
            exc.setMotorSpeeds(2, 20, -20);
        }

        // 양쪽 라인 및 정면 라인 센서 발견시
        if (F_R >= 250 && F_L >= 250 && B_R == 1) 
        {

            motorStop();
            delay(1000);            
            Serial.println("Line detected, stopping motors.");
            if (robot_x == 0) {      
                robot_y += 1;
                Serial.print("현재 로봇좌표 : X : ");
                Serial.println(robot_x);
                Serial.print("현재 로봇좌표 : Y : ");
                Serial.println(robot_y);
                checkHuskyLens();                 
            } else if (robot_x == 1) {
                if(yFlag == 1) {                   
                  robot_y -= 1; 
                }
                Serial.print("현재 로봇�