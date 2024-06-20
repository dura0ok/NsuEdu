CREATE OR REPLACE FUNCTION get_discount_id(frame_count INTEGER)
    RETURNS INTEGER AS
$$
DECLARE
    max_discount_id INTEGER;
BEGIN
    SELECT id
    INTO max_discount_id
    FROM print_discounts
    WHERE frame_count >= photo_amount
    ORDER BY discount DESC
    LIMIT 1;

    RETURN max_discount_id;
END;
$$ LANGUAGE plpgsql;


-- Автоматическое проставление скидки
CREATE OR REPLACE FUNCTION auto_set_print_discount()
    RETURNS TRIGGER AS
$$
DECLARE
    frame_count     INTEGER;
    max_discount_id INTEGER;
BEGIN
    RAISE NOTICE '%', NEW;
    -- Получаем количество кадров для данного заказа
    SELECT SUM(frames.amount)
    INTO frame_count
    FROM frames
    WHERE print_order_id = NEW.print_order_id;

    -- Логируем количество кадров
    RAISE NOTICE 'Frame count for order % is %', NEW.print_order_id, frame_count;

    -- Получаем максимальный discount_id
    max_discount_id := get_discount_id(frame_count);

    RAISE NOTICE 'Max discount id is %', max_discount_id;

    UPDATE print_orders SET discount_id = max_discount_id WHERE id = NEW.print_order_id;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


-- Создаем триггер, который будет вызывать функцию check_print_discount
CREATE OR REPLACE TRIGGER print_order_discount_check
    AFTER INSERT OR UPDATE
    ON frames
    FOR EACH ROW
EXECUTE FUNCTION auto_set_print_discount();
