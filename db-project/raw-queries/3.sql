-- Получить перечень и общее число заказов (отдельно простых и срочных) на отдельные
-- виды фоторабот по указанному филиалу, киоску приема заказов, поступивших в течение
-- некоторого периода времени

CREATE OR REPLACE TEMPORARY VIEW services_list AS
SELECT st.name,
       o.is_urgent,
       o.accept_timestamp,
       c.full_name,
       ot.address,
       o.accept_outlet_id
FROM service_orders so
         JOIN orders o ON so.order_id = o.id
         JOIN service_types st ON so.service_type_id = st.id
         JOIN public.clients c ON o.client_id = c.id
         JOIN outlets ot ON o.accept_outlet_id = ot.id;


SELECT *
FROM services_list
WHERE accept_outlet_id = 1
  AND is_urgent = TRUE
  AND accept_timestamp BETWEEN CURRENT_DATE - 3 AND CURRENT_TIMESTAMP;