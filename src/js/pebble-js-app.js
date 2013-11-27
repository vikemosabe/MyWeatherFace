function getWeatherFromLatLong(latitude, longitude) {
  var response;
  var req = new XMLHttpRequest();
  var url = "http://viwebworks.net/weatherpage.aspx?lat=" + latitude + "&lon=" + longitude;
  req.open('GET', url, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if (req.status == 200) {
        response = JSON.parse(req.responseText);
        //Pebble.showSimpleNotificationOnPebble("JSON", response);
        if (response && response.list && response.list.length > 0) {
            var weatherResult = response.list[0];
            Pebble.sendAppMessage({
            "temp":weatherResult.temp,
            "icon":weatherResult.icon,
            "bar":weatherResult.bar,
            "updated":weatherResult.now,
            "cond":weatherResult.image
          });
        }
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
  Pebble.sendAppMessage({
    "temp":err.code,
    "icon":"13",
    "bar":"heh",
    "updated":"never",
    "cond":err.message
  });
}
Pebble.addEventListener("ready", function(e) {
  updateWeather();
  setInterval(function() {
    updateWeather();
  }, 900000);
    //Pebble.showSimpleNotificationOnPebble("ready", "sesame");
});
