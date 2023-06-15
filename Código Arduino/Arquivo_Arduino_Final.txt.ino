//Inclusões das bibliotécas
#include <ESP8266WiFi.h> //Biblioteca para funcionamento do WiFi do ESP
#include <ESP8266WebServer.h> //Biblioteca para o ESP funcionar como servidor
#include <Wire.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

//Constantes 
const int TRIG = D3;
const int ECHO = D2;
const int LED = D4;
const int BUZZER = D6;
const int distancia_obstaculo = 20;

//Setup da conexão do Arduino
const char* ssid = "ESPM-ESTUDANTES";  // Rede WiFi
const char* password = "";  //Senha da Rede WiFi
ESP8266WebServer server(80); //server na porta 80

void setup(){
    Serial.begin(115200);       //inicialização do serial
    pinMode(TRIG, OUTPUT);      //Inicialização do Trigger do Sonar
    pinMode(ECHO, INPUT);       //Inicialização do ECHO do Sonar
    pinMode(LED, OUTPUT);       //Inicializaçao do LED
    pinMode(BUZZER, OUTPUT);    //Inicialização do Buzzer

    //Conexão do Arduino com o WI-FI 
    Serial.println();
    Serial.print("Conectando-se à rede Wi-Fi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) { //Aguarda até haver a conexão com o Arduino
        delay(500);
        Serial.print(".");
    }
    //Informa o IP em que está sendo hosteado e manda 
    Serial.println("");
    Serial.println("Conectado à rede Wi-Fi:");
    Serial.println("Endereço IP: " + WiFi.localIP().toString());
    server.on("/", handle_OnConnect); //Servidor recebe uma solicitação HTTP - chama a função handle_OnConnect
    server.onNotFound(handle_NotFound); //Servidor recebe uma solicitação HTTP não especificada - chama a função handle_NotFound
    server.begin(); //Inicializa o servidor
    Serial.println("Servidor HTTP inicializado");

}

void loop() {
  server.handleClient(); //Alterei de Server.handle_OnConnect() para server.handleClient()
}

void handle_OnConnect() {
  int distancia = calcularDistancia(TRIG, ECHO);
  String info = "";
  if (distancia <= 100) {
    digitalWrite(LED, HIGH);
    tone(BUZZER, 1000);
    delay(150);
    tone(BUZZER, 750);
    info += "Presença detectada! Alto Risco!\n";
    Serial.print(info);
    server.send(200, "text/html", EnvioHTML(distancia, "Alto")); //Aqui enviei a distância e o risco como parâmetros
  } else if (distancia > 100 && distancia <= 250) {
    digitalWrite(LED, HIGH);
    tone(BUZZER, 1000);
    delay(150);
    tone(BUZZER, 750);
    info += "Presença detectada! Médio Risco!\n";
    Serial.print(info);
    server.send(200, "text/html", EnvioHTML(distancia, "Médio")); //Aqui enviei a distância e o risco como parâmetros
  } else if (distancia > 250 && distancia < 345) {
    digitalWrite(LED, HIGH);
    tone(BUZZER, 1000);
    delay(150);
    tone(BUZZER, 750);
    info += "Presença detectada! Baixo Risco!\n";
    Serial.print(info);
    server.send(200, "text/html", EnvioHTML(distancia, "Baixo")); //Aqui enviei a distância e o risco como parâmetros
  }else if (distancia >=345){
    noTone(BUZZER);
    delay(500);
    server.send(200, "text/html", EnvioHTML(400, "Sem")); //Aqui enviei a distância e o risco como parâmetros
  }

}

//Método para calcular a distância em cm
int calcularDistancia(int pinotrig, int pinoecho) {
  digitalWrite(pinotrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinotrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinotrig, LOW);
  return pulseIn(pinoecho, HIGH) / 58;
}

void handle_NotFound() { //Função para lidar com o erro 404
  server.send(404, "text/plain", "Não encontrado"); //Envia o código 404, especifica o conteúdo como "text/plain" e envia a mensagem "Não encontrado"
}

String EnvioHTML(int distancia, String risco) { // Exibindo a página da web em HTML
String ptr = "<!DOCTYPE html>\n";
ptr += "<html>\n";
ptr += "<head>\n";
ptr += "<meta charset='UTF-8'>";
ptr += "<meta http-equiv='X-UA-Compatible' content='IE=edge'>";
ptr += "<meta http-equiv='refresh' content='7'>";
ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
ptr += "    <link href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css' rel='stylesheet'>\n";
ptr += "    <title>SISTEMA IPS</title>\n";
ptr += "    <style>\n";
ptr += "        body {\n";
ptr += "            background-color: #f5f5f5;\n";
ptr += "            padding-top: 50px;\n";
ptr += "            padding-bottom: 50px;\n";
ptr += "        }\n";
ptr += "        .container {\n";
ptr += "            max-width: 600px;\n";
ptr += "            margin: auto;\n";
ptr += "            background-color: white;\n";
ptr += "            border-radius: 5px;\n";
ptr += "            padding: 20px;\n";
ptr += "            box-shadow: 0px 0px 10px 0px rgba(0,0,0,0.1);\n";
ptr += "            margin-bottom: 50px;\n";
ptr += "        }\n";
ptr += "        #history {\n";
ptr += "            max-height: 200px;\n";
ptr += "            overflow-y: auto;\n";
ptr += "            display: none;\n";
ptr += "        }\n";
ptr += "        .btn {\n";
ptr += "            width: 100%;\n";
ptr += "            margin-top: 20px;\n";
ptr += "        }\n";
ptr += "        .close {\n";
ptr += "            float: right;\n";
ptr += "            font-size: 1.5rem;\n";
ptr += "            font-weight: 700;\n";
ptr += "            line-height: 1;\n";
ptr += "            color: #000;\n";
ptr += "            text-shadow: 0 1px 0 #fff;\n";
ptr += "            opacity: .5;\n";
ptr += "        }\n";
ptr += "    </style>\n";
ptr += "</head>\n";
ptr += "<body class='text-center'>\n";
ptr += "    <div class='container'>\n";
ptr += "        <h1>SISTEMA IPS</h1>\n";
ptr += "        <button class='btn btn-primary' onclick='detectPresence()'>Detectar Presença</button>\n";
ptr += "        <div id='alert' class='mt-3'></div>\n";
ptr += "        <button class='btn btn-secondary' onclick='showHistory()'>Mostrar Histórico</button>\n";
ptr += "        <button class='btn btn-danger' onclick='clearHistory()' style='display: none;'>Limpar Histórico</button>\n";
ptr += "        <div id='history' class='mt-3'>\n";
ptr += "            <button type='button' class='close' onclick='closeHistory()'>&times;</button>\n";
ptr += "            <table class='table table-striped'>\n";
ptr += "                <thead>\n";
ptr += "                    <tr>\n";
ptr += "                        <th>Data e Hora</th>\n";
ptr += "                        <th>Distância</th>\n";
ptr += "                        <th>Risco</th>\n";
ptr += "                    </tr>\n";
ptr += "                </thead>\n";
ptr += "                <tbody>\n";
ptr += "                </tbody>\n";
ptr += "            </table>\n";
ptr += "        </div>\n";
ptr += "    </div>\n";
ptr += "    <div class='container'>\n";
ptr += "        <h2>Informações sobre o Risco de Detecção</h2>\n";
ptr += "            <p><strong>Alto Risco:</strong> Distância ≤ 1m</p>\n";
ptr += "            <p><strong>Médio Risco:</strong> Distância > 1m e ≤ 2,5m</p>\n";
ptr += "            <p><strong>Baixo Risco:</strong> Distância> 2,5m e ≤ 4m</p>\n";
ptr += "    </div>\n";
ptr += "    <script>\n";
ptr += "        var detectionHistory = [];\n";
ptr += "        var historyVisible = false;\n";
ptr += "        \n";
ptr += "        function detectPresence() {\n";
ptr += "            var distance = " ;
ptr += distancia;
ptr += " / 100.0;\n";
ptr += "            var alertDiv = document.getElementById('alert');\n";
ptr += "            var risk;\n";
ptr += "            var alertClass;\n";
ptr += "            if (distance <= 1) {\n";
ptr += "        \n";
ptr += "                risk = 'Alto';\n";
ptr += "                alertClass = 'alert-danger';\n";
ptr += "            } else if (distance <= 2.5) {\n";
ptr += "                risk = 'Médio';\n";
ptr += "                alertClass = 'alert-warning';\n";
ptr += "            } else {\n";
ptr += "                risk = 'Baixo';\n";
ptr += "                alertClass = 'alert-success';\n";
ptr += "            }\n";
ptr += "            var detection = {\n";
ptr += "                time: new Date(),\n";
ptr += "                distance: distance.toFixed(2),\n";
ptr += "                risk: risk\n";
ptr += "            };\n";
ptr += "            detectionHistory.push(detection);\n";
ptr += "            alertDiv.innerHTML = `<div class='alert ${alertClass}'>Presença detectada! Nível de risco: ${risk} ${distance.toFixed(2)}m</div>`;\n";
ptr += "            if (historyVisible) {\n";
ptr += "                showHistory(); // Update the history table\n";
ptr += "            }\n";
ptr += "        }\n";
ptr += "        \n";
ptr += "        function showHistory() {\n";
ptr += "            historyVisible = true;\n";
ptr += "            var historyDiv = document.getElementById('history');\n";
ptr += "            var historyBody = historyDiv.getElementsByTagName('tbody')[0];\n";
ptr += "            historyBody.innerHTML = '';\n";
ptr += "            for (var i = 0; i < detectionHistory.length; i++) {\n";
ptr += "                var row = historyBody.insertRow();\n";
ptr += "                var cell1 = row.insertCell(0);\n";
ptr += "                var cell2 = row.insertCell(1);\n";
ptr += "                var cell3 = row.insertCell(2);\n";
ptr += "                cell1.innerHTML = detectionHistory[i].time.toLocaleString('pt-BR');\n";
ptr += "                cell2.innerHTML = detectionHistory[i].distance + 'm';\n";
ptr += "                cell3.innerHTML = detectionHistory[i].risk;\n";
ptr += "            }\n";
ptr += "            historyDiv.style.display = 'block';\n";
ptr += "            document.querySelector('.btn-danger').style.display = 'inline-block';\n";
ptr += "        }\n";
ptr += "        \n";
ptr += "        function clearHistory() {\n";
ptr += "            detectionHistory = [];\n";
ptr += "            document.getElementById('history').style.display = 'none';\n";
ptr += "            document.querySelector('.btn-danger').style.display = 'none';\n";
ptr += "        }\n";
ptr += "        \n";
ptr += "        function closeHistory() {\n";
ptr += "            historyVisible = false;\n";
ptr += "            document.getElementById('history').style.display = 'none';\n";
ptr += "            document.querySelector('.btn-danger').style.display = 'none';\n";
ptr += "        }\n";
ptr += "        </script>\n";
ptr += "</body>\n";
ptr += "</html>\n";
return ptr;
}
