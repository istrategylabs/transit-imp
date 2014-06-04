arduino <- hardware.uart57;
// globalString <- null;

function signWrite(Data){
  arduino.write(Data[9]);
  imp.sleep(2.0);
  arduino.write(Data[0]+"/"+Data[1]+"/"+Data[2]+"/"+Data[3]+"/"+Data[4]
                +"/"+Data[5]+"/"+Data[6]+"/"+Data[7]+"/"+Data[8]);
  
  server.log("Data sent to Arduino");
}

agent.on("Data", signWrite);
// agent.on("Weather", weatherWrite);

arduino.configure(9600, 8, PARITY_NONE, 1, NO_CTSRTS);