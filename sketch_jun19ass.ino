//로봇좌표 에러코드
//집기 백업
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

int locationArray[2];
Color objectArray[ARRAY_SIZE];
PRIZM prizm; // 메인 프리즘 보드
EXPANSION exc; // 익스펜션 컨트롤러 
HUSKYLENS huskylens;

double distance = 0;
int checkFinish = 0;
int firstMoving = 0;
int objectColorCount = 0;
int robot_x = 0;
int robot_y = 0;
int yFlag = 0;
int xFlag = 0;
int popObjectFlag = 0;
int checkArray = 2;
int gotObject = 0;
int initGet = 0;

int start_flag1 = 0;
int start_flag2 = 0;

struct Pillar {
    Color color;
    int pillar_x;
    int pillar_y;
};

Pillar pillars[3] = {
    {RED, 0, 0},
    {GREEN, 0, 0},
    {BLUE, 0, 0},
    
};

Color checkColor(int id) {
    switch (id) {
        case 1: return RED;
        case 2: return GREEN;
        case 3: return BLUE;        
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
  // Serial.println(distance);
  return distance;
}
bool checkSameLocation(){        
     if(locationArray[0] == robot_x && locationArray[1] == robot_y){
      // Serial.println("오브젝트와 기둥의 색상이 일치합니다.");
      return true;
     }else{
      // Serial.println("!! 일치하지 않는 오브젝트.");
      return false;
     }
}
void checkPillarsLocation(){
  Color nowColor = objectArray[checkArray];
  Serial.println(checkArray);
  for(int i = 2; i >= 0; --i){
    if (pillars[i].color == nowColor) {
              locationArray[0] = pillars[i].pillar_x;
              locationArray[1]  = pillars[i].pillar_y;
              Serial.print("location x : ");
              Serial.println(locationArray[0]);
              Serial.print("location y : ");
              Serial.println(locationArray[1]);

      }
  }
  
}

void getObject(){

  // exc.setMotorSpeeds(1, 90, 90); // 살짝 이동
  // exc.setMotorSpeeds(2, 90, 90); // 살짝 이동
  // delay(50);
  // motorStop(); // 이동 종료
  if(robot_x == 1 && robot_y == 5) { 
    goingLeft();
    delay(50);
   }
  if(objectColorCount == 0){ //첫 오브젝트 집기
    prizm.setServoPosition(1,0); // 집게 풀기
    delay(2000);
    while(checkDistance() > 13.6){
      exc.setMotorSpeeds(1, -60, 60); // 오브젝트로 가기
      exc.setMotorSpeeds(2, 60, -60);
    }    
    exc.setMotorSpeeds(1, 0, 0);  //그만 가기
    exc.setMotorSpeeds(2, 0, 0);
    delay(500);
    prizm.setServoPosition(1,180); // 집게 조이기  
    delay(2500);

  }else if(objectColorCount != 0){ // 두, 세번 째 오브젝트 집기
    prizm.setMotorSpeed(1, -300); // 레일 올리기 시작
    delay(2500);
    prizm.setMotorSpeed(1, 0); // 레일 올리기 종료
    while(checkDistance() > 13.6){
      exc.setMotorSpeeds(1, -60, 60); // 오브젝트로 가기
      exc.setMotorSpeeds(2, 60, -60);
    }
    exc.setMotorSpeeds(1, 0, 0); //오브젝트 이동 종료
    exc.setMotorSpeeds(2, 0, 0); // 오브젝트 이동 종료
    delay(500);
    prizm.setServoPosition(1,0); // 집게 풀기 진행
    delay(1000);
    prizm.setMotorSpeed(1, 300); // 레일 내리기 시작
    delay(2500);
    prizm.setMotorSpeed(1, 0); //레일 내리기 종료
    prizm.setServoPosition(1,180); // 집게 조이기      
    delay(1300);
  }
  while(checkDistance() < 24){
        exc.setMotorSpeeds(1, 50, -50);
        exc.setMotorSpeeds(2, -50, 50);
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
    exc.setMotorSpeeds(1, 140, 140);
    exc.setMotorSpeeds(2, 140, 140);
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

void checkHuskyLens(){
       if (huskylens.request()) {     
                // Serial.println("HuskyLens request successful.");
                delay(500);
                if (huskylens.count() > 1) {    
                    // Serial.println("Objects detected.");
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

                    // Serial.print("Pillar color: ");
                    // Serial.println(pillarColor);
                    // Serial.print("Object color: ");
                    // Serial.println(ObjectColor);

                    switch (pillarColor) {
                        case RED:
                            pillars[0].pillar_x = nowX;
                            pillars[0].pillar_y = nowY;
                            // Serial.print("RED X: ");
                            // Serial.println(nowX);
                            // Serial.print("RED Y: ");
                            // Serial.println(nowY);
                            break;
                        case GREEN:
                            pillars[1].pillar_x = nowX;
                            pillars[1].pillar_y = nowY;
                            // Serial.print("GREEN X: ");
                            // Serial.println(nowX);
                            // Serial.print("GRREN Y: ");
                            // Serial.println(nowY);
                            break;
                        case BLUE:
                            pillars[2].pillar_x = nowX;
                            pillars[2].pillar_y = nowY;
                            // Serial.print("BLUE X: ");
                            // Serial.println(nowX);
                            // Serial.print("BLUE Y: ");
                            // Serial.println(nowY);
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
                          // Serial.println("Color array limit exceeded.");
                          // Serial.println("컬러 배열 한도초과");
                      }
                      
                  } else {
                      // Serial.println("ObjectColor already in array.");
                      // Serial.println("ObjectColor가 배열에 이미 존재합니다.");
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

void popObject(){

  motorStop(); // 이동 종료
    
    prizm.setMotorSpeed(1, -300); // 레일 올리기 시작
    delay(1000);
    prizm.setMotorSpeed(1, 0); // 레일 올리기 종료
    while(checkDistance() > 13.7){
      exc.setMotorSpeeds(1, -60, 60); // 오브젝트로 가기
      exc.setMotorSpeeds(2, 60, -60);
    }
    exc.setMotorSpeeds(1, 0, 0); //오브젝트 이동 종료
    exc.setMotorSpeeds(2, 0, 0); // 오브젝트 이동 종료
    delay(500);
    prizm.setServoPosition(1,0); // 집게 풀기 진행
    delay(2000);
    prizm.setMotorSpeed(1, -300); // 레일 올리기 시작
    delay(1500);
    prizm.setMotorSpeed(1, 0); //레일 올리기 종료
    prizm.setServoPosition(1,180); // 집게 조이기      
    delay(2000);
  
  while(checkDistance() < 24){
        exc.setMotorSpeeds(1, 60, -60);
        exc.setMotorSpeeds(2, -60, 60);
  }
  exc.setMotorSpeeds(1, 0, 0);
  exc.setMotorSpeeds(2, 0, 0);
  prizm.setMotorSpeed(1, 300); // 레일 내리기 시작
  delay(2500);
  prizm.setMotorSpeed(1, 0); //레일 내리기 종료
  gotObject -= 1;
  
}



void setup() {
    Serial.begin(9600);
    Wire.begin();

    while (!huskylens.begin(Wire)) {
        // Serial.println("HuskyLens 초기화 실패. 연결을 확인하세요.");
        // Serial.println("1. HuskyLens의 'Protocol Type' 설정이 I2C인지 확인하세요.");
        // Serial.println("2. HuskyLens와 Tetrix Prizm 보드 간의 연결을 확인하세요.");
        delay(1000); // 1초 대기
    }
    // Serial.println("HuskyLens 초기화 성공!");

    // 기본 보드 및 보조 컨트롤러 초기화
    prizm.PrizmBegin();
    // prizm.setMotorInvert(2, 1);
    exc.setMotorInvert(1, 1, 1);
    // prizm.setServoSpeed(1, 50);
    // prizm.setMotorSpeed(1, 50);
    // prizm.setServoPosition(3, 60);
    exc.setMotorInvert(2, 1, 1);

    // 정면부 라인트레이서 초기화
    pinMode(FR, INPUT);
    pinMode(FL, INPUT);
    pinMode(BR, INPUT);

}


void loop() {
      if(firstMoving == 0){
        backward();
        delay(4400);
        goingLeft();
        delay(4500);
        firstMoving += 1;
        }
        
        int F_R = analogRead(FR);
        int F_L = analogRead(FL);
        int B_R = !digitalRead(BR);
        // int B_L = !digitalRead(BL);
        Serial.print("현재 로봇의 위치 X : ");
        Serial.println(robot_x);
        Serial.print("현재 로봇의 위치 Y : ");
        Serial.println(robot_y);
        // Serial.print("직진 오른쪽 센서 : ");
        // Serial.println(F_R);
        // Serial.print("직진 왼쪽 센서 : ");
        // Serial.println(F_L);
        // Serial.print("옆길 오른쪽 센서 : ");
        // Serial.println(B_R);
        // Serial.print("옆길 왼쪽 센서 : ");
        // Serial.println(B_L);
        // Serial.println("------------------");
        delay(10);
        
        if(F_R >= 300 && F_L >= 300){
          // Serial.print("popObjectFlag : ");
          // Serial.println(popObjectFlag);
          // Serial.print("objectColorCount : ");
          // Serial.println(objectColorCount);
          goingLeft();
        }else if(F_R <= 480 && F_L >= 600){     
          // Serial.println("헐 이거 작동함");
            exc.setMotorSpeeds(1, 140, 70);
            exc.setMotorSpeeds(2, 140, 70);
          }else if(F_R >= 600 && F_L <= 480){
            // Serial.println("헐 이게 왜 작동함??");
            exc.setMotorSpeeds(1, 70, 140);
            exc.setMotorSpeeds(2, 70, 140);
        }

if(popObjectFlag == 0){
        // 양쪽 라인 및 정면 라인 센서 발견시
        if (F_R >= 250 && F_L >= 250 && B_R == 1) 
        {

            motorStop();
            delay(500);            
            // Serial.println("Line detected, stopping motors.");
            if (robot_x == 0) {  
                if(xFlag != 1) { robot_y += 1; }
                
                // Serial.print("현재 로봇좌표 : X : ");
                // Serial.println(robot_x);
                // Serial.print("현재 로봇좌표 : Y : ");
                // Serial.println(robot_y);
                checkHuskyLens();                 
            } else if (robot_x == 1) {
                if(yFlag == 1) {                   
                  robot_y -= 1; 
                }
                // Serial.print("현재 로봇좌표 : X : ");
                // Serial.println(robot_x);
                // Serial.print("현재 로봇좌표 : Y : ");
                // Serial.println(robot_y);  
                checkHuskyLens();              
                                
            } 
            
            //렌즈를 통한 색상 인식
                                   
        }
  

    // 끝라인 도착시 회전
    if (B_R == 1 && robot_y == 4 && F_R <= 230 && F_L <= 230) {
        motorStop();
        delay(1000);
        
        // Serial.println("여기 왔어");
                   
            if (robot_x == 1) {
                if( robot_y ==2 ) { 
                  robot_x = 0; 
                  xFlag = 1;
                  }
                if( robot_y != 5 ) { robot_y -= 1; }                
                // Serial.println("아직 오브젝트가 남아있습니다..");
                // Serial.print("현재 로봇좌표 : X : ");
                // Serial.println(robot_x);
                // Serial.print("현재 로봇좌표 : Y : ");
                // Serial.println(robot_y);                

            } else if (robot_x == 0) {              
                robot_y += 1;               
                // Serial.println("아직 오브젝트가 남아있습니다..");
                // Serial.print("현재 로봇좌표 : X : ");
                // Serial.println(robot_x);
                // Serial.print("현재 로봇좌표 : Y : ");
                // Serial.println(robot_y);
                checkHuskyLens();
                robot_x = 1; 
              }
              turnLeft();
              delay(7600);

              
              
              

              
          
          
      }else if(B_R == 1 && robot_y == 2 && F_R <=250 && F_L <= 250){
            motorStop();
        delay(1000);
        
        // Serial.println("여기 왔어");
                   
            if (robot_x == 1) {
                if( robot_y ==2 ) { 
                  robot_x = 0; 
                  xFlag = 1;
                  }
                if( robot_y != 5 ) { robot_y -= 1; }                
                // Serial.println("아직 오브젝트가 남아있습니다..");
                // Serial.print("현재 로봇좌표 : X : ");
                // Serial.println(robot_x);
                // Serial.print("현재 로봇좌표 : Y : ");
                // Serial.println(robot_y);                

            } else if (robot_x == 0) {              
                robot_y += 1;               
                // Serial.println("아직 오브젝트가 남아있습니다..");
                // Serial.print("현재 로봇좌표 : X : ");
                // Serial.println(robot_x);
                // Serial.print("현재 로봇좌표 : Y : ");
                // Serial.println(robot_y);
                checkHuskyLens();
                robot_x = 1; 
              }
              turnLeft();
              delay(7900);
      }
}
/////////////////////////////////////////////////////////////////////////////////////////////
else if(popObjectFlag == 1 && checkArray != -1){
  if(initGet == 0){
    xFlag = 0;
    yFlag = 0;
    initGet += 1;
  }
  
  
  

  if (B_R == 1){
            motorStop();            
                       
            // Serial.println("Line detected, stopping motors.");
            if (robot_x == 0) {  
                robot_y += 1;
                // Serial.print("현재 로봇좌표 : X : ");
                // Serial.println(robot_x);
                // Serial.print("현재 로봇좌표 : Y : ");
                // Serial.println(robot_y);                             
            } else if (robot_x == 1) {                
                robot_y -= 1;                 
                // Serial.print("현재 로봇좌표 : X : ");
                // Serial.println(robot_x);
                // Serial.print("현재 로봇좌표 : Y : ");
                // Serial.println(robot_y);     
            }         
            ////////////////////// 
            // 길을 찾아가는 로직 ////
            //////////////////////
            checkPillarsLocation(); 
            delay(2000);   
            Serial.print("robot x : ");
            Serial.println(robot_x);
            Serial.print("robot y : ");
            Serial.println(robot_y);
            if(robot_x != locationArray[0] && robot_y == locationArray[1]){
              if(robot_x == 0){
                robot_x = 1;
                turnLeft();
                delay(7900);
                
                
                
              }else if(robot_x == 1){
                robot_x = 0;
                turnLeft();
                delay(7900);
                
                
              }
            }
            
            if(robot_x != locationArray[0] && robot_y > locationArray[1]) {
                if(robot_x == 1){
                  goingLeft();
                  delay(500);
                }else if(robot_x == 0){
                  robot_x = 0;
                  turnLeft();
                  delay(7900);
                  
                  
                }
              }else if(robot_x != locationArray[0] && robot_y < locationArray[1]){
              if(robot_x == 0){
                goingLeft();
                delay(500);
              }else if(robot_x == 1){
                robot_x = 0;
                turnLeft();
                delay(7900);
                
                
                }
              }
              
              if(robot_x == locationArray[0] && robot_y > locationArray[1]){
                if(robot_x == 0){
                  robot_x = 1;
                  turnLeft();
                  delay(7900);
                  
                }else if(robot_x == 1){
                  goingLeft();
                  delay(500);
                }
              }else if(robot_x == locationArray[0] && robot_y < loㄴ