#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <DS3232RTC.h>
#include <TimeLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x21, 20, 4);
DS3232RTC rtc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  lcd.begin(20, 4);
  rtc.begin();
  Wire.begin();

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, INPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
}

struct User {
  long userID;
  int password;
  bool admin;
  bool working;
  int start;
  int finish;
};
struct Plan {
  int start;
  int finish;
};
int lastAdd = 90;
int temp1 = EEPROM.read(0);
int times[4] = { 0, 0, 0, 0 };
int stat[8];
bool adminDoor = false;

void loop() {
  // put your main code here, to run repeatedly:
  User u1 = {
    111110, 0000, true, false, 0, 0
  };
  User u2 = {
    111111, 1231, false, false, 0, 0
  };
  User u3 = {
    111112, 1232, false, false, 0, 0
  };
  User u4 = {
    111113, 1233, false, false, 0, 0
  };
  User u5 = {
    111114, 1234, false, false, 0, 0
  };
  User u6 = {
    111115, 1235, false, false, 0, 0
  };

  EEPROM.put(30, u1);
  EEPROM.put(42, u2);
  EEPROM.put(54, u3);
  EEPROM.put(66, u4);
  EEPROM.put(78, u5);
  EEPROM.put(90, u6);

  Plan p1 = {
    1023, 1240
  };
  Plan p2 = {
    820, 920
  };
  Plan p3 = {
    1500, 1400
  };
  Plan p4 = {
    10, 100
  };
  Plan p5 = {
    1445, 2020
  };
  Plan p6 = {
    0, 1
  };
  Plan p7 = {
    800, 2000
  };
  EEPROM.put(1, p1);
  EEPROM.put(5, p2);
  EEPROM.put(9, p3);
  EEPROM.put(13, p4);
  EEPROM.put(17, p5);
  EEPROM.put(21, p6);
  EEPROM.put(25, p7);

  delay(100);
  getBinary();
  Check();
  mainMenu();
}

int getKeyState() {
  int a = analogRead(A0);
  delay(100);

  if (a < 70) {
    return 1;
  } else if (a < 235) {
    return 2;
  } else if (a < 415) {
    return 3;
  } else if (a < 620) {
    return 4;
  } else if (a < 880) {
    return 5;
  } else if (a < 1023) {
    return 6;
  }
  return 0;
}

void mainMenu() {
  int ct;
  printMainMenu();

  int line = 3;
  lcd.setCursor(1, line);
  lcd.print('>');
  lcd.setCursor(18, line);
  lcd.print('<');

  while (true) {
    ct = (long)analogRead(A1) * 0.5;
    lcd.setCursor(15, 1);
    lcd.print(ct);
printTermal();
    switch (getKeyState()) {
      case 1:
        if (line > 2) {
          line--;
        }
        printMainMenu();
        lcd.setCursor(1, line);
        lcd.print('>');
        lcd.setCursor(18, line);
        lcd.print('<');
        waitForKeyRelease();
        break;
      case 2:
        if (line < 3) {
          line++;
        }
        printMainMenu();
        lcd.setCursor(1, line);
        lcd.print('>');
        lcd.setCursor(18, line);
        lcd.print('<');
        waitForKeyRelease();
        break;
      case 3:
        switch (line) {
          case 2:
            setTemp();
            printMainMenu();
            lcd.setCursor(1, line);
            lcd.print('>');
            lcd.setCursor(18, line);
            lcd.print('<');
            break;
          case 3:
            userListMenu();
            printMainMenu();
            lcd.setCursor(1, line);
            lcd.print('>');
            lcd.setCursor(18, line);
            lcd.print('<');
            break;
        }
        waitForKeyRelease();
        break;
      case 0:
        Check();
        break;
    }
  }

  delay(50);
}
void printMainMenu() {
  if (temp1 == 255) {
    temp1 = 0;
  }
  int ct = (long)analogRead(A1) * 0.5;

  lcd.clear();

  lcd.setCursor(5, 0);
  lcd.print("TEMP: ");

  lcd.setCursor(12, 0);
  lcd.print(temp1);

  lcd.setCursor(5, 1);
  lcd.print("Current: ");

  lcd.setCursor(15, 1);
  lcd.print(ct);

  lcd.setCursor(5, 2);
  lcd.print("Set Temp");

  lcd.setCursor(3, 3);
  lcd.print("User List Menu");
}

void userListMenu() {
  int temp = 0;

  printUserListMenu(temp);
  while (getKeyState() != 4) {
printTermal();
    switch (getKeyState()) {
      case 1:
        if (temp > 0) {
          temp = temp - 1;
        }
        lcd.clear();
        printUserListMenu(temp);

        waitForKeyRelease();
        break;

      case 2:
        if (temp < 14) {
          temp = temp + 1;
        }
        lcd.clear();
        printUserListMenu(temp);

        waitForKeyRelease();
        break;

      case 3:
        waitForKeyRelease();
        User user;
        int address = 30 + (temp * 12);
        EEPROM.get(address, user);
        if (user.admin) {
          adminMode(temp);
        } else {
          userMode(temp);
        }
        break;

      case 0:
        Check();
        break;
    }
  }
  delay(50);
}

void printUserListMenu(int temp) {
  lcd.clear();

  lcd.setCursor(1, 0);
  lcd.print('>');
  lcd.setCursor(18, 0);
  lcd.print('<');

  User user;
  for (int i = 0; i < 4; i++) {
    int address = 30 + ((temp + i) * 12);
    EEPROM.get(address, user);
    lcd.setCursor(7, i);
    long id = user.userID;
    lcd.print(id);
  }
}


bool enterPassword(int temp) {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Please Enter Password");
  lcd.setCursor(7, 1);
  lcd.print("0000");

  int passTemp = 0;
  for (int t1 = 7; t1 < 11 && getKeyState() != 4; t1++) {
    int t2 = 0;
    bool st = true;
    passTemp *= 10;

    while (st && getKeyState() != 4) {
      lcd.setCursor(t1, 2);
      lcd.print("^");
      switch (getKeyState()) {
        case 1:
          if (t2 < 9) {
            t2++;
          }
          lcd.setCursor(t1, 1);
          lcd.print(t2);

          waitForKeyRelease();
          break;
        case 2:
          if (t2 > 0) {
            t2--;
          }
          lcd.setCursor(t1, 1);
          lcd.print(t2);

          waitForKeyRelease();
          break;
        case 3:
          waitForKeyRelease();
          passTemp += t2;
          st = false;
          break;
        case 0:
          Check();
          break;
      }
    }
  }
  User user;
  int address = 30 + (temp * 12);
  EEPROM.get(address, user);
  if (user.password == passTemp) {
    return true;
  }
  return false;
}
void adminMode(int temp) {
  if (enterPassword(temp)) {
    adminModeMenu(temp);
  } else {
    lcd.clear();
    lcd.setCursor(3, 1);
    lcd.print("Your Password is wrong");
    delay(300);
  }
  return;
}
void adminModeMenu(int temp) {
  User user;
  int address = 30 + (temp * 12);

  EEPROM.get(address, user);
  bool working = user.working;
  if (working) {
    printAdminMenu("End");
  } else {
    printAdminMenu("Start");
  }
  int line = 0;
  lcd.setCursor(1, 0);
  lcd.print('>');
  lcd.setCursor(18, 0);
  lcd.print('<');

  while (getKeyState() != 4) {
    switch (getKeyState()) {
      case 1:
        if (line > 0) {
          line--;
        }
        lcd.clear();
        EEPROM.get(address, user);
        //        working = user.working;
        if (working) {
          printAdminMenu("End");
        } else {
          printAdminMenu("Start");
        }
        lcd.setCursor(1, line);
        lcd.print('>');
        lcd.setCursor(18, line);
        lcd.print('<');

        waitForKeyRelease();
        break;

      case 2:
        if (line < 1) {
          line++;
        }
        lcd.clear();
        //        working = user.working;
        if (working) {
          printAdminMenu("End");
        } else {
          printAdminMenu("Start");
        }
        lcd.setCursor(1, line);
        lcd.print('>');
        lcd.setCursor(18, line);
        lcd.print('<');

        waitForKeyRelease();
        break;

      case 3:
        Serial.print(line);
        switch (line) {
          case 0:
            setAirConditioner();
            if (working) {
              printAdminMenu("End");
            } else {
              printAdminMenu("Start");
            }
            lcd.setCursor(1, line);
            lcd.print('>');
            lcd.setCursor(18, line);
            lcd.print('<');
            waitForKeyRelease();
            break;

          case 1:
            EEPROM.get(address, user);
            time_t t = now();
            int timeNow = (hour(t) * 100) + minute(t);
            if (user.working == false) {
              User uTemp = {
                user.userID, user.password, user.admin, true, timeNow, user.finish
              };
              Serial.println("Welcome");
              adminDoor = true;
              EEPROM.put(address, uTemp);
              working = uTemp.working;
              sendToTerminal(user.userID, hour(t), minute(t), true);



            } else {
              User uTemp = {
                user.userID, user.password, user.admin, false, user.start, timeNow
              };
              Serial.println("Bye Bye");
              adminDoor = false;
              EEPROM.put(address, uTemp);
              working = uTemp.working;
              sendToTerminal(user.userID, hour(t), minute(t), false);
            }
            lcd.clear();
            if (working) {
              printAdminMenu("End");
            } else {
              printAdminMenu("Start");
            }
            lcd.setCursor(1, line);
            lcd.print('>');
            lcd.setCursor(18, line);
            lcd.print('<');
            waitForKeyRelease();
            break;
        }
        waitForKeyRelease();
        break;

      case 0:
        Check();
        break;
    }
  }
  delay(100);
}

void printAdminMenu(String stat) {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("AC");
  lcd.setCursor(7, 1);
  lcd.print(stat);
}

void setTemp() {
  temp1 = EEPROM.read(0);
  if (temp1 >= 100) {
    temp1 = 20;
  }
  printSetTemp(temp1);
  int t2 = temp1;
  bool st = true;
  while (st && getKeyState() != 4) {
    switch (getKeyState()) {
      case 1:
        if (t2 < 99) {
          t2++;
        }
        printSetTemp(t2);
        waitForKeyRelease();
        break;
      case 2:
        if (t2 > 0) {
          t2--;
        }
        printSetTemp(t2);
        waitForKeyRelease();
        break;
      case 3:
        temp1 = t2;
        EEPROM.write(0, t2);
        st = false;
        waitForKeyRelease();
        break;
      case 0:
        Check();
        break;
    }
  }
}

void printSetTemp(int t2) {
  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print("-");
  if (t2 < 10) {
    lcd.setCursor(6, 1);
    lcd.print(t2);
  } else {
    lcd.setCursor(5, 1);
    lcd.print(t2);
  }
  lcd.setCursor(8, 1);
  lcd.print("+");
}


void userMode(int temp) {
  if (enterPassword(temp)) {
    userModeMenu(temp);
  } else {
    lcd.clear();
    lcd.setCursor(3, 1);
    lcd.print("Wrong Password!");
    delay(500);
  }
  return;
}

void userModeMenu(int temp) {
  User user;
  int address = 30 + (temp * 12);
  EEPROM.get(address, user);
  bool working = user.working;
  time_t t = now();
  int timeNow = (hour(t) * 100) + minute(t);
  printUserEnter(working);
  while (getKeyState() != 4) {
    switch (getKeyState()) {
      case 3:
        EEPROM.get(address, user);
        if (user.working == false) {
          User uTemp = {
            user.userID, user.password, user.admin, true, timeNow, user.finish
          };
          Serial.println("Welcome");
          EEPROM.put(address, uTemp);
          working = uTemp.working;
          sendToTerminal(user.userID, hour(t), minute(t), true);
        } else {
          User uTemp = {
            user.userID, user.password, user.admin, false, user.start, timeNow
          };

          Serial.println("Goodbye");
          EEPROM.put(address, uTemp);
          working = uTemp.working;
          sendToTerminal(user.userID, hour(t), minute(t), false);
        }
        printUserEnter(working);
        break;
      case 0:
        Check();
        break;
    }
  }
}
void printUserEnter(bool working) {
  lcd.clear();
  lcd.setCursor(7, 0);

  if (working) {
    lcd.print("End");
  } else {
    lcd.print("Start");
  }
  lcd.setCursor(1, 0);
  lcd.print('>');
  lcd.setCursor(18, 0);
  lcd.print('<');
}



void setAirConditioner() {
  int temp = 0;
  printAirConditionerMenu(temp);
  while (getKeyState() != 4) {

    switch (getKeyState()) {
      case 1:
        if (temp > 0) {
          temp = temp - 1;
        }
        lcd.clear();
        printAirConditionerMenu(temp);

        waitForKeyRelease();
        break;

      case 2:
        if (temp < 6) {
          temp = temp + 1;
        }
        lcd.clear();
        printAirConditionerMenu(temp);

        waitForKeyRelease();
        break;

      case 3:
        Plan plan;
        int address = 1 + (temp * 4);
        EEPROM.get(address, plan);
        changeAirConditioner(address);
        printAirConditionerMenu(temp);
        waitForKeyRelease();
        break;

      case 0:
        Check();
        break;
    }
  }
  delay(50);
}

void printAirConditionerMenu(int dayNum) {
  lcd.clear();

  lcd.setCursor(1, 0);
  lcd.print('>');
  lcd.setCursor(18, 0);
  lcd.print('<');

  Plan plan;
  for (int i = 0; i < 4; i++) {
    int address = 1 + ((dayNum + i) * 4);
    EEPROM.get(address, plan);

    lcd.setCursor(6, i);
    switch (address) {
      case 1:
        lcd.print("Saturday");
        break;
      case 5:
        lcd.print("Sunday");
        break;
      case 9:
        lcd.print("Monday");
        break;
      case 13:
        lcd.print("Tuesday");
        break;
      case 17:
        lcd.print("Wednesday");
        break;
      case 21:
        lcd.print("Thursday");
        break;
      case 25:
        lcd.print("Friday");
        break;
    }
  }
}
void changeAirConditioner(int address) {
  Plan plan;
  EEPROM.get(address, plan);

  printChangeAirConditioner();
  int sMin = plan.start % 100;
  int sHour = plan.start / 100;
  lcd.setCursor(3, 1);
  lcd.print(sHour);
  lcd.setCursor(6, 1);
  lcd.print(sMin);
  int fMin = plan.finish % 100;
  int fHour = plan.finish / 100;
  lcd.setCursor(12, 1);
  lcd.print(fHour);
  lcd.setCursor(15, 1);
  lcd.print(fMin);
  int sM, sH, fM, fH;

  for (int t1 = 0; t1 < 4 && getKeyState() != 4; t1++) {
    int t2 = 0;
    bool st = true;
    int col;
    switch (t1) {
      case 0:
        col = 3;
        t2 = sHour;
        break;
      case 1:
        col = 6;
        t2 = sMin;
        break;
      case 2:
        col = 12;
        t2 = fHour;
        break;
      case 3:
        col = 15;
        t2 = fMin;
        break;
    }
    while (st && getKeyState() != 4) {
      lcd.setCursor(col, 2);
      lcd.print("^");
      switch (getKeyState()) {
        case 1:
          if (((t1 == 0) || (t1 == 2)) && (t2 < 23)) {
            t2++;
          } else if (((t1 == 1) || (t1 == 3)) && (t2 < 59)) {
            t2++;
          }

          if (t2 > 9) {
            lcd.setCursor(col, 1);
            lcd.print(t2);
          } else {
            lcd.setCursor(col + 1, 1);
            lcd.print(t2);
          }
          waitForKeyRelease();
          break;
        case 2:
          if (t2 > 0) {
            t2--;
          }
          if (t2 > 9) {
            lcd.setCursor(col, 1);
            lcd.print(t2);
          } else {
            lcd.setCursor(col, 1);
            lcd.print("0");
            lcd.setCursor(col + 1, 1);
            lcd.print(t2);
          }
          waitForKeyRelease();
          break;
        case 3:
          switch (t1) {
            case 0:
              sH = t2;
              break;
            case 1:
              sM = t2;
              break;
            case 2:
              fH = t2;
              break;
            case 3:
              fM = t2;
              int sTemp = (sH * 100) + sM;
              int fTemp = (fH * 100) + fM;
              Plan pTemp = {
                sTemp, fTemp
              };
              EEPROM.put(address, pTemp);
              lcd.clear();
              lcd.setCursor(5, 1);
              lcd.print("Plan Set");
              break;
          }
          waitForKeyRelease();
          st = false;
          break;

        case 0:
          Check();
          break;
      }
    }
  }
}

void printChangeAirConditioner() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Start");
  lcd.setCursor(5, 1);
  lcd.print(":");
  lcd.setCursor(12, 0);
  lcd.print("End");
  lcd.setCursor(14, 1);
  lcd.print(":");
}

void Check() {
//  9 , 8 , 7 , 6 , 5 ,  4 , corridor , restaurant
  getBinary();
  temp1 = EEPROM.read(0);
  readFromTerminal();
  int ct = (long)analogRead(A1) * 0.5;
  if (adminDoor) {
    digitalWrite(3, HIGH);
  } else {
    if (digitalRead(10) == HIGH) {
      digitalWrite(2, HIGH);
    }
    digitalWrite(3, LOW);
  }

  if (isEmpty()) {
    for (int k = 0; k < 8; k++) {
      if (stat[k] == 1) {
        digitalWrite(2, HIGH);
      }
    }
    time_t t = now();
    int address = 1 + (weekday(t) * 4);
    Plan plan;
    EEPROM.get(address, plan);
    int sMin = plan.start % 100;
    int sHour = plan.start / 100;
    int fMin = plan.finish % 100;
    int fHour = plan.finish / 100;
    if (((sHour < hour(t)) || ((sHour == hour(t)) && (sMin <= minute(t))))
        && ((fHour > hour(t)) || ((fHour == hour(t)) && (fMin >= minute(t))))) {
      digitalWrite(8, HIGH);
    } else {
      digitalWrite(8, LOW);
    }
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);

    if (times[3] > 70) {
      digitalWrite(4, LOW);
      times[3] = 0;
    } else {
      times[3] = times[3] + 1;
    }
  } else {
    digitalWrite(11, HIGH);
    if (stat[6] == 1) {
      digitalWrite(12, HIGH);
      times[2] = 0;
    } else {
      if (times[2] > 50) {
        digitalWrite(12, LOW);
        times[2] = 0;
      } else {
        times[2] = times[2] + 1;
      }
    }
    digitalWrite(8, HIGH);
    digitalWrite(4, HIGH);
  }
  if (ct < temp1) {
    digitalWrite(9, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
  } else if (ct > temp1 && ct <= (temp1 + 4)) {
    digitalWrite(9, LOW);
    digitalWrite(5, LOW);
    digitalWrite(7, HIGH);
    if (times[0] > 40) {
      digitalWrite(6, HIGH);
      times[0] = 0;
    } else {
      times[0] = times[0] + 1;
    }
  } else if (ct > (temp1 + 4)) {
    digitalWrite(9, LOW);
    // digitalWrite(5, LOW);
    digitalWrite(7, HIGH);
    if (times[0] > 40) {
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
      times[0] = 0;
    } else {
      times[0] = times[0] + 1;
    }
  } else if (ct == temp1) {
    digitalWrite(9, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
  }
  delay(1);
}
bool isEmpty() {
  for (int i = 0; i < 6; i++) {
    User user;
    int address = 30 + (i * 12);
    EEPROM.get(address, user);
    if (user.working) {
      return false;
    }
  }
  return true;
}
byte readWire() {

  int n = Wire.requestFrom(0x22, 1);
  if (n == 1) {
    byte st = Wire.read();
    return st;
  }
}
void getBinary() {
  int num = readWire();
  int numTemp = num;
  int i;
  for (i = 0; numTemp > 0; i++) {
    numTemp = numTemp / 2;
  }
  for (int j = 0; j < 8; j++) {
    if (j < (8 - i)) {
      stat[j] = 0;
    }
  }
  for (int j = 7; j >= 8 - i; j--) {
    stat[j] = num % 2;
    num = num / 2;
  }
}
void sendToTerminal(long userID, int hour1, int min1, bool in) {
  String message = "PERSON " + String(userID) + "," + String(hour1) + ":" + String(min1) + ",";
  if (in) {
    message += "IN\r";
  } else {
    message += "OUT\r";
  }
  Serial.println(message);
}

void readFromTerminal() {

  if (Serial.available()) {
    String message = Serial.readStringUntil('\r');
    int len = message.length();
    char m[len];
    message.toCharArray(m, len);
    if (m[0] == 'S', m[1] == 'E', m[0] == 'T') {
      for (int i = 4; i < len; i += 14) {
        int j;
        long userID = 0;
        int password = 0;
        bool role = false;
        for (j = 0; j < 6; j++) {
          userID *= 10;
          userID += m[j + i];
        }
        for (j = 7; j < 11; j++) {
          password *= 10;
          password += m[j + i];
        }
        if (m[i + 12] == 'M') {
          role = true;
        } else if (m[i + 12] == 'A') {
          role = false;
        }
        lastAdd = EEPROM.read(29) + 12;
        User uTemp = {
          userID, password, role, false, 0, 0
        };
        EEPROM.put(lastAdd, uTemp);
        EEPROM.write(29, lastAdd);
      }
    }
  }
}

void waitForKeyRelease() {
  do {
    while (getKeyState() != 0) {}
    delay(20);
  } while (getKeyState() != 0);
}
String place[8]= {"Lobby","Gathering room","Fix","Produce room","sell room","WC","coridor","rsturant"}; 
void printTermal (){
for  (int i =0; i<8;i++){
if(stat[i] == 1){
  Serial.print("Sensor  of ");
    Serial.print(place[i]);
  Serial.println(" Buzzing ");
  

}  }
}