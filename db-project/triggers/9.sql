-- Проверить, что если заказ срочный, что тип принятого здания является филиал
CREATE OR REPLACE FUNCTION check_urgent_order_type()
    RETURNS TRIGGER AS
$$
BEGIN
    IF NEW.is_urgent THEN
        IF NOT EXISTS (SELECT 1
                       FROM outlets o
                                JOIN outlet_types ot ON o.type_id = ot.id
                       WHERE o.id = NEW.accept_outlet_id
                         AND ot.name = 'Филиал') THEN
            RAISE EXCEPTION 'Срочный заказ доступен только для филиалов';
        END IF;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER urgent_order_type_check
    BEFORE INSERT
    ON orders
    FOR EACH ROW
EXECUTE FUNCTION check_urgent_order_type();
