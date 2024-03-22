const express = require('express');
const http = require('http');
const path = require('path');

const app = express();
const webapp = express();
const rgbArr = ["off","0","0"];

const espserver = http.createServer((req, res) => {
  if (req.url === '/reqrgb' && req.method === 'GET') {
    // Handle GET request to /data endpoint
    res.writeHead(200, {'Content-Type': 'application/json'});
    res.end(JSON.stringify(rgbArr));
    console.log("Sent RGB values:", rgbArr);
  } else {
    // Handle other requests
    res.writeHead(404, {'Content-Type': 'text/plain'});
    res.end('Not Found');
    console.log("invalid url");
  }
});

let bR = 0;
let bG = 0;
let bB = 0;
app.get('/setRGB', (req, res) => {
  let { r, g, b } = req.query;
 
  let incAmount = 0.5;
  
  if (r == "more"){
    if ( (rgbArr[0] / incAmount) <= 255 && rgbArr[0] >= 1 ){
      r = Math.floor((rgbArr[0] / incAmount));
      r = r.toString();
      bR = r;
    } else if (rgbArr[0] == 0){
      r = "0";
    } else {
      r = bR;
    }
    
    if ( (rgbArr[1] / incAmount) <= 255 && rgbArr[1] >= 1 ){
      g = Math.floor(rgbArr[1] / incAmount);
      g = g.toString();
      bG = g;
    } else if (rgbArr[1] <= 1){
      g = "0";
    } else {
      g = bG;
    }

    if ( (rgbArr[2] / incAmount) <= 255 && rgbArr[2] >= 1 ){
      b = Math.floor(rgbArr[2] / incAmount);
      b = b.toString();
      bB = b;
    } else if (rgbArr[2] == 0){
      b = "0";
    } else {
      b = bB;
    }
  }
  
  if (r == "less"){
    if ( (rgbArr[0] * incAmount) >= 0 && rgbArr[0] != 0 ){
      r = Math.floor((rgbArr[0] * incAmount));
      r = r.toString();
    } else {
      r = "0";
    } 
    
    if ( (rgbArr[1] / incAmount) >= 0 && rgbArr[1] != 0 ){
      g = Math.floor(rgbArr[1] * incAmount);
      g = g.toString();
    } else {
      g = "0";
    } 

    if ( (rgbArr[2] / incAmount) >= 0 && rgbArr[2] != 0 ){
      b = Math.floor(rgbArr[2] * incAmount);
      b = b.toString();
    } else {
      b = "0";
    } 
  }
  
  rgbArr[0] = r;
  rgbArr[1] = g;
  rgbArr[2] = b;

  if (!isNaN(parseInt(r))){
    bR = r;
    bG = g;
    bB = b;
  }

 


  console.log('Received RGB values:', { r, g, b });
  res.send('RGB values received successfully');
});

app.listen(2001, () => {
  console.log(`RGB controler at 2001`);
});

espserver.listen(3001, () => {
  console.log(`ESPSV at 3001`);
});

webapp.set('view engine', 'ejs');
webapp.set('views', path.join(__dirname, 'views'));
webapp.get('/', (req, res) => {
    res.render('index', { message: 'Hello, world!' });
});

webapp.listen(80, () => {
    console.log(`web server on 80`);
});
