-- Проверить при заказе услуги, что если услуги нужен какой-то материал для произведения, что он есть на складе
CREATE OR REPLACE FUNCTION check_material_availability()
    RETURNS TRIGGER AS
$$
DECLARE
    required_materials RECORD;
    available_quantity INTEGER;
BEGIN
    FOR required_materials IN
        SELECT item_id, count
        FROM service_types_needed_items
        WHERE service_type_id = NEW.service_type_id
        LOOP
        --         RAISE NOTICE 'ORDER_ID = %', NEW.order_id;
--         RAISE NOTICE 'ITEM_REQUIRED = %', required_materials.item_id;
--         RAISE NOTICE 'ITEM_REQUIRED = %', required_materials.count;
            SELECT quantity
            INTO available_quantity
            FROM storage_items si
                     JOIN storage s ON s.outlet_id = (SELECT accept_outlet_id FROM orders WHERE id = NEW.order_id)
            WHERE si.item_id = required_materials.item_id
              AND si.storage_id = s.id;


            IF available_quantity IS NULL THEN
                available_quantity := 0;
            END IF;

            IF available_quantity < required_materials.count * NEW.count THEN
                RAISE EXCEPTION 'Недостаточно материалов для производства услуги %. Не хватает товара %, Нужно %, Доступно %',
                    (SELECT name from service_types where id = NEW.service_type_id),
                    (select product_name from items where id = required_materials.item_id),
                    required_materials.count,
                    available_quantity;
            ELSE
                RAISE NOTICE 'Material available on the storage for this service order. Item ID: %, Required Count: %, Available Count: %', required_materials.item_id, required_materials.count * NEW.count, available_quantity;

                UPDATE storage_items
                SET quantity = quantity - required_materials.count * NEW.count
                FROM storage s
                WHERE storage_items.storage_id = s.id
                  AND storage_items.item_id = required_materials.item_id;
            END IF;
        END LOOP;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;



CREATE OR REPLACE TRIGGER check_material_availability_trigger
    BEFORE INSERT
    ON service_orders
    FOR EACH ROW
EXECUTE FUNCTION check_material_availability();
