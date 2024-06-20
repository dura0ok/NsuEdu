-- Проверка при формировании поставки(deliviries), что поставщик такой товар поставить может
CREATE OR REPLACE FUNCTION check_vendor_item_exists() RETURNS TRIGGER AS
$$
DECLARE
    delivery_vendor_id INTEGER;
BEGIN
    -- Получаем vendor_id по delivery_id
    SELECT vendor_id
    INTO delivery_vendor_id
    FROM deliveries
    WHERE id = NEW.delivery_id;

    RAISE NOTICE 'Delivery Vendor ID: %', delivery_vendor_id;

    -- Проверяем, существует ли запись в таблице vendor_items
    IF NOT EXISTS (SELECT 1
                   FROM vendor_items
                   WHERE vendor_id = delivery_vendor_id
                     AND item_id = NEW.item_id) THEN
        RAISE EXCEPTION 'Поставщик не поставляет этот товар';
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER check_vendor_item_trigger
    BEFORE INSERT
    ON delivery_items
    FOR EACH ROW
EXECUTE FUNCTION check_vendor_item_exists();
