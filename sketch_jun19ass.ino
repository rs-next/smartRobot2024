#include <Wire.h>
#include <HUSKYLENS.h>
#include <PRIZM.h> // 테트릭스 프리즘 보드 초기화
#define IRR 4 // 정면부 라인트레이서 오른쪽
#define IRL 5 // 정면부 라인트레이서 왼쪽
#define BIRL 3 // 측면부 라인트레이서 왼쪽
#define BIRR 2 // 측면부 라인트레이서 오른쪽
#define ARRAY_SIZE 3
#define HUSKYLENS_ADDRESS 0x32

enum Color { RED, GREEN, BLUE, YELLOW, NONE };

Color objectArray[ARRAY_SIZE];
PRIZM prizm; // 메인 프리즘 보드
EXPANSION exc; // 익스펜션 컨트롤러 
HUSKYLENS huskylens;

int objectColorCount = 0;
int robot_x = 0;
int robot_y = 0;

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

void forward() {
    exc.setMotorSpeeds(1, -150, 150);
    exc.setMotorSpeeds(2, 150, -150);
}

void backward() {
    exc.setMotorSpeeds(1, 150, -150);
    exc.setMotorSpeeds(2, -150, 150);
}

void turnRight() {
    exc.setMotorSpeeds(1, -150, 150);
    exc.setMotorSpeeds(2, -150, 150);
}

void turnLeft() {
    exc.setMotorSpeeds(1, 150, -150);
    exc.setMotorSpeeds(2, 150, -150);
}

void goingRight() {
    exc.setMotorSpeeds(1, -150, -150);
    exc.setMotorSpeeds(2, -150, -150);
}

void goingLeft() {
    exc.setMotorSpeeds(1, 150, 150);
    exc.setMotorSpeeds(2, 150, 150);
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
    pinMode(IRL, INPUT);
    pinMode(IRR, INPUT);
    pinMode(BIRL, INPUT);
    pinMode(BIRR, INPUT);
}

void checkLineTraisor() { // 라인트레이서 인식 메소드
    Serial.println("Starting line tracing...");

    while (true) {
        Serial.println("Checking sensors...");
        // 정면부 센서 값 시리얼 모니터에 출력
        Serial.print("F_Left Sensor: ");
        Serial.print(digitalRead(IRL));
        Serial.print(" | Right Sensor: ");
        Serial.println(digitalRead(IRR));

        // 측면부 센서 값 시리얼 모니터 출력
        Serial.print("B_Left Sensor: ");
        Serial.print(digitalRead(BIRL));
        Serial.print(" | B_Right Sensor: ");
        Serial.println(digitalRead(BIRR));
        
        delay(100);

        // 양쪽 라인 센서 발견시
        if ((digitalRead(IRL) == HIGH && digitalRead(IRR) == HIGH 
            && digitalRead(BIRL) == HIGH && digitalRead(BIRR) == HIGH) 
            || (digitalRead(BIRL) == HIGH && digitalRead(BIRR) == HIGH)) {
            motorStop();
            Serial.println("Line detected, stopping motors.");
            if (robot_x == 0) {      
                robot_y += 1;
            } else if (robot_x == 1) {
                robot_y -= 1;
            }
            Serial.print("현재 로봇좌표 : X : ");
            Serial.println(robot_x);
            Serial.print("현재 로봇좌표 : Y : ");
            Serial.println(robot_y);

            // 허스키 렌즈를 통한 색상 체크 
            if (huskylens.request()) {     
                Serial.println("HuskyLens request successful.");
                if (huskylens.count() > 1) {    
                    Serial.println("Objects detected.");
                    // 오브젝트 인식
                    HUSKYLENSResult result1 = huskylens.read();
                    HUSKYLENSResult result2 = huskylens.read();

                    // 기둥 및 오브젝트 구분
                    HUSKYLENSResult pillarObject, Object;
                    if (result1.yCenter < result2.yCenter) {
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
                            pillars[1