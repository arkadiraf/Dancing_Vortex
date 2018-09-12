// Version 0.0.6

import processing.serial.*;
import controlP5.*;

import ddf.minim.*;
import ddf.minim.spi.*; // for AudioRecordingStream
import ddf.minim.ugens.*;

Minim minim;
TickRate rate_control_1;
TickRate rate_control_2;
FilePlayer file_player_1;
FilePlayer file_player_2;
AudioOutput out;

Serial wheel_serial;
Serial tornado_serial;
ControlP5 cp5;

String serial_start_string = "$";
String serial_delimiter_string = ",";
String serial_end_string = "\r\n";

int led_mode = 0;
int work_mode = 0;
int max_sine_value = 255;

float gui_gap = 0;
float slider_width = 0;
float slider_height = 40;

float toggle_width = 0;
float toggle_height = 40;

int pump_1_value = 0;
int pump_2_value = 0;
boolean fan_value = false;
boolean mist_value = false;
int led_value = 0;

boolean communicate = false;

float base_height = 0.0;
float spread = 0.0;

int current_wheel_value = 0;
int max_wheel_value = 0;

void setup() {
  size(600, 600);
  frameRate(30);
  printArray(Serial.list());
  //println(Serial.list());
  if (Serial.list().length>0) {
    tornado_serial = new Serial(this, Serial.list()[1], 57600);
    tornado_serial.bufferUntil(10); // 10 == lf
    if (Serial.list().length>1) {
      wheel_serial = new Serial(this, Serial.list()[0], 9600);
      wheel_serial.bufferUntil(10); // 10 == lf
    }
  }

  gui_gap = min(width, height)*0.075;
  slider_width = min(width, height)*0.85;
  toggle_width = min(width, height)*0.39;

  cp5 = new ControlP5(this);

  cp5.addSlider("pump_1_value")
    .setPosition(gui_gap, gui_gap)
    .setSize((int)slider_width, (int)slider_height)
    .setRange(0, max_sine_value)
    ;

  cp5.addSlider("pump_2_value")
    .setPosition(gui_gap, gui_gap*2+(int)slider_height)
    .setSize((int)slider_width, (int)slider_height)
    .setRange(0, max_sine_value)
    ;

  cp5.addToggle("fan_value")
    .setPosition(gui_gap, gui_gap*4+(int)toggle_height)
    .setSize((int)toggle_width, (int)toggle_height)
    ;

  cp5.addToggle("mist_value")
    .setPosition(gui_gap*2+(int)toggle_width, gui_gap*4+(int)toggle_height)
    .setSize((int)toggle_width, (int)toggle_height)
    ;

  /*cp5.addToggle("led_value")
   .setPosition(gui_gap, gui_gap*6+(int)toggle_height)
   .setSize((int)toggle_width, (int)toggle_height)
   ;*/

  cp5.addToggle("communicate")
    .setPosition(gui_gap*2+(int)toggle_width, gui_gap*6+(int)toggle_height)
    .setSize((int)toggle_width, (int)toggle_height)
    ;

  cp5.addTextlabel("led_label")
    .setText("LED mode")
    .setPosition(gui_gap, -toggle_height*0.5+gui_gap*8+(int)toggle_height)
    .setColorValue(0xffffff00)
    .setFont(createFont("Georgia", 10))
    ;

  cp5.addButtonBar("led_mode_bar")
    .setPosition(gui_gap, gui_gap*8+(int)toggle_height)
    .setSize((int)slider_width, (int)toggle_height)
    .addItems(split("0 1 2 3 4", " "))
    ;  

  cp5.addTextlabel("work_mode_label")
    .setText("Work mode")
    .setPosition(gui_gap, -toggle_height*0.5+gui_gap*10+(int)toggle_height)
    .setColorValue(0xffffff00)
    .setFont(createFont("Georgia", 10))
    ;

  cp5.addButtonBar("work_mode_bar")
    .setPosition(gui_gap, gui_gap*10+(int)toggle_height)
    .setSize((int)slider_width, (int)toggle_height)
    .addItems(split("0 1 2 3 4 5 6 7 8 9 10 11 12", " "))
    ;

  cp5.getController("pump_1_value").getValueLabel().align(ControlP5.LEFT, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0);
  cp5.getController("pump_1_value").getCaptionLabel().align(ControlP5.RIGHT, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0);

  cp5.getController("pump_2_value").getValueLabel().align(ControlP5.LEFT, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0);
  cp5.getController("pump_2_value").getCaptionLabel().align(ControlP5.RIGHT, ControlP5.BOTTOM_OUTSIDE).setPaddingX(0);

  // create our Minim object for loading audio
  minim = new Minim(this);

  // this opens the file and puts it in the "play" state.                           
  file_player_1 = new FilePlayer( minim.loadFileStream("two_crazy.mp3") );
  file_player_2 = new FilePlayer( minim.loadFileStream("two_crazy.mp3") );
  // and then we'll tell the recording to loop indefinitely
  file_player_1.loop();
  file_player_2.loop();

  // this creates a TickRate UGen with the default playback speed of 1.
  // ie, it will sound as if the file is patched directly to the output
  rate_control_1 = new TickRate(1.f);
  rate_control_2 = new TickRate(1.f);

  // get a line out from Minim. It's important that the file is the same audio format 
  // as our output (i.e. same sample rate, number of channels, etc).
  out = minim.getLineOut();

  // patch the file player through the TickRate to the output.
  file_player_1.patch(rate_control_1).patch(out);

  rate_control_1.setInterpolation( true );
}



void draw() {
  background(127);

  switch(work_mode) {
  case 0: 
    update_values();
    break;

  case 1: 
    pump_1_value = (int)(max_sine_value*0.5+max_sine_value*0.5*sin(millis()*0.0001));
    pump_2_value = (int)(max_sine_value*0.5+max_sine_value*0.5*cos(millis()*0.0001));
    update_values();
    break;

  case 2: 
    pump_1_value = (int)(max_sine_value*0.5+max_sine_value*0.5*sin(millis()*0.01));
    pump_2_value = (int)(max_sine_value*0.5+max_sine_value*0.5*cos(millis()*0.01));
    update_values();
    break;

  case 3: 
    pump_1_value = (int)(max_sine_value*0.5+max_sine_value*0.5*sin(millis()*0.1));
    pump_2_value = (int)(max_sine_value*0.5+max_sine_value*0.5*cos(millis()*0.1));
    update_values();
    break;

  case 4: 
    pump_1_value = (int)(max_sine_value*0.5+max_sine_value*0.5*sin(millis()*0.008));
    pump_2_value = (int)(255-(max_sine_value*0.5+max_sine_value*0.5*sin(millis()*0.008)));
    update_values();
    break;

  case 5: 
    base_height = 0.5;
    spread = min(base_height, 1-base_height);
    pump_1_value = (int)(max_sine_value*base_height+max_sine_value*spread*sin(millis()*0.01));
    pump_2_value = (int)((max_sine_value*base_height+max_sine_value*spread*sin(PI+millis()*0.01)));
    update_values();
    break;

  case 6: 
    base_height = 0.75;
    spread = min(base_height, 1-base_height);
    pump_1_value = (int)(max_sine_value*base_height+max_sine_value*spread*sin(millis()*0.01));
    pump_2_value = (int)((max_sine_value*base_height+max_sine_value*spread*sin(PI+millis()*0.01)));
    update_values();
    break;

  case 7: 
    base_height = 0.15;
    spread = min(base_height, 1-base_height);
    pump_1_value = (int)(max_sine_value*base_height+max_sine_value*spread*sin(millis()*0.01));
    pump_2_value = (int)((max_sine_value*base_height+max_sine_value*spread*sin(PI+millis()*0.01)));
    update_values();
    break;

  case 8: 
    base_height = 0.5+sin(millis()*0.001)*0.5;
    spread = min(base_height, 1-base_height);
    pump_1_value = (int)(max_sine_value*base_height+max_sine_value*spread*sin(millis()*0.01));
    pump_2_value = (int)((max_sine_value*base_height+max_sine_value*spread*sin(PI+millis()*0.01)));
    update_values();
    break;

  case 9: 
    base_height = 0.5+sin(millis()*0.01)*0.5;
    spread = min(base_height, 1-base_height);
    pump_1_value = (int)(max_sine_value*base_height+max_sine_value*spread*sin(millis()*0.01));
    pump_2_value = (int)((max_sine_value*base_height+max_sine_value*spread*sin(PI+millis()*0.01)));
    update_values();
    break;

  case 10: 
    pump_1_value = (int)(current_wheel_value);
    pump_2_value = (int)(0);
    update_values();
    break;

  case 11: 
    pump_1_value = (int)(current_wheel_value);
    pump_2_value = (int)(current_wheel_value);
    update_values();
    break;

  case 12: 
    pump_1_value = (int)(max_sine_value*0.5+max_sine_value*0.5*sin(millis()*0.001));
    pump_2_value = (int)(max_sine_value-(max_sine_value*0.5+max_sine_value*0.5*sin(millis()*0.001)));
    pump_1_value=constrain(pump_1_value, 0, 200);
    pump_2_value=constrain(pump_2_value, 50, 255);
    update_values();
    break;

  default:
    break;
  }


  create_and_send_msg();
}

void create_and_send_msg() {
  String new_msg;
  new_msg = compose_msg();
  send_msg(new_msg);
}

void reset_values() {
  pump_1_value = 0;
  pump_2_value = 0;
  fan_value = false;
  mist_value = false;
  //led_value = 0;
  update_values();
}

void update_values() {
  //rate_control_1.value.setLastValue(pump_1_value/127.0);
  //rate_control_1.value.setLastValue(current_wheel_value/127.0);
  if(current_wheel_value<20){
    rate_control_1.value.setLastValue(1.0);
  }else{
    rate_control_1.value.setLastValue(map(current_wheel_value,0.0,255.0,0.7,1.3));
  }
  
//rate_control_1.value.setLastValue(map(pump_1_value,0.0,255.0,0.7,1.3));

  
  cp5.getController("pump_1_value").setValue(pump_1_value);
  cp5.getController("pump_2_value").setValue(pump_2_value);
  cp5.getController("fan_value").setValue(int(fan_value));
  cp5.getController("mist_value").setValue(int(mist_value));
  cp5.getController("communicate").setValue(int(communicate));
}

void led_mode_bar(int _n) {
  led_value = _n;
  create_and_send_msg();
  println("Led mode: ", _n);
}

void work_mode_bar(int _n) {
  reset_values();
  work_mode = _n;
  create_and_send_msg();
  println("Work mode: ", _n);
}

String compose_msg() {
  String msg = "";
  msg += serial_start_string;
  msg += pump_1_value;
  msg += serial_delimiter_string;
  msg += pump_2_value;
  msg += serial_delimiter_string;
  msg += int(fan_value);
  msg += serial_delimiter_string;
  msg += int(mist_value);
  msg += serial_delimiter_string;
  msg += int(led_value);
  msg += serial_end_string;
  return(msg);
}

void send_msg(String _msg) {
  //println(_msg);
  if ((communicate)&&(Serial.list().length>0)) {
    tornado_serial.write(_msg);
  }
}

void serialEvent(Serial input_port) {
  String incoming_string = input_port.readString();
  if (input_port==tornado_serial) {
    println("Recieved: "+incoming_string);
  } else if (input_port==wheel_serial) {
    //println(incoming_string);
    current_wheel_value=int(incoming_string.trim());

    max_wheel_value = max(max_wheel_value, current_wheel_value);

    println("Wheel: "+current_wheel_value + ", Max: "+max_wheel_value);
  }
}

void stop() {
  exit();
  super.stop();
}

void exit() {
  special_exit();
  super.exit();
}

void special_exit() {
  reset_values();
  create_and_send_msg();
}