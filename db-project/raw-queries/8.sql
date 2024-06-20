Получить список клиентов в целом по фотоцентру, клиентов указанного
филиала, имеющих скидки, сделавших заказы определенного объема.
SELECT *
FROM clients;

SELECT distinct c.*
FROM clients c
         INNER JOIN orders o ON o.client_id = c.id
         INNER JOIN outlets ot ON ot.id = o.accept_outlet_id
         INNER JOIN branches b ON b.outlet_id = ot.id
WHERE b.outlet_id = :b_outlet_id;

SELECT *
FROM clients
WHERE discount > 0;

SELECT c.*
FROM clients c
         INNER JOIN orders o ON o.client_id = c.id
WHERE o.total_amount_price > :volume;