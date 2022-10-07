var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {
    var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + pos.coords.lalitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

    xhrRequest(url, 'GET', function(responseText) {
        var json = JSON.parse(responseText);

        var temperature = Math.round(json.main.temperature - 273.15);
        console.log(temperature);

        var conditions = json.weather[0].main;
        console.log(conditions)

        var dictionary = {
            "TEMPERATURE": temperature,
            "CONDITIONS": conditions
        }

        Pebble.sendAppMessage(dictionary,
            function(e) {
                console.log('Weather info send to Pebble successfully!')
            },
            function(e) {
                console.log('Error sending weather info to Pebble!')
            })
    })
}

function locationError(err) {
    console.log('Error requesting location!');
}

function getWeather() {
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 15000, maximumAge: 60000}
    );
}

Pebble.addEventListener('ready', function(e) {
    console.log('PebbleKit JS ready!');
    getWeather();
})

Pebble.addEventListener('appmessage', function(e) {
    console.log('AppMessage received!');
    getWeather();
  }                     
);