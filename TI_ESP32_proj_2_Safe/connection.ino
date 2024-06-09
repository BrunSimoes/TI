


//////////////////////////////////////// wifi connection
void setup_wifi(){
  Serial.print("Connecting to ");
  Serial.println(SSID_MY_WIFI);
  
  WiFi.disconnect(true);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_MY_WIFI, PASSWORD_MY_WIFI);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  randomSeed(micros());

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}


//////////////////////////////////////// connection to MQTT
void reconnect(){
  while(!client.connected()){
    delay(500);
    Serial.println("Connecting to MQTT");
    String clientID = "ESP32Client-";
    clientID += String(random(0xffff), HEX);

    if(client.connect(clientID.c_str()/* , MQTT_USER, MQTT_PASSWORD */)){
          Serial.println("MQTT connected!");
        client.subscribe(TOPIC_USER_REQ_AUTHGARAGEM);
        client.subscribe(TOPIC_PORTA_ACTION);
        client.subscribe(TOPIC_IMAGEM_PEDIR);
    }
    else{
      Serial.println("MQTT Failed!");
      Serial.println("trying in 5s");
      Serial.println(client.state());
      delay(5000);
    }
  }
}


//////////////////////////////////////// publicar msgs
void publish(const char* topic, String payload, boolean retained){
  if(client.publish(topic, payload.c_str(), true)){
    Serial.println("YES:" + *topic);
    Serial.println(payload);
  }
}


//////////////////////////////////////// callback quando se recebe topico
void callback(const char* topic, byte* payload, unsigned int length){
  String incommingMessage = "";
  for(int i = 0; i < length; i++){
    incommingMessage += (char) payload[i];
  }

  Serial.println("Recebido: [" + String(topic) + "] " + incommingMessage);

  if(strcmp(topic, TOPIC_PORTA_ACTION) == 0){
      Serial.println("A:");
      Serial.println(PORTA_STATE);
    if(incommingMessage.equals("abre-te sésamo") && PORTA_STATE==0)
    {  
      PORTA_STATE==2;
      //Frente();
      publish(TOPIC_PORTA_STATE, "abrindo", false);
    }
    else if(incommingMessage.equals("fecha-te chia") && PORTA_STATE==1)
    {  
      PORTA_STATE==3;
      //Atras();
      publish(TOPIC_PORTA_STATE, "fechando", false);
    }
  }
  else if(strcmp(topic, TOPIC_USER_REQ_AUTHGARAGEM) == 0){
    if(incommingMessage.equals("authreq_local1_garage1") && !OCUPADO){
      Serial.println("Enbiado");
      OCUPADO = true;
      publish(TOPIC_USER_RES_AUTHGARAGEM, "local1_garage1_sim", false);
    }else if (incommingMessage.equals("authreq_local1_garage1") && OCUPADO){
      publish(TOPIC_USER_RES_AUTHGARAGEM, "local1_garage1_nao", false);
    }
  }
}