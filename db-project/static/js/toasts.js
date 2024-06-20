import Toastify from 'toastify-js';

import 'toastify-js/src/toastify.css';

export const showToast = (message, type) => {
    let backgroundColor;

    switch (type) {
        case 'success':
            backgroundColor = "linear-gradient(to right, #00b09b, #96c93d)";
            break;
        case 'error':
            backgroundColor = "linear-gradient(to right, #ff5f6d, #ffc371)";
            break;
        case 'info':
            backgroundColor = "linear-gradient(to right, #2193b0, #6dd5ed)";
            break;
        default:
            backgroundColor = "linear-gradient(to right, #000000, #434343)";
            break;
    }

    Toastify({
        text: message,
        duration: 3000,
        close: true,
        gravity: "top", // `top` or `bottom`
        position: "right", // `left`, `center` or `right`
        backgroundColor: backgroundColor,
        stopOnFocus: true, // Prevents dismissing of toast on hover
    }).showToast();
}