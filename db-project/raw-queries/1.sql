-- Получить перечень и общее число пунктов приема заказов на фотоработы по филиалам, по
-- киоскам приема заказов, в целом по фотоцентру.

SELECT o.address,
       b.num_workers
FROM branches b
         JOIN public.outlets o ON b.outlet_id = o.id;

SELECT COUNT(outlet_id) as branches_cnt
FROM branches;

SELECT o.address,
       k.num_workers
FROM kiosks k
         JOIN public.outlets o ON k.outlet_id = o.id;

SELECT COUNT(outlet_id) as kiosks_cnt
FROM kiosks;

SELECT o.address
FROM photo_stores ps
         JOIN public.outlets o ON ps.outlet_id = o.id;

SELECT COUNT(outlet_id) as photostores_cnt
FROM photo_stores;


SELECT o.address,
       t.name
FROM outlets o
         JOIN outlet_types t ON o.type_id = t.id