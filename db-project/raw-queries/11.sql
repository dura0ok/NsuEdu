-- Получить перечень реализованных фототоваров и объемы их реализации в целом по
-- фотоцентру, по указанному филиалу, проданных в течение некоторого периода времени.
WITH SalesData AS (SELECT f.name                                           AS firm_name,
                          i.product_name                                   AS product_name,
                          SUM(so.amount)                                   AS count_saled,
                          ROW_NUMBER() OVER (ORDER BY SUM(so.amount) DESC) AS sales_rank
                   FROM firms f
                            INNER JOIN
                        items i ON i.firm_id = f.id
                            INNER JOIN
                        sale_orders so ON so.item_id = i.id
                   GROUP BY f.name, i.product_name)

SELECT firm_name,
       product_name,
       count_saled,
       sales_rank
FROM SalesData
ORDER BY sales_rank;
