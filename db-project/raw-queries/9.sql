-- Получить сумму выручки от реализации фототоваров в целом по фотоцентру, по
-- указанному филиалу, проданных в течение некоторого периода времени.
SELECT SUM(so.amount * i.price) * (100 - c.discount) AS total_revenue
FROM sale_orders so
         INNER JOIN items i ON i.id = so.item_id
         INNER JOIN public.orders o on so.order_id = o.id
         INNER JOIN public.clients c on o.client_id = c.id
group by c.discount;

SELECT SUM(so.amount * i.price) * (100 - c.discount) AS total_revenue
FROM sale_orders so
         INNER JOIN items i ON i.id = so.item_id
         INNER JOIN orders o ON o.id = so.order_id
         INNER JOIN outlets ot ON ot.id = o.accept_outlet_id
         INNER JOIN branches b ON b.outlet_id = ot.id
         INNER JOIN public.orders o on so.order_id = o.id
         INNER JOIN public.clients c on o.client_id = c.id
WHERE b.outlet_id = :b_outlet_id
group by c.discount;

SELECT SUM(so.amount * i.price) * (100 - c.discount) AS total_revenue
FROM sale_orders so
         INNER JOIN items i ON i.id = so.item_id
         INNER JOIN orders o ON o.id = so.order_id
         INNER JOIN public.orders o on so.order_id = o.id
         INNER JOIN public.clients c on o.client_id = c.id
WHERE o.accept_timestamp BETWEEN :start_date AND :end_date
group by c.discount;