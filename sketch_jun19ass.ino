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

double distance = 0;
int firstMoving = 0;
int objectColorCount = 0;
int robot_x = 0;
int robot_y = 0;
int middleValue = 512;
int yFlag = 0;
int xFlag = 0;
int popObjectFlag = 0;
int checkArray = 0;
int gotObject = 0;
int initGet = 0;

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
double checkDistance(){
  double volt = map(analogRead(A1), 0, 1023, 0, 5000); 
  distance = (27.61 / (volt - 0.1696)) * 1000;
  Serial.println(distance);
  return distance;
}
bool checkSameObject(){        
    Color nowColor = objectArray[checkArray];
    int target_x = -1;
    int target_y = -1;
 // nowColor와 동일한 색을 가진 Pillar를 찾기
     for (int i = 0; i < 3; i++) {
         if (pillars[i].color == nowColor) {
            target_x = pillars[i].pillar_x;
            target_y = pillars[i].pillar_y;
            Serial.print("target_x : ");
            Serial.println(target_x);
            Serial.print("target_y : ");
            Serial.println(target_y);
            Serial.print("now robot_x : ");
            Serial.println(robot_x);
            Serial.print("now robot_y : ");
            Serial.println(robot_y);
         }

     }

     if(target_x == robot_x && target_y == robot_y){
      return true;
     }else{
      return false;
     }
         
}
void popObject(){
  // getObject -= 1;
}
void getObject(){
    //if(objectColorCount != 0){
    // prizm.setMotorSpeed(1, -300); // 레일 올리기
    // delay(1500);
    // forward();
    // prizm.setServoPosition(1,0); // 집게 풀기
    //delay(800);
    // prizm.setMotorSpeed(1, 300); // 레일 내리기
    // delay(1500);
    // prizm.setServoPosition(1,180); // 집게 조이기    

  //}

  //체크 거리해서 몇 센티까지 오는지 ?
  exc.setMotorSpeeds(1, 90, 90); // 살짝 이동
  exc.setMotorSpeeds(2, 90, 90); // 살짝 이동
  delay(50);
  motorStop(); // 이동 종료
  if(objectColorCount == 0){ //첫 오브젝트 집기
    prizm.setServoPosition(1,0); // 집게 풀기
    delay(2000);
    while(checkDistance() > 14){
      exc.setMotorSpeeds(1, -30, 30); // 오브젝트로 가기
      exc.setMotorSpeeds(2, 30, -30);
    }    
    exc.setMotorSpeeds(1, 0, 0);  //그만 가기
    exc.setMotorSpeeds(2, 0, 0);
    delay(500);
    prizm.setServoPosition(1,180); // 집게 조이기  
    delay(2000);

  }else if(objectColorCount != 0){ // 두, 세번 째 오브젝트 집기
    prizm.setMotorSpeed(1, -300); // 레일 올리기 시작
    delay(2500);
    prizm.setMotorSpeed(1, 0); // 레일 올리기 종료
    while(checkDistance() > 14){
      exc.setMotorSpeeds(1, -30, 30); // 오브젝트로 가기
      exc.setMotorSpeeds(2, 30, -30);
    }
    exc.setMotorSpeeds(1, 0, 0); //오브젝트 이동 종료
    exc.setMotorSpeeds(2, 0, 0); // 오브젝트 이동 종료
    delay(500);
    prizm.setServoPosition(1,0); // 집게 풀기 진행
    delay(2000);
    prizm.setMotorSpeed(1, 300); // 레일 내리기 시작
    delay(2500);
    prizm.setMotorSpeed(1, 0); //레일 내리기 종료
    prizm.setServoPosition(1,180); // 집게 조이기      
    delay(700);
  }
  while(checkDistance() < 23){
        exc.setMotorSpeeds(1, 30, -30);
        exc.setMotorSpeeds(2, -30, 30);
  }
  exc.setMotorSpeeds(1, 0, 0);
  exc.setMotorSpeeds(2, 0, 0);
  gotObject += 1;
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
                            Serial.print("RED X: ");
                            Serial.println(nowX);
                            Serial.print("RED Y: ");
                            Serial.println(nowY);
                            break;
                        case GREEN:
                            pillars[1].pillar_x = nowX;
                            pillars[1].pillar_y = nowY;
                            Serial.print("GREEN X: ");
                            Serial.println(nowX);
                            Serial.print("GRREN Y: ");
                            Serial.println(nowY);
                            break;
                        case BLUE:
                            pillars[2].pillar_x = nowX;
                            pillars[2].pillar_y = nowY;
                            Serial.print("BLUE X: ");
                            Serial.println(nowX);
                            Serial.print("BLUE Y: ");
                            Serial.println(nowY);
                            break;
                        default:
                            break;
                    }

                   if (!isColorInArray(ObjectColor)) {
                      if (objectColorCount < ARRAY_SIZE) {  
                          delay(500);
                          getObjectif()popObjectFlag == 0{}{}}