import axios from 'axios';

const OPEN_TRIP_MAP_API_URL = 'http://api.opentripmap.com/0.1/ru/places/';

export default class Weather {
  async getPlacesByName(name) {
    const key = import.meta.env.VITE_GRASSHOPPER_KEY;
    const GEOCODING_API_URL = `https://graphhopper.com/api/1/geocode?q=${name}&locale=ru&key=${key}`;

    return axios.get(GEOCODING_API_URL);
  }

  async getWeatherByCoords(lat, lon) {
    const key = import.meta.env.VITE_OPENWEATHER_KEY;
    const WEATHER_API_URL = 'http://api.openweathermap.org/data/2.5/weather';
    const url = `${WEATHER_API_URL}?lat=${lat}&lon=${lon}&appid=${key}`;

    return axios.get(url);
  }

  async getInterestingPlaces(lat, lon) {
    const key = import.meta.env.VITE_OPENTRIP_KEY;

    return axios.get(
      OPEN_TRIP_MAP_API_URL + 'radius?radius=2000&lon=' + lon + '&lat=' + lat + '&limit=5' + '&apikey=' + key,
    );
  }

  async getInterestingPlaceDesc(id) {
    const key = import.meta.env.VITE_OPENTRIP_KEY;
    return axios.get(OPEN_TRIP_MAP_API_URL + 'xid/' + id + '?apikey=' + key);
  }
}
