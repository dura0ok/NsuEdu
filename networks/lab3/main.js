import Weather from './requests';
import toast from './toasts';
import renderModal from './modal';

const weather = new Weather();

const searchFormBtn = document.querySelector('.search-button');
const placesList = document.querySelector('.found-places');
const placeName = document.querySelector('#place').value;
const modalWrapper = document.querySelector('.modal-wrapper');

const name = 'Stepan';

// ???
const drawPlacesList = (places) => {
  places.forEach((el) => {
    //console.log(el)
    const placeTemplate = `
         <li class="collection-item" data-json='${JSON.stringify(el)}'>
                <input type="hidden" name="point">
                <div>Город: ${el['city']} (${el['postcode']}) Имя: ${
      el['name']
    }<a href="#my-modal" class="secondary-content"><i class="material-icons place-template-choose">send</i></a></div>
         </li>`;
    placesList.insertAdjacentHTML('beforeend', placeTemplate);
  });

  placesList.style.display = 'block';
};

const init = (lat, lng) => {
  const myMap = new ymaps.Map('map', {
    center: [lat, lng],
    zoom: 15,
  });

  const marker = new ymaps.Placemark([lat, lng]);

  myMap.geoObjects.add(marker);
};

const fillModalContent = async (clickedParentNode) => {
  const { lat, lng } = JSON.parse(clickedParentNode.dataset.json).point;
  const weatherData = await weather.getWeatherByCoords(lat, lng);
  const interestingPlacesResp = await weather.getInterestingPlaces(lat, lng);

  console.log(interestingPlacesResp.data)
  for (const el of interestingPlacesResp.data["features"]) {
      const descResp = await weather.getInterestingPlaceDesc(el["id"])
      if (descResp.data["wikipedia_extracts"] && descResp.data["wikipedia_extracts"]["text"]) {
        el.desc = descResp.data["wikipedia_extracts"]["text"];
      } else {
        el.desc = null;
      }

    console.log("kek", descResp)

  }
  const childData = { ...JSON.parse(clickedParentNode.dataset.json), "weather": { ...weatherData.data }, "interesting_places": { ...interestingPlacesResp.data } };


  ymaps.ready(() => init(lat, lng));
  console.log(childData)
  modalWrapper.innerHTML = renderModal(childData);

  document.querySelector(".preloader-wrapper").classList.remove("active")
};

const showModal = () => {
  const renderedModal = document.querySelector('#my-modal');
  const instance = M.Modal.init(renderedModal);

  instance.open();
};

const handleClickOnPlace = async (e) => {
  e.preventDefault();


  const target = e.target;
  const clickedParentNode = target.closest('li');
  const { lat, lng } = JSON.parse(clickedParentNode.dataset.json).point;


  document.querySelector(".preloader-wrapper").classList.add("active")
  fillModalContent(clickedParentNode).then(() => showModal());

};

const findPlaces = (e) => {
  e.preventDefault();

  weather
    .getPlacesByName(placeName)
    .then((r) => {
      const places = r.data['hits'];

      toast.success('Успешно что-то произошло');
      drawPlacesList(places);

      const icons = document.querySelectorAll('.place-template-choose');

      icons.forEach((placeEl) => placeEl.addEventListener('click', handleClickOnPlace));
    })
    .catch((error) => toast.error(error));
};

searchFormBtn.addEventListener('click', findPlaces);
