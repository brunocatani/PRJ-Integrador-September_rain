#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>


#define WIFI_SSID "PROF.RAFAEL"
#define WIFI_PASSWORD ""
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define chuvaAnalog A0 //Pino Analógico
#define chuvaDigital 16 //Pino digital
// Credenciais  
#define AUTHOR_EMAIL "eduardodallarosa321@gmail.com"
#define AUTHOR_PASSWORD "ihrihnikelqslvwf"
// Para
#define RECIPIENT_EMAIL "manugrafff@gmail.com"
// Sessão SMTP usado para envio de e-mail
SMTPSession smtp;

bool test = true;
int time2;

//Função
void Email(){
  // Habilita o debug via porta serial
  // * nenhum debug - 0
  // * depuração básica - 1
  smtp.debug(1);
  // Declara os dados de configuração da sessão 32
  ESP_Mail_Session session;
  // Set configuração
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";
  // Declara a classe da mensagem
  SMTP_Message message;
  // Configura os cabeçalhos das mensagens
  message.sender.name = "ESP- 8266";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Teste Email";
  message.addRecipient("---", RECIPIENT_EMAIL);
  //Editar a mensagem com HTML
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>TESTE!</h1><p>- Chuvaaaaaaaaa </p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
}
void setup() {
  pinMode(chuvaDigital,INPUT); //Pino como entrada
  Serial.begin(9600);
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
void loop() {
  int chuvaAnalogVal = analogRead(chuvaAnalog);
  //Lendo a porta digital
  int chuvaDigitalVal = digitalRead(chuvaDigital);
  Serial.print(chuvaAnalogVal);
  Serial.print("\t");
  Serial.println(chuvaDigitalVal);
  delay(500);
  int time = (millis());
  

  if((time-time2)>300000){
    test = true;
  }

  if((chuvaDigitalVal==0)&&(test==true)){
    time2 = time;
    test = false;
    time = 0;
    Email();
  }
}