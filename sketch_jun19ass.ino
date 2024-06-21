
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
  goingLeft(); // 집기 여유공간
  delay(50);
  if(objectColorCount == 0){ //첫 오브젝트 집기
    prizm.setServoPosition(1,0); // 집게 풀기
    delay(700);
    while(checkDistance() > 14){
      exc.setMotorSpeeds(1, -20, 20);
      exc.setMotorSpeeds(2, 20, -20);
    }
    delay(500);
    prizm.setServoPosition(1,180); // 집게 조이기  
    delay(700);

  }else if(objectColorCount != 0){ // 두, 세번 째 오브젝트 집기
    prizm.setMotorSpeed(1, -300); // 레일 올리기
    delay(2000);
    while(checkDistance() > 14){
      exc.setMotorSpeeds(1, -20, 20); // 오브젝트로 가기
      exc.setMotorSpeeds(2, 20, -20);
    }
    delay(500);
    prizm.setServoPosition(1,0); // 집게 풀기
    delay(700);
    prizm.setMotorSpeed(1, 300); // 레일 내리기
    delay(2000);
    prizm.setServoPosition(1,180); // 집게 조이기      
    delay(700);
  }
while(checkDistance() < 23){
      exc.setMotorSpeeds(1, 20, -20);
      exc.setMotorSpeeds(2, -20, 20);
}

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
                          delay(500);
                          getObject();
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
            if(xFlag == 1) { xFlag = 0; }  
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
    // prizm.setMotorInvert(2, 1);
    // exc.setMotorInvert(1, 1, 1);
    // prizm.setServoSpeed(1, 50);
    // prizm.setMotorSpeed(1, 50);
    // prizm.setServoPosition(3, 60);
    // exc.setMotorInvert(2, 1, 1);

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
   int checkArray = 0;if(popObjectFlag == 1){
  int checkArray = 0;
  while(1){
    if(checkArray == 3) break;    
    Color nowColor = objectArray[checkArray];
    
  }
}      
     // nowColor와 동일한 색을 가진 Pillar를 찾기
            for (int i = 0; i < 4; i++) {
                if (pillars[i].color == nowColor) {
                    std::cout << "Pillar found for color: " << nowColor << std::endl;
                    std::cout << "Pillar x: " << pillars[i].pillar_x << std::endl;
                    std::cout << "Pillar y: " << pillars[i].pillar_y << std::endl;
                    break;
                }
            }

            checkArray++; // nowColor와 동일한 색을 가진 Pillar를 찾기
            for (int i = 0; i < 4; i++) {
                if (pillars[i].color == nowColor) {
                    std::cout << "Pillar found for color: " << nowColor << std::endl;
                    std::cout << "Pillar x: " << pillars[i].pillar_x << std::endl;
                    std::cout << "Pillar y: " << pillars[i].pillar_y << std::endl;
                    break;
                }
            }

            checkArray++;         
      
         