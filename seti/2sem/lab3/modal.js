export default function renderModal(options) {
    const features = options["interesting_places"]["features"]
    let listItems = '';

    //console.log("asd", options, features)
    features.forEach(item => {
        if (item["properties"]["name"]) {
            const descPart = item["desc"] ? ` ---- ${item["desc"]}` : "";
            listItems += `<li>${item["properties"]["name"]}${descPart}</li>`;
        }
    });

    return `<div id="my-modal" class="modal">
     <div class="modal-content">
        <h1>🗺️ О месте: ${options['country']} ${options['city']} ${options['name']}</h1>
         <div id="map" style="width: 600px; height: 400px"></div>
        <p>📍 ${options['point']['lat']} ${options['point']['lng']}</p>
       
     </div>
      <div>
      <span>
      ☁️ ${Math.round(options["weather"]["main"]["temp"] - 273.15)}° градусов. 
      🌡️ Описание (${options["weather"]["weather"][0]["description"]})</span><br>
      </div>
        
      <ul>
        ${listItems}
      </ul>
     <div class="modal-footer">
        <a href="#!" class="modal-close waves-effect waves-green btn-flat">Close</a>
     </div>
 </div>`;
}
