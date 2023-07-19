function main() {
    let temp = document.getElementById("temperature");
    let pressure = document.getElementById("pressure");
    let hum = document.getElementById("humidity");
    let win_speed = document.getElementById("wind_speed");
    let win_dir = document.getElementById("wind_direction");

    let form = document.getElementById("get_data");

    form.addEventListener("submit", function (event) {

        event.preventDefault();
        const xhr = new XMLHttpRequest();
        xhr.open("GET", "/api/refresh");

        xhr.onload = () => {
            const data = JSON.parse(xhr.response);
            console.log(data);
            temp.textContent = data["temperature"];
            pressure.textContent = data["pressure"];
            hum.textContent = data["humidity"];
            win_speed.textContent = data["wind_speed"];
            win_dir.textContent = data["wind_direction"];
        }

        xhr.send();
    });
}

function gear(){
    let gear = document.getElementById("gear").value;
    var xhr = new XMLHttpRequest();
            xhr.open("GET", "/api/get_gear", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var responseData = xhr.responseText;
                        document.getElementById("selectedGear").value = responseData;
                        console.log(responseData);
                    } else {
                        document.getElementById("selectedGear").value = "error";
                    }
                }
            };
            xhr.send();
    
    
}

function calibrate(){
    let gear = document.getElementById("gear").value;
    var xhr = new XMLHttpRequest();
            xhr.open("GET", "/api/calibrate?gear=" + gear, true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var responseData = xhr.responseText;
                        console.log(responseData);
                    } else {
                        document.getElementById("selectedGear").value = "error";
                    }
                }
            };
            xhr.send();
}

function position(){
    var xhr = new XMLHttpRequest();
            xhr.open("GET", "/api/position", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var responseData = xhr.responseText.split(",");
                        document.getElementById("lineare").value = responseData[0];
                        document.getElementById("rotativo").value = responseData[1];
                    } else {
                        document.getElementById("lineare").value = "error";
                        document.getElementById("rotativo").value = "error";
                    }
                }
            };
            xhr.send();
}