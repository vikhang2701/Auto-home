#define BLYNK_TEMPLATE_ID "TMPL6PEqrHY_b"
#define BLYNK_TEMPLATE_NAME "CNCB"
#define BLYNK_AUTH_TOKEN "m2ok08ypanuImlOreu-SH8COfDiBa8Gl"
// Dòng 1,2,3 là mã của blynk cung cấp cho đoạn code để có thể điều khiển được chương trình
#define BLYNK_PRINT Serial //các thông báo từ thư viện Blynk sẽ được gửi đến cổng Serial để hiển thị trên Serial Monitor để theo dõi

#include <WiFi.h> // các thư viện wifi
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h> //  thư viện Blynk 
#include <DHTesp.h> // thư viện DHT
#include <ESP32Servo.h> // thư viện ESP32 để điều khiển servo

char auth[] = BLYNK_AUTH_TOKEN ; 
char ssid[] = "Khang"; // Tên SSID của wifi chúng ta sử dụng 
char pass[] = "20161208@@"; // mật khẩu của SSID chúng ta sử dụng nhưng vì mô phỏng nên nhóm em không đặt pass

const int DHT_PIN = 15; // khai báo chân CỦA DHT11 kết nối với ESP32
#define relay1 26 //den 220
#define relay2 27 //quat
int servoPin = 13; // cua chinh
int servo1Pin = 12; // cua so
int sts ; // khai báo biến trạng thái ( auto, manual) kiểu số nguyên.
Servo myServo; // khai báo biến điều khiển servo dùng để mở cửa chính
Servo servo1; // khai báo biến điều khiển servo dùng để mở góc của sổ 
DHTesp dht; // khai báo biến dht trong thư viện DHTesp dùng để đọc dữ liệu nhiệt độ và độ ẩm.
BlynkTimer timer; // thiết lập bộ timer


void sendSensor() // chương trình con DHT22
{
 TempAndHumidity  data = dht.getTempAndHumidity(); // đọc giá trị độ ẩm và giá trị nhiệt độ và lưu vào biến data 
//Các câu lệnh dưới đây dùng để ghi giá trị đọc được từ cảm biến và hiện thị lên màn hình monitor.
Serial.print(" Nhiet do: "); // xuất ra ở trên màn hình monitor là Nhiet do
Serial.print(data.temperature);
Serial.println("°C "); // xuất ra ở trên màn hình moniotr là °C
Serial.print(" Do am: ");
Serial.print(data.humidity);
Serial.println("% ");
Blynk.virtualWrite(V0, data.temperature); //Ghi nhiệt độ lên chân ảo, ở đây chân ảo được thiết lập là V0 trong Blynk. 
Blynk.virtualWrite(V1, data.humidity); //Ghi  độ ẩm lên chân ảo, ở đây chân ảo được thiết lập là V1 trong Blynk
}
// Hàm này sẽ được gọi mỗi khi widget nút nhấn trên phần mềm Blynk được nhấn
// và gửi lệnh bật tắt vào Virtual Pin V2
BLYNK_WRITE(V2) //Hàm thực hiện điều khiển den.
 {
  bool value1 = param.asInt();
  // kiểm tra trên blynk nút nhấn có được nhấn hay không.
  if (value1 == 1) {
    digitalWrite(relay1, HIGH); // mở đèn 
  } else {
    digitalWrite(relay1, LOW); // tắt đèn 
  }
}
BLYNK_WRITE(V3) //Hàm thực hiện điều quat.
 {
  bool value2 = param.asInt();
  // kiểm tra trên blynk nút nhấn có được nhấn hay không.
  if (sts == 0){ // MANUAL
     digitalWrite(relay2, LOW); // tắt quạt
  if (value2 == 1) {
    digitalWrite(relay2, HIGH); // mở quạt
  } else {
    digitalWrite(relay2, LOW);
  }
}
}
BLYNK_WRITE(V4)
{
  bool value3 = param.asInt();
  if (value3 == 1) 
  {
   myServo.write(15); // mở cửa
  } else {
   myServo.write(155); // đóng cửa
  }
  //int s0 = param.asInt();
  //myServo.write(s0);
  //Blynk.virtualWrite(V4,s0);
}
BLYNK_WRITE(V5) // chương trình điều khiển mở cửa sổ theo góc mình muốn
{
  int s0 = param.asInt(); // nhận giá trị từ blynk gửi xuống esp32 
  servo1.write(s0); // điều khiển góc servo theo giá trị từ blynk gửi xuống
  Blynk.virtualWrite(V5,s0); // gửi giá trị mở ra theo góc từ esp32 lên Blynk
}
BLYNK_WRITE(V6)  // chương trình điều khiển 2 chế độ auto và manual
{
  bool value4 = param.asInt();
  if (value4 == 1) {
    sts = 1; // auto
  } else {
    sts = 0; // manual
  }
}
void setup()
{
Serial.begin(115200); 
dht.setup(DHT_PIN, DHTesp::DHT11); //Khai báo loại cảm biến, cụ thể là cảm biến nhiệt độ độ ẩm DHT11
myServo.attach(servoPin); //Kết nối ESP32 Servo với chân 13
servo1.attach(servo1Pin); //Kết nối ESP32 Servo với chân 12
pinMode(relay1, OUTPUT); // thiết lập relay là ngõ ra.
pinMode(relay2, OUTPUT); 
Blynk.begin(auth, ssid, pass); //Khởi động Blynk, Cấu hình và App Blynk kết nối với Wifi.
timer.setInterval(1000, sendSensor); //được sử dụng để thực hiện việc đọc dữ liệu từ cảm biến và hiển thị trên ứng dụng Blynk mỗi 1 giây.
digitalWrite(relay1, LOW);
digitalWrite(relay2, LOW);
}
void loop()
{
  float a = dht.getTemperature(); // lấy giá trị nhiệt độ từ DHT11 và gán vào giá trị a kiểu dữ liệu số thực
  if (sts == 1) // kiểm tra xem có ở chế độ auto 
  {
  if (a>=30) // kiểm tra nhiệt độ có lớn hơn hoặc bằng 30 độ 
  {
    digitalWrite(relay2, HIGH); // mở quạt
  }
  else
  {
    digitalWrite(relay2, LOW); // tắt quạt
  }
  }

  

Blynk.run(); // chạy chương trình
timer.run(); //chạy bộ timer
}
