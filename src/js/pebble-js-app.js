function getWeatherFromLatLong(latitude, longitude) {
  var response;
  var req = new XMLHttpRequest();
  var url = "http://viwebworks.net/weatherpage.aspx?lat=" + latitude + "&lon=" + longitude;
  req.open('GET', url, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if (req.status == 200) {
        // console.log(req.responseText);
        response = JSON.parse(req.responseText);
        if (response) {
          Pebble.sendAppMessage({
            "temp":response.0 + "\u00B0F",
            "icon":response.1,
            "bar":response.2,
            "updated":response.3,
            "cond":response.4
          });
        }
      } else {
        console.log("Error");
      }
    }
  }
  req.send(null);
}
function updateWeather() {
  window.navigator.geolocation.getCurrentPosition(locationSuccess,
                                                    locationError,
                                                    locationOptions);
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 };

function locationSuccess(pos) {
  var coordinates = pos.coords;
  getWeatherFromLatLong(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "temp":err.code,
    "icon":"13",
    "bar":"heh",
    "updated":"never",
    "cond":err.message
  });
}

Pebble.addEventListener("ready", function(e) {
  console.log("connect!" + e.ready);
    Pebble.sendAppMessage({
    "temp":"here",
    "icon":"yes",
    "bar":"what",
    "updated":"ok",
    "cond":e.ready
  });
  updateWeather();
  setInterval(function() {
    console.log("timer fired");
    updateWeather();
  }, 900000);
  console.log(e.type);
});