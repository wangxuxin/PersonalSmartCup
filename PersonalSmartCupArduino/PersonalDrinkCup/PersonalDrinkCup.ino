String cdata = "";
String cmd[100];
unsigned long previousMillis = 0;

// 电子秤ADC（HX771AD）对应的引脚号
const int VT  = 11;
const int WSCK = 12;

const int light1=2;//电热器指示灯
const int light2=3;//红 水是否能喝
const int light3=4;//绿 同上
const int temp1=A3;//杯温
const int temp2=A4;//水温
int delaytime=1000;//串口发送间隔时间
long weight0 = 16740001;//当重量为0时的数值
long weight50 = 10000;//每增加50g数值减少

// 称重函数
long weight(void){
  long data = 0;   // 压力传感器ADC数值
  int i;          // 循环计数器
  digitalWrite(WSCK, LOW);   // 拉低时钟，准备读取ADC
  while(digitalRead(VT));   // 等待数据恢复低电平（防止ADC忙）
  // 逐为读取ADC数据（从高位到低位）
  for(i = 0; i < 24; i++){
    data <<= 1;
    digitalWrite(WSCK, HIGH);
    data += digitalRead(VT);
    digitalWrite(WSCK, LOW); 
  }
  // 结束脉冲的个数将决定下一个数据的采样通道和增益
  // ....一个结束脉冲——通道A，增益128
  // ....两个结束脉冲——通道B，增益64
  // ....三个结束脉冲——通道A，增益64
  for(i = 0; i < 3; i++){   // 通道A，增益64
    digitalWrite(WSCK, HIGH);
    digitalWrite(WSCK, LOW); 
  }
  
  return data;
}

long getweight(){
  //return weight();
  return (weight0-weight())*50/weight50;
}

int gettemp(int io){
  return analogRead(io)*330/1023;
}

void sendSerial(){
  Serial.print("weight:");
  Serial.print(getweight());
  Serial.print(" ");
  Serial.print("temp1:");
  Serial.print(gettemp(temp1));
  Serial.print(" ");
  Serial.print("temp2:");
  Serial.print(gettemp(temp2));
  Serial.print("\n");
}

void setup() {
  pinMode(VT,  INPUT);
  pinMode(WSCK, OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  // 串口初始化，波特率9600
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis-previousMillis>=delaytime){
    sendSerial();
    previousMillis=currentMillis;
  }

  while (Serial.available() > 0)  
    {
        cdata += char(Serial.read());
        delay(2);
    }
  char delims[] = " ";
  char *result = NULL;
  int i = 0;
  result = strtok( cdata.c_str(), delims );
  while( result != NULL ) {
    cmd[i]=result;
    i++;
    result = strtok( NULL, delims );
  }
  if(cmd[0]!=""){
    Serial.println(cmd[0]);
  }

  if(cmd[0]=="temp1high"){
    digitalWrite(light2,LOW);
    digitalWrite(light3,HIGH);
  }
  if(cmd[0]=="temp1ok"){
    digitalWrite(light3,LOW);
    digitalWrite(light2,HIGH);
  }
  if(cmd[0]=="temp2ok"){
    digitalWrite(light1,LOW);
  }
  if(cmd[0]=="temp2low"){
    digitalWrite(light1,HIGH);
  }
  
  cleanVar();
}

void cleanVar(){
  cdata="";
  if(cmd[0]!=""){
    for(int i=0;i<=100;i++){
      cmd[i]="";
    }
  }
}
