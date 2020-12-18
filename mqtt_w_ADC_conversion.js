const serialport = require('serialport') 

const serial_options={ baudRate: 9600 };
const serial_port="COM7"; // change this to your TTL port!

let myPort = new serialport(serial_port, serial_options)

//Set up a parser to use for the data on the serial port
let Readline = serialport.parsers.Readline; // make instance of Readline parser
let parser = new Readline(); // make a new parser to read ASCII lines
myPort.pipe(parser); // pipe the serial stream to the parser

//Map callbacks for events on serial port
myPort.on('open', showPortOpen);
parser.on('data', readSerialData);
myPort.on('close', showPortClose);
myPort.on('error', showError);

//Callbacks for events on serial port
function showPortOpen() {
    console.log('port open. Data rate: ' + myPort.baudRate);
    //client.publish("grupp4/serialport", "open"); 
}

// read data from UART
function readSerialData(data) {     //denna körs då data tas emot i serieporten (då det skickas från UART)

    console.log("Data from serial port: ", data); //print in console
    
    // omvandla ADC-data till cm
    let range_cm = 0.0;
    if (data >= 512) {
        range_cm = (-0.0207*data + 29.62);
    } 
    else if (data < 512 && data >= 354) {
        range_cm = (-0.0635*data + 52.5);
    }
    else if (data < 354 && data >= 197) {
        range_cm = (-0.1693*data + 90);
    }
    else {
        range_cm = (-0.3772*data + 137.43);
    }
    if (range_cm >= 100)
    {
        range_cm = 99;
    }
    client.publish("grupp4/range/ADC", data, options_P); //publish data [TOPIC, MESSAGE, OPTIONS]
    client.publish("grupp4/range/cm", 'A' + range_cm.toFixed(0), options_P);    //publicera data i cm, med 0 decimaler
    client.publish("grupp4/range/single_ASCII", String.fromCodePoint(range_cm.toFixed(0)), options_P); 
        // omvandlar till ett enda tecken vilket är enklare att läsa för AVR ^
}

function showPortClose() {
    console.log('Serial port closed.' + '\n' + 'Shutting down!');
    process.exit(1);
}
function showError(error) {
    console.log('Serial port error: ' + error);
    console.log('You must change to current port in this script');
    console.log('Shutting down!');
    process.exit(1);
}


//--- CONNECT ---
let mqtt = require('mqtt');
let url = "mqtt://130.239.163.210:1883";

options={           //inställningar för att logga in på klienten
    clientId:"hej",
    username:"mikro2",
    password:"mAkRo",
    clean:true
};

let client = mqtt.connect(url,options);
client.on("connect", function()     
{ 
    //console.log("connected " + client.connected); 
    //client.publish("grupp4/client", "connected " + client.connected)
})


//--- SUBSCRIBE ---

    ////prenumerera på flera topics med denna:
//let topics={"grupp4/range/cm":1,"grupp4/range/ADC":1,"grupp4/direction/remote":1};
//client.subscribe(topics);

let topic_D ="grupp4/direction/remote_1";   //define topic
client.subscribe(topic_D,{qos:1});          // andsubscribe to topic

client.on('message', function(topic_D, message, packet){        //denna körs då ett meddelande tas emot på topic_D
    console.log("message is "+ message);                        //print message in console
    //console.log("topic is "+ topic_s);                          //print topic in console
    myPort.write(message);                                  //skicka vidare meddelande till UART
});


/* let topic_R ="grupp4/range/cm";        //define topic
client.subscribe(topic_R,{qos:1});        // and subscribe to topic

client.on('message', function(topic_R, message, packet){        //denna körs då ett meddelande tas emot på topic_R
    console.log("message is "+ message);                        //print message in console
    console.log("topic is "+ topic_R);                          //print topic in console
    //myPort.write("R" + (message.toString()).padStart(3, "0")); //skicka vidare meddelande till UART
        //(den fyller ut med nollor ifall talet är mindre än hundra)
}); */




//--- PUBLISH OPTIONS---

let options_P={
    retain: true,
    qos: 1
    };

