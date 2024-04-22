const WebSocket = require('ws');
const fs = require("fs");
const express = require('express');

const espAquarium = "192.168.10.164";

let waterTemp = [];
loadDataFiles();


//        MAIN WEBSOCKET CODE

const wss = new WebSocket.Server({ port: 8080 });

wss.on('listening', () => {
  const address = wss.address();
  console.log(`WebSocket server is listening on ${address.address}:${address.port}`);
});

wss.on('connection', function connection(ws, req) {
  //Log the IP of the connection for further verification
  const deviceIp = extractIPv4(req.socket.remoteAddress);
  console.log(`Client connected from IP: ${deviceIp}`);

  ws.on('message', (message) => {    
    // Handle the message received
    let parsedMessage = parseMessage(message);
    if (deviceIp === espAquarium){
      console.log("Message received: ESP_Aquarium\nMessage: ", parsedMessage, "\n");
      savewaterTemp(parsedMessage);
    } else {
      console.log(`Device IP: ${deviceIp}\nMessage received: Uknow device\nMessage: ${parsedMessage} \n`);
    }
  });

  ws.on('close', function close() {
    console.log('Client disconnected');
  });
});
////////////////////


//    HANDLERS AND OTHER FUNCTIONS
function loadDataFiles(){
  //Load the temperature list to the global list
  fs.readFile("data/waterTemp.json", (error, data) => {
    if (data != 0){
      waterTemp = JSON.parse(data);
    }
  });
}
  
function parseMessage(message){
  //Safely convert the message received and return it
  let parsedMessage;
  try {
    parsedMessage = JSON.parse(message);
  } catch (error) {
    console.error('Error parsing JSON message:', error);
    return;
  }
  return parsedMessage;
}

function savewaterTemp(push){
  //Push the value to the current list and log it.
  if (waterTemp.length > 4){
    waterTemp.push(push);
    waterTemp.shift();
  } else {
    waterTemp.push(push);
  }

  fs.writeFile("data/waterTemp.json", JSON.stringify(waterTemp), (error) => {
    if (error) {
      console.error(error);
      throw error;
    }
  });
};

function extractIPv4(ip) {
  if (ip.includes('::ffff:')) {
    return ip.split('::ffff:')[1];
  } else {
    return ip;
  }
}
//////////////

//WebServer
const webServer = express();
webServer.set('view engine', 'ejs');

webServer.get('/', (req, res) => {
    const value = 'Hello from Node.js server!';
    res.render('index', { waterTemp: waterTemp });
});

webServer.listen(80, () => {
    console.log(`Server is running on http://localhost:80`);
});