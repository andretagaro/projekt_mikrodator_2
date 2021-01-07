

const serial_options = {baudRate: 9600}; // Set baud-rate
const serial_port = "COM13";  // Declare port name

const serialport = require('serialport');
let myPort = new serialport(serial_port, serial_options);

// Set a parser to use for the data on the serial port
let Readline = serialport.parsers.Readline; // parser to read ASCII lines
let parser = new Readline(); // make parser
myPort.pipe(parser); // pipe the serial stream to the parser

//Map callback for incomming data events on parser
myPort.on('open', showPortOpen);
parser.on('data', readSerialData);
myPort.on('close', showPortClose);
myPort.on('error', showError);


//Callbacks for events on serial port
function showPortOpen() {
  console.log('port open. Data rate: ' + myPort.baudRate);
}

var read_data1 = 0;
var read_data2 = 0;

//Data från serialport (uart received)
// First bite determine where to store data
function readSerialData(data) {
  console.log("Program ended");
  console.log('Data received: ', data)

  if (data[0] == "A") {
    read_data2 = data;
  }
  if (data[0] == "B") {
    read_data1 = data;
  }
}


function showPortClose() {
  console.log("Serial port closed. " + '\n' + 'Shutting down!' );
  process.exit(1);
}

function showError(error) {
  console.log('Serial port error: ' + error);
  console.log('Change current port in this script');
  console.log('Shutting down!');
  process.exit(1);
}


//Write first data to serialport

let timerVer = setInterval(sendSerialData, 900);
function sendSerialData() {

  console.log('send_serial: ' + text );
  myPort.write(text +  '\n');

}


//Second data to serialport

//var text1 = 'B2001';
let timerVer1 = setInterval(sendSerialData2, 910);
function sendSerialData2() {
  console.log('send_serial2: ' + text2)
  myPort.write(text2 + '\n');
}

//************MQTT SETUP******************

// Initiate mqtt setup requirement
let mqtt = require('mqtt');
let mqtt_server = "mqtt://130.239.163.210:1883";

connect_options = {
  clientId: "ok",
  username: "mikro2",
  password: "mAkRo",
  clean: true
};


//UNDO THIS************************************************
let mqtt_client = mqtt.connect(mqtt_server, connect_options);
mqtt_client.on("connect", function() {
    console.log("connection: " + mqtt_client.connected)

})

//*********************test*******************************
var count_var = 1;
var count_var2 = 1;

const test_publish = function() {
  mqtt_client.publish('grupp4/range/test', count_var + '\n');
}
const timer_test = setInterval(test_publish, 1000);

  //******************Publish to car A***********************
 const publish_this_1 = function () {
      mqtt_client.publish('grupp4/direction/remote_2', read_data2 + '\n');
  }
  const timer_count1 = setInterval(publish_this_1, 250);

  //****************Publish to car B************************
    const publish_this = function () {
      mqtt_client.publish('grupp4/direction/remote_1', read_data1 + '\n');
  }
  const timer_count = setInterval(publish_this, 250);


  //Subscribe to multiple topics by format topic/sub_topic/#
  let topic_in = "grupp4/range/#";

  mqtt_client.subscribe(topic_in, {qos:1});

  // Default values if no data comes from mqtt
  var text = "C";
  var text2 =  "D";

  //Set up a callback event for incoming messages on mqtt
  //One connection to multiple topics
  mqtt_client.on("message", function(topic_in, message, packet) {

      if (topic_in == 'grupp4/range/test') {
        count_var++;
        count_var2++;
      }

    // If no new data is received after 10s count_var > 20
    if ((topic_in == 'grupp4/range/car_2') || (count_var > 20)) {
      console.log("topic is: " + topic_in);
      console.log("Message is: " + message);

      // If topic is not online send C else send message
      // Increment of count_var to a value over 20 means the topic is offline
      if ((message == "ANaN") || (count_var > 20)) {
        text = "C";
      }
      else {
        text = message;
      }
      count_var = 1;
    }

    // If no new data is received after 10s count_var > 20
    if ((topic_in == 'grupp4/range/car_1') || (count_var2 > 20)) {
      console.log("topic is: " + topic_in);
      console.log("Message is: " + message);

      // If topic is not online send D else send message2
      // Increment of count_var to a value over 20 means the topic is offline
      if ((message == "ANaN") || (count_var2 > 20)) {
        text2 = "D";
      }
      else {
      text2 = message;
      }
      count_var2 = 1;
    }
  })

//UNDO THIS************************************************

//
