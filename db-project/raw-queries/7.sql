-- Получить перечень поставщиков в целом по фотоцентру, поставщиков отдельных видов
-- фототоваров, сделавших поставки в некоторый период, поставки определенного объема.
SELECT v.name            AS vendor_name,
       vi.item_id,
       COUNT(di.item_id) AS delivery_count,
       SUM(di.amount)    AS total_delivery_amount
FROM vendors v
         JOIN vendor_items vi ON v.id = vi.vendor_id
         JOIN delivery_items di ON vi.item_id = di.item_id
         JOIN deliveries d ON di.delivery_id = d.id
WHERE d.delivery_date BETWEEN :start_date AND :end_date
  AND di.amount >= :min_delivery_amount
GROUP BY v.name, vi.item_id;
