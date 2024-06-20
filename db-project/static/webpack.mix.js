const mix = require('laravel-mix');

mix
    .sass('./scss/main.scss', 'dist/bundle.css')
    .js(['js/queries.js', 'js/toasts.js'], 'dist/bundle.js')
    .js(['js/order.js', 'js/toasts.js'], 'dist/order.js')