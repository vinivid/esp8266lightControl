let lightState = -1;

var lightSwitch = document.getElementById('lamp1switch');
var lampStateHTML = document.getElementById('lamp1State');

lightSwitch.addEventListener('change', ()=>{
    if(lightSwitch.checked){
        let xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/lightON", true);
        xhttp.send();
        lightState = 0;
        lampStateHTML.className = "lampstateON";
    }else{
        let xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/lightOFF", true);
        xhttp.send();
        lightState = 1;
        lampStateHTML.className = "lampstateOFF";
    }
});

//Checa constatemente se mudou fisicamente o estado da lampada
setInterval(() => {
    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            lightState = xhttp.responseText;
        }
    };
    xhttp.open("GET", "/lightState", true);
    xhttp.send();

    if(lightState == 0){
        lightSwitch.checked = true;
        lampStateHTML.className = "lampstateON"
        console.log("turnedOn");
    }else{
        console.log("turned off");
        lampStateHTML.className = "lampstateOFF"
        lightSwitch.checked = false;
    }
}, 2000);