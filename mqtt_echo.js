const serialport = require('serialport') 

const serial_options={ baudRate: 9600 };
const serial_port="COM7"; // change this to your TTL port!

let myPort = new serialport(serial_port, serial_options)

//Set up a parser to use for the data on the seriall port
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
}

// read data from UART
function readSerialData(data) {
    console.log("Data from serial port: ", data); //print in console
    client.publish("avr", "data is: " + data, options_P); //publish data [TOPIC, MESSAGE, OPTIONS]
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

//this sends data to mcu every second
/* let timeVar = setInterval(myTimer1, 1000);
function myTimer1() {
    console.log('\n' + 'Asking for data!');
    myPort.write("this is sent to mcu" + "\n");
} */

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
client.on("connect", function() { console.log("connected " + client.connected); })


//--- SUBSCRIBE ---

let topic_s ="avr";                 //topic with name "avr"
client.subscribe(topic_s,{qos:1});  //subscribe to this topic

client.on('message',function(topic_s, message, packet){
console.log("message is "+ message);
console.log("topic is "+ topic_s);
myPort.write('A');  // <-- 'A' is sent to UART!
});



//--- PUBLISH ---

let options_P={
    retain: true,
    qos: 1
    };

client.publish("avr", "message" , options_P);
