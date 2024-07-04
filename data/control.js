/*JavaScript simples com o intuito apenas de constantemente
escanear os dados adquiridos pela esp. 
*/
let lightState = -1;
var lightSwitch = document.getElementById('lamp1switch');
var lampStateHTML = document.getElementById('lamp1State');

//Lê se o botão da pagina foi pressionado para ligar ou desligar a luz
lightSwitch.addEventListener('change', ()=>{
    if(lightSwitch.checked){
        //Pede para ligar a luz
        let xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/lightON", true);
        xhttp.send();
        lightState = 0;

        //muda o icone da lâmpada
        lampStateHTML.className = "lampstateON";
    }else{
        //Pede para desligar a luz
        let xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/lightOFF", true);
        xhttp.send();
        lightState = 1;

        //muda o icone da lâmpada
        lampStateHTML.className = "lampstateOFF";
    }
});

//Checa constatemente se mudou fisicamente o estado da lampada
setInterval(() => {
    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            //o programa sempre retornara 0 ou 1 como estado da luz
            lightState = xhttp.responseText;
        }
    };
    xhttp.open("GET", "/lightState", true);
    xhttp.send();

    //corrigi o botão e o icone da lampâda se estiverem na posição errada
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

//Essa função lê os dados armazenados no JSON pela ESP
async function readJson(){
    let savedThings;
    let pageLig = document.getElementById('luzLig');
    let pageSens = document.getElementById('ligPSens');
    let ligSwitch = document.getElementById('LigSwitch');

    try{
        const response = await fetch('/store');
        if (!response.ok) {
            throw new Error(`Stado: ${response.status}`);
        }
  
        const json = await response.json();
        console.log(json);
        savedThings = json;
        pageLig.innerHTML = `Luz ligada por: ${(savedThings.time)/60000} min`;
        pageSens.innerHTML = `Luz ligada pelo sensor: ${savedThings.sensor} vezes`;
        ligSwitch.innerHTML = `Usou o interruptor: ${savedThings.switches} vezes`;
    } catch (error) {
        console.error(error.message);
    }
}

//Como mandar o JSON requer um pouco a mais de tempo o melhor é fazer por ultimo
readJson()

setInterval(readJson, 15000);