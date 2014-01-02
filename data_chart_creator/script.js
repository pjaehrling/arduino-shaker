var SEPERATOR = ";";
    SHOW_TIME = true;

var createBtn = document.getElementById("createBtn"),
    inputTxt  = document.getElementById("dataArea");

    google.load("visualization", "1", {packages:["corechart"]});

function drawChart(dataArray) {
    var data = google.visualization.arrayToDataTable(dataArray),
        options = { title: 'Shake Test (ADXL335 Accelerometer)' };

    var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
    chart.draw(data, options);
}

createBtn.addEventListener('click', function() {
    if (inputTxt.value.length > 0) {

      var dataArray   = new Array(['time in sec.', 'X', 'Y', 'Z']),
          dataString  = inputTxt.value,
          dataRows    = dataString.split(/\r\n|\r|\n/g),
          len         = dataRows.length,
          time        = 0.0;

      for (var i = 0; i < len; i++) {
        var stringVals = dataRows[i].split(SEPERATOR);

        if (stringVals.length === 3) {
          var tmp = (SHOW_TIME) ? new Array( time.toFixed(1) + "s" ) : new Array( "" ) ;
          for (var j = 0; j < 3; j++) {
            tmp.push(parseFloat(stringVals[j]));
          }
          dataArray.push(tmp);
        }

        time += 0.1;
      }

      drawChart(dataArray);

    } else {
      alert("Put some data in the textfield first!");
    }


}, false);