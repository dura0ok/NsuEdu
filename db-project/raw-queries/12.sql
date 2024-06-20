SELECT name,
       total_workers
FROM (SELECT ot.name,
             SUM(num_workers) AS total_workers
      FROM outlets
               JOIN public.outlet_types ot ON outlets.type_id = ot.id
      GROUP BY ot.name

      UNION

      SELECT 'Global'                      AS name,
             COALESCE(SUM(num_workers), 0) AS total_workers
      FROM outlets) AS subquery
ORDER BY name;
