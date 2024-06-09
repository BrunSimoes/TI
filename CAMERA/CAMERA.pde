import processing.video.*;
import processing.serial.*;
import java.util.Base64;

import ch.bildspur.vision.*;
import ch.bildspur.vision.result.*;

import processing.serial.*;

import processing.core.PApplet;
import processing.core.PConstants;
import processing.core.PImage;
import processing.video.Capture;

DeepVision deepVision = new DeepVision(this);
YOLONetwork yolo;
ResultList<ObjectDetectionResult> detections;
String [] arrayAtivadores = {"person"}; 

int textSize = 12;

// 
boolean ative = false;
boolean captureImage = false;

PImage image;

Capture cam;
PImage capture;
boolean captureON = false;
Serial myPort;

void setup() {
  size(400, 400);
  
  colorMode(HSB, 360, 100, 100);
  yolo = deepVision.createYOLOv4Tiny();
  yolo.setup();
  
  //CAM
  cam = new Capture(this, "pipeline:autovideosrc");
  cam.start();
  
  String portName = Serial.list()[1];
  myPort = new Serial(this, portName, 115200);
  myPort.bufferUntil('\n');
  
  delay(2000);
}

void draw() {
  if(captureImage){
      getImage();
  }
  
  if (cam.available()) {
    cam.read();
  }

  image(cam, 0, 0);

  if (cam.width == 0) {
    return;
  }
  
  serialEvent(myPort);
 
}

//Serial
void serialEvent(Serial myPort){
  try{
    String val = myPort.readString();
    
    if(val!=null){
      if(val.equals("CONVERTER_IMAGE")){
         sendImageToArduino(capture);
         if(checkForPerson()){
           myPort.write("ALERTA_PESSOA");
         }
      }
    }
  }
  catch(Exception e){
    e.printStackTrace();
  }
}

void sendImageToArduino(PImage img) {
  img.loadPixels();
  
  // Converte a imagem para um array de bytes
  byte[] imgBytes = new byte[img.pixels.length * 3];
  int index = 0;
  for (int i = 0; i < img.pixels.length; i++) {
    color c = img.pixels[i];
    imgBytes[index++] = (byte)red(c);
    imgBytes[index++] = (byte)green(c);
    imgBytes[index++] = (byte)blue(c);
  }
  
  // Codifica os bytes da imagem em Base64
  String imgBase64 = Base64.getEncoder().encodeToString(imgBytes);
  
  // Envia a string Base64 via serial
  myPort.write("IMAGE:" + imgBase64);
}

boolean checkForPerson(){
  boolean personV = false;
  
    yolo.setConfidenceThreshold(0.5f);
  detections = yolo.run(cam);

  strokeWeight(3f);
  textSize(textSize);

  for (ObjectDetectionResult detection : detections) {
    int hue = (int)(360.0 / yolo.getLabels().size() * detection.getClassId());

    noFill();
    stroke(hue, 80, 100);
    rect(detection.getX(), detection.getY(), detection.getWidth(), detection.getHeight());

    fill(hue, 80, 100);
    rect(detection.getX(), detection.getY() - (textSize + 3), textWidth(detection.getClassName()) + 4, textSize + 3);

    fill(0);
    textAlign(LEFT, TOP);
    text(detection.getClassName(), detection.getX() + 2, detection.getY() - textSize - 3);
    
    for(int i = 0; i<arrayAtivadores.length; i++){  
      if(detection.getClassName().equals(arrayAtivadores[i]))personV = true;
    }
  }
  
  return personV; 
}

void getImage(){
    image = cam.get();
    captureImage = false;
}
