const WebSocket = require('ws');
const fs = require("fs");
const express = require('express');

const espAquarium = "192.168.10.164";

let aquariumValues = [];
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
      aquariumSave(parsedMessage);
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
  fs.readFile("data/espAquarium.json", (error, data) => {
    if (data != 0){
      aquariumValues = JSON.parse(data);
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

function aquariumSave(push){
  const currentTime = new Date();
  
  let parsedTime = `${currentTime.getHours()}:${currentTime.getMinutes()}h`;
  push = {
    time: parsedTime,
    temp: push
  };
  
  if (aquariumValues.length > 4){
    aquariumValues.push(push);
    aquariumValues.shift();
  } else {
    aquariumValues.push(push);
  }

  fs.writeFile("data/espAquarium.json", JSON.stringify(aquariumValues), (error) => {
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
///////////////////////////////////

//WebServer
const webServer = express();
webServer.use(express.static('public'));
webServer.set('view engine', 'ejs');

webServer.get('/', (req, res) => {
    res.render('hub');
});

webServer.get('/aquarium', (req, res) => {
  res.render('aquarium', { aquariumValues: aquariumValues });
});

webServer.listen(80, () => {
    console.log(`Server is running on http://localhost:80`);
});