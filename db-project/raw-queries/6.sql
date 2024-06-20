-- Получить количество проявленных фотопленок в рамках простых и срочных заказов по
-- указанному филиалу, киоску приема заказов, фотоцентру в целом за некоторый период
-- времени.
SELECT CASE WHEN o.is_urgent THEN 'Срочный' ELSE 'Простой' END AS order_type,
       COUNT(*)                                                AS developed_films_count
FROM public.orders o
         LEFT JOIN
     public.film_development_orders fdo ON o.id = fdo.order_id
WHERE o.accept_outlet_id = 1
  AND o.accept_timestamp BETWEEN :start_date AND :end_date
GROUP BY order_type;