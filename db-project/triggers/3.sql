-- Проверить при вставке нового товара в storage_items, что емкость склада не превышена
CREATE OR REPLACE FUNCTION check_storage_capacity()
    RETURNS TRIGGER AS
$$
DECLARE
    total_quantity INTEGER;
    capacity_limit integer;
BEGIN

    SELECT COALESCE(SUM(quantity), 0)
    INTO total_quantity
    FROM storage_items
    WHERE storage_id = NEW.storage_id
      AND item_id != NEW.item_id;

    SELECT capacity INTO capacity_limit FROM storage WHERE id = NEW.storage_id;
    IF capacity_limit IS NULL THEN
        RAISE EXCEPTION 'Определите вместимость склада';
    end if;

    RAISE NOTICE 'Total loaded % Add capacity % Capacity limit %', total_quantity, NEW.quantity, capacity_limit;

    IF total_quantity + NEW.quantity > capacity_limit THEN
        RAISE EXCEPTION 'Хранилище с идентификатором % и ограничением емкости % превысило свою емкость на % единиц.',
            NEW.storage_id, capacity_limit, total_quantity + NEW.quantity - capacity_limit;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER check_storage_capacity_trigger
    BEFORE INSERT OR UPDATE
    ON storage_items
    FOR EACH ROW
EXECUTE FUNCTION check_storage_capacity();
