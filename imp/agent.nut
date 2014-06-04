// Metro data provided by WMATA API.
// Bikeshare data provided by http://www.citybik.es/ under LGPL license.

local debug = true;

server.log("Transit Agent Running");

//Replace X's with API keys
local wmataURL = "http://api.wmata.com/StationPrediction.svc/json/GetPrediction/A03?api_key="+XXXX; 
local bikeURL = "http://api.citybik.es/capital-bikeshare.json";
local weatherURL = "http://api.worldweatheronline.com/free/v1/weather.ashx?q=20001&format=json&num_of_days=1&key="+XXXX;

local destination = ["", "", "", "", "", ""];
local min = ["", "", "", "", "", ""];

local response;
local responseTwo;
local responseThree;

function getConditions() {
  server.log("Getting Transit Dupont Data");

  local req = http.get(wmataURL);
  local reqBike = http.get(bikeURL);
  local reqWeather = http.get(weatherURL);

  local res = req.sendsync();
  local resTwo = reqBike.sendsync();
  local resThree = reqWeather.sendsync();

  server.log(format("WMATA response returned with status %d", res.statuscode));
  if (res.statuscode != 200) {
      server.log("Request for WMATA data failed.");
      return;
  }
  
  server.log(format("Bike response returned with status %d", resTwo.statuscode));
  if (resTwo.statuscode != 200) {
      server.log("Request for bike data failed.");
      return;
  }
  
  server.log(format("Weather response returned with status %d", resThree.statuscode));
  if (resThree.statuscode != 200) {
      server.log("Request for weather data failed.");
      return;
  }

  // hand off data to be parsed
  try {
    response = http.jsondecode(res.body);
  } catch (ex) {
    server.log("Error while decoding wmata body: " + ex);
  }
  
  try {
    responseTwo = http.jsondecode(resTwo.body);
  } catch (ex) {
    server.log("Error while decoding bike body: " + ex);
  }
  
  try {
    responseThree = http.jsondecode(resThree.body);
  } catch (ex) {
    server.log("Error while decoding weather body: " + ex);
  }
  
  //Check if trains exist
  if (!response.Trains) {
    server.log("No Train arrays.");
    imp.wakeup(600, getConditions);
  }

  server.log("Number of trains in array: " + response.Trains.len());

  // Check for valid train strings and then set variables
  for (local i = 0; i < response.Trains.len(); i++) {
    if (response.Trains[i].DestinationName && response.Trains[i].Min) {
      destination[i] = response.Trains[i].DestinationName;
      min[i] = response.Trains[i].Min;
    } else {
      destination[i] = "No Tr";
      min[i] = "0";
    }
  }
  
  // log the train names for debugging
  if (debug) {
    server.log("Printing Metro names from JSON")
    for (local i = 0; i < response.Trains.len(); i++) {
      server.log(destination[i]);
    }
  }
  
  //Check for ghost trains late at night
  for (local i = 0; i < response.Trains.len(); i++) {
    if (destination[i].len() == 0) {
      destination[i] = "No Tr";
      min[i] = "0";
    }     
  }
  
  //Check if JSON is valid. If not rerun data pull
  for (local i = 0; i < response.Trains.len(); i++) {
    if (destination[i].len() < 5) {
      server.log("Bad slice data " + i + ", restarting in 30");
      imp.wakeup(30, getConditions);
      return;
    } else {
      destination[i] = destination[i].slice(0,5);
    }
  }

  //define bikes and weather
  local bikesAvailable = responseTwo[49].bikes;
  local weather = responseThree.data.current_condition[0].temp_F;

  local stringData = [destination[0], min[0], destination[1], min[1], 
                      destination[2], min[2], destination[3], min[3], bikesAvailable, weather];
  
  if (debug) {
    for (local i = 0; i < response.Trains.len(); i++) {
      server.log("Next Train: " + destination[i] + ", Arriving in " + min[i] + " Minutes");
    }
    server.log("Bikes Free: " + bikesAvailable);
    server.log("Temperature: " + weather);
  }

  // Send the string to the device
  device.send("Data", stringData);
  server.log("Data sent to device side");
  
  imp.wakeup(30, getConditions);
}

getConditions();