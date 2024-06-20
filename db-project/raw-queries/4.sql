-- Получить сумму выручки с заказов (отдельно простых и срочных) на отдельные виды
-- фоторабот по указанному филиалу, киоску приема заказов, поступивших в течение
-- некоторого периода времени.

WITH film_development_price AS (SELECT price
                                FROM public.service_types
                                WHERE name = 'Проявка плёнки')
SELECT fdo.order_id,
       SUM(
               CASE
                   WHEN o2.accept_outlet_id = o.accept_outlet_id THEN 0
                   ELSE (SELECT price FROM film_development_price)
                   END) *
       (100 - c.discount) AS total_price
FROM public.film_development_orders fdo
         JOIN
     public.orders o ON fdo.order_id = o.id
         JOIN
     public.clients c ON c.id = o.client_id
         JOIN
     films f ON fdo.code = f.code
         LEFT JOIN
     sale_films sf ON f.id = sf.id
         LEFT JOIN sale_orders so ON sf.sale_order_id = so.id
         LEFT JOIN orders o2 ON so.order_id = o2.id
GROUP BY fdo.order_id, c.discount;


WITH DiscountedFrames AS (SELECT f.print_order_id,
                                 pp.price * f.amount                            AS discounted_price,
                                 COALESCE(
                                         (SELECT MAX(pd.discount)
                                          FROM public.print_discounts pd
                                          WHERE pd.photo_amount <= (SELECT COUNT(*)
                                                                    FROM public.frames
                                                                    WHERE print_order_id = f.print_order_id)
                                          GROUP BY pd.photo_amount), 0) / 100.0 AS discount
                          FROM public.frames f
                                   INNER JOIN
                               public.print_prices pp ON f.print_price_id = pp.id)

SELECT po.order_id,
       SUM(df.discounted_price * (1 - df.discount)) *
       (100 - c.discount) AS total_amount
FROM public.print_orders po
         INNER JOIN
     DiscountedFrames df ON po.id = df.print_order_id
         JOIN orders o on po.order_id = o.id
         JOIN clients c on c.id = o.client_id
GROUP BY po.order_id, c.discount;
