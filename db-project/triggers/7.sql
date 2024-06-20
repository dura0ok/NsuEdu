-- Проверка при продаже товара, что такой товар есть на складе в нужном количестве
CREATE OR REPLACE FUNCTION check_and_update_storage()
    RETURNS TRIGGER AS
$$
DECLARE
    SALE_STORAGE_ID    INTEGER;
    AVAILABLE_QUANTITY INTEGER;
BEGIN
    SELECT s.id
    INTO SALE_STORAGE_ID
    FROM storage s
             JOIN orders o ON o.id = NEW.order_id
    WHERE o.accept_outlet_id = s.outlet_id;

    RAISE NOTICE 'Sale storage ID: %', SALE_STORAGE_ID;

    SELECT quantity
    INTO AVAILABLE_QUANTITY
    FROM storage_items si
    WHERE si.item_id = NEW.item_id
      AND si.storage_id = SALE_STORAGE_ID;

    RAISE NOTICE 'Available quantity: %', AVAILABLE_QUANTITY;

    IF AVAILABLE_QUANTITY IS NULL OR AVAILABLE_QUANTITY < NEW.amount THEN
        RAISE EXCEPTION 'Недостаточное количество товара на складе % Доступно %',
            (select product_name from items where id = NEW.item_id), AVAILABLE_QUANTITY;
    END IF;

    UPDATE storage_items
    SET quantity = quantity - NEW.amount
    WHERE item_id = NEW.item_id
      AND storage_id = SALE_STORAGE_ID;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER check_and_update_storage_trigger
    BEFORE INSERT
    ON sale_orders
    FOR EACH ROW
EXECUTE FUNCTION check_and_update_storage();
