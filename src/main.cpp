#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>
#include <ESPAsyncWebServer.h>


#define WIFI_SSID "Maria Duda"
#define WIFI_PASSWORD "uvoi6289"
IPAddress ip(192,168,1,69);//Mude o IP
IPAddress gateway(192,168,1,1); 
IPAddress subnet(255,255,255,0);
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

AsyncWebServer server(80);
AsyncEventSource events("/events");
//variável boleano de trava
//se inicia como true ao ligar o sensor para permitir o envio do primeiro email;

String processor(const String& var){

  return String();
}



const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>BME680 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1"; charset=utf-8>
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v6.2.0/css/all.css">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    i { font-size: 6em;}
    .topnav { overflow: hidden; background-color: #4B1D3F; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .card.chuva{ color: #0e7c7b; }
    .card.email { color: #17bebb; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>WEB SERVER</h3>
    Bruno Catani
    Emanuelli Graff
    Maria Eduarda Martinelli
    Eduardo Dallarosa
  </div>
  <div class="content">
    <div class="cards">
      <div class="card chuva">
        <h1><i class="fa-solid fa-cloud-rain"></i></h1><p><span class="reading"><span id="chuva">%STATUS_CHUVA% Não está Chovendo</span></span></p>
      </div>
      <div class="card email">
        <h1><i class="fa-solid fa-envelope"></i></h1><p><span class="reading"><span id="email">%STATUS_EMAIL%</span></span></p>
        <p><span id="end_email">%END_EMAIL%</span></p>
      </div>
  </div>
  <br> 
  <br> 
<div>
<iframe id="ytplayer" "width="400" height="300" src="http://www.youtube.com/embed/5TVFeUtwxzI?autoplay=0&mute=0&rel=0&enablejsapi=1" frameborder="0" allow="autoplay"> </iframe>
</div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('chuva', function(e) {
  console.log("chuva", e.data);
  document.getElementById("chuva").innerHTML = e.data;
 }, false);
  source.addEventListener('email', function(e) {
  console.log("email", e.data);
  document.getElementById("email").innerHTML = e.data;
 }, false);
 source.addEventListener('end_email', function(e) {
  console.log("end_email", e.data);
  document.getElementById("end_email").innerHTML = e.data;
 }, false);
                
}
</script>
</body>
</html>)rawliteral";

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
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 2000);
  });
  server.addHandler(&events);
  server.begin();
}
void loop() {
  int chuvaAnalogVal = analogRead(chuvaAnalog);
  //Lendo a porta digital
  int chuvaDigitalVal = digitalRead(chuvaDigital);
  Serial.print(chuvaAnalogVal);
  Serial.print("\t");
  Serial.println(chuvaDigitalVal);
  delay(500);

  //contador de tempo inicial

  int time = (millis());
  
  //Compara o tempo incial com o tempo registrado a partir da publicação do email, se > 5 minutos ---->
  if((time-time2)>50000){
    //modifica a "trava" para poder enviar o email
    test = true;
  }

  //Se o sensor indicar chuva e a variavel de trava permitir se envia um email
  if((chuvaDigitalVal==0)&&(test==true)){
    time2 = time;
    //variavel de trava é ativada para bloquear envio repetido de dados
    test = false;
    time = 0;

    events.send(String("Ta Chovendo").c_str(), "chuva",millis());
    events.send(String("Email Enviado").c_str(), "email",millis());
    // email é enviado
    Email();
  }
}