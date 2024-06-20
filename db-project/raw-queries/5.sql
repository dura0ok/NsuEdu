-- Получить количество отпечатанных фотографий в рамках простых и срочных заказов по
-- указанному филиалу, киоску приема заказов, фотоцентру в целом за некоторый период
-- времени.
SELECT CASE WHEN o.is_urgent THEN 'Срочный' ELSE 'Простой' END AS order_type,
       COALESCE(SUM(f.amount), 0)                              AS printed_photos_count
FROM public.orders o
         LEFT JOIN
     public.print_orders po ON o.id = po.order_id
         LEFT JOIN
     public.frames f ON po.id = f.print_order_id
WHERE o.accept_outlet_id = 1
  AND o.accept_timestamp BETWEEN :start_date AND :end_date
GROUP BY order_type;