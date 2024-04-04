const WebSocket = require('ws');
const fs = require("fs");

const wss = new WebSocket.Server({ port: 8080 });

let jWaterTemp = [];
loadDataFiles();


//        MAIN WEBSOCKET CODE

wss.on('listening', () => {
  const address = wss.address();
  console.log(`WebSocket server is listening on ${address.address}:${address.port}`);
});

wss.on('connection', function connection(ws, req) {
  //Log the IP of the connection for further verification
  const deviceIp = req.socket.remoteAddress;
  console.log(`Client connected from IP: ${deviceIp}`);

  ws.on('message', (message) => {    
    // Handle the message received
    let parsedMessage = parseMessage(message);

    saveJWaterTemp(parsedMessage);
  });


  ws.on('close', function close() {
    console.log('Client disconnected');
  });
});

////////////////////


//    HANDLERS AND OTHER FUNCTIONS

function loadDataFiles(){
  //Load the temperature list to the global list
  fs.readFile("data/jWaterTemp.json", (error, data) => {
    if (data != 0){
      jWaterTemp = JSON.parse(data);
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

function saveJWaterTemp(push){
  //Push the value to the current list and log it.
  jWaterTemp.push(push);
  // console.log("New updated list: \n");
  // jWaterTemp.forEach((x) => console.log(x));
  // console.log("\n\n");

  fs.writeFile("data/jWaterTemp.json", JSON.stringify(jWaterTemp), (error) => {
    if (error) {
      console.error(error);
      throw error;
    }
  });
};