-- Проверить при добавлении услуги в заказ, что данный тип здания может произвести такую услугу
CREATE OR REPLACE FUNCTION check_outlet_service_type()
    RETURNS TRIGGER AS
$$
BEGIN
    IF EXISTS (SELECT 1
               FROM service_types_outlets sto
                        JOIN outlets o ON sto.outlet_type_id = o.type_id
                        JOIN orders ord ON o.id = ord.accept_outlet_id
               WHERE sto.service_type_id = NEW.service_type_id
                 AND ord.id = NEW.order_id) THEN
        RETURN NEW;
    ELSE
        RAISE EXCEPTION 'Выбранная торговая точка не может предоставить выбранный тип услуги';
    END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER check_outlet_service_type_trigger
    BEFORE INSERT OR UPDATE
    ON service_orders
    FOR EACH ROW
EXECUTE FUNCTION check_outlet_service_type();
