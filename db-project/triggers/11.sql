CREATE OR REPLACE FUNCTION check_film_development_order() RETURNS TRIGGER AS
$$
DECLARE
    type_id BIGINT;
BEGIN
    SELECT id INTO type_id FROM service_types WHERE name = 'Проявка плёнки';
    IF NOT EXISTS (SELECT 1 FROM service_orders WHERE service_type_id = type_id AND id = NEW.service_order_id) THEN
        RAISE EXCEPTION 'Тип Услуги должен быть проявка плёнки';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER check_correct_service_type_selected_film_order
    BEFORE INSERT OR UPDATE
    ON film_development_orders
    FOR EACH ROW
EXECUTE FUNCTION check_film_development_order();
