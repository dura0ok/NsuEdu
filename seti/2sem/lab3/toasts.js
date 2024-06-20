import Toastify from 'toastify-js';

import 'toastify-js/src/toastify.css';

const showErrorToast = (message) => {
  Toastify({
    text: message,
    duration: 3000,
    close: true,
    gravity: 'top', // Display the toast at the top of the screen
    position: 'left', // Position the toast on the left side of the screen
    backgroundColor: 'red', // Set the background color of the toast to red
    stopOnFocus: true, // Stop the toast from automatically hiding when it receives focus
  }).showToast();
};

const showSuccessToast = (message) => {
  Toastify({
    text: message,
    duration: 2500,
    close: true,
    style: {
      background: 'linear-gradient(to right, #00b09b, #96c93d)',
    },
  }).showToast();
};

export default {
  error: showErrorToast,
  success: showSuccessToast,
};
