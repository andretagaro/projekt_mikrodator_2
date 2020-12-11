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
    client.publish("grupp4/serialport", "open"); 
}

// read data from UART
function readSerialData(data) {

    console.log("Data from serial port: ", data); //print in console

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
    client.publish("grupp4/range/ADC", data, options_P); //publish data [TOPIC, MESSAGE, OPTIONS]
    client.publish("grupp4/range/cm", range_cm.toFixed(0), options_P);
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

options={
    clientId:"hej",
    username:"mikro2",
    password:"mAkRo",
    clean:true
};

let client = mqtt.connect(url,options);
client.on("connect", function() 
{ 
    console.log("connected " + client.connected); 
    client.publish("grupp4/client", "connected " + client.connected)
})


//--- SUBSCRIBE ---
//let topics={"grupp4/range/cm":1,"grupp4/range/ADC":1,"grupp4/direction/remote":1};
//client.subscribe(topics);


let topic_s ="grupp4/range/single_ASCII";        //topic(s)
client.subscribe(topic_s,{qos:1});  //subscribe to this topic

client.on('message', function(topic_s, message, packet){
    console.log("message is "+ message);
    //console.log("topic is "+ topic_s);
    myPort.write(message);  // <-- char is sent to UART
});


//--- PUBLISH ---

let options_P={
    retain: true,
    qos: 1
    };

