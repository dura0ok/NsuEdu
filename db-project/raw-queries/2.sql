-- Получить перечень и общее число заказов на фотоработы по филиалам, киоскам приема
-- заказов, в целом по фотоцентру, поступивших в течение некоторого периода времени.

SELECT c.full_name,
       ot.address,
       o.accept_timestamp
FROM public.orders o
         JOIN public.outlets ot ON o.accept_outlet_id = ot.id
         JOIN public.outlet_types ot_types ON ot.type_id = ot_types.id
         JOIN clients c ON client_id = c.id
WHERE ot_types.name = 'Филиал';


SELECT c.full_name,
       ot.address,
       o.accept_timestamp
FROM public.orders o
         JOIN public.outlets ot ON o.accept_outlet_id = ot.id
         JOIN public.outlet_types ot_types ON ot.type_id = ot_types.id
         JOIN clients c ON client_id = c.id
WHERE ot_types.name = 'Киоск';


SELECT c.full_name,
       ot.address,
       o.accept_timestamp
FROM public.orders o
         JOIN public.outlets ot ON o.accept_outlet_id = ot.id
         JOIN public.outlet_types ot_types ON ot.type_id = ot_types.id
         JOIN clients c ON client_id = c.id
WHERE ot_types.name = 'Фотомагазин';


SELECT c.full_name,
       ot.address,
       o.accept_timestamp,
       ot_types.name AS accept_type
FROM public.orders o
         JOIN public.outlets ot ON o.accept_outlet_id = ot.id
         JOIN public.outlet_types ot_types ON ot.type_id = ot_types.id
         JOIN clients c ON client_id = c.id;


CREATE OR REPLACE TEMPORARY VIEW order_details AS
SELECT c.full_name,
       ot.address,
       o.accept_timestamp,
       ot_types.name AS accept_type
FROM public.orders o
         JOIN public.outlets ot ON o.accept_outlet_id = ot.id
         JOIN public.outlet_types ot_types ON ot.type_id = ot_types.id
         JOIN clients c ON o.client_id = c.id;


SELECT full_name,
       address,
       accept_timestamp
FROM order_details
WHERE accept_type = 'Филиал'
  AND accept_timestamp BETWEEN :start_date AND :end_date;


SELECT full_name,
       address,
       accept_timestamp
FROM order_details
WHERE accept_type = 'Киоск';


SELECT full_name,
       address,
       accept_timestamp
FROM order_details
WHERE accept_type = 'Фотомагазин';


SELECT full_name,
       address,
       accept_timestamp
FROM order_details;