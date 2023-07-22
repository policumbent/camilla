function gear(){
    let gear = document.getElementById("gear").value;
    var xhr = new XMLHttpRequest();
            xhr.open("GET", "/api/get_gear?gear=" + gear, true);
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
                        document.getElementById("selectedGear").value = responseData;
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